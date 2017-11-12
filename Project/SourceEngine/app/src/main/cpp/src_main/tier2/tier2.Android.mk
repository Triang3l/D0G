LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := tier2

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1 \
  $(LOCAL_PATH)/../public/tier2

LOCAL_SRC_FILES := \
  beamsegdraw.cpp \
  defaultfilesystem.cpp \
  dmconnect.cpp \
  fileutils.cpp \
  keybindings.cpp \
  meshutils.cpp \
  p4helpers.cpp \
  renderutils.cpp \
  riff.cpp \
  soundutils.cpp \
  tier2.cpp \
  util_init.cpp \
  utlstreambuffer.cpp \
  vconfig.cpp \
  ../public/map_utils.cpp \
  ../public/materialsystem/MaterialSystemUtil.cpp

include $(BUILD_STATIC_LIBRARY)