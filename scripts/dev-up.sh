#!/usr/bin/env bash
# Starts (or reuses) throwaway local Postgres and Garage containers (no host
# ports: reach them from scripts/dev.sh with DEV_NETWORK=accounts-dev) on the
# docker network "accounts-dev", and writes their coordinates to
# scripts/.dev-env for the tests and a local server. Idempotent.
#
#   scripts/dev-up.sh
#   DEV_NETWORK=accounts-dev DEV_ENV_FILE=scripts/.dev-env \
#     scripts/dev.sh ctest --test-dir accounts-api/build/debug --output-on-failure
#
#   scripts/dev-up.sh down     # remove the containers and their data
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NETWORK=accounts-dev
PG_CONTAINER=accounts-postgres-dev
PG_IMAGE=postgres:17-trixie@sha256:2d2b8998d31037bf721cfdf764d76ba74171b4fab3431b7f72c27c56ddbdf9e3
GARAGE_CONTAINER=accounts-garage-dev
GARAGE_IMAGE=dxflrs/garage:v2.4.1@sha256:9c96caa2612d3411acc5b0e6701fb238dbfba33e533a6d7d3d811a4b12d0d020
BUCKET=accounts-test
KEY_NAME=accounts-test-key
GEN_DIR="${SCRIPT_DIR}/dev/.generated"

if [[ "${1:-}" == down ]]; then
  docker rm -f "$PG_CONTAINER" "$GARAGE_CONTAINER" >/dev/null 2>&1 || true
  docker volume rm accounts-garage-dev-meta accounts-garage-dev-data >/dev/null 2>&1 || true
  rm -rf "$GEN_DIR" "${SCRIPT_DIR}/.dev-env"
  echo "dev services removed"
  exit 0
fi

# macOS's LibreSSL cannot generate Ed25519 keys; prefer Homebrew's OpenSSL 3.
OPENSSL=openssl
[[ -x /opt/homebrew/opt/openssl@3/bin/openssl ]] && OPENSSL=/opt/homebrew/opt/openssl@3/bin/openssl

running() { docker ps --format '{{.Names}}' | grep -qx "$1"; }
garage() { docker exec "$GARAGE_CONTAINER" /garage "$@"; }

docker network inspect "$NETWORK" >/dev/null 2>&1 || docker network create "$NETWORK" >/dev/null

# --- Postgres (data is disposable: no volume) ---------------------------------
if ! running "$PG_CONTAINER"; then
  docker rm -f "$PG_CONTAINER" >/dev/null 2>&1 || true
  docker run -d --name "$PG_CONTAINER" --network "$NETWORK" \
    -e POSTGRES_USER=accounts -e POSTGRES_PASSWORD=accounts -e POSTGRES_DB=accounts \
    "$PG_IMAGE" >/dev/null
fi
for _ in $(seq 1 30); do
  docker exec "$PG_CONTAINER" pg_isready -U accounts >/dev/null 2>&1 && break
  sleep 1
done

# --- Garage --------------------------------------------------------------------
mkdir -p "$GEN_DIR"
if [[ ! -f "${GEN_DIR}/garage.toml" ]]; then
  sed -e "s#__RPC_SECRET__#$("$OPENSSL" rand -hex 32)#" -e "s#__ADMIN_TOKEN__#$("$OPENSSL" rand -base64 32)#" \
    "${SCRIPT_DIR}/dev/garage.toml" > "${GEN_DIR}/garage.toml"
fi
if ! running "$GARAGE_CONTAINER"; then
  docker rm -f "$GARAGE_CONTAINER" >/dev/null 2>&1 || true
  docker run -d --name "$GARAGE_CONTAINER" --network "$NETWORK" \
    -v "${GEN_DIR}/garage.toml:/etc/garage.toml" \
    -v accounts-garage-dev-meta:/var/lib/garage/meta -v accounts-garage-dev-data:/var/lib/garage/data \
    "$GARAGE_IMAGE" >/dev/null
  for _ in $(seq 1 20); do garage status >/dev/null 2>&1 && break; sleep 1; done
fi
node_id="$(garage node id -q | cut -d@ -f1)"
if ! garage layout show | grep -q "${node_id:0:16}"; then
  garage layout assign -z dc1 -c 1G "$node_id" >/dev/null
  version="$(garage layout show | sed -nE 's/.*version:? *([0-9]+).*/\1/p' | head -1)"
  garage layout apply --version "$(( ${version:-0} + 1 ))" >/dev/null
fi
garage bucket create "$BUCKET" >/dev/null 2>&1 || true
garage key info "$KEY_NAME" >/dev/null 2>&1 || garage key create "$KEY_NAME" >/dev/null
garage bucket allow --read --write --owner "$BUCKET" --key "$KEY_NAME" >/dev/null
key_info="$(garage key info "$KEY_NAME" --show-secret)"
access_key="$(sed -nE 's/.*Key ID:[[:space:]]*//p' <<<"$key_info" | head -1)"
secret_key="$(sed -nE 's/.*Secret key:[[:space:]]*//p' <<<"$key_info" | head -1)"
[[ -n "$access_key" && -n "$secret_key" ]] || { echo "cannot read the Garage key:" >&2; echo "$key_info" >&2; exit 1; }

cat > "${SCRIPT_DIR}/.dev-env" <<ENV
ACCOUNTS_TEST_DATABASE_URL=host=${PG_CONTAINER} port=5432 dbname=accounts user=accounts password=accounts
ACCOUNTS_TEST_S3_ENDPOINT=http://${GARAGE_CONTAINER}:3900
ACCOUNTS_TEST_S3_BUCKET=${BUCKET}
ACCOUNTS_TEST_S3_ACCESS_KEY=${access_key}
ACCOUNTS_TEST_S3_SECRET_KEY=${secret_key}
ENV
# Secret files for running the server itself with accounts-api/accounts.example.toml.
printf '%s' accounts > "${GEN_DIR}/pg-password"
printf '%s' "$access_key" > "${GEN_DIR}/s3.access"
printf '%s' "$secret_key" > "${GEN_DIR}/s3.secret"
[[ -f "${GEN_DIR}/jwt-ed25519.pem" ]] || "$OPENSSL" genpkey -algorithm ed25519 -out "${GEN_DIR}/jwt-ed25519.pem" 2>/dev/null

echo "dev services ready on network ${NETWORK}; settings in scripts/.dev-env"
