#
#  Copyright (C) 2015 NXP Semiconductors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#########Build DTA Application and dependent modules#############################
$(info "NXP-NFC-DTA>Build DTA appln and dependent modules..")
LOCAL_PATH:= $(call my-dir)/../../

include $(CLEAR_VARS)
#include $(call all-subdir-makefiles)
#include $(filter-out $(call my-dir)/Android.mk,$(shell find $(LOCAL_PATH)/ -type f -name Android.mk))
UI_PATH   := $(LOCAL_PATH)/dtaUI/dtaGui/src/comps/android/ui
JNI_PATH  := $(LOCAL_PATH)/dtaUI/dtaGui/src/comps/android/jni
DTA_PATH  := $(LOCAL_PATH)/dtaLib
OSAL_PATH := $(LOCAL_PATH)/dtaPlatform/phDtaOsal/src/comps/android
MWIF_PATH := $(LOCAL_PATH)/dtaMwAl/src/comps/android

include $(OSAL_PATH)/Android.mk
include $(MWIF_PATH)/Android.mk
include $(DTA_PATH)/Android.mk
include $(UI_PATH)/Android.mk
include $(JNI_PATH)/Android.mk

