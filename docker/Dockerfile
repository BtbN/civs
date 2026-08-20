# A CIVS instance: Apache with mod_cgi, the Perl modules CIVS needs, and the
# helper programs built from src/.
#
# The install itself happens when the container starts rather than here,
# because install-civs bakes the host name, the URLs and the admin key into
# every script and page it writes, and those are not known until then. It
# takes a second or two.
#
# Build from the top of the source tree, not from this directory:
#
#     docker build -f docker/Dockerfile -t civs .
#
# The submodules under js/ must be checked out first:
#
#     git submodule update --init

FROM debian:bookworm-slim AS build

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        g++ \
        make \
    && rm -rf /var/lib/apt/lists/*

COPY src /usr/src/civs/src
RUN cd /usr/src/civs/src && make -s gettimeofday timeout lockserv cschulze



FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        apache2 \
        perl \
        libcgi-pm-perl \
        libhtml-parser-perl \
        libhtml-tagset-perl \
        libhtml-tagfilter-perl \
        liburi-perl \
        libauthen-sasl-perl \
        libnet-ssleay-perl \
        libio-socket-ssl-perl \
        make \
        openssl \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN a2enmod cgid negotiation mime \
    && a2dissite 000-default
COPY docker/civs.conf /etc/apache2/sites-available/civs.conf
RUN a2ensite civs

COPY . /usr/src/civs
COPY --from=build /usr/src/civs/src/gettimeofday \
                  /usr/src/civs/src/timeout \
                  /usr/src/civs/src/lockserv \
                  /usr/src/civs/src/cschulze \
                  /usr/src/civs/src/

# install-civs builds these with make, and make goes by modification time.
# The sources and the binaries arrive here from two independent COPYs, so a
# cached build stage can leave a binary older than the source beside it --
# a checkout that rewrites a file with the same contents is enough to do it
# -- and there is no compiler here to recover with. Stamp them newer.
RUN touch /usr/src/civs/src/gettimeofday /usr/src/civs/src/timeout \
          /usr/src/civs/src/lockserv /usr/src/civs/src/cschulze
COPY docker/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

VOLUME /var/lib/civs
EXPOSE 80

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["apache2ctl", "-D", "FOREGROUND"]
