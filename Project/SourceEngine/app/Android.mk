SOURCEENGINE_PATH := $(call my-dir)/src/main/cpp/src_main
include $(SOURCEENGINE_PATH)/mathlib/mathlib.Android.mk
include $(SOURCEENGINE_PATH)/tier0/tier0.Android.mk
include $(SOURCEENGINE_PATH)/tier1/tier1.Android.mk
include $(SOURCEENGINE_PATH)/tier2/tier2.Android.mk
include $(SOURCEENGINE_PATH)/vstdlib/vstdlib.Android.mk