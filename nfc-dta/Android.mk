#########Build DTA Application and dependent modules#############################
$(info "NXP-NFC-DTA>Build DTA appln and dependent modules..")
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
#include $(call all-subdir-makefiles)
#include $(filter-out $(call my-dir)/Android.mk,$(shell find $(LOCAL_PATH)/ -type f -name Android.mk))
NFCDTA_TOP:= $(LOCAL_PATH)
include $(NFCDTA_TOP)/nfcdta/src/comps/phDTALib/src/comps/phDTAOSAL/src/comps/Android/Android.mk
include $(NFCDTA_TOP)/nfcdta/src/comps/phDTALib/src/comps/phMwIf/src/comps/Android/Android.mk
include $(NFCDTA_TOP)/nfcdta/src/comps/phDTALib/src/comps/phDTATst/src/comps/Android/Android.mk
include $(NFCDTA_TOP)/nfcdta/src/comps/phDTAGUI/src/comps/JNI/Android/Android.mk
include $(NFCDTA_TOP)/nfcdta/src/comps/phDTAGUI/src/comps/Android/Android.mk
