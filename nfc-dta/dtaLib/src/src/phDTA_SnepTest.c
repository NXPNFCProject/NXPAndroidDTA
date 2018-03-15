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
 * \file phDTA_SNEPTest.c
 *
 * Project: NFC DTA
 *
 */

#if __APPLE__
#include <phDtaOsal_OS_X.h>
#include <phDTAOSAL.h>
#elif WIN32
#include <windows.h>
#include <phDTAOSAL.h>
#include "phDtaLog_DtaLib.h"
#endif
#include "phDTALib.h"
#include "phDTATst.h"

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

/* Snep Client config params */
phMwIf_SnepConfig_t snepClientConfigInfo;
/* Snep Client config params */
phMwIf_SnepConfig_t snepServerConfigInfo;
/* Snep client Handler */
HANDLE *snepSessionHandle = NULL;
/* Short DATA package - 27 bytes Text RTD */
const char *data1 = "Lorem ipsum dolor sit amet.";
/* Long DATA package - 492 bytes Text RTD */
const char *data2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus at"
                    " lorem nunc, ut venenatis quam. Etiam id dolor quam, at viverra dolor."
                    " Phasellus eu lacus ligula, quis euismod erat. Sed feugiat, ligula at"
                    " mollis aliquet, justo lacus condimentum eros, non tincidunt neque"
                    " ipsum eu risus. Sed adipiscing dui euismod tellus ullamcorper ornare."
                    " Phasellus mattis risus et lectus euismod eu fermentum sem cursus."
                    " Phasellus tristique consectetur mauris eu porttitor. Sed lobortis"
                    " porttitor orci.";
/* Extended Snep Server name */
uint8_t snepServerName[]="urn:nfc:sn:sneptest";
phMwIf_sBuffParams_t serverName;
/* Server Response Data */
phMwIf_sBuffParams_t responseData;
/* Server Inbox */
phMwIf_sBuffParams_t serverInbox;
/* GET request received */
phMwIf_sBuffParams_t getRequest;
/* Global context for dtalib handle */
extern phDtaLib_sHandle_t g_DtaLibHdl;

/* To print the test case ID of Snep Default Server under execution */
static void print_default_server_caseID(phDtaLib_snepDefServerTestCases serverCaseID);
/* To print the test case ID of Snep server Extended execution */
static void print_extended_server_caseID(phDtaLib_snepExtServerTestCases serverCaseID);
/* To print received message */
static void phDta_printMessage(phMwIf_sBuffParams_t message, uint16_t request);

