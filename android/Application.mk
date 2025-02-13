# all, armeabi-v7a, arm64-v8a, x86, x86_64
APP_ABI := armeabi-v7a arm64-v8a
APP_PLATFORM := android-22
APP_BUILD_SCRIPT := android/Android.mk

# C++ Runtime (c++_shared, c++_static, none, or system) (default for ndk-build is none)
APP_STL := c++_static

# This optional variable sets compiler flags for the build system to pass when building C and C++ source files.
APP_CFLAGS := -Wall -Wextra -Wshadow

# Flags to be passed for all C compiles in the project. These flags will not be used for C++ code.
APP_CONLYFLAGS := -std=c17

# An optional set of compiler flags that will be passed when building C++ source files only.
# They will appear after the LOCAL_CFLAGS on the compiler's command-line.
APP_CPPFLAGS := -std=c++20

# how to build
# export PATH=$PATH:~/Android/Sdk/platform-tools/
# export PATH=$PATH:~/Downloads/android-ndk-r26b/
# export NDK_PROJECT_PATH=android/
# ndk-build NDK_APPLICATION_MK=android/Application.mk clean
# ndk-build NDK_APPLICATION_MK=android/Application.mk

# APP_PLATFORM := android-34
# was 34 but changed to 22 as 34 caused app launch error on Fire TV 4K Max 1st-Gen
# https://github.com/android/ndk/issues/1433

# reference
# https://developer.android.com/ndk/guides/android_mk
# https://developer.android.com/ndk/guides/application_mk
# https://developer.android.com/ndk/guides/cpp-support
# https://developer.android.com/ndk/guides/sdk-versions