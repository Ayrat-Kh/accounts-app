// The C++ core (../../core) compiled for Android, plus thin Kotlin bindings.
// Its dependencies (Tesseract, llama.cpp, curl, OpenSSL, SQLite) are built by
// vcpkg for the Android triplets during the CMake configure step, which needs
// ANDROID_NDK_HOME in the environment (see android/README.md).
plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
    alias(libs.plugins.kotlin.serialization)
}

val repositoryRoot = rootProject.projectDir.parentFile

android {
    namespace = "com.airatkhisamiev.accounts.core"
    compileSdk = 36
    ndkVersion = libs.versions.ndk.get()

    defaultConfig {
        minSdk = 28
        consumerProguardFiles("consumer-rules.pro")
        ndk {
            abiFilters += listOf("arm64-v8a", "x86_64") // phones, and the emulator on x86 hosts
        }
        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DCMAKE_TOOLCHAIN_FILE=$repositoryRoot/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake",
                    "-DRECEIPTS_REPOSITORY_ROOT=$repositoryRoot"
                )
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.31.6"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
}

kotlin {
    compilerOptions {
        jvmTarget.set(org.jetbrains.kotlin.gradle.dsl.JvmTarget.JVM_17)
    }
}

dependencies {
    implementation(libs.kotlinx.coroutines.android)
    implementation(libs.kotlinx.serialization.json)
    testImplementation(libs.junit)
}