/* To Do*/
DTASTATUS phDtaLibi_snepClient_basic(phDtaLib_snepClientTestCases clientCaseID)
{
    DTASTATUS dwDtaStatus = DTASTATUS_FAILED;
    MWIFSTATUS mwIfStatus = MWIFSTATUS_FAILED;
    phMwIf_sBuffParams_t pPutData;
    /* common Client configuration */
    snepClientConfigInfo.SnepServerName = NULL;
    snepClientConfigInfo.SnepServerType = phMwIf_SnepServer_Default;
    snepClientConfigInfo.sOptions.miu = 1000;
    snepClientConfigInfo.sOptions.rw = 5;
    LOG_FUNCTION_ENTRY;

    if((clientCaseID < PH_DTA_TC_C_BIT_BV_01) || (clientCaseID > PH_DTA_TC_C_BIT_BI_01_1))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Invalid case ID passed\n");
        return DTASTATUS_INVALID_PARAM;
    }

    snepSessionHandle = phOsal_GetMemory(sizeof(void *));
    if(NULL == snepSessionHandle)
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Memory allocation failed in client\n");
        return DTASTATUS_FAILED;
    }

    /* Frame the NDEF packet */
    /* data length = Text RTD length + status byte length + language code length */
    pPutData.dwBuffLength = strlen(data1) + PH_DTA_OFFSET_THREE;
    pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
    /* Status byte */
    pPutData.pbBuff[0] = 0x02;
    /* language code 'la' */
    phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
    /* Text RTD */
    phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data1,strlen(data1));

    switch(clientCaseID)
    {
        /* 1.Basic Interconnection Test [TC_C_BIT_BV_01] */
        case PH_DTA_TC_C_BIT_BV_01 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("====== Executing SNEP Client Basic Interconnection Test =====");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 1 - TC_C_BIT_BV_01 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(MWIFSTATUS_FAILED == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BV_01 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BV_01 case failed ! \n\n");
            }
            break;
        /* 2.Basic Interconnection Test [TC_C_BIT_BI_01_0] */
        case PH_DTA_TC_C_BIT_BI_01_0 :
            {
                PH_LOG_DTALIB_INFO_STR("==============================================================");
                PH_LOG_DTALIB_INFO_STR("====== Executing SNEP Client Basic Interconnection Test ======");
                PH_LOG_DTALIB_INFO_STR("========= SNEP Client Test case 2 - TC_C_BIT_BI_01_0 ========");
                PH_LOG_DTALIB_INFO_STR("==============================================================");
                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(DTASTATUS_SNEP_INVALID_PROTOCOL_DATA == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BI_01_0 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BI_01_0 case failed ! \n\n");
            }
            break;
        /* 3.Basic Interconnection Test [TC_C_BIT_BI_01_1] */
        case PH_DTA_TC_C_BIT_BI_01_1 :
            {
                PH_LOG_DTALIB_INFO_STR("==============================================================");
                PH_LOG_DTALIB_INFO_STR("====== Executing SNEP Client Basic Interconnection Test ======");
                PH_LOG_DTALIB_INFO_STR("========= SNEP Client Test case 3 - TC_C_BIT_BI_01_1 =========");
                PH_LOG_DTALIB_INFO_STR("==============================================================");
                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BI_01_1 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_BIT_BI_01_1 case failed ! \n\n");
            }
            break;
    }
    /* Clear resources */
    phOsal_FreeMemory(snepSessionHandle);
    snepSessionHandle = NULL;
    phOsal_FreeMemory(pPutData.pbBuff);
    pPutData.pbBuff = NULL;
    pPutData.dwBuffLength = 0;

    LOG_FUNCTION_EXIT;
    return dwDtaStatus;
}

DTASTATUS phDtaLibi_snepClient_put(phDtaLib_snepClientTestCases clientCaseID)
{
    DTASTATUS dwDtaStatus = DTASTATUS_FAILED;
    MWIFSTATUS mwIfStatus = MWIFSTATUS_FAILED;
    phMwIf_sBuffParams_t pPutData = {NULL,0};
    /* common Client configuration */
    snepClientConfigInfo.SnepServerName = NULL;
    snepClientConfigInfo.SnepServerType = phMwIf_SnepServer_Default;
    snepClientConfigInfo.sOptions.miu = PH_DTA_CLIENT_MIU;
    snepClientConfigInfo.sOptions.rw = PH_DTA_CLIENT_RW;
    LOG_FUNCTION_ENTRY;

    if((clientCaseID < PH_DTA_TC_C_PUT_BV_01) || (clientCaseID > PH_DTA_TC_C_PUT_BI_01))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Invalid case ID passed\n");
        return DTASTATUS_INVALID_PARAM;
    }

    snepSessionHandle = phOsal_GetMemory(sizeof(void *));
    if(NULL == (*snepSessionHandle))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Memory allocation failed in client\n");
        return DTASTATUS_FAILED;
    }

    switch(clientCaseID)
    {
        /* 4.Client Put Test - 1 [TC_C_PUT_BV_01] */
        case PH_DTA_TC_C_PUT_BV_01 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client PUT Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 4 - TC_C_PUT_BV_01 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data1) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data1,strlen(data1));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BV_01 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BV_01 case failed ! \n\n");
            }
            break;
        /* 5.Client Put Test - 2 [TC_C_PUT_BV_02] */
        case PH_DTA_TC_C_PUT_BV_02 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client PUT Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 5 - TC_C_PUT_BV_02 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data2) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data2,strlen(data2));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BV_02 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BV_02 case failed ! \n\n");
            }
            break;
        /* 6.Client Put Test - 3 [TC_C_PUT_BI_01] */
        case PH_DTA_TC_C_PUT_BI_01 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client PUT Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 6 - TC_C_PUT_BI_01 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data2) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data2,strlen(data2));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestPut(&pPutData);
                }
                if(MWIFSTATUS_FAILED == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BI_01 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_PUT_BI_01 case failed ! \n\n");
            }
            break;
    }

    /* Clear resources */
    phOsal_FreeMemory(snepSessionHandle);
    snepSessionHandle = NULL;
    phOsal_FreeMemory(pPutData.pbBuff);
    pPutData.pbBuff = NULL;
    pPutData.dwBuffLength = 0;

    LOG_FUNCTION_EXIT;
    return dwDtaStatus;
}

