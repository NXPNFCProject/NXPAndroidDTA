-By default release package is configured to build for PN557/PN81T with ANDROID-O Middleware

-To Build for PN557/PN81T with ANDROID-O-Generic the following modification needs to be done before building
  + In File dtaConfig.mk
  + In File nfc-dta\nfcdta\src\comps\phDTAGUI\src\comps\Android\src\com\phdtaui\utils\PhUtilities.java
    Set FLAG_NXP_HAL_EXTNS, ANDROID_O  and NCI_2_0 flags to true and Remaining flags should be false as below
    FLAG_NXP_HAL_EXTNS := true
    THREAD_PRIO_SUPPORT:= false
    ANDROID_O          := true
    AOSP_MASTER        := false
    NCI_2_0            := true
    PN81A              := false
    NFC_NXP_CHIP_TYPE  := PN557

-To Build for PN553 with N-Generic/L/KK-DEV/KK-OSP the following modification needs to be done before building
  + In File dtaConfig.mk
    Set THREAD_PRIO_SUPPORT flag to true and Remaining flags should be false as below
    FLAG_NXP_HAL_EXTNS := false
    THREAD_PRIO_SUPPORT:= true
    ANDROID_O          := false
    AOSP_MASTER        := false
    NCI_2_0            := false
    PN81A              := false
    NFC_NXP_CHIP_TYPE  := PN553

-To Build for PN553 with ANDROID-O-Generic the following modification needs to be done before building
  + In File dtaConfig.mk
    Set FLAG_NXP_HAL_EXTNS and ANDROID_O flags to true and Remaining flags should be false as below
    FLAG_NXP_HAL_EXTNS := true
    THREAD_PRIO_SUPPORT:= false
    ANDROID_O          := true
    AOSP_MASTER        := false
    NCI_2_0            := false
    PN81A              := false
    NFC_NXP_CHIP_TYPE  := PN553

-To Build for PN81A with ANDROID-MASTER the following modification needs to be done before building
  + In File dtaConfig.mk
  + In File nfc-dta\nfcdta\src\comps\phDTAGUI\src\comps\Android\src\com\phdtaui\utils\PhUtilities.java
    Set ANDROID_O,AOSP_MASTER, NCI_2_0 and PN81A flags to true and Remaining flags should be false as below
    FLAG_NXP_HAL_EXTNS := false
    THREAD_PRIO_SUPPORT:= false
    ANDROID_O          := true
    AOSP_MASTER        := true
    NCI_2_0            := true
    PN81A              := true
    NFC_NXP_CHIP_TYPE  := PN553

-Commands to be used to push DTA binaries to Android Device
    adb root
    adb wait-for-device
    adb remount
    adb wait-for-device
    adb shell rm -rf /data/app/NxpDTA/NxpDTA.apk
    adb shell mkdir /system/app/NxpDTA
    adb push libosal.so /system/lib64/
    adb push libmwif.so /system/lib64/
    adb push libdta.so /system/lib64/
    adb push libdta_jni.so /system/lib64/
    adb push NxpDTA.apk /system/app/NxpDTA/
    adb reboot
    pause
