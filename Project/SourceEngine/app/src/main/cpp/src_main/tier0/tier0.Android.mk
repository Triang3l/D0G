LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := tier0

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1
LOCAL_CFLAGS := -DTIER0_DLL_EXPORT

LOCAL_SRC_FILES := \
  assert_dialog.cpp \
  commandline.cpp \
  cpu.cpp \
  dbg.cpp \
  fasttimer.cpp \
  mem.cpp \
  mem_helpers.cpp \
  memdbg.cpp \
  memstd.cpp \
  memvalidate.cpp \
  minidump.cpp \
  pch_tier0.cpp \
  platform.cpp \
  pme.cpp \
  PMELib.cpp \
  progressbar.cpp \
  security.cpp \
  systeminformation.cpp \
  thread.cpp \
  threadtools.cpp \
  tslist.cpp \
  vcrmode.cpp \
  vprof.cpp \
  win32consoleio.cpp

include $(BUILD_SHARED_LIBRARY)