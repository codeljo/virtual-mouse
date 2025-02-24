# Virtual Mouse for Linux and Android
### Use your keyboard or remote as a simple mouse.

<br>

![C/C++ CI](https://github.com/codeljo/virtual-mouse/actions/workflows/cpp-make.yml/badge.svg)

<br>

## Usage
Function     | Key / Button
-------------|------------------------------
Activate     | `PLAY` (quickly double tap)
Deactivate   | `ESC` or `HOME`
Mouse Move   | `UP`, `DOWN`, `LEFT`, `RIGHT`
Mouse Click  | `ENTER` or `SELECT`
Mouse Scroll | `REWIND`, `FAST-FORWARD`

<br>

## Running the app
**Linux**
```shell
sudo build/virtual-mouse
```
> Note: Depending on your systems /dev/input/ permissions you may need to run the app via `sudo`.

**Android**
```shell
adb connect ANDROID_IP_HERE
adb push android/libs/armeabi-v7a/virtual-mouse /data/local/tmp/

adb shell
chmod 755 /data/local/tmp/virtual-mouse
/data/local/tmp/virtual-mouse
```

<br>

## Build for Linux
```shell
make
```
> The compiled binary will be output to build/virtual-mouse

<br>

## Build for Android
```shell
export PATH=$PATH:/path/to/Android/Sdk/platform-tools/
export PATH=$PATH:/path/to/android-ndk/
export NDK_PROJECT_PATH=android/
ndk-build NDK_APPLICATION_MK=android/Application.mk
```
> The compiled binary will be output to android/libs/virtual-mouse
>
> Note: The Android build is compatible with Android 10 and lower.
