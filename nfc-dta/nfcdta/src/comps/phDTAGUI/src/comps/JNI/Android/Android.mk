
$(info "NXP-NFC-DTA>Building JNI for DTA..")

LOCAL_PATH:= $(call my-dir)
D_CFLAGS := -DANDROID -DBUILDCFG=1

include $(CLEAR_VARS)
DTALIB_PATH:= $(LOCAL_PATH)/../../../../../phDTALib/
LOCAL_ARM_MODE := arm

LOCAL_MODULE_TAGS:= optional

#phExampleNfc
define all-cpp-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef


LOCAL_SRC_FILES += ../phDTALibJNI.cpp

#NXP PN547 Enable
D_CFLAGS += -DNXP_EXTNS=TRUE
D_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE


LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../ \
    $(DTALIB_PATH)/inc/ \
    external/libnfc-nci/src/gki/common/ \
    external/libnfc-nci/src/gki/ulinux/ \
    $(DTALIB_PATH)/src/comps/phMwIf/inc \
    $(DTALIB_PATH)/src/comps/phDTAOSAL/inc \
    $(DTALIB_PATH)/src/comps/phDTAOSAL/src/comps/Android/inc

LOCAL_CFLAGS += $(D_CFLAGS)

LOCAL_CFLAGS += -DNXP_EXTNS=TRUE
LOCAL_CFLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE

LOCAL_SHARED_LIBRARIES := libdta \
              libcutils \
              libosal \

LOCAL_MODULE:= libdta_jni

include $(BUILD_SHARED_LIBRARY)

