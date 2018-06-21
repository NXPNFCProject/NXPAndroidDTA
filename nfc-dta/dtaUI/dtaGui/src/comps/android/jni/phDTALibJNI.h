/*
* Copyright (C) 2015-2018 NXP Semiconductors
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

/*!
 *
 *
 * \file  phDtaLibJNI.h
 * \brief DTA LIB Interface header file.
 *
 * Project:  NFC DTA LIB
 */
#include <jni.h>
#ifdef __cplusplus
extern "C"
{
#endif

/** \ingroup grp_dta_lib
    Indicates DTA initialization status. */
#define DTAJNISTATUS_SUCCESS                  0x00
#define DTAJNISTATUS_FAILED                   0xFF
#define DTAJNISTATUS_ALREADY_INITIALIZED      100
#define DTAJNISTATUS_ALREADY_DEINITIALIZED    200
/*State of DTA Lib */
#define DTALIB_STATE_INITIALIZED   1
#define DTALIB_STATE_DEINITIALIZED 0

typedef jint dtaJniHandle;

/* Configuration for extending symbols for different platform */
#ifndef DTAJNI_LIB_EXTEND
#define DTAJNI_LIB_EXTEND extern
#endif

/**
 * \ingroup grp_dta_lib
 * \brief Initializes the DTA and NFC library
 *
 * This function initializes DTA library and NFC including underlying layers.
 * A session with NFC hardware will be established.
 *
 * \note Before successful initialization other NFC functionalities are not enabled.
 * this function doesn't initialise NFCEE.
 * \param[in]                           None
 * \retval #DTASTATUS_SUCCESS           DTA LIB Initialised successfully
 * \retval #DTASTATUS_FAILED            DTA LIB failed to initialise.
 *
 */
DTAJNI_LIB_EXTEND JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibInitJNI(JNIEnv* e, jobject obj);

/**
 * \ingroup grp_dta_lib
 * \brief DeInitializes the DTA and NFC library
 *
 * This function DeInitializes DTA library and NFC including underlying layers.
 * A session with NFC hardware will be closed.
 *
 * \param[in]                           None
 * \retval #DTASTATUS_SUCCESS           DTA LIB De-Initialised successfully
 * \retval #DTASTATUS_FAILED            DTA LIB failed to De-initialise.
 *
 */
DTAJNI_LIB_EXTEND JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibDeInitJNI(JNIEnv* e, jobject obj);

/**
 * \ingroup grp_dta_lib
 * \brief Enable discovery(Poll + Listen)
 *
 * This function configures enables NFCC to run discovery wheel.
 *
 * \param[in] DiscMask                  Technology types for poll and listen modes #phDtaLib_DiscMask_t
                                        DiscMask = 0 (Disable Discovery)
 * \retval #DTASTATUS_SUCCESS           Discovery enabled successfully
 * \retval #DTASTATUS_FAILED            Failed to enable discovery
 *
 */
DTAJNI_LIB_EXTEND JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibEnableDiscoveryJNI(JNIEnv *env, jobject obj, jobject phDtaLibsDiscParamst);

/**
 * \ingroup grp_dta_lib
 * \brief Disable discovery(Poll + Listen)
 *
 * This function Disables NFCC to run discovery wheel.
 *
 * \param[in] DiscMask                  Technology types for poll and listen modes #phDtaLib_DiscMask_t
                                        DiscMask = 0 (Disable Discovery)
 * \retval #DTASTATUS_SUCCESS           Discovery disabled successfully
 * \retval #DTASTATUS_FAILED            Failed to disable discovery
 *
 */
DTAJNI_LIB_EXTEND JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibDisableDiscoveryJNI(JNIEnv *env, jobject obj);

/**
 * \ingroup grp_dta_lib
 * \brief Get IUT Information.
 *
 * This function Retrieves Version information of DTA LIB, MW and FW.
 *
 * \param[in] IUTInfo                  Pointer to #phDtaLib_IUTInfo_t, Memory shall be allocated by caller.
 * \retval #DTASTATUS_SUCCESS           IUT information retrieved successfully.
 * \retval #DTASTATUS_FAILED            Failed to get IUT information
 * \retval #DTASTATUS_INVALID_PARAM     Invalid parameter to function
 */
DTAJNI_LIB_EXTEND JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibGetIUTInfoJNI(JNIEnv *env, jobject obj, jobject phDtaLibsIUTInfot);

/* P2P LLCP and SNEP*/
#ifdef __cplusplus
};
#endif
