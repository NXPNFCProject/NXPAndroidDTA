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
 * Host Card Emulation for Felica related operations
 */

/* macros */
#define T3T_NFCID2_SIZE 8
/* end */

extern phDtaLib_sHandle_t g_DtaLibHdl;


DTASTATUS phDtaLibi_HceFOperations()
{
    OSALSTATUS    dwOsalStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    phDtaLib_sQueueData_t* psQueueData=NULL;
    phDtaLib_sQueueData_t  sQueueData;

    /* Generic buff declarations */
    uint8_t readBuffer[400]={0}; /* Read the data from update command */
    uint32_t dwSizeOfReadBuff=0;
    uint8_t writeBuffer[400] = {0}; /* Write the data as acknowledgment for check command */
    uint32_t dwSizeOfwriteBuffer=0;
    uint32_t count=0;
    /* end */

    /* Update command declarations */
    uint8_t readUpdCmdBuffer[400];
    uint32_t dwSizeOfReadUpdCmdBuff=0;
    const int updCmdRsp[2] = {00,00}; /*Response data for update command*/
    uint8_t blkNum=0, chkCmdMask = 0x80;
    /* end */

    /*Check command declarations*/
    uint8_t writeChkCmdBuffer[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    /*end*/

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

       //phOsal_LogDebugU32d((const uint8_t*)"DTALib>sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf = ",(size_t)sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf);
       phOsal_LogBuffer((const uint8_t*)sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf, sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize, (const uint8_t *)"DTALib> Received data pvDataBuf = ");
       if(!sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf)
       {
           phOsal_LogErrorString((const uint8_t*)"DTALib>:Invalid Data Recvd",(const uint8_t *)sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf);
           return DTASTATUS_FAILED;
       }

       memcpy(readBuffer, sQueueData.uEvtInfo.uDpEvtInfo.sData.pvDataBuf, sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize);
       dwSizeOfReadBuff = sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize;
       phOsal_LogDebugU32d((const uint8_t *)"DEBUG> After copy", sQueueData.uEvtInfo.uDpEvtInfo.sData.dwSize);
       //blkNum = readBuffer[14];
       if(readBuffer[1] == 0x08){
           phOsal_LogDebug((const uint8_t*)"DEBUG DTALib> Update command received");
           memcpy(readUpdCmdBuffer, readBuffer, dwSizeOfReadBuff);
           dwSizeOfReadUpdCmdBuff = dwSizeOfReadBuff;
           blkNum = readUpdCmdBuffer[14];
           writeBuffer[count++] = 0x00;
           writeBuffer[count++] = 0x09;
           memcpy(writeBuffer+2, readBuffer+2, T3T_NFCID2_SIZE);
           count += T3T_NFCID2_SIZE;
           writeBuffer[count++] = updCmdRsp[0];
           writeBuffer[count++] = updCmdRsp[1];
           dwSizeOfwriteBuffer = count;
           writeBuffer[0] = (uint8_t)count; //count = 12

       }else if(readBuffer[1] == 0x06){
           phOsal_LogDebug((const uint8_t*)"DEBUG DTALib> preparing buffer to send check response");
           if((blkNum & chkCmdMask) && (memcmp(readUpdCmdBuffer+14, readBuffer+14, 2) == 0x00)){
                phOsal_LogDebug((const uint8_t*)"DEBUG DTALib> block list is 2 bytes");
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x07;
                memcpy(writeBuffer+count, readBuffer+2, T3T_NFCID2_SIZE);
                count += T3T_NFCID2_SIZE;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x01;
                memcpy(writeBuffer+count, readUpdCmdBuffer+16, 16);
                count += 16;
                dwSizeOfwriteBuffer = count; //count = 29
                writeBuffer[0] = (uint8_t)count; //count = 29
                phOsal_LogBuffer((const uint8_t *)writeBuffer, count, (const uint8_t *)"DTALib> Sending data writeBuffer = ");
           }else if(!(blkNum & chkCmdMask) && (memcmp(readUpdCmdBuffer+14, readBuffer+14, 3) == 0x00)){
                phOsal_LogDebug((const uint8_t*)"DEBUG DTALib> block list is 3 bytes");
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x07;
                memcpy(writeBuffer+2, readBuffer+2, T3T_NFCID2_SIZE);
                count += T3T_NFCID2_SIZE;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x01;
                memcpy(writeBuffer+13, readUpdCmdBuffer+17, 16);
                count += 16;
                dwSizeOfwriteBuffer = count;//count = 29
                writeBuffer[0] = (uint8_t)count;//count = 29
                phOsal_LogBuffer((const uint8_t *)writeBuffer, count, (const uint8_t *)"DTALib> Sending data writeBuffer = ");
            }else{
                phOsal_LogInfoString((const uint8_t*)"DEBUG> DTALib>",(const uint8_t*)" Error in block size or check command called before update command!! \n");
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x07;
                memcpy(writeBuffer+2, readBuffer+2, T3T_NFCID2_SIZE);
                count += T3T_NFCID2_SIZE;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x00;
                writeBuffer[count++] = 0x01;
                memcpy(writeBuffer+13, writeChkCmdBuffer, 16);
                count += 16;
                dwSizeOfwriteBuffer = count;//count = 29
                writeBuffer[0] = (uint8_t)count;//count = 29
                phOsal_LogBuffer((const uint8_t *)writeBuffer, count, (const uint8_t *)"DTALib> Sending data writeBuffer = ");
           }
       }
       gx_status = phMwIf_SendRawFrame(dtaLibHdl->mwIfHdl,writeBuffer,dwSizeOfwriteBuffer);
       if(gx_status != MWIFSTATUS_SUCCESS)
       {
         phOsal_LogError((const uint8_t*)"DTALib> Error Failed to tranceive data in loop back !! \n");
         break;
       }
       memset(writeBuffer, '\0', sizeof(writeBuffer));
       count = 0x00;
       phOsal_LogDebug ((const uint8_t*)"DTALib> Sending R-APDU Init... \n");

    }
    return MWIFSTATUS_SUCCESS;
}

#ifdef __cplusplus
}
#endif
