LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:=opencv_core
LOCAL_SRC_FILES:= external/$(TARGET_ARCH_ABI)/libopencv_core.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=opencv_highgui
LOCAL_SRC_FILES:= external/$(TARGET_ARCH_ABI)/libopencv_highgui.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=opencv_imgproc
LOCAL_SRC_FILES:= external/$(TARGET_ARCH_ABI)/libopencv_imgproc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=opencv_objdetect
LOCAL_SRC_FILES:= external/$(TARGET_ARCH_ABI)/libopencv_objdetect.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libjpeg
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/liblibjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libtiff
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/liblibtiff.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libtbb
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/libtbb.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libpng
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/liblibpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=zlib
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/libzlib.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libjasper
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/liblibjasper.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=libimf
LOCAL_SRC_FILES:= external/3rdparty/libs/$(TARGET_ARCH_ABI)/libIlmImf.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := nimbledetector
LOCAL_SRC_FILES := main.cpp
LOCAL_CFLAGS += -fPIC -DANDROID -fsigned-char
LOCAL_LDLIBS := -llog -lz -ldl -ljnigraphics -landroid

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES := opencv_objdetect opencv_imgproc opencv_highgui opencv_core libimf libjpeg libjasper libpng libtiff zLib libtbb

include $(BUILD_SHARED_LIBRARY)

