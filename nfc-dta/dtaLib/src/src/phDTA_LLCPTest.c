/*
* Copyright (C) 2015-2019 NXP Semiconductors
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

/*
 * phDTA_LLCPTest.c
 *
 *  Created on: Jul 7, 2014
 *      Author: nxp
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

#define PHDTALIB_SERVER_MIU ((uint16_t)0x87E) /*IUT srever Maximum Information Unit supported (MIUX = 0x07FF, MIU = MIUX + 128).*/
#define PHDTALIB_SERVER_RW  ((uint8_t)01)
#define PHDTALIB_CLIENT_MIU ((uint16_t)0x87E)/*IUT client Maximum Information Unit supported (MIUX = 0x07FF, MIU = MIUX + 128).*/
#define PHDTALIB_CLIENT_RW  ((uint8_t)01)
#define PHDTALIB_CLIENT_MIU_DEFAULT ((uint16_t)128)/*IUT client default MIU.*/
#define PHDTALIB_SERVER_MIU_DEFAULT ((uint16_t)128)/*IUT Server default MIU.*/

#define PHDTALIB_ADDITIONAL_LLCP_DATA 0x0800 /*Length of ADDITIONAL data other then MIUX data for loopback*/
#define PHDTALIB_MIUX_LLCP_DATA 0x07FF       /*Length of the MIUX data for loopback*/
#define PHDTALIB_MAX_LLCP_DATA (PHDTALIB_MIUX_LLCP_DATA + PHDTALIB_ADDITIONAL_LLCP_DATA)

extern  phDtaLib_sHandle_t g_DtaLibHdl;

/**
 * Operations related to LLCP starts below
 */
