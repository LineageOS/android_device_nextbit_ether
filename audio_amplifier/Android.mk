# Copyright (C) 2015 The CyanogenMod Project
# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	liblog libtinyalsa libdl libcutils

LOCAL_C_FLAGS += -DPLATFORM_MSM8994
LOCAL_C_INCLUDES += $(call project-path-for,qcom-audio)/hal
LOCAL_C_INCLUDES += $(call project-path-for,qcom-audio)/hal/audio_extn
LOCAL_C_INCLUDES += external/tinyalsa/include
LOCAL_C_INCLUDES += external/tinycompress/include
LOCAL_C_INCLUDES += $(call include-path-for, audio-route)

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := \
	audio_amplifier.c

LOCAL_MODULE := audio_amplifier.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)
