// Native methods of com.airatkhisamiev.accounts.core.NativeCore.
//
// Each method unwraps the handle, calls ReceiptsService, and hands the result
// back: values as JSON strings, failures as a thrown CoreException. Nothing
// else happens here; all logic stays in ../../../../../core.

#include "JniSupport.hpp"
#include "JsonMapping.hpp"
#include "KotlinPorts.hpp"
#include "receipts/ReceiptsService.hpp"

using receipts::ReceiptsService;
using receipts::Result;
using namespace receipts::jni;
using nlohmann::json;

namespace {

ReceiptsService &service(jlong handle) {
    return *reinterpret_cast<ReceiptsService *>(handle);
}

// Result<T> -> JSON string, or a thrown CoreException (and null).
template <class T, class ToJson>
jstring jsonOrThrow(JNIEnv *env, const Result<T> &result, ToJson toJsonValue) {
    if (!result) {
        throwCoreException(env, result.error());
        return nullptr;
    }
    return toJString(env, toJsonValue(*result).dump());
}

void throwIfFailed(JNIEnv *env, const Result<void> &result) {
    if (!result) {
        throwCoreException(env, result.error());
    }
}

// Malformed JSON from Kotlin is a programming error; report it as such
// instead of letting a C++ exception cross into the JVM (which would abort).
template <class Function>
auto guarded(JNIEnv *env, Function function) -> decltype(function()) {
    try {
        return function();
    } catch (const std::exception &error) {
        throwCoreException(env, {receipts::ErrorCode::Internal, error.what()});
        return {};
    }
}

} // namespace

extern "C" {

JNIEXPORT jlong JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_create(JNIEnv *env, jobject,
                                                                                jstring configJson, jobject secureStore,
                                                                                jobject logger) {
    return guarded(env, [&]() -> jlong {
        auto created = ReceiptsService::create(configFromJson(json::parse(toStdString(env, configJson))),
                                               std::make_shared<KotlinSecureStore>(env, secureStore),
                                               std::make_shared<KotlinLogger>(env, logger));
        if (!created) {
            throwCoreException(env, created.error());
            return 0;
        }
        return reinterpret_cast<jlong>(created->release());
    });
}

JNIEXPORT void JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_destroy(JNIEnv *, jobject, jlong handle) {
    delete reinterpret_cast<ReceiptsService *>(handle);
}

JNIEXPORT void JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_signInWithGoogle(JNIEnv *env, jobject,
                                                                                         jlong handle,
                                                                                         jstring idToken) {
    throwIfFailed(env, service(handle).signInWithGoogle(toStdString(env, idToken)));
}

JNIEXPORT void JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_signOut(JNIEnv *, jobject, jlong handle) {
    service(handle).signOut();
}

JNIEXPORT jboolean JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_isSignedIn(JNIEnv *, jobject,
                                                                                       jlong handle) {
    return service(handle).isSignedIn();
}

JNIEXPORT jboolean JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_modelsReady(JNIEnv *, jobject,
                                                                                        jlong handle) {
    return service(handle).modelsReady();
}

JNIEXPORT void JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_downloadModels(JNIEnv *env, jobject,
                                                                                       jlong handle, jobject progress) {
    KotlinProgressListener listener(env, progress);
    throwIfFailed(env, service(handle).downloadModels(&listener));
}

JNIEXPORT jstring JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_scanReceipt(JNIEnv *env, jobject,
                                                                                       jlong handle,
                                                                                       jstring imagePath) {
    return jsonOrThrow(env, service(handle).scanReceipt(toStdString(env, imagePath)),
                       [](const auto &scan) { return toJson(scan); });
}

JNIEXPORT jstring JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_saveReceipt(JNIEnv *env, jobject,
                                                                                       jlong handle,
                                                                                       jstring draftJson) {
    return guarded(env, [&]() -> jstring {
        const auto draft = draftFromJson(json::parse(toStdString(env, draftJson)));
        return jsonOrThrow(env, service(handle).saveReceipt(draft),
                           [](const auto &receipt) { return toJson(receipt); });
    });
}

JNIEXPORT jstring JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_listReceipts(JNIEnv *env, jobject,
                                                                                        jlong handle) {
    return jsonOrThrow(env, service(handle).listReceipts(), [](const auto &receipts) {
        json array = json::array();
        for (const auto &receipt : receipts) {
            array.push_back(toJson(receipt));
        }
        return array;
    });
}

JNIEXPORT void JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_deleteReceipt(JNIEnv *env, jobject,
                                                                                      jlong handle, jstring localId) {
    throwIfFailed(env, service(handle).deleteReceipt(toStdString(env, localId)));
}

JNIEXPORT jstring JNICALL Java_com_airatkhisamiev_accounts_core_NativeCore_sync(JNIEnv *env, jobject, jlong handle) {
    return jsonOrThrow(env, service(handle).sync(), [](const auto &report) { return toJson(report); });
}

} // extern "C"
