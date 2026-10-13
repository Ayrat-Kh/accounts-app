#!/usr/bin/env bash
# Clones vcpkg into third_party/vcpkg at the commit pinned by
# accounts-api/vcpkg.json's builtin-baseline (core/vcpkg.json must pin the
# same one; scripts/check-versions.sh verifies it) and bootstraps it.
# Idempotent. Inside scripts/dev.sh, third_party/ is a Docker volume, so the
# Linux and macOS checkouts never overwrite each other's vcpkg binary.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BASELINE="$(jq -r '.["builtin-baseline"]' "${REPO_ROOT}/accounts-api/vcpkg.json")"
VCPKG_DIR="${REPO_ROOT}/third_party/vcpkg"

if [[ -d "${VCPKG_DIR}/.git" ]]; then
  if [[ "$(git -C "${VCPKG_DIR}" rev-parse HEAD)" != "${BASELINE}" ]]; then
    git -C "${VCPKG_DIR}" fetch --quiet origin "${BASELINE}"
    git -C "${VCPKG_DIR}" checkout --quiet "${BASELINE}"
  fi
else
  git clone --quiet --filter=blob:none https://github.com/microsoft/vcpkg.git "${VCPKG_DIR}"
  git -C "${VCPKG_DIR}" checkout --quiet "${BASELINE}"
fi

if [[ ! -x "${VCPKG_DIR}/vcpkg" ]]; then
  "${VCPKG_DIR}/bootstrap-vcpkg.sh" -disableMetrics
fi
echo "vcpkg ready at ${VCPKG_DIR} (baseline ${BASELINE})"
