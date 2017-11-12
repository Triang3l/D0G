LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := vstdlib

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1
LOCAL_CFLAGS := -DVSTDLIB_DLL_EXPORT

LOCAL_SRC_FILES := \
  cvar.cpp \
  jobthread.cpp \
  KeyValuesSystem.cpp \
  random.cpp \
  vcover.cpp \
  ../public/tier0/memoverride.cpp

LOCAL_SHARED_LIBRARIES := tier0
LOCAL_STATIC_LIBRARIES := tier1

include $(BUILD_SHARED_LIBRARY)