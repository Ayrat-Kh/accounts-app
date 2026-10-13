#!/usr/bin/env bash
# Deploy main's HEAD right away through CI's backend workflow (workflow_dispatch
# with deploy=true). CI still builds, scans, signs and deploys the image exactly
# like a push; with the default, only the test jobs are skipped.
#
#   scripts/deploy-now.sh               build + deploy, skip tests
#   scripts/deploy-now.sh --with-tests  run the tests first
#   scripts/deploy-now.sh --push        git push first if main is ahead of origin
set -euo pipefail

SKIP_TESTS=true
PUSH=false
for arg in "$@"; do
  case "$arg" in
    --with-tests) SKIP_TESTS=false ;;
    --push) PUSH=true ;;
    *) echo "unknown option: $arg" >&2; exit 2 ;;
  esac
done

cd "$(dirname "${BASH_SOURCE[0]}")/.."
git fetch -q origin main
ahead="$(git rev-list --count origin/main..main)"
if (( ahead > 0 )); then
  [[ "$PUSH" == true ]] || { echo "main is ${ahead} commit(s) ahead of origin; push first or pass --push" >&2; exit 1; }
  git push origin main
fi

started="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
gh workflow run backend.yml --ref main -f deploy=true -f skip_tests="$SKIP_TESTS"
echo "Deploying $(git rev-parse --short origin/main) (skip_tests=${SKIP_TESTS})"

# `gh workflow run` doesn't print the run id; pick the dispatch run created just now.
run=""
for _ in $(seq 1 30); do
  run="$(gh run list --workflow backend.yml --event workflow_dispatch --limit 5 \
    --json databaseId,createdAt --jq "map(select(.createdAt >= \"${started}\")) | .[0].databaseId // empty")"
  [[ -n "$run" ]] && break
  sleep 2
done
[[ -n "$run" ]] || { echo "run did not show up; see: gh run list --workflow backend.yml" >&2; exit 1; }
gh run watch "$run" --exit-status --interval 15
