#include "JniSupport.hpp"

namespace receipts::jni {

namespace {

JavaVM *storedVm = nullptr;

const char *codeName(ErrorCode code) {
    switch (code) {
    case ErrorCode::InvalidInput: return "INVALID_INPUT";
    case ErrorCode::NotSignedIn: return "NOT_SIGNED_IN";
    case ErrorCode::Network: return "NETWORK";
    case ErrorCode::Server: return "SERVER";
    case ErrorCode::NotFound: return "NOT_FOUND";
    case ErrorCode::ModelMissing: return "MODEL_MISSING";
    case ErrorCode::Internal: return "INTERNAL";
    }
    return "INTERNAL";
}

} // namespace

JavaVM *javaVm() {
    return storedVm;
}

JNIEnv *currentEnv() {
    JNIEnv *env = nullptr;
    if (storedVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_EDETACHED) {
        storedVm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}

std::string toStdString(JNIEnv *env, jstring text) {
    if (text == nullptr) {
        return {};
    }
    const char *utf8 = env->GetStringUTFChars(text, nullptr);
    std::string result(utf8);
    env->ReleaseStringUTFChars(text, utf8);
    return result;
}

jstring toJString(JNIEnv *env, const std::string &text) {
    return env->NewStringUTF(text.c_str());
}

void throwCoreException(JNIEnv *env, const Error &error) {
    jclass exceptionClass = env->FindClass("com/airatkhisamiev/accounts/core/CoreException");
    jmethodID constructor = env->GetMethodID(exceptionClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    auto exception = static_cast<jthrowable>(env->NewObject(
        exceptionClass, constructor, toJString(env, codeName(error.code)), toJString(env, error.message)));
    env->Throw(exception);
}

} // namespace receipts::jni

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    receipts::jni::storedVm = vm;
    return JNI_VERSION_1_6;
}