DTASTATUS phDtaLibi_LlcpOperations(phDtaLib_sTestProfile_t* psTestProfile,
                                   phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                   phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo)
{
    MWIFSTATUS dwMwIfStatus;
    phDtaLib_sHandle_t *psDtaLibHdl = &g_DtaLibHdl;
    phMwIf_sLlcpClientConnectParams_t  sConnectPrms;
    uint8_t                            bRemoteSapCoEchoOut;
    phMwIf_sBuffParams_t               sLlcpConnOrientedData;

    uint16_t                    wClientMiu;
    uint16_t                    wServerMiu;
    LOG_FUNCTION_ENTRY;

    if(eLlcpEvtType == PHMWIF_LLCP_SERVER_CONN_REQ_EVT)
    {
        /*Accept the connection request*/
        phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Accepting Client Connection with Following Params");
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Client SAP=0x",puLlcpEvtInfo->sConnReq.bRemoteSap);
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Client RW=0x",puLlcpEvtInfo->sConnReq.bRemoteRw);
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Client MIU=0x",puLlcpEvtInfo->sConnReq.wRemoteMiu);
        psDtaLibHdl->pvCORemoteClientConnHandle = ((puLlcpEvtInfo->sConnReq.wConnHandle));
        /*Sending default MIU to middleware so that it doesn't send connect PDU params*/
        if(psDtaLibHdl->p2pType == PHDTALIB_P2P_LLCP_WITHOUT_CONN_PARAMS)
        {
            wClientMiu = PHDTALIB_CLIENT_MIU_DEFAULT;
            wServerMiu = PHDTALIB_SERVER_MIU_DEFAULT;
        }
        else
        {
            wClientMiu = PHDTALIB_CLIENT_MIU;
            wServerMiu = PHDTALIB_SERVER_MIU;
        }
        dwMwIfStatus = phMwIf_LlcpServerAcceptClientConn(psDtaLibHdl->mwIfHdl,
                                                         ((puLlcpEvtInfo->sConnReq.wServerHandle)),
                                                         wServerMiu,
                                                         PHDTALIB_SERVER_RW,
                                                         ((puLlcpEvtInfo->sConnReq.wConnHandle)));
        if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Accept Connection", (const uint8_t*)__FUNCTION__);
            return dwMwIfStatus;
        }
        if(psTestProfile->Pattern_Number == PHDTALIB_LLCP_CO_SET_NAME_OR_CL)
        {/*Connect to server by Service name*/
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:CO:Connect by name ");
            /*Connect client to Remote Server*/
            phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Connecting Client to LLCP Remote Server SAP=0x",PHDTALIB_SAP_LT_CO_OUT_DEST);
            sConnectPrms.eConnectType         = PHMWIF_SERVER_CONNECT_BY_NAME;
            sConnectPrms.bSap                 = 0;
            sConnectPrms.pbServerServiceName  = (uint8_t *)gs_abOutboundCoService;
            sConnectPrms.pfMwIfLlcpClientCb   = (phMWIf_LlcpEvtCb_t)phDtaLibi_EvtCb;
            sConnectPrms.pvApplHdl            = (void*)psDtaLibHdl;
            sConnectPrms.wClientMiu           = wClientMiu;
            sConnectPrms.bClientRw            = PHDTALIB_CLIENT_RW;
            dwMwIfStatus = phMwIf_LlcpConnOrientedClientConnect(psDtaLibHdl->mwIfHdl,
                                                    &sConnectPrms,
                                                    (void **)&psDtaLibHdl->pvCORemoteServerConnHandle);

        }
        else if(psTestProfile->Pattern_Number == PHDTALIB_LLCP_CO_SET_SNL_OR_CL)
        {/*If pattern is to get the SAP using SNL and then connect, Do SDP and get the SAP*/
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:CO:Getting SAP using SDP");
            dwMwIfStatus = phMwIf_LlcpServiceDiscovery( psDtaLibHdl->mwIfHdl,
                                                        (uint8_t *)"urn:nfc:sn:dta-co-echo-out",
                                                        (uint8_t *)&bRemoteSapCoEchoOut);
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not connect to Remote client", (const uint8_t*)__FUNCTION__);
                return dwMwIfStatus;
            }
            /*Connect client to Remote Server*/
            phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Connecting Client to LLCP Remote Server SAP=0x",PHDTALIB_SAP_LT_CO_OUT_DEST);
            sConnectPrms.eConnectType         = PHMWIF_SERVER_CONNECT_BY_SAP;
            sConnectPrms.bSap                 = bRemoteSapCoEchoOut;
            sConnectPrms.pbServerServiceName  = NULL;
            sConnectPrms.pfMwIfLlcpClientCb   = (phMWIf_LlcpEvtCb_t)phDtaLibi_EvtCb;
            sConnectPrms.pvApplHdl            = (void*)psDtaLibHdl;
            sConnectPrms.wClientMiu           = wClientMiu;
            sConnectPrms.bClientRw            = PHDTALIB_CLIENT_RW;
            dwMwIfStatus = phMwIf_LlcpConnOrientedClientConnect(psDtaLibHdl->mwIfHdl,
                                                    &sConnectPrms,
                                                    (void **)&psDtaLibHdl->pvCORemoteServerConnHandle);

        }
        else
        {
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:CO:SAP value set to LT_CO_OUT_DEST");
            bRemoteSapCoEchoOut = PHDTALIB_SAP_LT_CO_OUT_DEST;
            /*Connect client to Remote Server*/
            phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Connecting Client to LLCP Remote Server SAP=0x",PHDTALIB_SAP_LT_CO_OUT_DEST);
            sConnectPrms.eConnectType         = PHMWIF_SERVER_CONNECT_BY_SAP;
            sConnectPrms.bSap                 = bRemoteSapCoEchoOut;
            sConnectPrms.pbServerServiceName  = NULL;
            sConnectPrms.pfMwIfLlcpClientCb   = (phMWIf_LlcpEvtCb_t)phDtaLibi_EvtCb;
            sConnectPrms.pvApplHdl            = (void*)psDtaLibHdl;
            sConnectPrms.wClientMiu           = wClientMiu;
            sConnectPrms.bClientRw            = PHDTALIB_CLIENT_RW;
            dwMwIfStatus = phMwIf_LlcpConnOrientedClientConnect(psDtaLibHdl->mwIfHdl,
                                                    &sConnectPrms,
                                                    (void **)&psDtaLibHdl->pvCORemoteServerConnHandle);
        }
        if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP:Could not connect to Remote Server", (const uint8_t*)__FUNCTION__);
            phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP:Disconnecting Previous Connection to Remote client", (const uint8_t*)__FUNCTION__);
            dwMwIfStatus = phMwIf_LlcpConnOrientedClientDisconnect(psDtaLibHdl->mwIfHdl,
                                                    (puLlcpEvtInfo->sConnReq.wConnHandle));
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Disconnect to Remote client", (const uint8_t*)__FUNCTION__);
                return dwMwIfStatus;
            }
        }
    }

    if(eLlcpEvtType == PHMWIF_LLCP_SERVER_CONN_ORIENTED_DATA_EVT)
    {
        /*Accept the connection request*/
        phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:ConnectionOrientedServerParams:");
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Client SAP=",(uint32_t)puLlcpEvtInfo->sLlcpConnOrientedData.bRemoteSap);
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:ConnOrientedRemoteHandle=",(uint32_t)puLlcpEvtInfo->sLlcpConnOrientedData.wConnOrientedHandle);
        phOsal_LogDebugPtrh((const uint8_t*)"DTALib>LLCP:psDtaLibHdl->pvCORemoteClientConnHandle=",psDtaLibHdl->pvCORemoteClientConnHandle);
        psDtaLibHdl->pvCOServerHandle = (puLlcpEvtInfo->sConnReq.wServerHandle);

        phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Waiting for Data from Remote Client");
        sLlcpConnOrientedData.pbBuff       = (uint8_t *)malloc(PHDTALIB_MAX_LLCP_DATA);
        sLlcpConnOrientedData.dwBuffLength = PHDTALIB_MAX_LLCP_DATA;

        dwMwIfStatus = phMwIf_LlcpConnOrientedRecvData(psDtaLibHdl->mwIfHdl,
                                           psDtaLibHdl->pvCORemoteClientConnHandle,
                                           &sLlcpConnOrientedData);

        if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not connect to Remote client", (const uint8_t*)__FUNCTION__);
            return dwMwIfStatus;
        }
        phDtaLibi_PrintBuffer(sLlcpConnOrientedData.pbBuff,(uint16_t)sLlcpConnOrientedData.dwBuffLength,(const uint8_t*)"DTALib>LLCP:Received Data");
        if(psDtaLibHdl->bIsLlcpCoRemoteServerLinkCongested)
        {
            phMwIf_sBuffParams_t               *psLlcpConnOrientedData = (phMwIf_sBuffParams_t *)malloc(sizeof(phMwIf_sBuffParams_t));
            psLlcpConnOrientedData->pbBuff = sLlcpConnOrientedData.pbBuff;
            psLlcpConnOrientedData->dwBuffLength = sLlcpConnOrientedData.dwBuffLength;

            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP: link is congested, data pushed to queue");
            phOsal_QueuePush(psDtaLibHdl->qHdlCongestData, psLlcpConnOrientedData, 0);
            return DTASTATUS_SUCCESS;
        }
        else
        {
            /*Its a loopback data.Send it back*/
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Waiting for TDELAY_CL");
            phOsal_Delay(TDELAY_CL_IN_MS);
            /*For LLCP loopback testing, Data is sent to Remote Server only if Remote Client is also active*/
            if(psDtaLibHdl->bIsLlcpCoRemoteClientDisconnected != TRUE )
            {
                dwMwIfStatus = phMwIf_LlcpConnOrientedSendData(psDtaLibHdl->mwIfHdl,
                                               psDtaLibHdl->pvCORemoteServerConnHandle,
                                               &sLlcpConnOrientedData);

                if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
                {
                    phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Send ConnOriented Data", (const uint8_t*)__FUNCTION__);
                    return dwMwIfStatus;
                }
            }
            if(sLlcpConnOrientedData.pbBuff != NULL){
                free(sLlcpConnOrientedData.pbBuff);
                sLlcpConnOrientedData.pbBuff = NULL;
            }
        }
    }
    if(eLlcpEvtType == PHMWIF_LLCP_P2P_LINK_UNCONGESTED_EVT)
    {
        phDtaLibi_LlcpHandleUncongestedEvent();
    }

    LOG_FUNCTION_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * Operations to be done after remote client disconnects from local connection oriented server
 */
