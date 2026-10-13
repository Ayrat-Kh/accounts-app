#!/usr/bin/env bash
# Fails if the two vcpkg manifests pin different baselines: the backend and
# the core library must build against the same dependency versions.
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
api="$(jq -r '.["builtin-baseline"]' "${REPO_ROOT}/accounts-api/vcpkg.json")"
core="$(jq -r '.["builtin-baseline"]' "${REPO_ROOT}/core/vcpkg.json")"
if [[ "$api" != "$core" ]]; then
  echo "vcpkg baselines differ: accounts-api=${api} core=${core}" >&2
  exit 1
fi
echo "vcpkg baseline ${api} (accounts-api and core agree)"
