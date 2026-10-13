#pragma once

// Runtime configuration, loaded once at startup from a TOML file (see
// accounts.example.toml). Secrets are never inline: the TOML names files
// (Kubernetes Secret mounts) and loadConfig() reads them.

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace accounts {

struct ServerConfig {
    std::string bindHost = "0.0.0.0";
    std::uint16_t port = 8080;
    std::size_t maxJsonBodyBytes = 16UZ * 1024;
    std::size_t maxImageBodyBytes = 10UZ * 1024 * 1024;
};

struct DatabaseConfig {
    std::string connectionString; // libpq "key=value" string, password included
    std::size_t poolSize = 8;
};

struct AuthConfig {
    std::vector<std::string> googleClientIds; // `aud` values we accept (Android + Web client ids)
    std::vector<std::string> allowedEmails;
    std::string jwtPrivateKeyPem; // Ed25519 private key
    std::string jwtIssuer = "accounts-api";
    std::chrono::seconds accessTokenTtl{15 * 60};
    std::chrono::days refreshTokenTtl{30};
    std::chrono::days refreshTokenAbsoluteTtl{90};
};

struct S3Config {
    std::string endpoint; // e.g. "http://garage.app-obsync.svc.cluster.local:3900"
    std::string region = "garage";
    std::string bucket;
    std::string accessKey;
    std::string secretKey;
};

struct Config {
    ServerConfig server;
    DatabaseConfig database;
    AuthConfig auth;
    S3Config s3;
};

// Throws std::runtime_error with a readable message on any missing or invalid value.
Config loadConfig(const std::string &path);

} // namespace accounts
