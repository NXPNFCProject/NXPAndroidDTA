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
$(info "NXP-NFC-DTA>Building Middleware Infterface..")

# function to find all *.cpp files under a directory
define all-cpp-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef

# function to find all *.cc files under a directory
define all-cc-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cc" -and -not -name ".*") \
 )
endef

# function to find all *.c files under a directory
define all-c-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.c" -and -not -name ".*") \
 )
endef


LOCAL_PATH:= $(call my-dir)/../../../../
include $(LOCAL_PATH)/dtaConfig.mk


######################################
# Build shared library system/lib/libmwif.so for stack code.
include $(CLEAR_VARS)
MWIF_PATH := dtaMwAl
OSAL_PATH  := dtaPlatform
LIBNFC_NCI_PATH:= external/libnfc-nci
LIBNFC_NCI_PATH_O:= system/nfc
BASE_LOGGING_PATH_P:= frameworks/base

NFA := src/nfa
NFC := src/nfc
HAL := src/hal
UDRV := src/udrv

LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
ifeq (true,$(TARGET_IS_64_BIT))
LOCAL_MULTILIB := 64
else
LOCAL_MULTILIB := 32
endif

LOCAL_MODULE := libmwif
LOCAL_MODULE_TAGS := optional
#LOCAL_SHARED_LIBRARIES := libhardware_legacy libcutils liblog libdl libstlport libhardware

#Handle DATA chaining in DTA for L Release
#For lower versions Data chaining is already handled by Middleware
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -gt 21; echo $$?),0)
   D_CFLAGS += -DAPP_HANDLE_DATA_CHAINING
endif
LOCAL_CFLAGS := $(D_CFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(OSAL_PATH)/phDtaOsal/inc \
    $(LOCAL_PATH)/$(MWIF_PATH)/inc \
    $(LOCAL_PATH)/dtaPlatform/phInfra/inc \
    $(LOCAL_PATH)/$(MWIF_PATH)/src/comps/android/inc \
    $(LIBNFC_NCI_PATH)/src/include \
    $(LIBNFC_NCI_PATH_O)/utils/include \
    $(LIBNFC_NCI_PATH)/src/gki/ulinux \
    $(LIBNFC_NCI_PATH)/src/gki/common \
    $(LIBNFC_NCI_PATH)/$(NFA)/include \
    $(LIBNFC_NCI_PATH)/$(NFA)/int \
    $(LIBNFC_NCI_PATH)/$(NFC)/include \
    $(LIBNFC_NCI_PATH)/$(NFC)/int \
    $(LIBNFC_NCI_PATH)/src/hal/include \
    $(LIBNFC_NCI_PATH)/src/hal/int \
    $(LIBNFC_NCI_PATH_O)/src/include \
    $(LIBNFC_NCI_PATH_O)/src/gki/ulinux \
    $(LIBNFC_NCI_PATH_O)/src/gki/common \
    $(LIBNFC_NCI_PATH_O)/$(NFA)/include \
    $(LIBNFC_NCI_PATH_O)/$(NFA)/int \
    $(LIBNFC_NCI_PATH_O)/$(NFC)/include \
    $(LIBNFC_NCI_PATH_O)/$(NFC)/int \
    $(LIBNFC_NCI_PATH_O)/src/hal/include \
    $(LIBNFC_NCI_PATH_O)/src/hal/int \
    $(LOCAL_PATH)/dtaPlatform/phDtaOsal/src/comps/android/inc \
    $(BASE_LOGGING_PATH_P)/media/mca/filterfw/native/base
ifeq ($(FLAG_NXP_HAL_EXTNS), true)
    LOCAL_C_INCLUDES += vendor/nxp/opensource/hardware/interfaces/nxpnfc/1.0/default
endif

ifeq ($(ANDROID_P), true)
    LOCAL_C_INCLUDES += hardware/nxp/nfc/extns/impl
endif

LOCAL_SRC_FILES := \
    $(call all-c-files-under, dtaMwAl/src/comps/android) \
    $(call all-cc-files-under, dtaMwAl/src/comps/android) \
    $(call all-cpp-files-under, dtaMwAl/src/comps/android)

LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libcutils \
    libutils \
    libnfc-nci \
    libosal\
    libhardware \
    libc \
    libdl \
    liblog

ifeq ($(FLAG_NXP_HAL_EXTNS), true)
    LOCAL_SHARED_LIBRARIES += vendor.nxp.nxpnfc@1.0 \
                              android.hardware.nfc@1.0
endif
include $(BUILD_SHARED_LIBRARY)

