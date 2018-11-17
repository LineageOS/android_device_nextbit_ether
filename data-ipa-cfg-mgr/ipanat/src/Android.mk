ifneq (,$(filter $(QCOM_BOARD_PLATFORMS),$(TARGET_BOARD_PLATFORM)))
ifneq (, $(filter aarch64 arm arm64, $(TARGET_ARCH)))

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_HEADER_LIBRARIES := generated_kernel_headers

LOCAL_SRC_FILES := ipa_nat_drv.c \
                   ipa_nat_drvi.c

LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR)/lib64
LOCAL_MODULE_PATH_32 := $(TARGET_OUT_VENDOR)/lib
ifneq (,$(filter eng, $(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS := -DDEBUG
endif
LOCAL_MODULE := libipanat
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

endif # $(TARGET_ARCH)
endif
