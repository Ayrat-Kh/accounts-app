#pragma once

// The core's ports (receipts/Ports.hpp) implemented by calling the Kotlin
// interfaces in com.airatkhisamiev.accounts.core: SecureStore, CoreLogger,
// DownloadProgress. Each holds a global reference to its Kotlin object.

#include "JniSupport.hpp"
#include "receipts/Ports.hpp"

namespace receipts::jni {

class KotlinSecureStore : public SecureStore {
public:
    KotlinSecureStore(JNIEnv *env, jobject store);

    std::optional<std::string> read(const std::string &key) override;
    void write(const std::string &key, const std::string &value) override;
    void remove(const std::string &key) override;

private:
    GlobalRef store_;
    jmethodID read_;
    jmethodID write_;
    jmethodID remove_;
};

class KotlinLogger : public Logger {
public:
    KotlinLogger(JNIEnv *env, jobject logger);
    void log(LogLevel level, std::string_view message) override;

private:
    GlobalRef logger_;
    jmethodID log_;
};

// Lives for one downloadModels() call only, so a local reference is enough.
class KotlinProgressListener : public ProgressListener {
public:
    KotlinProgressListener(JNIEnv *env, jobject listener);
    void onProgress(std::uint64_t bytesDone, std::uint64_t bytesTotal) override;

private:
    JNIEnv *env_;
    jobject listener_;
    jmethodID onProgress_;
};

} // namespace receipts::jni
