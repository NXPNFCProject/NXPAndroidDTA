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
$(info "NXP-NFC-DTA>Building Middleware Infterface..")

# function to find all *.cpp files under a directory
define all-cpp-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef

LOCAL_PATH:= $(call my-dir)/../../../../
D_CFLAGS := -DANDROID -DBUILDCFG=1
#NXP PN547 Enable
#Chip selection
#variables for NFC_NXP_CHIP_TYPE
PN547C2 := 1
PN548AD := 2
PN551   := 3
PN553   := 4

ifeq ($(PN547C2),1)
D_CFLAGS += -DPN547C2=1
endif
ifeq ($(PN548AD),2)
D_CFLAGS += -DPN548AD=2
endif
ifeq ($(PN551),3)
D_CFLAGS += -DPN551=3
endif
ifeq ($(PN553),4)
D_CFLAGS += -DPN553=4
endif
#### Select the CHIP ####
NXP_CHIP_TYPE := $(PN553)

ifeq ($(NXP_CHIP_TYPE),$(PN547C2))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN547C2
else ifeq ($(NXP_CHIP_TYPE),$(PN548AD))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN548AD
else ifeq ($(NXP_CHIP_TYPE),$(PN551))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN551
else ifeq ($(NXP_CHIP_TYPE),$(PN553))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN553
endif
#end
D_CFLAGS += -DNXP_EXTNS=TRUE
D_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE
######################################
# Build shared library system/lib/libmwif.so for stack code.
include $(CLEAR_VARS)
MW_IF := phMwIf
OSAL := phDTAOSAL
LIBNFC_NCI_PATH:= external/libnfc-nci
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

LOCAL_CFLAGS := $(D_CFLAGS)

#Handle DATA chaining in DTA for L Release
#For lower versions Data chaining is already handled by Middleware
ifeq ($(PLATFORM_SDK_VERSION), 21)
    LOCAL_CFLAGS += -DAPP_HANDLE_DATA_CHAINING
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MW_IF)/inc \
    $(LOCAL_PATH)/$(OSAL)/inc \
    $(LOCAL_PATH)/$(MW_IF)/src/comps/Android/inc \
    $(LIBNFC_NCI_PATH)/src/include \
    $(LIBNFC_NCI_PATH)/src/gki/ulinux \
    $(LIBNFC_NCI_PATH)/src/gki/common \
    $(LIBNFC_NCI_PATH)/$(NFA)/include \
    $(LIBNFC_NCI_PATH)/$(NFA)/int \
    $(LIBNFC_NCI_PATH)/$(NFC)/include \
    $(LIBNFC_NCI_PATH)/$(NFC)/int \
    $(LIBNFC_NCI_PATH)/src/hal/include \
    $(LIBNFC_NCI_PATH)/src/hal/int \
    $(LOCAL_PATH)/phDTAOSAL/src/comps/Android/inc

LOCAL_SRC_FILES := \
    $(call all-c-files-under, $(MW_IF)/src/comps/Android/src) \
    $(call all-cpp-files-under, $(MW_IF)/src/comps/Android/src)

LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libcutils \
    libutils \
    libnfc-nci \
    libosal\
    libhardware \
    libc \
    libdl

include $(BUILD_SHARED_LIBRARY)

