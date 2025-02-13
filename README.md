# Virtual Mouse for Linux and Android
### Use your keyboard or remote as a simple mouse.

<br>

## Build for Linux
```shell
make
```
> The compiled binary will be output to build/main-linux

## Build for Android
```shell
export PATH=$PATH:/path/to/Android/Sdk/platform-tools/
export PATH=$PATH:/path/to/android-ndk/
export NDK_PROJECT_PATH=android/
ndk-build NDK_APPLICATION_MK=android/Application.mk
```
> The compiled binary will be output to android/libs/main-android
>
> Note: The Android build is compatible with Android 10 and lower.
