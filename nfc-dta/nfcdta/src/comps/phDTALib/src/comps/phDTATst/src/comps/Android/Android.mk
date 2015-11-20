#
#  The original Work has been changed by NXP Semiconductors.
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
$(info "NXP-NFC-DTA>Building DTA Lib Infterface..")

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
D_CFLAGS += -DNXP_EXTNS=TRUE
D_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE
######################################
# Build shared library system/lib/libmwif.so for stack code.
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libdta
LOCAL_MODULE_TAGS := optional
#LOCAL_SHARED_LIBRARIES := libhardware_legacy libcutils liblog libdl libstlport libhardware

LOCAL_CFLAGS := $(D_CFLAGS)

#FIXME:Enable the below line for PN548
LOCAL_CFLAGS += -DNFC_NXP_CHIP_PN548AD

LOCAL_C_INCLUDES := $(LOCAL_PATH)/phDTATst/inc/ \
    $(LOCAL_PATH)/phMwIf/inc/ \
    $(LOCAL_PATH)/../../inc \
    $(LOCAL_PATH)/phDTAOSAL/inc \
    $(LOCAL_PATH)/phDTAOSAL/src/comps/Android/inc \
    external/libnfc-nci/src/gki/common/ \
    external/libnfc-nci/src/gki/ulinux/


LOCAL_SRC_FILES := \
    $(call all-c-files-under, phDTATst/)

LOCAL_SHARED_LIBRARIES := libmwif \
              libosal

include $(BUILD_SHARED_LIBRARY)
