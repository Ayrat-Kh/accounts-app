#include "config/Config.hpp"

#include "util/Files.hpp"

#include <toml++/toml.hpp>

#include <stdexcept>

namespace accounts {

namespace {

// Small helpers that turn "key missing or wrong type" into one clear error,
// e.g. "config: [auth] jwt_private_key_file is required".
class Section {
public:
    Section(const toml::table &root, std::string name) : name_(std::move(name)) {
        if (const auto *table = root[name_].as_table()) {
            table_ = table;
        }
    }

    template <class T>
    T required(std::string_view key) const {
        if (auto value = lookup<T>(key)) {
            return *value;
        }
        throw std::runtime_error("config: [" + name_ + "] " + std::string(key) + " is required");
    }

    template <class T>
    T optional(std::string_view key, T fallback) const {
        return lookup<T>(key).value_or(std::move(fallback));
    }

    std::vector<std::string> requiredStringList(std::string_view key) const {
        const auto *array = table_ ? (*table_)[key].as_array() : nullptr;
        if (!array || array->empty()) {
            throw std::runtime_error("config: [" + name_ + "] " + std::string(key) + " must be a non-empty list");
        }
        std::vector<std::string> values;
        for (const auto &element : *array) {
            values.push_back(element.value<std::string>().value());
        }
        return values;
    }

    // `<key>` holds a path; the secret is the file's content.
    std::string requiredSecretFile(std::string_view key) const {
        return readSecretFile(required<std::string>(key));
    }

private:
    template <class T>
    std::optional<T> lookup(std::string_view key) const {
        if (!table_) {
            return std::nullopt;
        }
        return (*table_)[key].value<T>();
    }

    std::string name_;
    const toml::table *table_ = nullptr;
};

} // namespace

Config loadConfig(const std::string &path) {
    const toml::table root = toml::parse_file(path);
    Config config;

    const Section server(root, "server");
    config.server.bindHost = server.optional<std::string>("bind_host", config.server.bindHost);
    config.server.port = static_cast<std::uint16_t>(server.optional<std::int64_t>("port", config.server.port));
    config.server.maxJsonBodyBytes = server.optional<std::int64_t>("max_json_body_kb", 16) * 1024;
    config.server.maxImageBodyBytes = server.optional<std::int64_t>("max_image_mb", 10) * 1024 * 1024;

    const Section database(root, "database");
    // The password lives in a Secret; everything else is plain config.
    config.database.connectionString = database.required<std::string>("connection") +
                                       " password=" + database.requiredSecretFile("password_file");
    config.database.poolSize = database.optional<std::int64_t>("pool_size", 8);

    const Section auth(root, "auth");
    config.auth.googleClientIds = auth.requiredStringList("google_client_ids");
    config.auth.allowedEmails = auth.requiredStringList("allowed_emails");
    config.auth.jwtPrivateKeyPem = readFile(auth.required<std::string>("jwt_private_key_file"));
    config.auth.jwtIssuer = auth.optional<std::string>("jwt_issuer", config.auth.jwtIssuer);
    config.auth.accessTokenTtl = std::chrono::seconds(auth.optional<std::int64_t>("access_token_ttl_seconds", 900));
    config.auth.refreshTokenTtl = std::chrono::days(auth.optional<std::int64_t>("refresh_token_ttl_days", 30));
    config.auth.refreshTokenAbsoluteTtl = std::chrono::days(auth.optional<std::int64_t>("refresh_absolute_days", 90));

    const Section s3(root, "s3");
    config.s3.endpoint = s3.required<std::string>("endpoint");
    config.s3.region = s3.optional<std::string>("region", config.s3.region);
    config.s3.bucket = s3.required<std::string>("bucket");
    config.s3.accessKey = s3.requiredSecretFile("access_key_file");
    config.s3.secretKey = s3.requiredSecretFile("secret_key_file");

    return config;
}

} // namespace accounts
