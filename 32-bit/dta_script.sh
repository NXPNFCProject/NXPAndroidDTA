adb root
adb remount
adb shell mkdir  system/app/NxpDTA
adb push ../NxpDTA.apk system/app/NxpDTA/NxpDTA.apk
adb push libdta_jni.so system/lib/libdta_jni.so
#adb push libdta_jni.so system/lib64/libdta_jni.so
adb push libdta.so system/lib/libdta.so
#adb push libdta.so system/lib64/libdta.so
adb push libmwif.so system/lib/libmwif.so
#adb push libmwif.so system/lib64/libmwif.so
adb push libosal.so system/lib/libosal.so
#adb push libosal.so system/lib64/libosal.so
