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

/*!
 * \file phDTA_P2PTest.c
 *
 * Project: NFC DTA
 *
 */
/* Preprocessor includes for different platform */
#ifdef WIN32
#include <windows.h>
#include "phNfcTypes.h"
#else
#include <utils/Log.h>
#include "data_types.h"
#endif

#include "phMwIf.h"
#include "phDTALib.h"
#include "phDTAOSAL.h"
#include "phOsal_LinkList.h"
#include "phOsal_Queue.h"
#include "phDTATst.h"

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
    uint8_t resultBuffer[400],loopBakBuffer[400];
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
            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                            loopBakBuffer,
                                             dwSizeOfLoopBakBuff,
                                             resultBuffer,
                                             &dwSizeOfResultBuff);
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
    const uint8_t cspat_no_cmd[] = { 0xFF, 0x00, 0x00, 0x00 },     /* for NFC-F */
                  cswait_cmd[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x03 }; /* for NFC-F */
    MWIFSTATUS    dwMwIfStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    phDtaLib_sQueueData_t  sQueueData;
    uint8_t resultBuffer[400] = {0},loopBakBuffer[400] = {0};
    uint32_t dwSizeOfResultBuff=0, dwSizeOfLoopBakBuff=0;
    LOG_FUNCTION_ENTRY;

    /*Wait for Data Event to start loopback*/
    dwMwIfStatus = phMwIf_ReceiveP2PData(dtaLibHdl->mwIfHdl, resultBuffer, &dwSizeOfResultBuff);
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

        if(!dwSizeOfResultBuff)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib> :  ", (const uint8_t*)__FUNCTION__);
            phOsal_LogError((const uint8_t*)"Invalid Data Recvd1");
            return DTASTATUS_FAILED;
        }

        memcpy(loopBakBuffer,resultBuffer,dwSizeOfResultBuff);
        dwSizeOfLoopBakBuff = dwSizeOfResultBuff;

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
