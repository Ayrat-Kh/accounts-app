# Toolchain for building and testing accounts-api (Linux artifacts).
# Debian 13 ships GCC 14: std::expected, <format> and std::chrono::parse.
# Run it through scripts/dev.sh; never build the backend on the macOS host.
FROM debian:trixie-slim@sha256:a29215f6a35e51e22adffa17f89e9d2ef06214e64a2bad10d765c46aea49f11f

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build pkg-config \
      git curl ca-certificates zip unzip tar jq python3 \
      autoconf autoconf-archive automake libtool bison flex \
      linux-libc-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
CMD ["/bin/bash"]
