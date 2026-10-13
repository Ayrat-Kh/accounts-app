# Runtime image: distroless (no shell, no package manager), non-root, holding
# only the release binary. The binary is built in the dev container (CI's
# image job), where vcpkg's binary cache is restored, and the build context
# is a directory containing just that binary.
FROM gcr.io/distroless/cc-debian13@sha256:e792ab3d241a468a4fd7519ddbbebe66b49b5f365771716ea688ad40b6c6f1c2
COPY accounts-api /usr/local/bin/accounts-api
USER 65532:65532
EXPOSE 8080
ENTRYPOINT ["/usr/local/bin/accounts-api", "--config", "/etc/accounts/accounts.toml"]
