JNI_PATH := $(call my-dir)

LOCAL_PATH := $(JNI_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := easysodbg
LOCAL_C_INCLUDES += $(JNI_PATH)

ifeq ($(TARGET_CPU_SMP),true)
    targetSmpFlag := -DANDROID_SMP=1
else
    targetSmpFlag := -DANDROID_SMP=0
endif

LOCAL_SRC_FILES := main.cpp

LOCAL_LDLIBS := arch/$(TARGET_ARCH_ABI)/libEsodbg.so
LOCAL_CFLAGS := -fpermissive $(targetSmpFlag)
LOCAL_CPP_FEATURES += exceptions
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE


include $(BUILD_EXECUTABLE)
