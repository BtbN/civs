# Migrating an existing CIVS server into the compose stack

An instance running the pre-rebase fork can be moved across without losing
polls, ballots or the links already handed out. The database layout did not
change: the keys `election.pm` reads are the same ones the old fork wrote,
and the one key added since (`used_voters`) is read behind a check for
whether it exists, so polls written by the old code load unchanged.

What follows was worked out by doing it -- old instance built from the
pre-rebase branch, polls created and voted on, the data directory moved
across, everything checked on the other side.

## What to carry over

Everything under the old `$CIVSDATADIR` (`config/*` names it; typically
something like `/home/civs/data`). Three things in it matter beyond the
polls themselves:

- **`private_host_id`** -- the server's identity. Without the original file
  the CGI scripts stop serving existing polls entirely: a poll whose control
  link worked a moment earlier returns 503. This is the one file that must
  not be regenerated.
- **`nonce_seed`** -- carry it over too rather than letting the install seed
  a fresh one.
- **`elections/`** -- one directory per poll, each holding `election_data`,
  `vote_data`, `vote_log` and the `started`/`stopped` markers.

Two settings have to match the old server as well:

- **`ADMIN_KEY`.** On this fork it is also the password `create_election`
  demands, so poll creation stops working if it changes. It is the default
  for `EMAIL_SALT`, and changing that salt discards every recorded opt-in.
- **The host name.** Control and ballot links that have already been mailed
  out contain the old `$THISHOST`. Keep the domain pointed at the new server
  and they keep working; change it and every link already sent is dead.

## Doing it

On the old server, with the web server stopped so nothing is mid-write:

    tar -C /path/to/civs/data -cf civs-data.tar .

On the new one, having copied `civs-data.tar` next to `compose.yaml`:

    # Create the stack and its volume, then stop CIVS before touching data.
    docker compose up -d
    docker compose stop civs

    vol=$(docker volume ls -q | grep civs-data)

    docker run --rm -v "$vol":/data -v "$PWD":/backup:ro alpine \
        sh -c 'rm -rf /data/* && tar -C /data -xf /backup/civs-data.tar'

    # The image's web user is www-data, uid 33, which is unlikely to be what
    # owned these files before. Give the private key and the nonce seed the
    # permissions install-civs now sets on a fresh install.
    docker run --rm -v "$vol":/data alpine sh -c '
        chown -R 33:33 /data
        chmod 600 /data/private_host_id /data/nonce_seed
        find /data/elections -name "results_*"  -delete
        find /data/elections -name "ballots_*.dat" -delete
        find /data/elections -name "details_*.dat" -delete'

    docker compose start civs

The `results_*` files are cached result pages. Both versions name them the
same way, and a cached page is reused while it is newer than the last vote
-- so without this a closed poll would go on serving the page the old code
rendered, from the era when the results template was cut down to a single
completion rule. Deleting them costs one recomputation each.

The `ballots_*.dat` and `details_*.dat` files are leftovers from the old
cschulze wrapper, which left them in the poll directory; the download is
generated on request now and does not read them.

The helper programs (`gettimeofday`, `lockserv`, `timeout`, and `cschulze`
if it is there) are overwritten by `install-civs` at startup, so old builds
in the data directory are replaced with ones built for this image.

## Checking it worked

    # a closed poll still tabulates, with its title and ballots
    curl -s "$SITE/cgi-bin/civs/results.pl?id=E_..." | grep -i '<title>'

    # a control link handed out before the move still opens
    curl -s -o /dev/null -w '%{http_code}\n' \
        "$SITE/cgi-bin/civs/control.pl?id=E_...&key=...&akey=..."

A poll that is still open goes on accepting ballots, and ballots cast before
the move stay in its report alongside the new ones.

## Two things that change

**Ballot IDs are unaffected** -- they live in `election_data` under
`ballot_id <voter key>`, which is exactly where the rebased code looks.

**How a public poll recognises a voter does change.** The old fork took the
voter's address from the peer address, or from the `IPRemoteAddr` /
`Remote-Address` headers. The rebased code prefers `X-Real-IP`, then
`X-Forwarded-For`. If the old server faced the internet directly, both come
out as the client's own address and nothing changes. If it sat behind a
proxy of its own, the old install saw the proxy for every voter -- one
identity for everybody -- and after the move each voter is recognised
separately, which is the intended behaviour but not the old one.

## One oddity you may run into

If the very first poll ever created on that server has an id that is not
`E_` followed by hex -- the old `install-civs` seeded the nonce file with
base64, so the first id drawn from it could contain other characters -- it
will not load. It did not load on the old server either: both versions
check the id against the same pattern. Only that first poll can be affected;
the seed is rewritten as an MD5 hash on first use.
