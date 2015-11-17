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
