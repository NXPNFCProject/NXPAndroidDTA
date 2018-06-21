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
$(info "NXP-NFC-DTA>Building DTA Application..")
LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../../../../../../dtaConfig.mk
###########################################
# Build DTA.apk
include $(CLEAR_VARS)
# Build all java files in the java subdirectory

# Name of the APK to build
LOCAL_PACKAGE_NAME := NxpDTA
LOCAL_CERTIFICATE := platform
ifeq (true,$(ANDROID_P))
LOCAL_PRIVATE_PLATFORM_APIS := true
endif
LOCAL_MODULE_TAGS := tests
LOCAL_ARM_MODE := arm
ifeq (true,$(TARGET_IS_64_BIT))
LOCAL_MULTILIB := 64
else
LOCAL_MULTILIB := 32
endif

$(warning PLATFORM_SDK_VERSION is $(PLATFORM_SDK_VERSION))
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -gt 25; echo $$?),0)
$(warning "Building for Android O Release or Later")
LOCAL_SRC_FILES := $(call all-java-files-under, src/com) $/../jni/PhNXPJniHelper.java  $/src/o/PhCustomSNEPRTD.java
else ifeq ($(shell test $(PLATFORM_SDK_VERSION) -gt 20; echo $$?),0)
$(warning "Building for Android L / M / N Release")
ifeq ($(ANDROID_O), true)
LOCAL_JAVA_LIBRARIES := com.nxp.nfc
endif
LOCAL_SRC_FILES := $(call all-java-files-under, src/com) $/../jni/PhNXPJniHelper.java  $/src/l/PhCustomSNEPRTD.java
else
$(warning "Building for KK release or Lower")
LOCAL_SRC_FILES := $(call all-java-files-under, src/com) $/../jni/PhNXPJniHelper.java  $/src/kk/PhCustomSNEPRTD.java
endif

# Tell it to build an APK
include $(BUILD_PACKAGE)
