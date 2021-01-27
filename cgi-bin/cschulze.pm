package cschulze;

# This package computes the Schulze proportional ranking of the choices, by
# providing an appropriate rank_candidates function. See runoff.pm for a
# specification of that function.
#
# The ranking itself is computed by the cschulze program (src/cschulze.cpp),
# Markus Schulze's own implementation, which install-civs builds and puts
# next to the other helper programs. This package writes the ballots out in
# the format that program reads, runs it, and reads the ranking back.

use CGI qw(:standard -utf8);
use File::Temp qw(tempfile);
use election;
use strict;

my $cschulze = '@CIVSDATADIR@/cschulze';

# The rank a ballot carries for a choice the voter had no opinion on.
my $NO_OPINION = 'No opinion';

# The program keeps 26 candidate names, and identifies a candidate in its
# output by a letter when there are no more choices than that, and by a
# 1-based number when there are.
my $max_named_choices = 26;

# write_ballots($fh, $choices, $ballots): write the ballots to $fh in the
# program's "format 1": a header naming the choices, then one line per
# ballot holding a ballot number followed by the rank the voter gave each
# choice, in the order the choices are listed.
#
# This is also what the results page offers for download, so that the tally
# can be checked by running the program by hand.
sub write_ballots {
    my ($fh, $choices, $ballots) = @_;

    my $n = scalar @{$choices};

    # The number of winners is what the format calls M. The program ranks
    # every choice regardless, and CIVS takes the winners off the top of
    # that ranking itself.
    print $fh "M $num_winners\n";
    print $fh "C $n\n";
    print $fh 'N ', scalar @{$ballots}, "\n";
    print $fh "F 1\n";
    print $fh "\n";

    print $fh "# Candidates in order:\n";
    for (my $i = 0; $i < $n && $i < $max_named_choices; $i++) {
        # A name is read up to the end of its line, so it cannot contain one.
        my $name = $choices->[$i];
        $name =~ s/\s+/ /g;
        print $fh 'X ', chr(ord('A') + $i), " $name\n";
    }
    print $fh "\n";

    print $fh "BEGIN\n";
    my $number = 0;
    foreach my $ballot (@{$ballots}) {
        $number++;
        my @ranks;
        for (my $i = 0; $i < $n; $i++) {
            my $rank = $ballot->[$i];
            # A choice the voter had no opinion on ranks below every choice
            # they did rank, as it does everywhere else in CIVS.
            $ranks[$i] = (!defined($rank) || $rank eq $NO_OPINION)
                            ? $n + 1 : 0 + $rank;
        }
        printf $fh "%03d %s\n", $number, join(' ', @ranks);
    }
    print $fh "END\n";
}

# run_cschulze($choices, $ballots): run the program over the ballots and
# return its ranking summary and its detailed output, or undef and a message
# if that did not work.
sub run_cschulze {
    my ($choices, $ballots) = @_;

    my ($fh, $ballotfile) = tempfile('cschulze-XXXXXXXX', TMPDIR => 1,
                                     UNLINK => 1);
    write_ballots($fh, $choices, $ballots);
    close $fh;

    my ($dfh, $detailfile) = tempfile('cschulze-XXXXXXXX', TMPDIR => 1,
                                      UNLINK => 1);
    close $dfh;

    # Fork and exec rather than handing a command line to a shell, and fold
    # the program's standard error into the pipe so that a failure to run it
    # at all shows up in the result details rather than only in the server
    # log.
    my $pipe;
    my $pid = open($pipe, '-|');
    if (!defined($pid)) {
        return (undef, undef, "Could not run $cschulze: $!");
    }
    if (!$pid) {
        open(STDERR, '>&', \*STDOUT);
        exec($cschulze, $ballotfile, $detailfile);
        print "Could not run $cschulze: $!\n";
        exit 127;
    }
    my $summary = do { local $/; <$pipe> };
    $summary = '' if !defined($summary);
    if (!close($pipe)) {
        return (undef, undef, "$cschulze did not complete (exit status "
                              . ($? >> 8) . "):\n" . $summary);
    }

    my $details = '';
    if (open(my $dh, '<', $detailfile)) {
        $details = do { local $/; <$dh> };
        close $dh;
    }

    return ($summary, $details, undef);
}

# rank_candidates($n, $matrix, $ballots, $choices, $algorithm):
# construct a ranking of the choices. See runoff.pm.
sub rank_candidates {
    my ($n, $matrix, $ballots, $choices) = @_;

    # The program indexes its ballots from zero and would read off the end
    # of them if there were none.
    if (!@{$ballots} || $n == 0) {
        return ([], 'No ballots to rank.');
    }

    (my $summary, my $details, my $error) = run_cschulze($choices, $ballots);
    return ([], $error) if defined($error);

    # Each place in the ranking comes back as a line naming the choice that
    # took it, either by letter or, for a poll with more choices than there
    # are letters, by its 1-based position.
    my @rankings;
    my %seen;
    foreach my $line (split /\r?\n/, $summary) {
        next unless $line =~
            m/^The\s+\d+\. place goes to candidate ([A-Z]|[0-9]+)\b/;
        my $id = $1;
        my $i = ($id =~ m/^[0-9]+$/) ? $id - 1 : ord($id) - ord('A');
        next if $i < 0 || $i >= $n || $seen{$i}++;
        push @rankings, [$i];
    }

    if (!@rankings) {
        return ([], "Could not read the ranking from $cschulze:\n$summary");
    }

    return (\@rankings, $details);
}

# print_details($log, $n, $choices, $choice_index):
# Print out to RESULTS the details of the election algorithm, using the
# information in $log that was returned by rank_candidates.
sub print_details {
    my ($log, $num_choices, $choices, $ciref) = @_;

    print main::RESULTS p('The choices, in the order the ranking below
        refers to them:');
    print main::RESULTS '<pre>';
    for (my $i = 0; $i < $num_choices; $i++) {
        my $id = ($num_choices > $max_named_choices)
                    ? $i + 1 : chr(ord('A') + $i);
        print main::RESULTS escapeHTML("$id: $choices->[$i]"), "\n";
    }
    print main::RESULTS '</pre>';

    print main::RESULTS '<pre>', escapeHTML($log), '</pre>';
}

1; # ok!
