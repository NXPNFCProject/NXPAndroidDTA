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
 * \file phDTA_P2PTest.c
 *
 * Project: NFC DTA
 *
 */
/* Preprocessor includes for different platform */
#ifdef WIN32
#include <windows.h>
#else
#include <utils/Log.h>
#include "data_types.h"
#endif

#include "phDTAOSAL.h"
#include "phMwIf.h"
#include "phDTALib.h"
#include "phOsal_LinkList.h"
#include "phOsal_Queue.h"
#include "phDTATst.h"


#ifdef __cplusplus
extern "C" {
#endif

extern phDtaLib_sHandle_t g_DtaLibHdl;

/**
 * To handle Poll mode test cases
 */
DTASTATUS phDtaLibi_NfcdepTargetOperations(BOOLEAN* bStartDiscReqd, BOOLEAN* bStopDiscReqd) {
    uint8_t const cusot[] = { 0x00, 0x40, 0x00, 0x01, 0x10, 0x02, 0x01, 0x0E }, /**< Start of Frame in Loop back */
    cueot1[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x01 }, /**< EOT Frame 1*/
    cueot2[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x02 }; /**< EOT Frame 2*/
    MWIFSTATUS dwMwIfStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    uint8_t resultBuffer[2048],loopBakBuffer[2048];
    uint32_t dwSizeOfResultBuff=0, dwSizeOfLoopBakBuff=0;
    /* Send Start Frame */
    phOsal_LogDebug((const uint8_t*)"DTALib> NFC-DEP Loop Back sending Start Frame ..\n");
    dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                     (uint8_t *) cusot,
                                     sizeof(cusot),
                                     resultBuffer,
                                     &dwSizeOfResultBuff);
    phOsal_LogDebugU32h((const uint8_t*)"DTALib> First Transceive Done: ResultBuffSize= ..",dwSizeOfResultBuff);
    if (dwMwIfStatus == MWIFSTATUS_SUCCESS) {
        /* Begin Loop for TXVR */
        do {
            /* Check for EOT */
            if ((memcmp((const void *) resultBuffer, (const void *) cueot1,
                    sizeof(cueot1)) == 0)) {
                phOsal_LogError((const uint8_t*)"DTALib> EOT1 Received for NFC-DEP Loop Back \n");
                phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_SLEEP);
                phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
                *bStartDiscReqd = TRUE;
                *bStopDiscReqd  = TRUE;
                break;
            }
            if ((memcmp((const void *) resultBuffer, (const void *) cueot2,
                    sizeof(cueot2)) == 0)) {
                phOsal_LogError((const uint8_t*)"DTALib> EOT2 Received for NFC-DEP Loop Back \n");
                phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
                *bStartDiscReqd = TRUE;
                *bStopDiscReqd  = TRUE;
                break;
            }

            memcpy(loopBakBuffer,resultBuffer,dwSizeOfResultBuff);
            dwSizeOfLoopBakBuff = dwSizeOfResultBuff;

            phOsal_LogDebugU32h((const uint8_t*)"DTALib> Next Transceive,input buf size=",dwSizeOfLoopBakBuff);
#if (NFC_NXP_P2P_PERFORMANCE_TESTING == TRUE)
            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                             (uint8_t *) cueot1,
                                             sizeof(cueot1),
                                             resultBuffer,
                                             &dwSizeOfResultBuff);
#else

            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                             loopBakBuffer,
                                             dwSizeOfLoopBakBuff,
                                             resultBuffer,
                                             &dwSizeOfResultBuff);
#endif
            phOsal_LogDebugU32h((const uint8_t*)"DTALib> Next Transceive Done: ResultBuffSize= ..",dwSizeOfResultBuff);
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogError((const uint8_t*)"DTALib> Error Failed to transceive data in loop back !! \n");
                break;
            }
        } while (dwMwIfStatus == MWIFSTATUS_SUCCESS);
    }
    return dwMwIfStatus;
}

