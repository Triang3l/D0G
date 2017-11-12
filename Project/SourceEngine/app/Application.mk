APP_CFLAGS := \
  -ffast-math \
  -D_LINUX=1 \
  -Wno-deprecated-register \
  -Wno-invalid-offsetof \
  -Wno-switch \
  -Wno-undefined-bool-conversion \
  -Wno-vexing-parse
APP_CPPFLAGS := -fexceptions -frtti -std=c++11
APP_MODULES := \
  filesystem_stdio \
  mathlib \
  tier0 \
  tier1 \
  tier2 \
  vstdlib
APP_OPTIM := release
APP_PLATFORM := android-16
APP_STL := c++_shared
NDK_APP_SHORT_COMMANDS := true
NDK_TOOLCHAIN_VERSION := clang