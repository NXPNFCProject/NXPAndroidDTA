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
 * \file phDTA_T4TTest.c
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

static uint8_t const gs_pbStartOfTransaction[] = { 0x00, 0xA4, 0x04, 0x00, 0x0E,
                '2', 'N', 'F', 'C', '.', 'S', 'Y', 'S',
                '.', 'D', 'D', 'F', '0', '1', 0x00 },   /**< Start of Frame in Loop back */
    gs_pbEndOfTransaction[] = { 0xFF, 0xFF, 0xFF, 0x01, 0x01 };     /**< EOT Frame */

/**
 * Handle T4T Tag operations(Read/Write) based on the pattern number configured
 */
DTASTATUS phDtaLibi_T4TOperations(phDtaLib_sTestProfile_t TestProfile,
                                  BOOLEAN* bStartDiscReqd,
                                  BOOLEAN* bStopDiscReqd)
{
    MWIFSTATUS dwMwIfStatus = MWIFSTATUS_FAILED;
    DTASTATUS  dwDtaStatus = DTASTATUS_FAILED;
    uint8_t resultBuffer[400],loopBakBuffer[400];
    uint32_t dwSizeOfResultBuff=0, dwSizeOfLoopBakBuff=0;
    phMwIf_uTagOpsParams_t sTagOpsParams;
    phMwIf_sNdefDetectParams_t* psNdefDetectParams;
    phMwIf_sBuffParams_t*       psBuffParams;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    uint8_t outBuff[400];

    LOG_FUNCTION_ENTRY;

    sTagOpsParams.sBuffParams.pbBuff = outBuff;

    /**< Select operation based on pattern number */
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>T4T:pattern number  ", TestProfile.Pattern_Number);
      switch(TestProfile.Pattern_Number)
      {
        case 0x0000:
        case 0x0007:
        {
        /*Start with sending start of transaction and then do loopback send receive
         * until end of transaction*/
          phOsal_LogDebug((const uint8_t*)"DTALib> T4T: Loop Back sending Start Frame ..\n");
          dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                          (uint8_t *) gs_pbStartOfTransaction,
                                          sizeof(gs_pbStartOfTransaction),
                                           resultBuffer,
                                           &dwSizeOfResultBuff);
          if(dwMwIfStatus != MWIFSTATUS_SUCCESS)
          {
            phOsal_LogErrorU32h((const uint8_t*)"DTALib> T4T:Error Start of Frame not Sent/reply not received:Status=0x",dwMwIfStatus);
            break;
          }

          do
          {
            /*Check if End Of Transaction is received*/
            if(memcmp((const void *)resultBuffer,
                      (const void *)gs_pbEndOfTransaction,
                       sizeof(gs_pbEndOfTransaction))==0)
            {/*Send Deactivate command if end of transation is received*/
                 phOsal_LogDebug((const uint8_t*)"DTALib>T4T:EOT Received for T4T Loop Back \n");
                 phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
                 *bStartDiscReqd = FALSE;
                 *bStopDiscReqd  = FALSE;
             break;
            }

            /*Send the received buffer back-Loopback*/
            memcpy(loopBakBuffer,resultBuffer,dwSizeOfResultBuff-2);
            dwSizeOfLoopBakBuff = dwSizeOfResultBuff-2;
            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                                loopBakBuffer,
                                                dwSizeOfLoopBakBuff,
                                                resultBuffer,
                                                &dwSizeOfResultBuff);
            if(dwMwIfStatus != DTASTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib> T4T:Error Failed to tranceive data in loop back !! \n");
              break;
            }
          }while(dwMwIfStatus == DTASTATUS_SUCCESS);
        }
        break;
        case 0x0001:
        {
            phOsal_LogDebug ((const uint8_t*)"DTALib>T4T:Perform NDEF Check \n");
            dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
            if (dwDtaStatus != DTASTATUS_SUCCESS)
            {
                phOsal_LogError((const uint8_t*)"DTALib>T4T:Device is not NDEF Compliant\n");
                return dwDtaStatus;
            }

            psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
            if(!psNdefDetectParams->dwStatus)
            {
                phOsal_LogDebug((const uint8_t*)"DTALib> T4T:Tag is NDEF compliant \n");
                phOsal_LogDebug((const uint8_t*)"DTALib> T4T:Read NDEF \n");
                dwDtaStatus = phDtaLibi_ReadNDEF(&sTagOpsParams);
                if (dwDtaStatus != DTASTATUS_SUCCESS)
                {
                    phOsal_LogError((const uint8_t*)"DTALib> T4T:Error Could not read data !!\n");
                    break;
                }
            }
        }
        break;
        case 0x0002:
        {
          const uint8_t cArr[] = {"0"};
          phOsal_LogDebug ((const uint8_t*)"DTALib>T4T:Perform NDEF Check \n");
          dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
             phOsal_LogError((const uint8_t*)"DTALib>T4T:Device is not NDEF Compliant\n");
             return dwDtaStatus;
          }

          psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
          if(!psNdefDetectParams->dwStatus)
          {
              phOsal_LogDebug((const uint8_t*)"DTALib> T4T:Tag is NDEF compliant \n");
              phOsal_LogDebug((const uint8_t*)"DTALib>T4T:Write NDEF Message \n");
              psBuffParams = &sTagOpsParams.sBuffParams;
              psBuffParams->pbBuff = (uint8_t*)cArr;
              psBuffParams->dwBuffLength = sizeof(cArr)-1;
              dwDtaStatus = phDtaLibi_WriteNDEF(&sTagOpsParams);
              if(dwDtaStatus != DTASTATUS_SUCCESS)
              {
                  phOsal_LogError((const uint8_t*)"DTALib>T4T:Device is not NDEF Complaint\n");
                  return dwDtaStatus;
              }
          }
        }
        break;
        default:
          phOsal_LogError((const uint8_t*)"DTALib>T4T:Error Invalid Pattern Number for T4T !! \n");
          break;
      }
      LOG_FUNCTION_EXIT;
      /*For Pattern 0&7 return dwMwIfStatus & others return dwDtaStatus*/
      return (dwMwIfStatus | dwDtaStatus);
}

#ifdef __cplusplus
}
#endif
