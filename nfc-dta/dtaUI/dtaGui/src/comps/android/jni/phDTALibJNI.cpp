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

/*!
 * \file phDTALibJNI.cpp
 *
 * Project: NFC DTA
 *
 */

/*
 ************************* Header Files ****************************************
 */

#include <utils/Log.h>
#include "data_types.h"
#include "phDTALibJNI.h"
#include "phDTALib.h"
#include "phDTAOSAL.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_FUNC_ENTRY phOsal_LogDebugString((const uint8_t*)"DTAJni>:Enter:",(const uint8_t*)__FUNCTION__)
#define LOG_FUNC_EXIT  phOsal_LogDebugString((const uint8_t*)"DTAJni>:Exit:",(const uint8_t*)__FUNCTION__)

typedef struct phDtaJNI_sHandle {
    void*                           dtaLibIfHdl;
    jint                            dtalibState; /**<JNI handle contains DTALib Initialization. */
    jmethodID                       jmethodID;
    jobject                         jObject;
    JNIEnv*                         jniEnv;
    jobject                         jniObj;
    JavaVM *                        jVm;
    int                             JNIVersion;
} phDtaJNI_sHandle_t;

/*JNI handle contains DTALib Initialization. */
static phDtaJNI_sHandle_t g_dtaJNIHdl;

