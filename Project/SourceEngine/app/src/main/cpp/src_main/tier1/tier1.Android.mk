LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := tier1

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1
LOCAL_CFLAGS := -DTIER1_STATIC_LIB

LOCAL_SRC_FILES := \
  bitbuf.cpp \
  byteswap.cpp \
  characterset.cpp \
  checksum_crc.cpp \
  checksum_md5.cpp \
  commandbuffer.cpp \
  convar.cpp \
  datamanager.cpp \
  diff.cpp \
  generichash.cpp \
  interface.cpp \
  KeyValues.cpp \
  lzmaDecoder.cpp \
  lzss.cpp \
  mempool.cpp \
  memstack.cpp \
  NetAdr.cpp \
  newbitbuf.cpp \
  processor_detect.cpp \
  rangecheckedvar.cpp \
  stringpool.cpp \
  strtools.cpp \
  tier1.cpp \
  tokenreader.cpp \
  uniqueid.cpp \
  utlbuffer.cpp \
  utlbufferutil.cpp \
  utlstring.cpp \
  utlsymbol.cpp \
  v_qsort_r.c

include $(BUILD_STATIC_LIBRARY)