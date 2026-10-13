# Native code (KotlinPorts.cpp, JniSupport.cpp) finds these classes and
# methods by name, so R8 must not rename or remove them.
-keep class com.airatkhisamiev.accounts.core.CoreException { <init>(java.lang.String, java.lang.String); }
-keep interface com.airatkhisamiev.accounts.core.SecureStore { *; }
-keep interface com.airatkhisamiev.accounts.core.CoreLogger { *; }
-keep interface com.airatkhisamiev.accounts.core.DownloadProgress { *; }
-keep class * implements com.airatkhisamiev.accounts.core.SecureStore { *; }
-keep class * implements com.airatkhisamiev.accounts.core.CoreLogger { *; }
-keep class * implements com.airatkhisamiev.accounts.core.DownloadProgress { *; }
-keepclasseswithmembernames class * { native <methods>; }