DTASTATUS phDtaLibi_snepClient_get(phDtaLib_snepClientTestCases clientCaseID)
{
    DTASTATUS dwDtaStatus = DTASTATUS_FAILED;
    MWIFSTATUS mwIfStatus = MWIFSTATUS_FAILED;
    phMwIf_sBuffParams_t pPutData = {NULL,0};
    /* common Client configuration */
    serverName.pbBuff = (uint8_t *)&snepServerName;
    serverName.dwBuffLength = sizeof(snepServerName);
    snepClientConfigInfo.SnepServerName = &serverName;
    snepClientConfigInfo.SnepServerType = phMwIf_SnepServer_NonDefault;
    snepClientConfigInfo.sOptions.miu = PH_DTA_CLIENT_MIU;
    snepClientConfigInfo.sOptions.rw = PH_DTA_CLIENT_RW;
    LOG_FUNCTION_ENTRY;

    if((clientCaseID < PH_DTA_TC_C_GET_BV_01) || (clientCaseID > PH_DTA_TC_C_GET_BV_03))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Invalid case ID passed\n");
        return DTASTATUS_INVALID_PARAM;
    }

    snepSessionHandle = phOsal_GetMemory(sizeof(void *));
    if(NULL == (*snepSessionHandle))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Memory allocation failed in client\n");
        return DTASTATUS_FAILED;
    }

    switch(clientCaseID)
    {
        /* 7.Client Get Test - 1 [TC_C_GET_BV_01] */
        case PH_DTA_TC_C_GET_BV_01 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client GET Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 7 - TC_C_GET_BV_01 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data1) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data1,strlen(data1));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestGet(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_01 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_01 case failed ! \n\n");
            }
            break;
        /* 8.Client Get Test - 2 [TC_C_GET_BV_02] */
        case PH_DTA_TC_C_GET_BV_02 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client GET Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 8 - TC_C_GET_BV_02 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data1) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data1,strlen(data1));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestGet(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_02 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_02 case failed ! \n\n");
            }
            break;
        /* 9.Client Get Test - 3 [TC_C_GET_BV_03] */
        case PH_DTA_TC_C_GET_BV_03 :
            {
                PH_LOG_DTALIB_INFO_STR("=============================================================");
                PH_LOG_DTALIB_INFO_STR("=============== Executing SNEP Client GET Test ==============");
                PH_LOG_DTALIB_INFO_STR("========  SNEP Client Test case 9 - TC_C_GET_BV_03 ==========");
                PH_LOG_DTALIB_INFO_STR("=============================================================");

                /* Frame the NDEF packet */
                /* data length = Text RTD length + status byte length + language code length */
                pPutData.dwBuffLength = strlen(data1) + PH_DTA_OFFSET_THREE;
                pPutData.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * pPutData.dwBuffLength);
                /* Status byte */
                pPutData.pbBuff[0] = 0x02;
                /* language code 'la' */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_ONE,"la",2);
                /* Text RTD */
                phOsal_MemCopy(pPutData.pbBuff + PH_DTA_OFFSET_THREE, data1,strlen(data1));

                mwIfStatus = phMwIf_SnepClient_Init(&snepClientConfigInfo,snepSessionHandle);
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_RequestGet(&pPutData);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                {
                    mwIfStatus = phMwIf_SnepClient_DeInit(snepSessionHandle);
                }
                if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_03 case success \n\n");
                else
                    PH_LOG_DTALIB_INFO_STR("\n\nDTALib SNEP> TC_C_GET_BV_03 case failed ! \n\n");
            }
            break;
    }

    /* Clear resources */
    phOsal_FreeMemory(snepSessionHandle);
    snepSessionHandle = NULL;
    phOsal_FreeMemory(pPutData.pbBuff);
    pPutData.pbBuff = NULL;
    pPutData.dwBuffLength = 0;

    LOG_FUNCTION_EXIT;
    return dwDtaStatus;
}

