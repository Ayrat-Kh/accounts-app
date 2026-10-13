# Deploying accounts-api

This uses the same k3s VPS, tooling and conventions as `../obsidian/deploy`, in its own namespace **`app-accounts`**:

| Component | Kind | Notes |
|---|---|---|
| `accounts-api` | Deployment, 2 replicas | Stateless. A PodDisruptionBudget keeps one replica up. Migrations take an advisory lock. |
| `postgres` | StatefulSet, 1 replica | PostgreSQL 17 on a 10 Gi PVC. Only `accounts-api` and `pg-backup` can reach it. |
| `pg-backup` | CronJob, 02:15 UTC | Runs `pg_dump`, then rclone copies the dump to Garage `accounts/backups/`. Dumps are kept for 30 days. |
| Garage | *in `app-obsync`* | obsync's Garage, with our own bucket (`accounts`) and key (`accounts-api`). |
| `accounts-api` / `accounts-api-auth` | Ingress (Traefik) | Wildcard `*.fernfall.cloud` certificate. `/auth` is rate limited. |
| NetworkPolicies | | Default deny. API ← Traefik. API → Postgres, Garage, Google (443). |

## Where the settings live

| File | Holds | Goes to | In git? |
|---|---|---|---|
| `config.env` | Non-secret settings: host, image, allowed e-mails, Google client IDs, sizes | Rendered manifests (on your Mac and in CI) | Committed |
| `app-secrets.env` | JWT key, Postgres password, S3 key, GHCR pull token | **The cluster only**, applied by `deploy.sh secrets` | Gitignored, `0600` |
| `access.env` | `KUBE_CONFIG_B64` | GitHub (`deploy.sh gh-secrets`) and the SSH tunnel. Never the cluster | Gitignored, `0600` |

```
deploy/
├── config.env, app-secrets.env, access.env
├── deploy.sh               every task (run it without arguments for help)
├── accounts.toml           server config template (${VAR} from config.env)
└── k8s/
    ├── base/               generic manifests
    └── overlays/prod/      kustomization TEMPLATE, rendered into k8s/.build/prod (gitignored)
```

## First deployment

Prerequisites on your Mac: `kubectl`, `gh`, `perl`, and `brew install openssl@3`.

1. **Namespace (admin).** In vps-setup, add `app-accounts` to `APP_NAMESPACES` and run `./run-remote.sh 06`. This
   creates the namespace with Pod Security "restricted" and binds `kube-deploy`.
2. **Garage access (obsidian repo).** Merge the `allow-accounts-app-garage` branch of `../obsidian`, which adds one
   NetworkPolicy rule, and deploy obsync once so the rule is live.
3. **DNS.** Point `accounts.fernfall.cloud` at the node, or rely on the `*` record.
4. **Google.** In Google Auth Platform, create an *Android* OAuth client (package `com.airatkhisamiev.accounts`,
   with your signing SHA-1) and a *Web* client. The app requests ID tokens for the Web client. Put both client IDs in
   `GOOGLE_CLIENT_IDS` in `config.env`.
5. Generate the secrets, then fill in `GHCR_PULL_TOKEN` and `KUBE_CONFIG_B64` by hand:
   ```bash
   deploy/deploy.sh init
   ```
6. Create the bucket and key, apply the secrets, upload the CI credentials, and deploy:
   ```bash
   deploy/deploy.sh garage-init
   ```
   ```bash
   deploy/deploy.sh secrets
   ```
   ```bash
   deploy/deploy.sh gh-secrets
   ```
   ```bash
   deploy/deploy.sh apply
   ```
7. Publish the scanner models (Tesseract eng/deu/rus and EmbeddingGemma 2, about 350 MB) that phones download through
   `/v1/assets`:
   ```bash
   scripts/download-models.sh build/models
   ```
   ```bash
   deploy/deploy.sh publish-models build/models
   ```

After that, every push to `main` that touches `accounts-api/` or `deploy/` is built, scanned, signed and deployed by
`.github/workflows/backend.yml`. To deploy `main` right away, run `scripts/deploy-now.sh`.

## Day to day

```bash
deploy/deploy.sh kubectl get pods
```
```bash
deploy/deploy.sh kubectl logs deploy/accounts-api
```
```bash
deploy/deploy.sh db-shell
```
```bash
deploy/deploy.sh backup-now
```

**Restoring a backup:** download a dump from `accounts/backups/` (with rclone or `garage`), then run
`pg_restore --clean --dbname=accounts` inside the postgres pod.