DTASTATUS phDtaLibi_LlcpHandleCoClientDisconnect(phDtaLib_sTestProfile_t* psTestProfile,
                                                 phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                                 phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo)
{
    MWIFSTATUS dwMwIfStatus;
    phDtaLib_sHandle_t *psDtaLibHdl = &g_DtaLibHdl;
    LOG_FUNCTION_ENTRY;
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP Client Disconnect Event:",eLlcpEvtType);
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP Reason for Disconnect:",puLlcpEvtInfo->sDisconnect.bReasonForDisconnect);
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP Pattern Number:",psTestProfile->Pattern_Number);
    /*Disconnect Client from Remote Server*/
    dwMwIfStatus = phMwIf_LlcpConnOrientedClientDisconnect(psDtaLibHdl->mwIfHdl,
                                                           (void *)psDtaLibHdl->pvCORemoteServerConnHandle);
    if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
    {
        phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Disconnect from Remote server", (const uint8_t*)__FUNCTION__);
        return dwMwIfStatus;
    }

    LOG_FUNCTION_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * Operations related to LLCP starts below
 */
DTASTATUS phDtaLibi_LlcpConnLessOperations(phDtaLib_sTestProfile_t* psTestProfile,
                                           phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                           phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo)
{
    MWIFSTATUS dwMwIfStatus;
    phDtaLib_sHandle_t*  psDtaLibHdl = &g_DtaLibHdl;
    phMwIf_sBuffParams_t sLlcpConnLessData;
    uint8_t              bRemoteSapClEchoOut=0;
    uint8_t              bRemoteClientSap=0;
    uint8_t const abStartOfLlcpTestCmd[] = { 0x53, 0x4F, 0x54 };
    LOG_FUNCTION_ENTRY;
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP Pattern Number:",psTestProfile->Pattern_Number);

    if(eLlcpEvtType == PHMWIF_LLCP_SERVER_CONN_LESS_DATA_EVT)
    {
        /*Accept the connection request*/
        phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:ConnectionLessServerParams:");
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Client SAP=",(uint32_t)puLlcpEvtInfo->sLlcpConnLessData.bRemoteSap);
        phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:ConnLessRemoteHandle=",(uint32_t)puLlcpEvtInfo->sLlcpConnLessData.wConnLessHandle);
        psDtaLibHdl->pvCLRemoteClientConnLessHandle = INT_TO_PTR(puLlcpEvtInfo->sLlcpConnLessData.wConnLessHandle);
        bRemoteClientSap = puLlcpEvtInfo->sLlcpConnLessData.bRemoteSap;

        /*Wait for Data to start LLCP Transaction*/
        phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Waiting for Data from Remote Client");
        sLlcpConnLessData.pbBuff       = (uint8_t *)malloc(PHDTALIB_MAX_LLCP_DATA);
        sLlcpConnLessData.dwBuffLength = PHDTALIB_MAX_LLCP_DATA;
        dwMwIfStatus = phMwIf_LlcpConnLessRecvData(psDtaLibHdl->mwIfHdl,
                                           psDtaLibHdl->pvCLRemoteClientConnLessHandle,
                                           &bRemoteClientSap,
                                           &sLlcpConnLessData);
        if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
        {
            phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not connect to Remote client", (const uint8_t*)__FUNCTION__);
            return dwMwIfStatus;
        }

        /*Check if the data received is to indicate start of LLCP test
         * If its the start, Send SNL*/
        phDtaLibi_PrintBuffer(sLlcpConnLessData.pbBuff,(uint16_t)sLlcpConnLessData.dwBuffLength,(const uint8_t*)"DTALib>LLCP:Received Data");
        if ((memcmp((const void *) sLlcpConnLessData.pbBuff,
                    (const void *) abStartOfLlcpTestCmd,
                           sizeof(abStartOfLlcpTestCmd)) == 0))
        {
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Received Start Of Test Cmd ");
            dwMwIfStatus = phMwIf_LlcpServiceDiscovery( psDtaLibHdl->mwIfHdl,
                                                        (uint8_t *)"urn:nfc:sn:dta-cl-echo-out",
                                                        (uint8_t *)&bRemoteSapClEchoOut);
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not connect to Remote client", (const uint8_t*)__FUNCTION__);
                return dwMwIfStatus;
            }
            phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:RemoteClEchoOut Sap value=",bRemoteSapClEchoOut);
            psDtaLibHdl->bRemoteSapClEchoOut = bRemoteSapClEchoOut;
        }
        else
        {/*Its a loopback data.Send it back*/
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP:Waiting for TDELAY_CL");
            phOsal_Delay(TDELAY_CL_IN_MS);
            phOsal_LogDebugU32h((const uint8_t*)"DTALib>LLCP:Send Loopback Data back to SAP:",bRemoteSapClEchoOut);
            dwMwIfStatus = phMwIf_LlcpConnLessSendData(psDtaLibHdl->mwIfHdl,
                                                       psDtaLibHdl->bRemoteSapClEchoOut,
                                                       &sLlcpConnLessData);
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Send Connless Data", (const uint8_t*)__FUNCTION__);
                return dwMwIfStatus;
            }
            if(sLlcpConnLessData.pbBuff){
                free(sLlcpConnLessData.pbBuff);
                sLlcpConnLessData.pbBuff = NULL;
            }
        }
    }

    LOG_FUNCTION_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * Function to Handle Uncongested event.
 * Congested event is provided by MwIf when the one of the data link connection
 * cannot process to send next data. In this case Data is pushed to the queue.
 * When Uncongested event is received, the data is pulled from the queue and
 * sent to MwIf.
 */
