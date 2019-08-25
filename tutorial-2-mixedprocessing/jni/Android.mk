LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifdef OPENCV_ANDROID_SDK
  ifneq ("","$(wildcard $(OPENCV_ANDROID_SDK)/OpenCV.mk)")
    include ${OPENCV_ANDROID_SDK}/OpenCV.mk
  else
    include ${OPENCV_ANDROID_SDK}/sdk/native/jni/OpenCV.mk
  endif
else
  include ../../sdk/native/jni/OpenCV.mk
endif

LOCAL_MODULE    := gl3cv4jni
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := jni_part.cpp Texture.cpp Shader.cpp VideoRenderer.cpp AROverlayRenderer.cpp
LOCAL_LDLIBS +=  -llog -lGLESv1_CM -lGLESv3 -lm
LOCAL_C_INCLUDES += Texture.hpp Shader.hpp VideoRenderer.hpp AROverlayRenderer.hpp
include $(BUILD_SHARED_LIBRARY)
