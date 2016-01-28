LOCAL_PATH:= $(call my-dir)
MY_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
#LOCAL_ASSET_DIR += $(call find-subdir-assets)

LOCAL_MODULE:= libsotajni
LOCAL_SRC_FILES := sotajni.c

LOCAL_CFLAGS += -O3 -DHAVE_STDINT_H=1
LOCAL_C_INCLUDES += /home/caananth/android-builds/jansson-2.7/jni/src

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog 
LOCAL_SHARED_LIBRARIES := libc jansson crypto ssl sotaclient glib-2.0 gio-2.0 gmodule-2.0 gthread-2.0 gobject-2.0 xdelta z tar bz

include $(BUILD_SHARED_LIBRARY)


#################################################
# ADDITIONAL PRE-BUILT LIBRARIES
#------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE:= jansson
LOCAL_SRC_FILES:= libs/libjansson.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= crypto
LOCAL_SRC_FILES:= libs/libcrypto.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= ssl
LOCAL_SRC_FILES:= libs/libssl.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= tar
LOCAL_SRC_FILES:= libs/libtar.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= bz
LOCAL_SRC_FILES:= libs/libbz.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= sotaclient
LOCAL_SRC_FILES:= libs/libsotaclient.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= glib-2.0
LOCAL_SRC_FILES:= libs/libglib-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE:= gio-2.0
#LOCAL_SRC_FILES:= libs/libgio-2.0.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE:= gmodule-2.0
#LOCAL_SRC_FILES:= libs/libgmodule-2.0.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE:= gthread-2.0
#LOCAL_SRC_FILES:= libs/libgthread-2.0.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE:= gobject-2.0
#LOCAL_SRC_FILES:= libs/libgobject-2.0.so
#include $(PREBUILT_SHARED_LIBRARY)
#
include $(CLEAR_VARS)
LOCAL_MODULE:= xdelta
LOCAL_SRC_FILES:= libs/libxdelta.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= z
LOCAL_SRC_FILES:= libs/libz.so
include $(PREBUILT_SHARED_LIBRARY)
