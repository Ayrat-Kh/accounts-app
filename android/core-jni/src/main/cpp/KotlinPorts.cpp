#include "KotlinPorts.hpp"

namespace receipts::jni {

namespace {

jmethodID method(JNIEnv *env, jobject object, const char *name, const char *signature) {
    jclass objectClass = env->GetObjectClass(object);
    jmethodID id = env->GetMethodID(objectClass, name, signature);
    env->DeleteLocalRef(objectClass);
    return id;
}

// Local references pile up when a native method runs a long time and calls
// back into Kotlin many times; each helper releases what it creates.
struct LocalString {
    LocalString(JNIEnv *env, const std::string &text) : env(env), value(toJString(env, text)) {}
    ~LocalString() { env->DeleteLocalRef(value); }
    JNIEnv *env;
    jstring value;
};

} // namespace

KotlinSecureStore::KotlinSecureStore(JNIEnv *env, jobject store)
    : store_(env, store), read_(method(env, store, "read", "(Ljava/lang/String;)Ljava/lang/String;")),
      write_(method(env, store, "write", "(Ljava/lang/String;Ljava/lang/String;)V")),
      remove_(method(env, store, "remove", "(Ljava/lang/String;)V")) {}

std::optional<std::string> KotlinSecureStore::read(const std::string &key) {
    JNIEnv *env = currentEnv();
    LocalString jKey(env, key);
    auto value = static_cast<jstring>(env->CallObjectMethod(store_.get(), read_, jKey.value));
    if (value == nullptr) {
        return std::nullopt;
    }
    std::string result = toStdString(env, value);
    env->DeleteLocalRef(value);
    return result;
}

void KotlinSecureStore::write(const std::string &key, const std::string &value) {
    JNIEnv *env = currentEnv();
    LocalString jKey(env, key);
    LocalString jValue(env, value);
    env->CallVoidMethod(store_.get(), write_, jKey.value, jValue.value);
}

void KotlinSecureStore::remove(const std::string &key) {
    JNIEnv *env = currentEnv();
    LocalString jKey(env, key);
    env->CallVoidMethod(store_.get(), remove_, jKey.value);
}

KotlinLogger::KotlinLogger(JNIEnv *env, jobject logger)
    : logger_(env, logger), log_(method(env, logger, "log", "(ILjava/lang/String;)V")) {}

void KotlinLogger::log(LogLevel level, std::string_view message) {
    JNIEnv *env = currentEnv();
    LocalString jMessage(env, std::string(message));
    env->CallVoidMethod(logger_.get(), log_, static_cast<jint>(level), jMessage.value);
}

KotlinProgressListener::KotlinProgressListener(JNIEnv *env, jobject listener)
    : env_(env), listener_(listener), onProgress_(method(env, listener, "onProgress", "(JJ)V")) {}

void KotlinProgressListener::onProgress(std::uint64_t bytesDone, std::uint64_t bytesTotal) {
    env_->CallVoidMethod(listener_, onProgress_, static_cast<jlong>(bytesDone), static_cast<jlong>(bytesTotal));
}

} // namespace receipts::jni
