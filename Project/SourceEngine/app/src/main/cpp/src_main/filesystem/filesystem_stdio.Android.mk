LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := filesystem_stdio

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1
LOCAL_CFLAGS := -DFILESYSTEM_STDIO_EXPORTS -DDONT_PROTECT_FILEIO_FUNCTIONS -DPROTECTED_THINGS_ENABLE

LOCAL_SRC_FILES := \
  basefilesystem.cpp \
  filesystem_async.cpp \
  filesystem_stdio.cpp \
  filetracker.cpp \
  QueuedLoader.cpp \
  ../public/kevvaluescompiler.cpp \
  ../public/zip_utils.cpp \
  ../public/tier0/memoverride.cpp

LOCAL_SHARED_LIBRARIES := tier0 vstdlib
LOCAL_STATIC_LIBRARIES := tier1 tier2

include $(BUILD_SHARED_LIBRARY)