void phDtaJNIi_EvtCb(void* dtaApplHdl, phdtaAppLib_eEvtType eEvtType, phdtaLib_sEvtData_t  *sEvtData);
/**
 * JNI Test. This will be useful to know if JNI is working or not. This is the first JNI call from UI.
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_TestJNI(
        JNIEnv *env, jobject obj, jint inter) {
    LOG_FUNC_ENTRY;
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>env",(size_t)env);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>Received Interval Value:",inter);

    LOG_FUNC_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * JNI Initialize DTA Lib
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibInitJNI(
        JNIEnv *env, jobject obj) {

    DTASTATUS dwDtaStatus;
    phDtaJNI_sHandle_t *dtaJNIHdl = &g_dtaJNIHdl;
    LOG_FUNC_ENTRY;
    jint  dtaJniRetVal=0;

    phOsal_LogDebug((const uint8_t*)"DTAJni>Calling DTALib Init\n");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>env",(size_t)env);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);
    if(dtaJNIHdl->dtalibState == DTALIB_STATE_INITIALIZED)
    {
        phOsal_LogDebug((const uint8_t*)"DTAJni>DTA Already initialized\n");
        dtaJniRetVal = DTAJNISTATUS_ALREADY_INITIALIZED;
    }
    else
    {
        dwDtaStatus = phDtaLib_Init();
        if(dwDtaStatus == DTASTATUS_SUCCESS)
        {
            phOsal_LogDebug((const uint8_t*)"DTAJni>DTA initilization successfull\n");
            phDtaLib_RegisterCallback(dtaJNIHdl, phDtaJNIi_EvtCb);
            env->GetJavaVM(&dtaJNIHdl->jVm);
            dtaJNIHdl->dtalibState = DTALIB_STATE_INITIALIZED;
            dtaJniRetVal = DTAJNISTATUS_SUCCESS;
        }
        else
        {
            phOsal_LogDebug((const uint8_t*)"Error in DTA initilization\n");
            dtaJniRetVal = DTAJNISTATUS_FAILED;
        }
    }

    LOG_FUNC_EXIT;
    return dtaJniRetVal;
}

/**
 * JNI De-Initialize DTA Lib
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibDeInitJNI(
        JNIEnv *env, jobject obj) {
    DTASTATUS dwDtaStatus;
    phDtaJNI_sHandle_t *dtaJNIHdl = &g_dtaJNIHdl;
    LOG_FUNC_ENTRY;
    jint  dtaJniRetVal=0;

    phOsal_LogDebug((const uint8_t*)"DTAJni>Calling DTALib De-Init\n");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>env",(size_t)env);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);
    if(dtaJNIHdl->dtalibState == DTALIB_STATE_DEINITIALIZED )
    {
        phOsal_LogDebug((const uint8_t*)"DTAJni>DTA Already Deinitialized\n");
        dtaJniRetVal = DTAJNISTATUS_ALREADY_DEINITIALIZED;
    }
    else
    {
        dwDtaStatus = phDtaLib_DeInit();
        if(dwDtaStatus == DTASTATUS_SUCCESS)
        {
            phOsal_LogDebug((const uint8_t*)"DTAJni>DTA Deinitilization successfull\n");
            dtaJNIHdl->dtalibState = DTALIB_STATE_DEINITIALIZED;
            dtaJniRetVal = DTAJNISTATUS_SUCCESS;
        }
        else
        {
            phOsal_LogDebug((const uint8_t*)"Error in DTA Deinitilization\n");
            dtaJniRetVal = DTAJNISTATUS_FAILED;
        }
    }

    LOG_FUNC_EXIT;
    return dtaJniRetVal;
}

/**
 * JNI Enable Discovery
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibEnableDiscoveryJNI(
        JNIEnv *env, jobject obj, jobject phDtaLibsDiscParamst) {
    phDtaLib_sDiscParams_t discParams;
    phDtaJNI_sHandle_t *dtaJNIHdl = &g_dtaJNIHdl;
    phDtaLib_sTestProfile_t sTestProfile;
    jint p2pType;
    LOG_FUNC_ENTRY;
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);
    jclass phdtaclass = env->GetObjectClass(phDtaLibsDiscParamst);

    /*Set Test Profile before starting the discovery*/
    jfieldID patternNumberId = env->GetFieldID(phdtaclass, "patternNum","I");
    jint patternNumber = env->GetIntField(phDtaLibsDiscParamst, patternNumberId);
    sTestProfile.Pattern_Number = patternNumber;

    jfieldID certVerNumberId = env->GetFieldID(phdtaclass, "certificationVerNum","Ljava/lang/String;");
    jstring certString = (jstring)env->GetObjectField(phDtaLibsDiscParamst, certVerNumberId);
    const char *str = env->GetStringUTFChars(certString,NULL);
    strcpy(sTestProfile.Certification_Release, str);
    phOsal_LogDebugString((const uint8_t*)"DTAJni DEBUG> Certification_Release = ",(const uint8_t*)sTestProfile.Certification_Release);
    env->ReleaseStringUTFChars( certString, str);

    jfieldID jFidDtaDebugFlag = env->GetFieldID(phdtaclass, "dtaDebugFlag", "Z");
    jboolean dtaDebugFlag     = env->GetBooleanField(phDtaLibsDiscParamst, jFidDtaDebugFlag);
    sTestProfile.DtaDebugFlag = dtaDebugFlag;

    jfieldID jFidTimeSlotNumberF = env->GetFieldID(phdtaclass, "timeSlotNumberF", "I");
    jboolean timeSlotNumberF     = env->GetIntField(phDtaLibsDiscParamst, jFidTimeSlotNumberF);
    sTestProfile.TimeSlotNumber = timeSlotNumberF;

    jfieldID jFidConnDevLimit = env->GetFieldID(phdtaclass, "connectionDeviceLimit", "I");
    jboolean connectionDeviceLimit     = env->GetIntField(phDtaLibsDiscParamst, jFidConnDevLimit);
    sTestProfile.ConnDeviceLimit = connectionDeviceLimit;

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni> TestProfile Pattern Number = ",sTestProfile.Pattern_Number);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni> TestProfile DTA debug Flag = ",sTestProfile.DtaDebugFlag);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni> Time Slot NUmber = ",sTestProfile.TimeSlotNumber);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni> Connection device limit = ",sTestProfile.ConnDeviceLimit);
    phDtaLib_SetTestProfile(sTestProfile);

    jfieldID jFidPollP2p    = env->GetFieldID(phdtaclass,"pollP2P", "Z");
    jboolean pollP2p        = env->GetBooleanField(phDtaLibsDiscParamst, jFidPollP2p);
    jfieldID jFidPollRdWt   = env->GetFieldID(phdtaclass, "pollRdWt", "Z");
    jboolean pollRdWt       = env->GetBooleanField(phDtaLibsDiscParamst, jFidPollRdWt);
    jfieldID jFidListenP2p  = env->GetFieldID(phdtaclass, "listenP2P", "Z");
    jboolean listenP2p      = env->GetBooleanField(phDtaLibsDiscParamst, jFidListenP2p);
    jfieldID jFidListenUicc = env->GetFieldID(phdtaclass, "listenUicc", "Z");
    jboolean listenUicc     = env->GetBooleanField(phDtaLibsDiscParamst, jFidListenUicc);
    jfieldID jFidListenEse  = env->GetFieldID(phdtaclass, "listenEse", "Z");
    jboolean listenEse      = env->GetBooleanField(phDtaLibsDiscParamst, jFidListenEse);
    jfieldID jFidListenHce  = env->GetFieldID(phdtaclass, "listenHce", "Z");
    jboolean listenHce      = env->GetBooleanField(phDtaLibsDiscParamst, jFidListenHce);
    jfieldID jFidEnableLlcp = env->GetFieldID(phdtaclass, "enableLlcp", "Z");
    jboolean enableLlcp     = env->GetBooleanField(phDtaLibsDiscParamst, jFidEnableLlcp);
    jfieldID jFidEnableSnep = env->GetFieldID(phdtaclass, "enableSnep", "Z");
    jboolean enableSnep     = env->GetBooleanField(phDtaLibsDiscParamst, jFidEnableSnep);

    jfieldID jFidPollP2pRfTech      = env->GetFieldID(phdtaclass,"pollP2pRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjPollP2pRfTech      = env->GetObjectField(phDtaLibsDiscParamst,jFidPollP2pRfTech);
    jclass   jClsPollP2pRfTech      = env->GetObjectClass(jObjPollP2pRfTech);
    jfieldID jFidPollP2pRfTechTypeA = env->GetFieldID(jClsPollP2pRfTech, "technologyA", "Z");
    jboolean pollP2pRfTechTypeA     = env->GetBooleanField(jObjPollP2pRfTech, jFidPollP2pRfTechTypeA);
    jfieldID jFidPollP2pRfTechTypeB = env->GetFieldID(jClsPollP2pRfTech, "technologyB", "Z");
    jboolean pollP2pRfTechTypeB     = env->GetBooleanField(jObjPollP2pRfTech, jFidPollP2pRfTechTypeB);
    jfieldID jFidPollP2pRfTechTypeF = env->GetFieldID(jClsPollP2pRfTech, "technologyF", "Z");
    jboolean pollP2pRfTechTypeF     = env->GetBooleanField(jObjPollP2pRfTech, jFidPollP2pRfTechTypeF);

    jfieldID jFidPollRdWtRfTech      = env->GetFieldID(phdtaclass,"pollRdWtRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjPollRdWtRfTech      = env->GetObjectField(phDtaLibsDiscParamst,jFidPollRdWtRfTech);
    jclass   jClsPollRdWtRfTech      = env->GetObjectClass(jObjPollRdWtRfTech);
    jfieldID jFidPollRdWtRfTechTypeA = env->GetFieldID(jClsPollRdWtRfTech, "technologyA", "Z");
    jboolean pollRdWtRfTechTypeA     = env->GetBooleanField(jObjPollRdWtRfTech, jFidPollRdWtRfTechTypeA);
    jfieldID jFidPollRdWtRfTechTypeB = env->GetFieldID(jClsPollRdWtRfTech, "technologyB", "Z");
    jboolean pollRdWtRfTechTypeB     = env->GetBooleanField(jObjPollRdWtRfTech, jFidPollRdWtRfTechTypeB);
    jfieldID jFidPollRdWtRfTechTypeF = env->GetFieldID(jClsPollRdWtRfTech, "technologyF", "Z");
    jboolean pollRdWtRfTechTypeF     = env->GetBooleanField(jObjPollRdWtRfTech, jFidPollRdWtRfTechTypeF);

    jfieldID jFidListenP2pRfTech        = env->GetFieldID(phdtaclass,"listenP2pRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjListenP2pRfTech        = env->GetObjectField(phDtaLibsDiscParamst,jFidListenP2pRfTech);
    jclass   jClsListenP2pRfTech        = env->GetObjectClass(jObjListenP2pRfTech);
    jfieldID jFidListenP2pRfTechTypeA   = env->GetFieldID(jClsListenP2pRfTech, "technologyA", "Z");
    jboolean listenP2pRfTechTypeA       = env->GetBooleanField(jObjListenP2pRfTech, jFidListenP2pRfTechTypeA);
    jfieldID jFidListenP2pRfTechTypeB   = env->GetFieldID(jClsListenP2pRfTech, "technologyB", "Z");
    jboolean listenP2pRfTechTypeB       = env->GetBooleanField(jObjListenP2pRfTech, jFidListenP2pRfTechTypeB);
    jfieldID jFidListenP2pRfTechTypeF   = env->GetFieldID(jClsListenP2pRfTech, "technologyF", "Z");
    jboolean listenP2pRfTechTypeF       = env->GetBooleanField(jObjListenP2pRfTech, jFidListenP2pRfTechTypeF);

    jfieldID jFidListenUiccRfTech       = env->GetFieldID(phdtaclass,"listenUiccRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjListenUiccRfTech       = env->GetObjectField(phDtaLibsDiscParamst,jFidListenUiccRfTech);
    jclass   jClsListenUiccRfTech       = env->GetObjectClass(jObjListenUiccRfTech);
    jfieldID jFidListenUiccRfTechTypeA  = env->GetFieldID(jClsListenUiccRfTech, "technologyA", "Z");
    jboolean listenUiccRfTechTypeA      = env->GetBooleanField(jObjListenUiccRfTech, jFidListenUiccRfTechTypeA);
    jfieldID jFidListenUiccRfTechTypeB  = env->GetFieldID(jClsListenUiccRfTech, "technologyB", "Z");
    jboolean listenUiccRfTechTypeB      = env->GetBooleanField(jObjListenUiccRfTech, jFidListenUiccRfTechTypeB);
    jfieldID jFidListenUiccRfTechTypeF  = env->GetFieldID(jClsListenUiccRfTech, "technologyF", "Z");
    jboolean listenUiccRfTechTypeF      = env->GetBooleanField(jObjListenUiccRfTech, jFidListenUiccRfTechTypeF);

    jfieldID jFidListenHceRfTech        = env->GetFieldID(phdtaclass,"listenHceRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjListenHceRfTech        = env->GetObjectField(phDtaLibsDiscParamst,jFidListenHceRfTech);
    jclass   jClsListenHceRfTech        = env->GetObjectClass(jObjListenHceRfTech);
    jfieldID jFidListenHceRfTechTypeA   = env->GetFieldID(jClsListenHceRfTech, "technologyA", "Z");
    jboolean listenHceRfTechTypeA       = env->GetBooleanField(jObjListenHceRfTech, jFidListenHceRfTechTypeA);
    jfieldID jFidListenHceRfTechTypeB   = env->GetFieldID(jClsListenHceRfTech, "technologyB", "Z");
    jboolean listenHceRfTechTypeB       = env->GetBooleanField(jObjListenHceRfTech, jFidListenHceRfTechTypeB);
    jfieldID jFidListenHceRfTechTypeF   = env->GetFieldID(jClsListenHceRfTech, "technologyF", "Z");
    jboolean listenHceRfTechTypeF       = env->GetBooleanField(jObjListenHceRfTech, jFidListenHceRfTechTypeF);

    jfieldID jFidListenEseRfTech        = env->GetFieldID(phdtaclass,"listenEseRfTech","Lcom/phdtaui/structure/PhDtaLibStructure$PhRfTechnology;");
    jobject  jObjListenEseRfTech        = env->GetObjectField(phDtaLibsDiscParamst,jFidListenEseRfTech);
    jclass   jClsListenEseRfTech        = env->GetObjectClass(jObjListenEseRfTech);
    jfieldID jFidListenEseRfTechTypeA   = env->GetFieldID(jClsListenEseRfTech, "technologyA", "Z");
    jboolean listenEseRfTechTypeA       = env->GetBooleanField(jObjListenEseRfTech, jFidListenEseRfTechTypeA);
    jfieldID jFidListenEseRfTechTypeB   = env->GetFieldID(jClsListenEseRfTech, "technologyB", "Z");
    jboolean listenEseRfTechTypeB       = env->GetBooleanField(jObjListenEseRfTech, jFidListenEseRfTechTypeB);
    jfieldID jFidListenEseRfTechTypeF   = env->GetFieldID(jClsListenEseRfTech, "technologyF", "Z");
    jboolean listenEseRfTechTypeF       = env->GetBooleanField(jObjListenEseRfTech, jFidListenEseRfTechTypeF);

    jfieldID LLCPP2p = env->GetFieldID(phdtaclass, "enableLlcp", "Z");
    jint enableP2pLlcp = env->GetBooleanField(phDtaLibsDiscParamst, LLCPP2p);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>EnableP2PLLcp = ",enableP2pLlcp);

    jfieldID LLCPP2p_EnableCntPrms = env->GetFieldID(phdtaclass, "enableParamsInLlcpConnectPdu", "Z");
    jint enableP2pLlcpCntPrms = env->GetBooleanField(phDtaLibsDiscParamst, LLCPP2p_EnableCntPrms);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>EnableP2PLlcpConnectPduParams = ",enableP2pLlcpCntPrms);

    jfieldID SNEPP2p = env->GetFieldID(phdtaclass, "enableSnep", "Z");
    jint enableP2pSnep = env->GetBooleanField(phDtaLibsDiscParamst, SNEPP2p);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>SNEP P2P Selection Status = ",enableP2pSnep);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollP2P = ",pollP2p);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollP2PTechA = ",pollP2pRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollP2PTechB = ",pollP2pRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollP2PTechF = ",pollP2pRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollRdWt = ",pollRdWt);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollRdWtTechA = ",pollRdWtRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollRdWtTechB = ",pollRdWtRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>PollRdWtTechF = ",pollRdWtRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenP2P = ",listenP2p);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenP2PTechA = ",listenP2pRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenP2PTechB = ",listenP2pRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenP2PTechF = ",listenP2pRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenUICC = ",listenUicc);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenUICCTechA = ",listenUiccRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenUICCTechB = ",listenUiccRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenUICCTechF = ",listenUiccRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenHCE = ",listenHce);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenHCETechA = ",listenHceRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenHCETechB = ",listenHceRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenHCETechF = ",listenHceRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenESE = ",listenEse);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenESETechA = ",listenEseRfTechTypeA);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenESETechB = ",listenEseRfTechTypeB);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>ListenESETechF = ",listenEseRfTechTypeF);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>EnableLlcp = ",enableLlcp);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>EnableSnep = ",enableSnep);

    dtaJNIHdl->jObject = (jobject)env->NewGlobalRef(phDtaLibsDiscParamst);
