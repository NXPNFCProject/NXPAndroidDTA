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
 * \file phDTA_HCETest.c
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
    MWIFSTATUS    dwMwIfStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    uint8_t bSendBuffer[400];
    uint32_t dwSizeOfSendBuff=0;
    uint8_t resultBuffer[400];
    uint32_t dwSizeOfResultBuff=0;

    LOG_FUNCTION_ENTRY;

    /*Wait for Data Event to start loopback*/
    dwMwIfStatus = phMwIf_ReceiveData(dtaLibHdl->mwIfHdl, resultBuffer, &dwSizeOfResultBuff);
    if((!dwSizeOfResultBuff) && (dwMwIfStatus != MWIFSTATUS_SUCCESS))
    {
        phOsal_LogError((const uint8_t*)"DTALib>:Invalid Data Recvd");
        return DTASTATUS_FAILED;
    }
    do
    {
        if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xA4)&&
           (resultBuffer[2] == 0x04)&&(resultBuffer[3] == 0x00))
        {
            memcpy(bSendBuffer,g_P_AidData,g_P_AidDataLength);
            dwSizeOfSendBuff = g_P_AidDataLength;
            dwSizeOfResultBuff = sizeof(resultBuffer);
        }
        else if((resultBuffer[0] == 0x80)&&(resultBuffer[1] == 0xA8)&&
               (resultBuffer[2] == 0x00)&&(resultBuffer[3] == 0x00))
        {
            memcpy(bSendBuffer,gCApduRespData,gCApduRespDataLength);
            dwSizeOfSendBuff = gCApduRespDataLength;
            dwSizeOfResultBuff = sizeof(resultBuffer);
        }
        else if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xB2)&&
            (resultBuffer[2] == 0x01)&&(resultBuffer[3] == 0x0C))
        {
            memcpy(bSendBuffer,gCmdRespXchg1,gCmdRespXchgLength);
            dwSizeOfSendBuff = gCmdRespXchgLength;
            dwSizeOfResultBuff = sizeof(resultBuffer);
        }
        else if((resultBuffer[0] == 0x00)&&(resultBuffer[1] == 0xB2)&&
        (resultBuffer[2] == 0x02)&&(resultBuffer[3] == 0x0C))
        {
            memcpy(bSendBuffer,gCmdRespXchg2,gCmdRespXchgLength);
            dwSizeOfSendBuff = gCmdRespXchgLength;
            dwSizeOfResultBuff = sizeof(resultBuffer);
        }
        else if((resultBuffer[0] == 0x80)&&(resultBuffer[1] == 0xEE)&&
        (resultBuffer[2] == 0x00)&&(resultBuffer[3] == 0x00))
        {
            memcpy(bSendBuffer,gStatusWordData,gStatusWordDataLength);
            dwSizeOfSendBuff = gStatusWordDataLength;
            dwSizeOfResultBuff = sizeof(resultBuffer);
            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                        (uint8_t *)bSendBuffer,
                                        dwSizeOfSendBuff,
                                        resultBuffer,
                                         &dwSizeOfResultBuff);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
                 phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
                 break;
            }

            memcpy(bSendBuffer,gStatusWordData,gStatusWordDataLength);
            dwSizeOfResultBuff = sizeof(resultBuffer);
            dwSizeOfSendBuff = gStatusWordDataLength;
            dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                        (uint8_t *)bSendBuffer,
                                        dwSizeOfSendBuff,
                                        resultBuffer,
                                        &dwSizeOfResultBuff);
            if(gx_status != MWIFSTATUS_SUCCESS)
            {
                 phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
                 break;
            }
        }
        else
        {
        /*Do nothing*/
        }

        dwMwIfStatus = phMwIf_Transceive(dtaLibHdl->mwIfHdl,
                                        (uint8_t *)bSendBuffer,
                                        dwSizeOfSendBuff,
                                        resultBuffer,
                                        &dwSizeOfResultBuff);
        if(gx_status != MWIFSTATUS_SUCCESS)
        {
             phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
             break;
        }
    } while (dwMwIfStatus == MWIFSTATUS_SUCCESS);

    return MWIFSTATUS_SUCCESS;
}

#ifdef __cplusplus
}
#endif