DTASTATUS phDtaLibi_LlcpHandleUncongestedEvent()
{
    phDtaLib_sHandle_t*  psDtaLibHdl = &g_DtaLibHdl;
    OSALSTATUS                  dwOsalStatus = 0;
    MWIFSTATUS dwMwIfStatus;
    LOG_FUNCTION_ENTRY;
    while(psDtaLibHdl->bIsLlcpCoRemoteServerLinkCongested == FALSE)
    {
            phMwIf_sBuffParams_t               *psLlcpConnOrientedData;
            phOsal_LogDebug((const uint8_t*)"DTALib>LLCP: Pulling Data from Q");
            dwOsalStatus = phOsal_QueuePull(psDtaLibHdl->qHdlCongestData,(void**) &psLlcpConnOrientedData,10);
            if (dwOsalStatus != 0) {
                phOsal_LogErrorU32h((const uint8_t*)"Status = ", dwOsalStatus);
                if(dwOsalStatus == OSALSTATUS_Q_UNDERFLOW){
                    phOsal_LogDebug((const uint8_t*)"DTALib>LLCP: Q UNDERFLOW");
                }
                else{
                    phOsal_LogErrorString((const uint8_t*)"DTALib> : Error while pulling data from Q, Exiting-QueuPull ", (const uint8_t*)__FUNCTION__);
                }
                return DTASTATUS_FAILED;
            }

            phDtaLibi_PrintBuffer(psLlcpConnOrientedData->pbBuff,(uint16_t)psLlcpConnOrientedData->dwBuffLength,(const uint8_t*)"DTALib>LLCP:Pulled Data from queue");
            dwMwIfStatus = phMwIf_LlcpConnOrientedSendData(psDtaLibHdl->mwIfHdl,
                                           psDtaLibHdl->pvCORemoteServerConnHandle,
                                           psLlcpConnOrientedData);
            if (dwMwIfStatus != MWIFSTATUS_SUCCESS)
            {
                phOsal_LogErrorString((const uint8_t*)"DTALib>LLCP::Could not Send ConnOriented Data", (const uint8_t*)__FUNCTION__);
                if((psLlcpConnOrientedData != NULL) && (psLlcpConnOrientedData->pbBuff !=NULL)){
                    free(psLlcpConnOrientedData->pbBuff);
                    free(psLlcpConnOrientedData);
                }
                return dwMwIfStatus;
            }
            if((psLlcpConnOrientedData != NULL) && (psLlcpConnOrientedData->pbBuff !=NULL)){
                free(psLlcpConnOrientedData->pbBuff);
                psLlcpConnOrientedData->pbBuff = NULL;
                free(psLlcpConnOrientedData);
                psLlcpConnOrientedData = NULL;
            }
            phOsal_Delay(50);/*Delay added for processing last sent I-PDU*/
    }
    LOG_FUNCTION_EXIT;
    return DTASTATUS_SUCCESS;
}

