#!/usr/bin/env bash
# Builds the accounts-dev image and runs a command in it with the repo
# mounted at /src. Backend builds happen here, never on the macOS host.
#
#   scripts/dev.sh scripts/bootstrap-vcpkg.sh
#   scripts/dev.sh cmake -S accounts-api --preset debug
#   scripts/dev.sh cmake --build accounts-api/build/debug
#   DEV_NETWORK=accounts-dev DEV_ENV_FILE=scripts/.dev-env \
#     scripts/dev.sh ctest --test-dir accounts-api/build/debug
#
# DEV_NETWORK   join a docker network (scripts/dev-up.sh creates accounts-dev)
# DEV_ENV_FILE  pass an env file through (scripts/dev-up.sh writes scripts/.dev-env)
# DEV_PORTS=1   publish port 8080 to 127.0.0.1
# VCPKG_CACHE_DIR=<dir>  keep vcpkg's binary cache in a host folder (CI caches it)
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_TAG="accounts-dev"

docker build --quiet --tag "${IMAGE_TAG}" --file "${REPO_ROOT}/accounts-api/docker/dev.Dockerfile" \
  "${REPO_ROOT}/accounts-api/docker" >/dev/null

flags=(--rm -i)
[[ -t 0 && -t 1 ]] && flags+=(-t)
[[ "${DEV_PORTS:-0}" == 1 ]] && flags+=(--publish 127.0.0.1:8080:8080)
[[ -n "${DEV_NETWORK:-}" ]] && flags+=(--network "${DEV_NETWORK}")
[[ -n "${DEV_ENV_FILE:-}" ]] && flags+=(--env-file "${DEV_ENV_FILE}")

cache_mount="type=volume,source=accounts-vcpkg-binary-cache,target=/root/.cache/vcpkg"
if [[ -n "${VCPKG_CACHE_DIR:-}" ]]; then
  mkdir -p "${VCPKG_CACHE_DIR}"
  cache_mount="type=bind,source=$(cd "${VCPKG_CACHE_DIR}" && pwd),target=/root/.cache/vcpkg"
fi

exec docker run "${flags[@]}" \
  --mount "type=bind,source=${REPO_ROOT},target=/src" \
  --mount "${cache_mount}" \
  --mount "type=volume,source=accounts-third-party,target=/src/third_party" \
  --mount "type=volume,source=accounts-api-build,target=/src/accounts-api/build" \
  --workdir /src \
  --env VCPKG_DEFAULT_BINARY_CACHE=/root/.cache/vcpkg \
  "${IMAGE_TAG}" "$@"
