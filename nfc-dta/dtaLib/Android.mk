#
#  Copyright (C) 2015-2018 NXP Semiconductors
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
$(info "NXP-NFC-DTA>Building DTA Lib Infterface..")

# function to find all *.cpp files under a directory
define all-cpp-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef

LOCAL_PATH:= $(call my-dir)/../
include $(LOCAL_PATH)/dtaConfig.mk
MWIF_PATH := dtaMwAl
OSAL_PATH := dtaPlatform
DTA_PATH  := dtaLib

######################################
# Build shared library system/lib/libmwif.so for stack code.
include $(CLEAR_VARS)

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -gt 21; echo $$?),0)
   D_CFLAGS += -DAPP_HANDLE_DATA_CHAINING
endif

LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
ifeq (true,$(TARGET_IS_64_BIT))
LOCAL_MULTILIB := 64
else
LOCAL_MULTILIB := 32
endif

LOCAL_MODULE := libdta
LOCAL_MODULE_TAGS := optional
#LOCAL_SHARED_LIBRARIES := libhardware_legacy libcutils liblog libdl libstlport libhardware

LOCAL_CFLAGS := $(D_CFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/dtaLib/src/inc/ \
    $(LOCAL_PATH)/dtaMwAl/inc/ \
    $(LOCAL_PATH)/dtaLib/inc \
    $(LOCAL_PATH)/dtaPlatform/phInfra/inc \
    $(LOCAL_PATH)/dtaPlatform/phDtaOsal/inc \
    $(LOCAL_PATH)/dtaPlatform/phDtaOsal/src/comps/android/inc \
    system/nfc/src/gki/common/ \
    system/nfc/src/gki/ulinux/ \
    external/libnfc-nci/src/gki/common/ \
    external/libnfc-nci/src/gki/ulinux/

LOCAL_SRC_FILES := \
    $(call all-c-files-under, dtaLib)

LOCAL_SHARED_LIBRARIES := libmwif \
              libosal

include $(BUILD_SHARED_LIBRARY)
