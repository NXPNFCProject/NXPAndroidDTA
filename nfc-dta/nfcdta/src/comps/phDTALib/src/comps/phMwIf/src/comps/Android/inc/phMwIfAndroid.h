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

#ifndef PHMWIFANDROID_H_
#define PHMWIFANDROID_H_

#include "phMwIf.h"
#include "errno.h"
#ifdef __cplusplus
extern "C" {
#endif

/** section MACRO DEFINES */

#define MAX_NUM_EE 8
#define MAX_NXPCFG_RSP_DATA 32
#define NFCID_LEN       8

/* Default LTO is 100ms. It should be sufficiently larger than RWT
Logically RWT should be larger than LTO*/
#define PHMWIF_LLCP_LTO_VALUE           1000 /*1000ms*/
#define PHMWIF_LLCP_WAITING_TIME        10   /*(256*16/Fc)*2^WT*/
#define PHMWIF_LLCP_DELAY_RESP_TIME     100  /*100ms*/
#define OSP_ENABLE                      0 /*Enable L-OSP specific API updates*/
#define ENABLE_AGC_DEBUG                0
#define FREQ_AGC_CMD                    500  /*AGC Debug command is sent every 500ms*/

#undef UNUSED
#define UNUSED(X) (void)(X) /**< To Mask the Unused Variables in functions */

/**< Error handler with PERR Setting and exit on failure */
#define PH_HANDLE_ERROR_EN(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

/**< Error handler with PERR Setting - No Exit */
#define PH_HANDLE_ERROR(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

/**< Generic Error Exit for wrong Status */
#define PH_ON_ERROR_EXIT(status,exitcode,message) \
    if(gx_status != status)\
    {\
      ALOGE (message);\
      ALOGE ("Error!! Status=0x%x",gx_status);\
      return gx_status;\
    }

/**< Error Comparison with Print */
#define PH_ON_ERROR_PRINT(status,message) \
    if(gx_status != status)\
    {\
      ALOGE (message);\
    }

/**< Error Comparison with Return and Print */
#define PH_ON_ERROR_RETURN(status,retstatus,message) \
    if(retstatus != status)\
    {\
      ALOGE (message);\
      return retstatus;\
    }
/**< Break if there is an error in status */
#define PH_ON_ERROR_BREAK(status,retstatusok) \
    if((status)!=retstatusok)break

typedef struct tNFA_NXPCFG_EVT_DATA_
{
    uint32_t            dwSize;
    uint8_t             abCfgRspData[MAX_NXPCFG_RSP_DATA];
}tNFA_NXPCFG_EVT_DATA;

typedef union phMwIf_uCbEvtData
{
    tNFA_HCI_EVT_DATA  sHciEvtData;
    tNFA_EE_CBACK_DATA sEeCbData;
    tNFA_CONN_EVT_DATA sConnCbData;
    tNFA_DM_CBACK_DATA sDevMgmtCbData;
    tNFA_NDEF_EVT_DATA sNdefEvtData;
    tNFA_P2P_EVT_DATA  sP2pEvtData;
    tNFA_NXPCFG_EVT_DATA sNxpCfgEvtData;
}phMwIf_uCbEvtData_t;

typedef struct phMwIf_sQueueData
{
    uint32_t             dwEvtType;
    phMwIf_uCbEvtData_t  uEvtData;
}phMwIf_sQueueData_t;

/* Wait for Specific event from Middleware */
#define PH_WAIT_FOR_CBACK_EVT(queueHdl,event,timeout,message,qdata) \
do{\
    OSALSTATUS dwStatus;\
    phMwIf_sQueueData_t*         psQueueData;\
    dwStatus = phOsal_QueuePull(queueHdl,(void**)&psQueueData, timeout);\
    if (dwStatus == OSALSTATUS_Q_UNDERFLOW) {\
            ALOGE("MwIf> %s:%d Timeout receiving event:%d", __FUNCTION__,__LINE__,event);\
            ALOGE(message);\
            return MWIFSTATUS_FAILED;\
        }\
    if (dwStatus != OSALSTATUS_SUCCESS) {\
        ALOGE("MwIf> %s:%d Error in receiving event:%d", __FUNCTION__,__LINE__,event);\
        ALOGE(message);\
        return MWIFSTATUS_FAILED;\
    }\
    g_dwEvtType = psQueueData->dwEvtType;\
    if(((g_dwEvtType == NFA_DEACTIVATED_EVT) && (event != NFA_DEACTIVATED_EVT)) || (g_dwEvtType == NFA_RW_INTF_ERROR_EVT)){\
        ALOGE("MwIf> %s:%d Recvd Deactivated Evt while waiting for Evt %d", __FUNCTION__,__LINE__,event);\
        if(event == NFA_RF_DISCOVERY_STOPPED_EVT){\
            ALOGE("MwIf>Deactivated Evt expected during Disc stop");}\
        else {ALOGE(message);\
              free(psQueueData);\
            return MWIFSTATUS_FAILED;}}\
    ALOGE("MWIf>>Recvd Evt=%d while waiting for evt=%d",g_dwEvtType,event);\
    *(qdata) = *psQueueData;\
    free(psQueueData);\
}while(g_dwEvtType != event)

/** end section MACRO DEFINES */

typedef enum eDtaDeviceState
{
    DEVICE_IN_IDLE_STATE = 0x00,
    DEVICE_IN_DISCOVERY_STARTED_STATE = 0X01,
    DEVICE_IN_POLL_ACTIVE_STATE = 0X02
}eDtaDeviceState_t;

typedef struct nci_datax
{
    unsigned char responseType;
    unsigned char groupId;
    unsigned char opcodeId;
    unsigned int  dataLength;
    unsigned char nciBuffer[256];
}tNCI_Data;

typedef struct phMwIf_llcpServerPrms
{
    phMwIf_sLlcpSrvrRegnParams_t      sSrvrRegnPrms;
    tNFA_HANDLE                       wServerHandle;
    tNFA_HANDLE                       wRemoteClientConnHandle;
    uint16_t                          wRemoteClientMIU;
    uint8_t                           wRemoteClientRW;
}phMwIf_llcpServerPrms_t;

typedef struct phMwIf_llcpClientPrms
{
    phMwIf_sLlcpClientConnectParams_t sLlcpClientConnectPrms;
    tNFA_HANDLE                       wClientHandle;
    tNFA_HANDLE                       wRemoteServerConnHandle;
    uint16_t                          wRemoteServerMIU;
    uint8_t                           wRemoteServerRW;
}phMwIf_llcpClientPrms_t;

typedef struct phMwIf_llcpPrms
{
    phMwIf_llcpServerPrms_t     sConnOrientedServer;
    phMwIf_llcpClientPrms_t     sConnOrientedClient;
    phMwIf_llcpServerPrms_t     sConnLessServer;
    phMwIf_llcpClientPrms_t     sConnLessClient;
    phMwIf_llcpClientPrms_t     sConnLessSDPClient;
    phMwIf_sLlcpInitParams_t    sLlcpInitPrms;
}phMwIf_sLlcpPrms_t;

typedef struct phMwIf_sHandle
{
    void*                       nfcAdapter;
    tHAL_NFC_ENTRY*             halentrypoint;
    phMwIf_sDiscCfgPrms_t       sDiscCfg;
    uint8_t                     numNfcEE;
    tNFA_EE_INFO                infoNfcEE[MAX_NUM_EE];
    void*                       appCbHdl;
    phMWIf_EvtCb_t              mwIfCb;
    phMwIf_sNdefDetectParams_t  sNdefDetectParams;
    void*                       pvQueueHdl;
    phMwIf_sLlcpPrms_t          sLlcpPrms;
    phMwIf_sQueueData_t         sLastQueueData;       /**< Last data fetched from queue*/
    void*                       pvSemIntgrnThrd;      /**< Semaphore to sync between Integration and MW callback */
    pthread_t                   sIntegrationThrdHdl;  /**< Integration thread handles the required functionality after discovery and before activation*/
    BOOLEAN                     blStopIntegrationThrd;/**< Set to TRUE to stop Integration thread*/
    eDtaDeviceState_t           eDeviceState;
    pthread_t                   sAgcDbgThrdHdl;       /**< Debug thread gets the AgcDebug Values periodically*/
    BOOLEAN                     blStopAgcDbgThrd;     /**< Set to TRUE to stop Agc thread */
    UINT16                      systemCode;           /**< System code used ofr HCE-F command/response */
    uint8_t                     nfcid2[NFCID_LEN];    /**< NFCID2 */
    uint16_t                    nfcHceFHandle;        /**< Handle used for HCEF operations */
    uint32_t                    routingProtocols;     /**< Contains the details of protocols routing */
    BOOLEAN                     bLlcpEnabled;         /**< TRUE If LLCP enabled, FALSE If LLCP disabled*/
}phMwIf_sHandle_t;

#define NCI_MSG_TYPE_MASK         0xE0
#define NCI_MSG_TYPE_SHIFT        5
#define NCI_MSG_TYPE_DATA         0x00
#define NCI_MSG_TYPE_CMD          1   /* (NCI_MT_CMD << NCI_MT_SHIFT) = 0x20 */
#define NCI_MSG_TYPE_RSP          2   /* (NCI_MT_RSP << NCI_MT_SHIFT) = 0x40 */
#define NCI_MSG_TYPE_NTF          3   /* (NCI_MT_NTF << NCI_MT_SHIFT) = 0x60 */
#define NCI_MSG_TYPE_CFG          4

/* GID: Group Identifier (byte 0) */
#define NCI_GROUP_IDF_MASK        0x0F
#define NCI_GROUP_IDF_SHIFT       0
#define NCI_GROUP_IDF_CORE        0x00    /* 0000b NCI Core group */
#define NCI_GROUP_IDF_RF_MANAGE   0x01    /* 0001b RF Management group */
#define NCI_GROUP_IDF_EE_MANAGE   0x02    /* 0010b NFCEE Management group */
#define NCI_GROUP_IDF_PROP        0x0F    /* 1111b Proprietary */

/* OID: Opcode Identifier (byte 0) */
#define NCI_OID_MASK        0x3F
#define NCI_OID_SHIFT       0

/*Datalen: Data Length*/
#define NCI_DATALEN_MASK        0x20

#define NfcParseData(pubuflocal, psmsgtype, psgid, psoid) \
    psmsgtype = (*(pubuflocal) & NCI_MSG_TYPE_MASK) >> NCI_MSG_TYPE_SHIFT; \
    psgid = *(pubuflocal)++ & NCI_GROUP_IDF_MASK;\
    psoid = *(pubuflocal)++ & NCI_OID_MASK;\

#define GetDatainfo(psdatalen, pudataptr); \
    psdatalen = *(pubuflocal)++& 0xFF;\
    pudataptr = pubuflocal ;

typedef void (tNFA_NXP_NCI_RSP_CBACK) (UINT8 event, UINT16 param_len, UINT8 *p_param);

void        phMwIfi_IntegrationThread(void *arg);
void        phMwIfi_IncreaseStackSize(void);
void        phMwIfi_GlobalDtaModeInit(void);
tNFA_STATUS phMwIfi_StackInit(phMwIf_sHandle_t *mwIfHdl);
tNFA_STATUS phMwIfi_SelectDevice();
tNFA_STATUS phMwIfi_StackDeInit();
tNFA_STATUS phMwIfi_ConfigureDriver(phMwIf_sHandle_t *mwIfHdl);
tNFA_STATUS phMwIfi_DeConfigureDriver();
tNFA_STATUS phMwIfi_SetDiscoveryConfig(phMwIf_sDiscCfgPrms_t* discCfgParams,
                                       uint16_t discLoopDurationInMs);
tNFA_STATUS phMwIfi_ResetDiscoveryConfig(phMwIf_sHandle_t *mwIfHdl);
tNFA_STATUS phMwIfi_HciRegister(phMwIf_sHandle_t *mwIfHdl);
tNFA_STATUS phMwIfi_EeGetInfo(phMwIf_sHandle_t *mwIfHdl,
                              phMwIf_eCeDevType_t eDevType);
MWIFSTATUS  phMwIfi_HceConfigNciParams(phMwIf_sHandle_t* mwIfHandle);
MWIFSTATUS  phMwIfi_CeRegisterAID(phMwIf_sHandle_t* mwIfHandle);
void        phMwIfi_NfaNxpNciAgcDbgRspCallback(UINT8 NxpNci_Event,
                                    UINT16 param_len, UINT8 *p_param);
void        phMwIfi_NfaNxpNciPropCommRspCallback(UINT8 NxpNci_Event,
                                    UINT16 param_len, UINT8 *p_param);
void        phMwIfi_NfaConnCallback(UINT8 ConnCb_Event,
                                    tNFA_CONN_EVT_DATA *ConnCb_Event_Data);
void        phMwIfi_NfaDevMgmtCallback(UINT8 DevMgmtCb_Event,
                                       tNFA_DM_CBACK_DATA *DevMgmtCb_Event_Data);
void        phMwIfi_NfaEeCallback(tNFA_EE_EVT xevent,tNFA_EE_CBACK_DATA *px_data);
void        phMwIfi_NfaHciCallback(tNFA_HCI_EVT xevent, tNFA_HCI_EVT_DATA *px_data);
void        phMwIfi_NfaLlcpServerCallback(tNFA_P2P_EVT xevent,tNFA_P2P_EVT_DATA *px_data);
void        phMwIfi_NfaLlcpClientCallback(tNFA_P2P_EVT xevent,tNFA_P2P_EVT_DATA *px_data);
void        phMwIfi_NfaLlcpConnLessServerCallback(tNFA_P2P_EVT eP2pEvent,tNFA_P2P_EVT_DATA *psP2pEventData);
MWIFSTATUS  phMwIfi_RegisterServer(phMwIf_sHandle_t *mwIfHdl,
                                  uint8_t bSap,
                                  tNFA_P2P_LINK_TYPE linkType,
                                  char *serviceName);
MWIFSTATUS  phMwIfi_RegisterClient(phMwIf_sHandle_t *mwIfHdl,
                                 tNFA_P2P_LINK_TYPE linkType);
MWIFSTATUS  phMwIfi_CreateNdefMsg(phMwIf_sHandle_t *mwIfHdl,
                                 uint8_t* pData,
                                 uint32_t dwLength);
void*       phMwIfi_MemAllocCb(void* memHdl, uint32_t size);
int32_t     phMwIfi_MemFreeCb(void* memHdl, void* ptrToMem);
void        phMwIfi_MapRfInterface(UINT8* protocol, int *rf_interface);

void phMwIfi_PrintBuffer(uint8_t *pubuf,uint16_t uilen,const char *pcsmessage);
void phMwIfi_PrintBuffer16(uint16_t *pubuf,uint16_t uilen,const char *pcsmessage);
void phMwIfi_PrintNfaEeEventCode(tNFA_EE_EVT xevent);
void phMwIfi_PrintNfaProtocolCode (tNFC_PROTOCOL xprotocol);
void phMwIfi_PrintDiscoveryType(tNFC_DISCOVERY_TYPE xmode);
void phMwIfi_PrintBitRate(tNFC_BIT_RATE xbitrate);
void phMwIfi_PrintIntfType(tNFC_INTF_TYPE xtype);
void phMwIfi_PrintDMEventCode(UINT8 uevent);
void phMwIfi_PrintNfaStatusCode(tNFA_STATUS xstatus);
void phMwIfi_PrintNfaHciEventCode(tNFA_HCI_EVT xevent);
void phMwIfi_PrintNfaActivationParams(tNFC_ACTIVATE_DEVT* psActivationNtfPrms);
MWIFSTATUS phMwIfi_SendNxpNciCommand(void *mwIfHandle,
                                     uint8_t cmd_params_len,
                                     uint8_t *p_cmd_params,
                                     tNFA_NXP_NCI_RSP_CBACK *p_cback);
MWIFSTATUS phMwIfi_CheckNdef(void*                       mwIfHandle,
                                    phMwIf_sNdefDetectParams_t* psNdefDetectParams);
MWIFSTATUS phMwIfi_WriteNdef(void* mwIfHandle,
                                    uint8_t* pData,
                                    uint32_t length);
MWIFSTATUS phMwIfi_SetTagReadOnly(void*      mwIfHandle,
                                         BOOLEAN    readOnly);
MWIFSTATUS phMwIfi_ReadNdef(void*                 mwIfHandle,
                                   phMwIf_sBuffParams_t* psBuffParams);
MWIFSTATUS phMwIfi_HandleT2TCmd(void*                  mwIfHandle,
                                phMwIf_sT2TParams_t*   psTagParams);
void       phMwIfi_HandleActivatedEvent(tNFA_ACTIVATED*     psActivationPrms,
                                               BOOLEAN*            pblPushToQReqd,
                                               BOOLEAN*            pblCallbackReqd,
                                               phMWIf_uEvtInfo_t*  puEvtInfo,
                                               phMWIf_eEvtType_t*  peMwIfEvtType);
void       phMwIfi_CopyActivationPrms(phMWIf_sActivatedEvtInfo_t* psActivationPrms,
                                             tNFC_ACTIVATE_DEVT*         psActivationNtfPrms);
MWIFSTATUS phMwIfi_DiscoveryStop(void* mwIfHandle);
MWIFSTATUS phMwIfi_DiscoveryStart(void* mwIfHandle);
tNFA_STATUS phMwIfi_EeSetDefaultTechRouting (tNFA_HANDLE          ee_handle,
                                             tNFA_TECHNOLOGY_MASK technologies_switch_on,
                                             tNFA_TECHNOLOGY_MASK technologies_switch_off,
                                             tNFA_TECHNOLOGY_MASK technologies_battery_off);
tNFA_STATUS phMwIfi_EeSetDefaultProtoRouting (tNFA_HANDLE         ee_handle,
                                              tNFA_PROTOCOL_MASK  protocols_switch_on,
                                              tNFA_PROTOCOL_MASK  protocols_switch_off,
                                              tNFA_PROTOCOL_MASK  protocols_battery_off);
void*       phMwIfi_DbgAgcThread(void *arg);
MWIFSTATUS phMwIfi_P2pRoutingConfigure(void* mwIfHandle,
                               phMwIf_RfTechMask   dwRfTechMask);
MWIFSTATUS phMwIfi_InitRouting(void* mwIfHandle);
MWIFSTATUS phMwIfi_RoutingUpdate(void* mwIfHandle);
#ifdef __cplusplus
}
#endif
#endif /* PHMWIFANDROID_H_ */
