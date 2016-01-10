LOCAL_PATH:= $(call my-dir)
MY_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
#LOCAL_ASSET_DIR += $(call find-subdir-assets)

LOCAL_MODULE:= libsotajni
LOCAL_SRC_FILES := sotajni.c

LOCAL_CFLAGS += -O3 -DHAVE_STDINT_H=1
LOCAL_C_INCLUDES += /home/caananth/android-builds/jansson-2.7/jni/src

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog 
LOCAL_SHARED_LIBRARIES := libc jansson crypto ssl sotaclient

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
LOCAL_MODULE:= sotaclient
LOCAL_SRC_FILES:= libs/libsotaclient.so
include $(PREBUILT_SHARED_LIBRARY)