DTASTATUS phDtaLib_snepClientSetUp()
{
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
//    uint8_t configData[16];
    MWIFSTATUS wRetStatus = MWIFSTATUS_FAILED;
    /* To Disable RTOX set the configuration data with 0x0F */
    //configData[0] = 0x0F;
    //wRetStatus = phMwIf_SetConfig(dtaLibHdl->mwIfHdl, PHMWIF_NCI_CONFIG_NFCDEP_OP, 0x01, configData);
    return (DTASTATUS)wRetStatus;
}

DTASTATUS phDtaLib_snepServerSetUp()
{
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
//    uint8_t configData[16];
    MWIFSTATUS wRetStatus = MWIFSTATUS_FAILED;
    /* To Disable RTOX set the configuration data with 0x0F */
    //configData[0] = 0x0F;
    //wRetStatus = phMwIf_SetConfig(dtaLibHdl->mwIfHdl, PHMWIF_NCI_CONFIG_NFCDEP_OP, 0x01, configData);
    return (DTASTATUS)wRetStatus;
}

DTASTATUS phDtaLib_serverInboxCb(phMwIf_sBuffParams_t *receivedMsg)
{
    if(receivedMsg->dwBuffLength > PH_MW_SNEP_SERVER_INBOX_SIZE)
    {
        PH_LOG_DTALIB_CRIT_STR("Received message too big for Inbox");
        return DTASTATUS_FAILED;
    }
    serverInbox.dwBuffLength = receivedMsg->dwBuffLength;
    serverInbox.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * serverInbox.dwBuffLength);
    phOsal_MemCopy(serverInbox.pbBuff, receivedMsg->pbBuff, serverInbox.dwBuffLength);
    PH_LOG_DTALIB_INFO_STR("Message received from client and placed in Server Inbox\n");
    return DTASTATUS_SUCCESS;
}

DTASTATUS phDtaLib_getMessageCb(phMwIf_sBuffParams_t *receivedMsg)
{
    if(receivedMsg->dwBuffLength > PH_MW_SNEP_SERVER_INBOX_SIZE)
    {
        PH_LOG_DTALIB_CRIT_STR("Received message too big for Inbox");
        return DTASTATUS_FAILED;
    }
    getRequest.dwBuffLength = receivedMsg->dwBuffLength;
    getRequest.pbBuff = phOsal_GetMemory(sizeof(uint8_t) * getRequest.dwBuffLength);
    phOsal_MemCopy(getRequest.pbBuff, receivedMsg->pbBuff, getRequest.dwBuffLength);
    PH_LOG_DTALIB_INFO_STR("GET request received from client\\n");
    return DTASTATUS_SUCCESS;
}