/**
 * Function to handle LLCP de-activation
 * Flushes all the data present in the congested Queue
 */
DTASTATUS phDtaLibi_LlcpHandleDeactivatedEvent()
{
    phDtaLib_sHandle_t*  psDtaLibHdl = &g_DtaLibHdl;
    OSALSTATUS                  dwOsalStatus = 0;
    phMwIf_sBuffParams_t               *psLlcpConnOrientedData;
    LOG_FUNCTION_ENTRY;
    while(1){
        dwOsalStatus = phOsal_QueuePull(psDtaLibHdl->qHdlCongestData,(void**) &psLlcpConnOrientedData,10);
        if (dwOsalStatus != 0) {
            phOsal_LogErrorU32h((const uint8_t*)"Status = ", dwOsalStatus);
            if(dwOsalStatus == OSALSTATUS_Q_UNDERFLOW){
                phOsal_LogDebug((const uint8_t*)"DTALib>LLCP: Q UNDERFLOW");
                return DTASTATUS_SUCCESS;
            }
            else{
                phOsal_LogErrorString((const uint8_t*)"DTALib> : Error while pulling data from Q, Exiting-QueuPull ", (const uint8_t*)__FUNCTION__);
                return DTASTATUS_FAILED;
            }
        }
        if((psLlcpConnOrientedData != NULL) && (psLlcpConnOrientedData->pbBuff !=NULL)){
            free(psLlcpConnOrientedData->pbBuff);
            psLlcpConnOrientedData->pbBuff = NULL;
            free(psLlcpConnOrientedData);
            psLlcpConnOrientedData = NULL;
        }
    }

    LOG_FUNCTION_EXIT;
}

#ifdef __cplusplus
}
#endif
