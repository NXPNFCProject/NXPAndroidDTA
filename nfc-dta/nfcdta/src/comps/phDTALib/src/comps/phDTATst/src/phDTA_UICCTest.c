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
