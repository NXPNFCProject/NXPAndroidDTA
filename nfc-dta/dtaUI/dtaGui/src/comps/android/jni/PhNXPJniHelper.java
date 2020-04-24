/*
* Copyright (C) 2015-2020 NXP Semiconductors
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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
                String PATH = System.getProperty("java.library.path");
                Log.d(PhUtilities.UI_TAG, "The library path = "+PATH);
                Log.d(PhUtilities.UI_TAG, "Trying to load "+"dta_jni");
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