/*
*  To handle NFC_DEP listen mode test cases means LT is polling and is initiator
*/
uint8_t Pgu_event;                      /**< To indicate the Last event that occurred from any Call Back */
DTASTATUS phDtaLibi_NfcdepInitiatorOperations() {
#if (NFC_NXP_P2P_PERFORMANCE_TESTING == TRUE)
    uint8_t const cusot[] = { 0x00, 0x40, 0x00, 0x01, 0x10, 0x02, 0x01, 0x0E } , /**< Start of Frame in Loop back */
                  cueot1[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x01 }; /**< EOT Frame 1*/
    int i=0, j=0;
#endif
    const uint8_t cspat_no_cmd[] = { 0xFF, 0x00, 0x00, 0x00 },     /* for NFC-F */
                  cswait_cmd[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x03 }; /* for NFC-F */
    MWIFSTATUS    dwMwIfStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    //phDtaLib_sQueueData_t  sQueueData;
    uint8_t resultBuffer[2048] = {0},loopBakBuffer[2048] = {0x00};
    uint32_t dwSizeOfResultBuff=0, dwSizeOfLoopBakBuff=0;

    LOG_FUNCTION_ENTRY;

#if (NFC_NXP_P2P_PERFORMANCE_TESTING == TRUE)
    for(i=0;i<2048;i++){
       loopBakBuffer[i] = j++;
       if(j == 0xFF)
           j=0x00;
    }
#endif

    /*Wait for Data Event to start loopback*/
    dwMwIfStatus = phMwIf_ReceiveData(dtaLibHdl->mwIfHdl, resultBuffer, &dwSizeOfResultBuff);
    if((!dwSizeOfResultBuff) && (dwMwIfStatus != MWIFSTATUS_SUCCESS))
    {
         phOsal_LogError((const uint8_t*)"DTALib>:Invalid Data Recvd");
         return DTASTATUS_FAILED;
    }
    do
    {
        /*Check for EOT*/
        if ((memcmp((const void *) resultBuffer, (const void *) cspat_no_cmd,
                sizeof(cspat_no_cmd)) == 0))
        {
            phOsal_LogError((const uint8_t*)"DTALib> Pattern Command- change pattern number Reboot stack TBD");
            break;
        }
        else if (memcmp((const void *) resultBuffer,
                (const void *) cswait_cmd, sizeof(cswait_cmd)) == 0)
        {
            phOsal_LogError((const uint8_t*)"DTALib> Wait command - Delay generate \n");
            phOsal_Delay(200);
        }
#if (NFC_NXP_P2P_PERFORMANCE_TESTING == TRUE)
        else if (memcmp((const void *) resultBuffer,
                (const void *) cusot, sizeof(cusot)) == 0)
        {
            phOsal_LogError((const uint8_t*)"DTALib> Wait command - Delay generate \n");
        }

        else if (memcmp((const void *) resultBuffer,
                (const void *) cueot1, sizeof(cueot1)) == 0)
        {
            phOsal_LogError((const uint8_t*)"DTALib> Wait command - Delay generate \n");
            break;
        }
#endif
        if(!dwSizeOfResultBuff)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib> :  ", (const uint8_t*)__FUNCTION__);
            phOsal_LogError((const uint8_t*)"Invalid Data Recvd1");
            return DTASTATUS_FAILED;
        }

#if (NFC_NXP_P2P_PERFORMANCE_TESTING == TRUE)
        dwSizeOfLoopBakBuff = sizeof(loopBakBuffer);
#else
        memcpy(loopBakBuffer,resultBuffer,dwSizeOfResultBuff);
        dwSizeOfLoopBakBuff = dwSizeOfResultBuff;
#endif
        dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                        (uint8_t *)loopBakBuffer,
                                        dwSizeOfLoopBakBuff,
                                        resultBuffer,
                                         &dwSizeOfResultBuff);
        if (dwMwIfStatus != MWIFSTATUS_SUCCESS) {
            phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
            break;
        }
    } while (dwMwIfStatus == MWIFSTATUS_SUCCESS);
    LOG_FUNCTION_EXIT;
    return gx_status;
}

#ifdef __cplusplus
}
#endif
