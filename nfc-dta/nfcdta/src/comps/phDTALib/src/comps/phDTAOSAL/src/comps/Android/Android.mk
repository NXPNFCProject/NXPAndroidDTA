# function to find all *.cpp files under a directory
$(info "NXP-NFC-DTA>Building OSAL..")
LOCAL_PATH:= $(call my-dir)
D_CFLAGS := -DANDROID -DBUILDCFG=1
#NXP PN547 Enable
D_CFLAGS += -DNXP_EXTNS=TRUE
D_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE
######################################
# Build shared library system/lib/libmwif.so for stack code.
include $(CLEAR_VARS)
MW_IF := phMwIf
LIBNFC_NCI_PATH:= external/libnfc-nci
NFA := src/nfa
NFC := src/nfc
HAL := src/hal
UDRV := src/udrv

LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libosal
LOCAL_MODULE_TAGS := optional
#LOCAL_SHARED_LIBRARIES := libhardware_legacy libcutils liblog libdl libstlport libhardware

LOCAL_CFLAGS := $(D_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc \
    $(LOCAL_PATH)/../../../inc \
    $(LIBNFC_NCI_PATH)/src/include \
    $(LIBNFC_NCI_PATH)/src/gki/ulinux \
    $(LIBNFC_NCI_PATH)/src/gki/common \
    $(LIBNFC_NCI_PATH)/$(NFA)/include \
    $(LIBNFC_NCI_PATH)/$(NFA)/int \
    $(LIBNFC_NCI_PATH)/$(NFC)/include \
    $(LIBNFC_NCI_PATH)/$(NFC)/int \
    $(LIBNFC_NCI_PATH)/src/hal/include \
    $(LIBNFC_NCI_PATH)/src/hal/int
    
LOCAL_SRC_FILES := \
    $(call all-c-files-under, src) \
    $(call all-c-files-under, ../src)

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libc \
    libdl

include $(BUILD_SHARED_LIBRARY)

