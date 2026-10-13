#pragma once

// The handful of JNI chores every native method needs: string conversion,
// turning a core Error into a Kotlin CoreException, and global references
// that are released automatically.

#include "receipts/Result.hpp"

#include <jni.h>

#include <string>

namespace receipts::jni {

// The process's Java VM, captured in JNI_OnLoad.
JavaVM *javaVm();

// The JNIEnv of the current thread. Core calls arrive on threads Kotlin
// started, which are already attached to the VM.
JNIEnv *currentEnv();

std::string toStdString(JNIEnv *env, jstring text);
jstring toJString(JNIEnv *env, const std::string &text);

// Throws com.airatkhisamiev.accounts.core.CoreException(code, message) in
// Kotlin when the native method returns.
void throwCoreException(JNIEnv *env, const Error &error);

// Owns a JNI global reference, so a Kotlin object can be kept beyond one call.
class GlobalRef {
public:
    GlobalRef(JNIEnv *env, jobject object) : object_(env->NewGlobalRef(object)) {}
    ~GlobalRef() { currentEnv()->DeleteGlobalRef(object_); }
    GlobalRef(const GlobalRef &) = delete;
    GlobalRef &operator=(const GlobalRef &) = delete;

    jobject get() const { return object_; }

private:
    jobject object_;
};

} // namespace receipts::jni
