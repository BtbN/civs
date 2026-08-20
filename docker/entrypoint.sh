#!/bin/bash
#
# Install CIVS with the settings given in the environment, then hand over to
# whatever the container was asked to run (Apache, by default).
#
# install-civs writes the host name, the URLs, the admin key and the mail
# settings into every script and page it installs, so the install has to
# happen here rather than when the image is built.

set -e

src=/usr/src/civs
data=${CIVS_DATA_DIR:-/var/lib/civs}
settings=/etc/civs-settings

# The admin key protects poll creation and the admin views. Keeping a
# generated one in the data directory means a container started without
# CIVS_ADMIN_KEY still works, and still works the same way next time.
keyfile="$data/admin_key"
if [ -z "${CIVS_ADMIN_KEY:-}" ]; then
    if [ ! -s "$keyfile" ]; then
        mkdir -p "$data"
        openssl rand -hex 16 > "$keyfile"
        chmod 600 "$keyfile"
        echo "No CIVS_ADMIN_KEY given; generated one in $keyfile"
    fi
    CIVS_ADMIN_KEY=$(cat "$keyfile")
fi

# install-civs substitutes these into sed expressions unquoted, so a value
# with a space in it would split into several sed arguments and break the
# install. The same goes for the | that separates a sed expression.
for var in CIVS_ADMIN_KEY CIVS_HOST CIVS_PROTO CIVS_SUPERVISOR; do
    case "${!var:-}" in
        *[[:space:]\|]*)
            echo "ERROR: $var must not contain whitespace or '|'" >&2
            exit 1;;
    esac
done

host=${CIVS_HOST:-localhost:8080}
proto=${CIVS_PROTO:-http}
supervisor=${CIVS_SUPERVISOR:-civs@localhost}

# Without a mail server there is no way to send a supervisor the control
# link for a poll they have just created, so CIVS prints it on the page
# instead. That is what LOCALDEBUG does, and it is the only way a container
# with no SMTP host configured is usable at all.
if [ -n "${CIVS_SMTP_HOST:-}" ]; then
    localdebug=${CIVS_LOCALDEBUG:-}
else
    localdebug=${CIVS_LOCALDEBUG:-1}
fi

cat > "$settings" <<EOF
CIVSHOME='${proto}://${host}/civs'
PROTO=${proto}
THISHOST='${host}'
SUPERVISOR='${supervisor}'
AUTH_SENDER='${CIVS_AUTH_SENDER:-$supervisor}'

SMTP_HOST='${CIVS_SMTP_HOST:-localhost}'
SMTP_PORT=${CIVS_SMTP_PORT:-25}
SMTP_USE_SSL=${CIVS_SMTP_USE_SSL:-0}
SMTP_STARTTLS=${CIVS_SMTP_STARTTLS:-0}
SMTP_AUTH_USER='${CIVS_SMTP_AUTH_USER:-}'
SMTP_AUTH_PASSWD='${CIVS_SMTP_AUTH_PASSWD:-}'

PERL='/usr/bin/perl'
PERLEXT='.pl'
ADDTOPATH=''

HTMLDIR='/var/www/html/civs'
CGIBINDIR='/usr/lib/cgi-bin/civs'
CIVSDATADIR='${data}'
CIVSBINURL='/cgi-bin/civs'
CIVSURL='/civs'

LOCALDEBUG='${localdebug}'
FILTER_TAGS='yes'
USING_ISA='${CIVS_USING_ISA:-}'

MAX_VOTER_ADD=${CIVS_MAX_VOTER_ADD:-1000}
MAX_CHOICES=${CIVS_MAX_CHOICES:-100}
MAX_QUESTIONS=${CIVS_MAX_QUESTIONS:-20}
MAX_EMAIL_LOAD=${CIVS_MAX_EMAIL_LOAD:-5000}
MAX_IMAGE_SIZE=${CIVS_MAX_IMAGE_SIZE:-1000000}
MAX_UPLOAD_SIZE=${CIVS_MAX_UPLOAD_SIZE:-1000000}
MAX_TEST_BALLOTS=${CIVS_MAX_TEST_BALLOTS:-200000}

SUGGESTION_BOX='${CIVS_SUGGESTION_BOX:-}'
DONATE_URL='${CIVS_DONATE_URL:-}'
LOG_HOME_VISITS=

EN_SUFFIX='.en'
UTF8_SUFFIX='.utf8'
FIXUTF8=${CIVS_FIXUTF8:-0}

ADMIN_KEY='${CIVS_ADMIN_KEY}'
EMAIL_SALT='${CIVS_EMAIL_SALT:-$CIVS_ADMIN_KEY}'

WEB_USER=www-data
WEB_GROUP=www-data
CHOWN=/usr/bin/chown
CHGRP=/usr/bin/chgrp
EOF
chmod 600 "$settings"

mkdir -p "$data/elections" "$data/invites" /var/www/html/civs /usr/lib/cgi-bin/civs

echo "=== installing CIVS for ${proto}://${host} ==="
( cd "$src" && ./install-civs "$settings" )

# install-civs only chowns what it creates, and a mounted volume arrives
# owned by whoever made it.
chown -R www-data:www-data "$data"

echo "=== CIVS is at ${proto}://${host}/civs/ ==="
if [ -z "${CIVS_ADMIN_KEY_QUIET:-}" ]; then
    echo "=== admin key: ${CIVS_ADMIN_KEY} ==="
fi

exec "$@"
