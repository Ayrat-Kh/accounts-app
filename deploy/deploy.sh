#!/usr/bin/env bash
# =====================================================================
# deploy/deploy.sh: every accounts-api deployment task, driven by three files:
#   deploy/config.env        non-secret settings (committed)
#   deploy/app-secrets.env   secrets that go into the cluster, applied by `secrets`
#   deploy/access.env        the kubeconfig (-> GitHub and the tunnel); never the cluster
# Both secret files are gitignored and 0600 (templates: *.env.example). Each may only
# hold its own keys (APP_SECRET_KEYS / ACCESS_KEYS below): a misplaced one is an error.
#
# Same layout and conventions as ../obsidian/deploy/deploy.sh: kubectl runs as
# vps-setup's limited kube-deploy identity (app-* namespaces only) through an
# SSH tunnel to the API server, exactly like the CI deploy job.
#
#   deploy.sh init            generate missing secrets (never overwrites)
#   deploy.sh secrets         apply the Secrets accounts-secrets and ghcr
#   deploy.sh apply           render + apply the manifests and wait for the rollout
#                             (ACCOUNTS_DIGEST=sha256:... picks the image; default:
#                             keep the running one, or the newest in GHCR)
#   deploy.sh garage-init     create our bucket + key in obsync's Garage. Idempotent
#   deploy.sh kubectl ARGS... kubectl in app-accounts through the tunnel
#   deploy.sh render          render the manifests only, into deploy/k8s/.build/prod
#   deploy.sh gh-secrets      upload CI credentials to the GitHub "production" environment
#   deploy.sh db-shell        psql into the production database
#   deploy.sh backup-now      run the pg-backup CronJob once, now
# =====================================================================
set -euo pipefail
DEPLOY_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$DEPLOY_DIR")"
NS=app-accounts
GARAGE_NS=app-obsync
K8S_BUILD="$DEPLOY_DIR/k8s/.build/prod"
BUILD="$DEPLOY_DIR/.build"

TMP_DIRS=()
trap 'rm -rf ${TMP_DIRS[@]+"${TMP_DIRS[@]}"}' EXIT   # temp dirs may hold secrets

log()  { printf '\n\033[1;32m==> %s\033[0m\n' "$*" >&2; }
warn() { printf '\033[1;33m!! %s\033[0m\n' "$*" >&2; }
die()  { printf '\033[1;31mxx %s\033[0m\n' "$*" >&2; exit 1; }

# --- config -----------------------------------------------------------
APP_SECRETS="$DEPLOY_DIR/app-secrets.env"
ACCESS="$DEPLOY_DIR/access.env"
APP_SECRET_KEYS="JWT_ED25519_PEM_B64 PG_PASSWORD S3_ACCESS_KEY S3_SECRET_KEY GHCR_PULL_TOKEN"
ACCESS_KEYS="KUBE_CONFIG_B64"

# Source a secrets file after checking its mode and that it defines only its own keys.
load_secrets() {  # $1 file, $2 allowed keys
  local f="$1" key
  [[ -f "$f" ]] || return 0
  [[ "$(stat -c %a "$f" 2>/dev/null || stat -f %Lp "$f")" == 600 ]] || warn "${f#"$REPO_DIR"/} should be chmod 600"
  for key in $(sed -nE 's/^([A-Za-z_][A-Za-z0-9_]*)=.*/\1/p' "$f"); do
    [[ " $(echo $2) " == *" $key "* ]] || die "${f#"$REPO_DIR"/}: $key doesn't belong here (allowed: $(echo $2))"
  done
  # shellcheck source=/dev/null
  source "$f"
}

set -a   # export everything, so the template renderer (perl, $ENV) sees it
# shellcheck source=config.env
source "$DEPLOY_DIR/config.env"
load_secrets "$APP_SECRETS" "$APP_SECRET_KEYS"
load_secrets "$ACCESS" "$ACCESS_KEYS"
set +a

