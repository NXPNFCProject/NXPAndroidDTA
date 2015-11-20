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
$(info "NXP-NFC-DTA>Building DTA Application..")
LOCAL_PATH := $(call my-dir)
###########################################
# Build DTA.apk
include $(CLEAR_VARS)
# Build all java files in the java subdirectory
 
# Name of the APK to build
LOCAL_PACKAGE_NAME := NxpDTA
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_TAGS := tests

$(warning PLATFORM_SDK_VERSION is $(PLATFORM_SDK_VERSION))
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -gt 20; echo $$?),0)
$(warning "Building for L release or Later")
LOCAL_SRC_FILES := $(call all-java-files-under, src/com) $/../JNI/PhNXPJniHelper.java  $/src/l/PhCustomSNEPRTD.java
else
$(warning "Building for KK release or Lower")
LOCAL_SRC_FILES := $(call all-java-files-under, src/com) $/../JNI/PhNXPJniHelper.java  $/src/kk/PhCustomSNEPRTD.java
endif

# Tell it to build an APK
include $(BUILD_PACKAGE)