DTASTATUS phDtaLibi_snepServerDefault(phDtaLib_snepDefServerTestCases serverCaseID)
{
    MWIFSTATUS mwIfStatus = MWIFSTATUS_FAILED;
    int deInit_flag = 0;
    snepServerConfigInfo.SnepServerName = NULL;
    snepServerConfigInfo.SnepServerType = phMwIf_SnepServer_Default;
    snepServerConfigInfo.sOptions.miu = 1000;
    snepServerConfigInfo.sOptions.rw = 5;
    LOG_FUNCTION_ENTRY;

    print_default_server_caseID(serverCaseID);
    snepSessionHandle = phOsal_GetMemory(sizeof(void *));
    if(NULL == (*snepSessionHandle))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Memory allocation failed in client\n");
        return DTASTATUS_FAILED;
    }

    mwIfStatus = phMwIf_SnepServer_Init(&snepServerConfigInfo,snepSessionHandle);
    if(MWIFSTATUS_SUCCESS == mwIfStatus)
    {
        mwIfStatus = phMwIf_SnepServer_Accept(snepSessionHandle, phDtaLib_serverInboxCb ,phDtaLib_getMessageCb);
    }
    /* Client connected */
    if(MWIFSTATUS_SUCCESS == mwIfStatus)
    {
        switch(serverCaseID)
        {
            case PH_DTA_TC_S_BIT_BI_01_0 :
            case PH_DTA_TC_S_BIT_BI_01_1 :
            case PH_DTA_TC_S_BIT_BI_01_2 :
            case PH_DTA_TC_S_BIT_BI_02  :
            case PH_DTA_TC_S_BIT_BV_02  :
                {
                    /* Wait for PUT/GET or any other invalid request from Client */
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                }
                if(PH_DTA_REQUEST_REJECTED == mwIfStatus)
                {
                    phOsal_Delay(200);
                    mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
                }
                return mwIfStatus;
        }

    }
    if(MWIFSTATUS_SUCCESS == mwIfStatus)
    {
        mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
    }
    if(MWIFSTATUS_PUT_REQUEST == mwIfStatus)
    {
        mwIfStatus = phMwIf_SnepServer_Response(MWIFSTATUS_SUCCESS, NULL);
    }
    else if(MWIFSTATUS_LINK_DEACTIVATED == mwIfStatus)
    {
        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
        deInit_flag = 1;
    }

    if(!deInit_flag)
        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);

    /* Clear resources */
    free(snepSessionHandle);
    snepSessionHandle = NULL;

    /* Flush the server Inbox */
    phOsal_FreeMemory(serverInbox.pbBuff);
    serverInbox.dwBuffLength = 0;
    serverInbox.pbBuff = NULL;

    LOG_FUNCTION_EXIT;
    //return dwDtaStatus;
    return mwIfStatus;
}

