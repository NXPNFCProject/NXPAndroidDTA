/*
*                    Copyright (c), NXP Semiconductors
*
*                       (C)NXP Semiconductors B.V.2014
*         All rights are reserved. Reproduction in whole or in part is
*        prohibited without the written consent of the copyright owner.
*    NXP reserves the right to make changes without notice at any time.
*   NXP makes no warranty, expressed, implied or statutory, including but
*   not limited to any implied warranty of merchantability or fitness for any
*  particular purpose, or that the use will not infringe any third party patent,
*   copyright or trademark. NXP must not be liable for any loss or damage
*                            arising from its use.
*
*/

package com.phdtaui.helper;

import android.util.Log;
import android.os.DeadObjectException;
import com.phdtaui.mainactivity.PhDTAUIMainActivity;
import com.phdtaui.structure.PhDtaLibStructure;
import com.phdtaui.utils.PhUtilities;

public class PhNXPJniHelper {

    public native int TestJNI(int param);
    public native int phDtaLibInitJNI();
    public native int phDtaLibDisableDiscoveryJNI();

    public native int phDtaLibDeInitJNI();

    public native int phDtaLibEnableDiscoveryJNI(
        PhDtaLibStructure phDtaLibStructure);

    public native int phDtaLibConfigureP2PJNI(
        PhDtaLibStructure phDtaLibStructure);

    public native int phDtaLibGetIUTInfoJNI(PhDtaLibStructure phDtaLibStructure);

    /*create singleton*/
    protected PhNXPJniHelper(){
    }

    private static PhNXPJniHelper jniHelperObjRef;
    public static PhNXPJniHelper getInstance(){
        if(jniHelperObjRef == null)
            jniHelperObjRef = new PhNXPJniHelper();
        return jniHelperObjRef;
    }

    static {
        try {
            if (PhUtilities.NDK_IMPLEMENTATION) {
                System.loadLibrary("dta_jni");

                Log.d(PhUtilities.UI_TAG, "Initializing System Load Library");
            } else {
                Log.d(PhUtilities.UI_TAG, "NDK is Disabled");
            }
        } catch (UnsatisfiedLinkError ule) {
            /**
             * UnstaisfiedLinkErrorhandled here
             */
            PhUtilities.NDK_IMPLEMENTATION = false;
            Log.d(PhUtilities.UI_TAG,
                    " Could not load native library: " + ule.getMessage());
        } catch (NoClassDefFoundError error) {
            /**
             * NoClassDefFoundError Path mismatch error
             */

            PhUtilities.NDK_IMPLEMENTATION = false;
            Log.d(PhUtilities.UI_TAG, error.getMessage());
        } catch (NoSuchMethodError e) {
            /**
             * NoSuchMethodError Library not loaded correctly
             */
            Log.d(PhUtilities.UI_TAG, e.getMessage());
        }
    }
    public void enableDiscovery(PhDtaLibStructure phDtaLibStructureObjRef) {
        phDtaLibEnableDiscoveryJNI(phDtaLibStructureObjRef);
    }

    public PhDtaLibStructure versionLib(PhDtaLibStructure phDtaLibStructureObjRef){
        phDtaLibGetIUTInfoJNI(phDtaLibStructureObjRef);
    return phDtaLibStructureObjRef;
    }

    public int phDtaLibDeInit(){
        return phDtaLibDeInitJNI();
    }
    public int phDtaLibDisableDiscovery(){
        return phDtaLibDisableDiscoveryJNI();
    }

    public int startPhDtaLibInit() {
        return phDtaLibInitJNI();
    }
    public int startTest() {
        return TestJNI(10);
    }
}