need() {  # need VAR... : die naming every empty one
  local v missing=()
  for v in "$@"; do [[ -n "${!v:-}" ]] || missing+=("$v"); done
  (( ${#missing[@]} == 0 )) || die "Empty in deploy/{config,app-secrets,access}.env: ${missing[*]}"
}

# Replace ${VAR} in a template with the environment's value; an empty one is an error.
render_file() {  # $1 template, $2 output
  perl -pe 's/\$\{([A-Z0-9_]+)\}/defined $ENV{$1} && length $ENV{$1} ? $ENV{$1} : die "$ARGV: \${$1} is empty (deploy\/{config,app-secrets,access}.env)\n"/ge' \
    "$1" > "$2"
}

toml_list() {  # "a,b" -> ["a", "b"]
  local IFS=, item out=()
  for item in $1; do out+=("\"$item\""); done
  local joined; joined="$(printf '%s, ' "${out[@]}")"
  printf '[%s]' "${joined%, }"
}

# Set KEY=VALUE in a config file: replace the line, or append it.
set_var() {  # $1 file, $2 key, $3 value
  if grep -q "^$2=" "$1"; then
    K="$2" V="$3" perl -i -pe 's/^\Q$ENV{K}\E=[^#\n]*?(\s*#.*)?$/$ENV{K}=$ENV{V}$1/' "$1"
  else
    printf '%s=%s\n' "$2" "$3" >> "$1"
  fi
  export "$2=$3"
}

# "postgres:17@sha256:abc" -> name "postgres", digest "sha256:abc" (kustomize wants them apart)
export_image_parts() {  # $1 image reference, $2 variable prefix
  local ref="$1" name digest
  [[ "$ref" == *@sha256:* ]] || die "$2 must be pinned by digest (name@sha256:...), got '$ref'"
  digest="${ref#*@}"; name="${ref%@*}"; name="${name%:*}"
  export "${2}_NAME=$name" "${2}_DIGEST=$digest"
}

# --- kubectl through the kube-deploy tunnel ---------------------------
KUBECONFIG_FILE=""
tunnel() {
  [[ -n "$KUBECONFIG_FILE" ]] && return
  need DEPLOY_HOST DEPLOY_SSH_PORT KUBE_DEPLOY_SSH_KEY KUBE_CONFIG_B64
  mkdir -p "$BUILD" && chmod 700 "$BUILD"
  KUBECONFIG_FILE="$BUILD/kubeconfig"
  (umask 077; printf '%s' "$KUBE_CONFIG_B64" | base64 -d > "$KUBECONFIG_FILE")
  # Reuse a tunnel that's already up (e.g. one opened by ../obsidian/deploy/deploy.sh).
  if ! (exec 3<>/dev/tcp/127.0.0.1/6443) 2>/dev/null; then
    log "SSH tunnel to the API server (kube-deploy@${DEPLOY_HOST})"
    ssh -f -N -p "$DEPLOY_SSH_PORT" -i "$KUBE_DEPLOY_SSH_KEY" \
      -o IdentitiesOnly=yes -o BatchMode=yes -o StrictHostKeyChecking=yes \
      -o ExitOnForwardFailure=yes -o ServerAliveInterval=30 \
      -L 127.0.0.1:6443:127.0.0.1:6443 "kube-deploy@${DEPLOY_HOST}"
  fi
}
k()       { tunnel; kubectl --kubeconfig "$KUBECONFIG_FILE" -n "$NS" "$@"; }
kgarage() { tunnel; kubectl --kubeconfig "$KUBECONFIG_FILE" -n "$GARAGE_NS" "$@"; }

# --- render -----------------------------------------------------------
render_k8s() {
  need ACCOUNTS_DIGEST ACCOUNTS_HOST GOOGLE_CLIENT_IDS ALLOWED_EMAILS
  [[ "$ACCOUNTS_DIGEST" =~ ^sha256:[0-9a-f]{64}$ ]] || die "ACCOUNTS_DIGEST must be sha256:<64 hex>, got '$ACCOUNTS_DIGEST'"
  [[ "$GOOGLE_CLIENT_IDS" != *REPLACE_ME* ]] || die "Set GOOGLE_CLIENT_IDS in deploy/config.env"
  export_image_parts "$POSTGRES_IMAGE" POSTGRES_IMAGE
  export_image_parts "$RCLONE_IMAGE" RCLONE_IMAGE
  GOOGLE_CLIENT_IDS_TOML="$(toml_list "$GOOGLE_CLIENT_IDS")"; export GOOGLE_CLIENT_IDS_TOML
  ALLOWED_EMAILS_TOML="$(toml_list "$ALLOWED_EMAILS")"; export ALLOWED_EMAILS_TOML

  rm -rf "$K8S_BUILD"; mkdir -p "$K8S_BUILD"
  render_file "$DEPLOY_DIR/k8s/overlays/prod/kustomization.yaml" "$K8S_BUILD/kustomization.yaml"
  render_file "$DEPLOY_DIR/accounts.toml" "$K8S_BUILD/accounts.toml"
  kubectl kustomize "$K8S_BUILD" > /dev/null   # fails here, before touching the cluster
}

# Secret files as accounts-api, Postgres and pg-backup read them.
write_secret_files() {  # $1 dir
  need JWT_ED25519_PEM_B64 PG_PASSWORD S3_ACCESS_KEY S3_SECRET_KEY
  (umask 077; mkdir -p "$1"
   printf '%s' "$JWT_ED25519_PEM_B64" | base64 -d > "$1/jwt-ed25519.pem"
   printf '%s' "$PG_PASSWORD"   > "$1/pg-password"
   printf '%s' "$S3_ACCESS_KEY" > "$1/s3.access"
   printf '%s' "$S3_SECRET_KEY" > "$1/s3.secret")
}

# --- commands -----------------------------------------------------------

cmd_init() {
  local f openssl=openssl
  for f in "$APP_SECRETS" "$ACCESS"; do
    if [[ ! -f "$f" ]]; then
      (umask 077; cp "$f.example" "$f")
      log "Created ${f#"$REPO_DIR"/} from the example"
    fi
  done
  # macOS's LibreSSL cannot generate Ed25519 keys.
  [[ -x /opt/homebrew/opt/openssl@3/bin/openssl ]] && openssl=/opt/homebrew/opt/openssl@3/bin/openssl
  gen() {  # $1 file, $2 key, $3... command printing the value
    local file="$1" key="$2"; shift 2
    [[ -z "${!key:-}" ]] || return 0
    local value; value="$("$@")"; [[ -n "$value" ]] || die "Generating ${key} failed"
    set_var "$file" "$key" "$value"; log "Generated ${key}"
  }
  jwt_key() { "$openssl" genpkey -algorithm ed25519 | base64 | tr -d '\n'; }
  hex32()   { "$openssl" rand -hex 32; }
  gk_id()   { echo "GK$("$openssl" rand -hex 12)"; }   # Garage's access key ID format
  gen "$APP_SECRETS" JWT_ED25519_PEM_B64 jwt_key
  gen "$APP_SECRETS" PG_PASSWORD hex32
  gen "$APP_SECRETS" S3_ACCESS_KEY gk_id
  gen "$APP_SECRETS" S3_SECRET_KEY hex32
  chmod 600 "$APP_SECRETS" "$ACCESS"
  warn "Back up deploy/app-secrets.env in your password manager."
  local v todo=()
  for v in GHCR_PULL_TOKEN KUBE_CONFIG_B64; do [[ -n "${!v:-}" ]] || todo+=("$v"); done
  [[ "$GOOGLE_CLIENT_IDS" != *REPLACE_ME* ]] || todo+=("GOOGLE_CLIENT_IDS (config.env)")
  (( ${#todo[@]} == 0 )) || warn "Still to fill in by hand: ${todo[*]}"
}

cmd_secrets() {
  local tmp changed=""; tmp="$(mktemp -d)"; TMP_DIRS+=("$tmp")
  apply_secret() {  # $1 manifest file
    local out; out="$(k apply -f "$1")"; echo "$out" >&2
    if [[ "$out" =~ configured|created ]]; then changed=1; fi
  }

  if [[ -n "${GHCR_PULL_TOKEN:-}" ]]; then
    k create secret docker-registry ghcr --docker-server=ghcr.io \
      --docker-username="$GHCR_USER" --docker-password="$GHCR_PULL_TOKEN" --dry-run=client -o yaml > "$tmp/ghcr.yaml"
    apply_secret "$tmp/ghcr.yaml"
  else
    warn "GHCR_PULL_TOKEN empty: skipping Secret ghcr (the private image won't pull)"
  fi

  write_secret_files "$tmp/accounts"
  k create secret generic accounts-secrets --from-file="$tmp/accounts" --dry-run=client -o yaml > "$tmp/accounts.yaml"
  apply_secret "$tmp/accounts.yaml"

  # Secrets are read at startup only; restart what already runs. (Postgres
  # reads POSTGRES_PASSWORD_FILE only when it creates the database: to rotate
  # the password, ALTER ROLE first, then run this.)
  if [[ -n "$changed" ]] && k get deploy/accounts-api >/dev/null 2>&1; then
    k rollout restart deploy/accounts-api
  fi
}

resolve_digest() {
  [[ -n "${ACCOUNTS_DIGEST:-}" ]] && return
  local img
  img="$(k get deploy/accounts-api -o jsonpath='{.spec.template.spec.containers[0].image}' 2>/dev/null || true)"
  if [[ "$img" == *@sha256:* ]]; then
    ACCOUNTS_DIGEST="${img#*@}"; log "Keeping the running image ${ACCOUNTS_DIGEST}"
  else
    local pkg="${ACCOUNTS_IMAGE#ghcr.io/*/}"   # accounts-app/accounts-api
    ACCOUNTS_DIGEST="$(gh api "/users/${GHCR_USER}/packages/container/${pkg//\//%2F}/versions" \
      --jq '.[0].name' 2>/dev/null || true)"
    [[ -n "$ACCOUNTS_DIGEST" ]] || die "No image digest: set ACCOUNTS_DIGEST=sha256:... (from the CI run) or push to main first"
    log "Using the newest GHCR image ${ACCOUNTS_DIGEST}"
  fi
  export ACCOUNTS_DIGEST
}

cmd_apply() {
  tunnel
  k auth can-i patch deployments.apps >/dev/null \
    || die "kube-deploy can't manage ${NS}: add it to APP_NAMESPACES in vps-setup and run ./run-remote.sh 06"
  resolve_digest
  render_k8s
  log "Applying ${K8S_BUILD}"
  k apply -k "$K8S_BUILD"
  if ! k get secret accounts-secrets >/dev/null 2>&1; then
    warn "Secret accounts-secrets missing: pods wait for it (deploy.sh secrets)"
    return
  fi
  k rollout status sts/postgres --timeout=5m
  k rollout status deploy/accounts-api --timeout=5m
}

cmd_garage_init() {
  need ACCOUNTS_BUCKET
  # The garage CLI logs its RPC connection on stderr: drop those lines, keep real errors.
  g() { kgarage exec garage-0 -- /garage "$@" 2> >(grep -v 'garage_net::' >&2); }
  g bucket create "$ACCOUNTS_BUCKET" >/dev/null 2>&1 || true   # already exists: fine
  # The key comes from app-secrets.env (`init` generated it); Garage only imports
  # it. CI has no secret files, so it skips this: the first manual run imported it.
  if [[ -n "${S3_ACCESS_KEY:-}" && -n "${S3_SECRET_KEY:-}" ]]; then
    if ! g key info "$S3_ACCESS_KEY" >/dev/null 2>&1; then
      g key import --yes -n accounts-api "$S3_ACCESS_KEY" "$S3_SECRET_KEY" >/dev/null
      log "Imported Garage key accounts-api ($S3_ACCESS_KEY)"
    fi
    g bucket allow --read --write --owner "$ACCOUNTS_BUCKET" --key "$S3_ACCESS_KEY" >/dev/null
    log "Bucket ${ACCOUNTS_BUCKET} ready in ${GARAGE_NS}"
  fi
}

cmd_gh_secrets() {
  need KUBE_CONFIG_B64 KUBE_DEPLOY_SSH_KEY DEPLOY_HOST DEPLOY_SSH_PORT
  cd "$REPO_DIR"
  gh api -X PUT 'repos/{owner}/{repo}/environments/production' > /dev/null
  printf '%s' "$KUBE_CONFIG_B64" | gh secret set KUBE_CONFIG --env production
  gh secret set SSH_PRIVATE_KEY --env production < "$KUBE_DEPLOY_SSH_KEY"
  # Prefer the host key you already trust over a fresh (unauthenticated) scan.
  local known
  known="$(ssh-keygen -F "[${DEPLOY_HOST}]:${DEPLOY_SSH_PORT}" 2>/dev/null | grep -v '^#' || true)"
  if [[ -z "$known" ]]; then
    warn "No known_hosts entry for [${DEPLOY_HOST}]:${DEPLOY_SSH_PORT}: using ssh-keyscan (verify the fingerprint!)"
    known="$(ssh-keyscan -p "$DEPLOY_SSH_PORT" "$DEPLOY_HOST" 2>/dev/null)"
    ssh-keygen -lf - <<<"$known" >&2
  fi
  [[ -n "$known" ]] || die "Couldn't get the host key of ${DEPLOY_HOST}"
  printf '%s\n' "$known" | gh secret set SSH_KNOWN_HOSTS --env production
  log "GitHub environment 'production' has KUBE_CONFIG, SSH_PRIVATE_KEY, SSH_KNOWN_HOSTS"
}

cmd_backup_now() {
  local job="pg-backup-manual-$(date -u +%Y%m%d%H%M%S)"
  k create job "$job" --from=cronjob/pg-backup
  k wait --for=condition=complete "job/$job" --timeout=10m
  log "Backup uploaded to ${ACCOUNTS_BUCKET}/backups in Garage"
}

CMD="${1:-}"; shift || true
case "$CMD" in
  init)        cmd_init ;;
  secrets)     cmd_secrets ;;
  apply)       cmd_apply ;;
  garage-init) cmd_garage_init ;;
  kubectl)     k "$@" ;;
  render)      resolve_digest; render_k8s; log "Rendered to ${K8S_BUILD}" ;;
  gh-secrets)  cmd_gh_secrets ;;
  db-shell)    k exec -it sts/postgres -- psql -U accounts accounts ;;
  backup-now)  cmd_backup_now ;;
  *) sed -n '2,25p' "$0" | sed 's/^# \{0,1\}//'; exit 1 ;;
esac
