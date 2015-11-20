/*
* Copyright (C) 2015 NXP Semiconductors
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
 * \file phDTA_HCETest.c
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

/**
 * Host Card Emulation related operations
 */

/*HCE related data*/
static uint8_t g_P_AidData[] = {0x01,0x00,0x90,0x00};
static uint8_t g_P_AidDataLength = 0x04;
static uint8_t gStatusWordData [] = {0x90,0x00};
static uint8_t gStatusWordDataLength = 0x02;
static uint8_t gCApduRespData [] = {0x80,0xA8,0x00,0x00,0x01,0x02};
static uint8_t gCApduRespDataLength = 6;
static uint8_t gCmdRespXchg1 [] = {0x00,0xB2,0x01,0x0C,0x00};
static uint8_t gCmdRespXchg2 [] = {0x00,0xB2,0x02,0x0C,0x00};
static uint8_t gCmdRespXchgLength = 5;
extern phDtaLib_sHandle_t g_DtaLibHdl;

/**
 * Handle Host card emulation related operations
 */
DTASTATUS phDtaLibi_HceOperations()
{
    OSALSTATUS    dwOsalStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    phDtaLib_sQueueData_t* psQueueData=NULL;
    phDtaLib_sQueueData_t  sQueueData;
    uint8_t resultBuffer[400];
    uint32_t dwSizeOfResultBuff=0;
    LOG_FUNCTION_ENTRY;

    while(1)
    {
    /*Wait for Data Event(Select P_AID) to start CE Operations*/
    do {
        dwOsalStatus = phOsal_QueuePull(dtaLibHdl->queueHdl, (void**)&psQueueData, 0);
          if (dwOsalStatus != 0) {
              phOsal_LogErrorString((const uint8_t*)"DTALib> : Exiting-QueuPull ", (const uint8_t*)__FUNCTION__);
              phOsal_LogErrorU32h((const uint8_t*)"Status = ",dwOsalStatus);

              return DTASTATUS_FAILED;
          }

          phOsal_LogDebugString((const uint8_t*)"DTALib> : Recvd Object", (const uint8_t*)__FUNCTION__);
          phOsal_LogDebugU32h((const uint8_t*)"eEvtType = ", psQueueData->uEvtType.eDpEvtType);
          if ((psQueueData->uEvtType.eDpEvtType == PHMWIF_DEACTIVATED_EVT) && (psQueueData->uEvtInfo.uDpEvtInfo.eDeactivateType == PHMWIF_DEACTIVATE_TYPE_IDLE))
          {
              phOsal_LogDebug((const uint8_t*)"DTALib>psQueueData->uEvtType.eDpEvtType == PHMWIF_DEACTIVATED_EVT");
              free(psQueueData);
            return DTASTATUS_FAILED;
          }

          sQueueData = *psQueueData;
          /*Free Data if its some other event*/
          if(sQueueData.uEvtType.eDpEvtType != PHMWIF_CE_DATA_EVT)
              free(psQueueData);
    } while (sQueueData.uEvtType.eDpEvtType != PHMWIF_CE_DATA_EVT);

    if (dwOsalStatus != OSALSTATUS_SUCCESS)
     {
         phOsal_LogError((const uint8_t*)"DTALib> Error Failed to receive the First packet !! \n");
         return dwOsalStatus;
     }

     phOsal_LogDebugU32d((const uint8_t*)"DTALib>sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf = ",(size_t)sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf);
     if(!sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf)
     {
         phOsal_LogErrorString((const uint8_t*)"DTALib>:Invalid Data Recvd",sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf);
         return DTASTATUS_FAILED;
     }
     memcpy(resultBuffer, sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf, sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize);
     dwSizeOfResultBuff = sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize;

    if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xA4)&&
       (resultBuffer[2] == 0x04)&&(resultBuffer[3] == 0x00))
    {
        gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,g_P_AidData,g_P_AidDataLength);
        if(gx_status != MWIFSTATUS_SUCCESS)
        {
            phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
        }
        phOsal_LogDebug((const uint8_t*)"DTALib> Sending R-APDU for Select P_AID... \n");
    }
    else if((resultBuffer[0] == 0x80)&&(resultBuffer[1] == 0xA8)&&
           (resultBuffer[2] == 0x00)&&(resultBuffer[3] == 0x00))
        {
            gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,gCApduRespData,gCApduRespDataLength);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
              break;
            }
            phOsal_LogDebug ((const uint8_t*)"DTALib> Sending R-APDU Init... \n");
        }
     else if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xB2)&&
           (resultBuffer[2] == 0x01)&&(resultBuffer[3] == 0x0C))
        {
            gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,gCmdRespXchg1,gCmdRespXchgLength);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
              break;
            }
            phOsal_LogDebug ((const uint8_t*)"DTALib> Sending R-APDU 1... \n");
        }
     else if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xB2)&&
           (resultBuffer[2] == 0x02)&&(resultBuffer[3] == 0x0C))
        {
            gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,gCmdRespXchg2,gCmdRespXchgLength);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
              break;
            }
            phOsal_LogDebug ((const uint8_t*)"DTALib> Sending R-APDU 2... \n");
        }
     else if((resultBuffer[0] == 0x80)&&(resultBuffer[1] == 0xEE)&&
           (resultBuffer[2] == 0x00)&&(resultBuffer[3] == 0x00))
        {
            gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,gStatusWordData,gStatusWordDataLength);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
              break;
            }
            phOsal_LogDebug ((const uint8_t*)"DTALib> Sending Status Words on Reception of Chained I Block... \n");
        }

    }

    return MWIFSTATUS_SUCCESS;
}
