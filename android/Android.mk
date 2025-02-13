LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := main-android
LOCAL_SRC_FILES := ../src/signals.c ../src/main.cpp ../src/Devices.cpp ../src/EventDesc.cpp ../src/MouseHandler.cpp ../src/PairHolder.cpp ../src/RemoteHandler.cpp
include $(BUILD_EXECUTABLE)
