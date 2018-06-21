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

extern phDtaLib_sHandle_t g_DtaLibHdl;

/**
 * UICC card emulation related operations
 */
DTASTATUS phDtaLibi_UICCOperations()
{
    OSALSTATUS    dwOsalStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    phDtaLib_sQueueData_t* psQueueData=NULL;
    phDtaLib_sQueueData_t  sQueueData;
    LOG_FUNCTION_ENTRY;

    /*Wait for Data Event(Select P_AID) to start CE Operations*/
    do
    {
        dwOsalStatus = phOsal_QueuePull(dtaLibHdl->queueHdl, (void**)&psQueueData, 0);
        if (dwOsalStatus != 0)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib> : Exiting-QueuPull ", (const uint8_t*)__FUNCTION__);
            phOsal_LogErrorU32h((const uint8_t*)"Status = ",dwOsalStatus);

            return DTASTATUS_FAILED;
        }
        sQueueData = *psQueueData;
        phOsal_LogDebugString((const uint8_t*)"DTALib> : Recvd Object", (const uint8_t*)__FUNCTION__);
        phOsal_LogDebugU32h((const uint8_t*)"eEvtType = ", psQueueData->uEvtType.eDpEvtType);
    } while (sQueueData.uEvtType.eDpEvtType != PHMWIF_DEACTIVATED_EVT);

    LOG_FUNCTION_EXIT;
    return DTASTATUS_SUCCESS;
}

#ifdef __cplusplus
}
#endif