DTASTATUS phDtaLibi_snepServerExtended(phDtaLib_snepExtServerTestCases serverCaseID)
{
    MWIFSTATUS mwIfStatus = MWIFSTATUS_FAILED;
    serverName.pbBuff = (uint8_t *)&snepServerName;
    serverName.dwBuffLength = strlen((char *)snepServerName);
    snepServerConfigInfo.SnepServerName = &serverName;
    snepServerConfigInfo.SnepServerType = phMwIf_SnepServer_NonDefault;
    snepServerConfigInfo.sOptions.miu = 1000;
    snepServerConfigInfo.sOptions.rw = 5;
    LOG_FUNCTION_ENTRY;

    print_extended_server_caseID(serverCaseID);
    snepSessionHandle = (void *)malloc(sizeof(uint8_t *));
    if(NULL == (*snepSessionHandle))
    {
        PH_LOG_DTALIB_CRIT_STR("DTALib SNEP> : Memory allocation failed in client\n");
        return DTASTATUS_FAILED;
    }

    mwIfStatus = phMwIf_SnepServer_Init(&snepServerConfigInfo,snepSessionHandle);
    if(MWIFSTATUS_SUCCESS == mwIfStatus)
    {
        mwIfStatus = phMwIf_SnepServer_Accept(snepSessionHandle, &phDtaLib_serverInboxCb, &phDtaLib_getMessageCb);
    }

    if(MWIFSTATUS_SUCCESS == mwIfStatus)
    {
        switch(serverCaseID)
        {
            case PH_DTA_TC_S_ACC_BV_05_0 :
            case PH_DTA_TC_S_ACC_BV_05_1 :
            case PH_DTA_TC_S_ACC_BV_05_2 :
                {
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    if(MWIFSTATUS_LINK_DEACTIVATED == mwIfStatus)
                    {
                        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
                    }
                }
                break;
            case PH_DTA_TC_S_ACC_BV_01 :
            case PH_DTA_TC_S_ACC_BV_03 :
                {
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    if(MWIFSTATUS_PUT_REQUEST == mwIfStatus)
                    {
                        mwIfStatus = phMwIf_SnepServer_Response(MWIFSTATUS_SUCCESS, NULL);
                    }
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    if(MWIFSTATUS_LINK_DEACTIVATED == mwIfStatus)
                    {
                        //phOsal_Delay(200);
                        phMwIf_SnepServer_DeInit(snepSessionHandle);
                        /* Print the result, i.e received PUT message */
                        phDta_printMessage(serverInbox, MWIFSTATUS_PUT_REQUEST);
                    }
                }
                break;
            case PH_DTA_TC_S_RET_BV_01 :
            case PH_DTA_TC_S_RET_BI_01 :
            case PH_DTA_TC_S_RET_BV_03 :
                {
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    if(MWIFSTATUS_PUT_REQUEST == mwIfStatus)
                    {/* Received a PUT request from client */
                        mwIfStatus = phMwIf_SnepServer_Response(MWIFSTATUS_SUCCESS, NULL);
                    }
                    /* wait for GET request */
                    if(MWIFSTATUS_SUCCESS == mwIfStatus)
                    {
                        mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    }
                    if(MWIFSTATUS_GET_REQUEST == mwIfStatus)
                    {
                        /* Search the inbox to check if requested message is available */
                        if(strstr((char *)serverInbox.pbBuff, (char *)getRequest.pbBuff) != NULL)
                        {
                            mwIfStatus = phMwIf_SnepServer_Response(MWIFSTATUS_SUCCESS, &serverInbox);
                        }
                        else
                        {
                            PH_LOG_DTALIB_CRIT_STR("Requested message not found in Inbox\n");
                        }
                    }
                    else if(PH_DTA_REQUEST_REJECTED == mwIfStatus)
                    {
                        /* Received an improper request from Client, so disconnect it */
                        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
                        break;
                    }
                    else
                    {
                        mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    }
                    if((MWIFSTATUS_LINK_DEACTIVATED == mwIfStatus)  || (PH_DTA_INVALID_REQUEST == mwIfStatus) || (0x00 == mwIfStatus))
                    {
                        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
                           /* Print the result, i.e received PUT message */
                        phDta_printMessage(serverInbox, MWIFSTATUS_PUT_REQUEST);
                    }
                }
                break;
            case PH_DTA_TC_S_RET_BV_02 :
                {
                    mwIfStatus = phMwIf_SnepServer_GenericWait(snepSessionHandle);
                    if(MWIFSTATUS_GET_REQUEST == mwIfStatus)
                    {
                        /* Received request from Client. Search inbox for the message. */
                        if(strstr((char *)serverInbox.pbBuff, (char *)getRequest.pbBuff) == NULL)
                        {
                            /* Message Not Found. Send NOT FOUND RESPONSE */
                            PH_LOG_DTALIB_INFO_STR("Sending Not found response");
                            mwIfStatus = phMwIf_SnepServer_Response(PH_DTA_RESPONSE_NOT_FOUND, NULL);
                        }
                    }
                    if(MWIFSTATUS_LINK_DEACTIVATED == mwIfStatus)
                    {
                        mwIfStatus = phMwIf_SnepServer_DeInit(snepSessionHandle);
                    }
                }
                break;
        }
    }

    /* Clear resources */
    free(snepSessionHandle);
    snepSessionHandle = NULL;
    /* Flush the server Inbox */
    phOsal_FreeMemory(serverInbox.pbBuff);
    serverInbox.dwBuffLength = 0;
    serverInbox.pbBuff = NULL;

    LOG_FUNCTION_EXIT;
    //return dwDtaStatus;
    return mwIfStatus;
}