#if 0
    jclass cls = (env)->GetObjectClass(dtaJNIHdl->jObject);
    dtaJNIHdl->jmethodID = env->GetMethodID(cls,"phDtaUIi_EvtCb","(II)V");
    if (dtaJNIHdl->jmethodID == 0)
    {
        phOsal_LogDebug((const uint8_t*)"DTAJni> DTA status failed error in GetMethodID\n");
        return DTASTATUS_FAILED;
    }
#endif

    phOsal_LogDebug((const uint8_t*)"DTAJni> Calling Enable Discovery\n");
    memset(&discParams,0,sizeof(phDtaLib_sDiscParams_t));
    discParams.dwPollP2P =    pollP2p *
                              ((pollP2pRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (pollP2pRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (pollP2pRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));
    discParams.dwPollRdWrt =  pollRdWt *
                              ((pollRdWtRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (pollRdWtRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (pollRdWtRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));
    discParams.dwListenP2P =  listenP2p *
                              ((listenP2pRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (listenP2pRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (listenP2pRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));
    discParams.dwListenHCE =  listenHce *
                              ((listenHceRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (listenHceRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (listenHceRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));
    discParams.dwListenUICC = listenUicc *
                              ((listenUiccRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (listenUiccRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (listenUiccRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));
    discParams.dwListenESE =  listenEse *
                              ((listenEseRfTechTypeA * DTALIB_RFTECHNOLOGY_MASK_A) |
                              (listenEseRfTechTypeB * DTALIB_RFTECHNOLOGY_MASK_B) |
                              (listenEseRfTechTypeF * DTALIB_RFTECHNOLOGY_MASK_F));

    p2pType = (enableP2pLlcp * enableP2pLlcpCntPrms * PHDTALIB_P2P_LLCP_WITH_CONN_PARAMS) +
              (enableP2pLlcp * (!enableP2pLlcpCntPrms) * PHDTALIB_P2P_LLCP_WITHOUT_CONN_PARAMS) +
              (enableP2pSnep * PHDTALIB_P2P_SNEP);

    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>p2pType.phDtaLib_LLCP_P2P:",p2pType);
    phOsal_LogDebug((const uint8_t*)"DTAJni>Calling DTALib Configure P2P LLCP & SENP \n");
    phDtaLib_ConfigureP2p((phDtaLib_eP2PType_t)p2pType);

    if(phDtaLib_EnableDiscovery(&discParams) != DTASTATUS_SUCCESS){
        return DTASTATUS_FAILED;
    }
    LOG_FUNC_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * JNI Disable Discovery
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibDisableDiscoveryJNI(
        JNIEnv *env, jobject obj) {
    LOG_FUNC_ENTRY;
    phDtaJNI_sHandle_t *dtaJNIHdl = &g_dtaJNIHdl;
    phOsal_LogDebug((const uint8_t*)"DTAJni>Calling DTALib Disable Discovery\n");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>env",(size_t)env);
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);
    phDtaLib_DisableDiscovery();
    env->DeleteGlobalRef(dtaJNIHdl->jObject);
    LOG_FUNC_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * JNI Get MW, FW and DTA LIB version
 */
JNIEXPORT jint JNICALL Java_com_phdtaui_helper_PhNXPJniHelper_phDtaLibGetIUTInfoJNI(
        JNIEnv *env, jobject obj, jobject phDtaLibsIUTInfot) {
    phDtaLib_sIUTInfo_t IUTInfo;

    jfieldID dtaLibVerMajor, dtaLibVerMinor;
    jfieldID mwVerMajor, mwVerMinor;
    jfieldID fwVerMajor, fwVerMinor;

    LOG_FUNC_ENTRY;
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>obj", (size_t)&obj);

    jclass phDtaclass = env->GetObjectClass(phDtaLibsIUTInfot);
    phOsal_LogDebug((const uint8_t*)"DTAJni>After the class  Entered:");

    phOsal_LogDebug((const uint8_t*)"DTAJni>Calling Version Info:");
    phDtaLib_GetIUTInfo(&IUTInfo);

    dtaLibVerMajor = env->GetFieldID(phDtaclass, "dtaLibVerMajor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>dtaLibVerMajor = ",IUTInfo.dtaLibVerMajor);
    env->SetIntField(phDtaLibsIUTInfot, dtaLibVerMajor,
            IUTInfo.dtaLibVerMajor);

    dtaLibVerMinor = env->GetFieldID(phDtaclass, "dtaLibVerMinor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>dtaLibVerMinor = ",IUTInfo.dtaLibVerMinor);
    env->SetIntField(phDtaLibsIUTInfot, dtaLibVerMinor,
            IUTInfo.dtaLibVerMinor);

    mwVerMajor = env->GetFieldID(phDtaclass, "mwVerMajor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>mwVerMajor = ",IUTInfo.mwVerMajor);
    env->SetIntField(phDtaLibsIUTInfot, mwVerMajor,
            IUTInfo.mwVerMajor);

    mwVerMinor = env->GetFieldID(phDtaclass, "mwVerMinor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>mwVerMinor = ",IUTInfo.mwVerMinor);
    env->SetIntField(phDtaLibsIUTInfot, mwVerMinor,
            IUTInfo.mwVerMinor);

    fwVerMajor = env->GetFieldID(phDtaclass, "fwVerMajor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>fwVerMajor = ",IUTInfo.fwVerMajor);
    env->SetIntField(phDtaLibsIUTInfot, fwVerMajor, IUTInfo.fwVerMajor);

    fwVerMinor = env->GetFieldID(phDtaclass, "fwVerMinor", "I");
    phOsal_LogDebugU32h((const uint8_t*)"DTAJni>fwVerMinor = ",IUTInfo.fwVerMinor);
    env->SetIntField(phDtaLibsIUTInfot, fwVerMinor, IUTInfo.fwVerMinor);

    LOG_FUNC_EXIT;
    return DTASTATUS_SUCCESS;
}

void phDtaJNIi_EvtCb(void* dtaApplHdl, phdtaAppLib_eEvtType eEvtType, phdtaLib_sEvtData_t  *sEvtData)
{
    phDtaJNI_sHandle_t *dtaJNIHdl = (phDtaJNI_sHandle_t *)dtaApplHdl;
    //int getEnvStat;
    LOG_FUNC_ENTRY;
    if((dtaJNIHdl == NULL) || (dtaJNIHdl->jVm == NULL))
    {
        phOsal_LogDebugString((const uint8_t*)"DTAJni>:DTALIB Invalid param",(const uint8_t*)__FUNCTION__);
        return;
    }

    switch(eEvtType)
    {
        case PHDTALIB_TEST_CASE_EXEC_STARTED:
        #if 0
            getEnvStat = (dtaJNIHdl->jVm)->GetEnv((void **)&(dtaJNIHdl->jniEnv), JNI_VERSION_1_6);
            if (getEnvStat == JNI_EDETACHED) {
                phOsal_LogDebug((const uint8_t*)"DTAJni>:GetEnv: not attached\n");
                if ((dtaJNIHdl->jVm)->AttachCurrentThread(&(dtaJNIHdl->jniEnv), NULL) != 0) {
                    phOsal_LogDebug((const uint8_t*)"DTAJni>:Failed to attach\n");
                }
            }
            else if (getEnvStat == JNI_OK) {
                phOsal_LogDebug((const uint8_t*)"DTAJni>:GetEnv: already attached\n");
            }
            else if (getEnvStat == JNI_EVERSION) {
                phOsal_LogDebug((const uint8_t*)"DTAJni>:GetEnv: version not supported\n");
            }
        #endif
            phOsal_LogDebugString((const uint8_t*)"DTAJni>:PHDTALIB_TEST_CASE_EXEC_STARTED",(const uint8_t*)__FUNCTION__);
            //(dtaJNIHdl->jniEnv)->CallVoidMethod(dtaJNIHdl->jObject, dtaJNIHdl->jmethodID, PHDTALIB_TEST_CASE_EXEC_STARTED, 0);
        break;
        case PHDTALIB_TEST_CASE_EXEC_COMPLETED:
            phOsal_LogDebugString((const uint8_t*)"DTAJni>:PHDTALIB_TEST_CASE_EXEC_COMPLETED",(const uint8_t*)__FUNCTION__);
            //(dtaJNIHdl->jniEnv)->CallVoidMethod(dtaJNIHdl->jObject, dtaJNIHdl->jmethodID, PHDTALIB_TEST_CASE_EXEC_COMPLETED, 0);
        break;
        case PHDTALIB_TEST_EXEC_DEINIT:
            phOsal_LogDebugString((const uint8_t*)"DTAJni>:PHDTALIB_TEST_EXEC_DEINIT thread detaching",(const uint8_t*)__FUNCTION__);
            //(dtaJNIHdl->jVm)->DetachCurrentThread();
        break;
        default:
            phOsal_LogDebugString((const uint8_t*)"DTAJni>:Invalid event type",(const uint8_t*)__FUNCTION__);
            phOsal_LogDebugU32h((const uint8_t*)"DTAJni>EvtData DataLength= ",sEvtData->wDataLength);
        break;
    }
    LOG_FUNC_EXIT;
}

#ifdef __cplusplus
}
#endif
