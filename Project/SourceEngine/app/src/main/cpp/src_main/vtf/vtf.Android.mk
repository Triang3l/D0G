LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := vtf

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1

LOCAL_SRC_FILES := \
  convert_x360.cpp \
  s3tc_decode.cpp \
  vtf.cpp

include $(BUILD_STATIC_LIBRARY)