static void phDta_printMessage(phMwIf_sBuffParams_t message, uint16_t request)
{
    uint16_t bIndex;
    uint16_t offset;
    offset = ((MWIFSTATUS_PUT_REQUEST == request)? (NDEF_RTD_OFFSET):(GET_REQ_RTD_OFFSET));
    PH_LOG_DTALIB_INFO_STR("\nMessage Received\n");
    PH_LOG_DTALIB_INFO_STR("=================================================================\n");
    for(bIndex = NDEF_RTD_OFFSET;bIndex < message.dwBuffLength;bIndex++)
    {
        printf("%c",message.pbBuff[bIndex]);
    }
    printf("\n\n");
    PH_LOG_DTALIB_INFO_STR("=================================================================");

}

static void print_default_server_caseID(phDtaLib_snepDefServerTestCases serverCaseID)
{
    PH_LOG_DTALIB_INFO_STR("===================================================");
    switch(serverCaseID)
    {
    case PH_DTA_TC_S_BIT_BV_01 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("=================  TC_S_BIT_BV_01 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BV_02 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("=================  TC_S_BIT_BV_02 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BV_03 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("=================  TC_S_BIT_BV_03 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BI_01_0 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_BIT_BI_01_0 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BI_01_1 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_BIT_BI_01_1 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BI_01_2 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_BIT_BI_01_2 =================");
        }
        break;
    case PH_DTA_TC_S_BIT_BI_02 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================  TC_S_BIT_BI_02 ==================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_02 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_ACC_BV_02 =================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_04 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================  TC_S_ACC_BV_04 ==================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_07 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================  TC_S_ACC_BV_07 ==================");
        }
        break;
    }
    PH_LOG_DTALIB_INFO_STR("===================================================");
}

static void print_extended_server_caseID(phDtaLib_snepExtServerTestCases serverCaseID)
{
    PH_LOG_DTALIB_INFO_STR("===================================================");
    switch(serverCaseID)
    {
    case PH_DTA_TC_S_ACC_BV_01 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("=================  TC_S_ACC_BV_01 =================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_03 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("=================  TC_S_ACC_BV_03 =================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_05_0 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================ TC_S_ACC_BV_05_0 =================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_05_1 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_ACC_BV_05_1 =================");
        }
        break;
    case PH_DTA_TC_S_ACC_BV_05_2 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("===============  TC_S_ACC_BV_05_2 =================");
        }
        break;
    case PH_DTA_TC_S_RET_BV_01 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================= TC_S_RET_BV_01 ==================");
        }
        break;
    case PH_DTA_TC_S_RET_BV_02 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================ TC_S_RET_BV_02 ===================");
        }
        break;
    case PH_DTA_TC_S_RET_BV_03 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================= TC_S_RET_BV_03 ==================");
        }
        break;
    case PH_DTA_TC_S_RET_BI_01 :
        {
            PH_LOG_DTALIB_INFO_STR("======== Executing SNEP Default Server Test =======");
            PH_LOG_DTALIB_INFO_STR("================= TC_S_RET_BI_01 ==================");
        }
        break;
    }
    PH_LOG_DTALIB_INFO_STR("===================================================");
}

#ifdef __cplusplus
}
#endif
#endif /* WIN32 */
