LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := mathlib

LOCAL_ARM_NEON := true

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../common \
  $(LOCAL_PATH)/../public \
  $(LOCAL_PATH)/../public/tier0 \
  $(LOCAL_PATH)/../public/tier1 \
  $(LOCAL_PATH)/../public/mathlib

LOCAL_SRC_FILES := \
  anorms.cpp \
  bumpvects.cpp \
  color_conversion.cpp \
  halton.cpp \
  IceKey.cpp \
  imagequant.cpp \
  lightdesc.cpp \
  mathlib_base.cpp \
  polyhedron.cpp \
  powsse.cpp \
  quantize.cpp \
  randsse.cpp \
  simdvectormatrix.cpp \
  sparse_convolution_noise.cpp \
  sseconst.cpp \
  ssenoise.cpp \
  vector.cpp \
  vmatrix.cpp
ifeq ($(TARGET_ARCH_ABI),x86)
  LOCAL_SRC_FILES += \
    3dnow.cpp \
    sse.cpp
endif

include $(BUILD_STATIC_LIBRARY)