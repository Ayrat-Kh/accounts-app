#pragma once

// "Ports": the few things the core needs from the operating system. Each host
// implements them in its own language: Kotlin on Android (Keystore-backed
// storage, Logcat), Swift on macOS (Keychain, os_log), plain C++ in the CLI.
// The core never includes a platform header.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace receipts {

// Small secrets (tokens, API keys), kept in the platform's protected storage.
class SecureStore {
public:
    virtual ~SecureStore() = default;
    virtual std::optional<std::string> read(const std::string &key) = 0;
    virtual void write(const std::string &key, const std::string &value) = 0;
    virtual void remove(const std::string &key) = 0;
};

enum class LogLevel { Debug, Info, Warning, Error };

class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(LogLevel level, std::string_view message) = 0;
};

// Called while model files download. May be called from the downloading thread.
class ProgressListener {
public:
    virtual ~ProgressListener() = default;
    virtual void onProgress(std::uint64_t bytesDone, std::uint64_t bytesTotal) = 0;
};

} // namespace receipts
