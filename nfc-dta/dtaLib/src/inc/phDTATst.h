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

#ifndef __PH_DTATST_H__
#define __PH_DTATST_H__

#include "phDTAOSAL.h"
#include "phMwIf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DTALIB_VERSION_STR "NFCDTA_11.15"
#define DTALIBVERSION_MAJOR (0x11)
#define DTALIBVERSION_MINOR (0x15)
#define PHDTALIB_PATTERN_NUM_ANALOG_TEST 0x1000

/**< Error Comparison with Return and Print */
#define PH_ON_ERROR_RETURN(status,retstatus,message) \
    if(retstatus != status)\
    {\
        phOsal_LogError(message);\
        return retstatus;\
    }
#define LOG_FUNCTION_ENTRY phOsal_LogDebugString((const uint8_t*)"DtaLib>:Entered:",(const uint8_t*)__FUNCTION__)
#define LOG_FUNCTION_EXIT  phOsal_LogDebugString((const uint8_t*)"DtaLib>:Exit:",(const uint8_t*)__FUNCTION__)

/** \ingroup grp_dta_lib
    List of LLCP variables */
static const char gs_abInboundCoService[]     = "urn:nfc:sn:dta-co-echo-in";
#define PHDTALIB_SAP_LT_CO_IN_SRC             ((uint8_t) 0x20) /* Conn Oriented Client on LT*/
#define PHDTALIB_SAP_IUT_CO_IN_DEST           ((uint8_t) 0x14) /* Conn Oriented Server on IUT*/
static const char gs_abOutboundCoService[]    = "urn:nfc:sn:dta-co-echo-out";
#define PHDTALIB_SAP_LT_CO_OUT_DEST           ((uint8_t) 0x12) /* Conn Oriented Server on LT*/
#define PHDTALIB_SAP_IUT_CO_OUT_SRC           ((uint8_t) 0x22) /* Conn Oriented Client on IUT*/
static const char gs_abInboundClService[]     = "urn:nfc:sn:dta-cl-echo-in";
#define PHDTALIB_SAP_LT_CL_IN_SRC             ((uint8_t) 0x21) /* Conn Less Client on LT*/
#define PHDTALIB_SAP_IUT_CL_IN_DEST           ((uint8_t) 0x13) /* Conn Less Server on IUT*/
static const char gs_abOutboundClService[]    = "urn:nfc:sn:dta-cl-echo-out";
#define PHDTALIB_SAP_LT_CL_OUT_DEST           ((uint8_t) 0x11) /* Conn Less Server on LT*/
#define PHDTALIB_SAP_IUT_CL_OUT_SRC           ((uint8_t) 0x23) /* Conn Less Client on IUT*/

#ifdef WIN32

/**\ingroup grp_mwif_snep
 * \def PH_MW_SNEP_SERVER_INBOX_SIZE
 * This Macro indicates server inbox size.
 */
#define PH_MW_SNEP_SERVER_INBOX_SIZE        1024

/**\ingroup grp_mwif_snep
 * \def MWIFSTATUS_PUT_GET_REQUEST
 * This Macro is used to indicate reception of GET / PUT request.
 */
#define MWIFSTATUS_PUT_REQUEST      0x01

/**\ingroup grp_mwif_snep
 * \def MWIFSTATUS_LINK_DEACTIVATED
 * This Macro is used to indicate that link has been deactivated.
 */
#define MWIFSTATUS_LINK_DEACTIVATED     0x03

/**\ingroup grp_mwif_snep
 * \def MWIFSTATUS_PUT_GET_REQUEST
 * This Macro is used to indicate reception of GET / PUT request.
 */
#define MWIFSTATUS_GET_REQUEST      0x02

#define NDEF_RTD_OFFSET 10

#define GET_REQ_RTD_OFFSET 4

#endif
/** \ingroup grp_dta_lib
    LLCP related constants */
/**< LLCP Connection Oriented Functionality setting up by SAP or  Connection-less Functionality */
#define    PHDTALIB_LLCP_CO_SET_SAP_OR_CL   0x1200
/**< LLCP Connection Oriented Functionality setting-up by name or  Connection-less Functionality */
#define    PHDTALIB_LLCP_CO_SET_NAME_OR_CL  0x1240
/**< LLCP Connection Oriented Functionality setting up by using SNL or  Connection-less Functionality */
#define    PHDTALIB_LLCP_CO_SET_SNL_OR_CL   0x1280
/*Delay Time before sending the loopback data for connectionless service.As per DTA spec 3000ms */
#define    TDELAY_CL_IN_MS                  3000

typedef struct phDtaLib_sHandle {
    void*                           dtaApplHdl;              /**<Application handle*/
    void*                           mwIfHdl;
    void*                           dtaThreadHdl;
    void*                           queueHdl;
    phDtaLib_sTestProfile_t         sTestProfile;
    phDtaLib_sDiscParams_t          sAppDiscCfgParams;
    phMwIf_sDiscCfgPrms_t           sPrevMwIfDiscCfgParams;
    phDtaLib_eP2PType_t             p2pType;
    phMWIf_sActivatedEvtInfo_t      sT3TActivationParams;
    phdtaLib_EvtCb_t                dtaApplCb;                 /**<Callback interface between DTALIB and JNI layer*/
    void*                           pvCORemoteServerConnHandle; /**<Connection Oriented Remote Server Connection*/
    void*                           pvCORemoteClientConnHandle; /**<Connection Oriented Remote Client Connection*/
    void*                           pvCOServerHandle;           /**<Connection Oriented Server*/
    void*                           pvCOClientHandle;           /**<Connection Oriented Server*/
    void*                           pvCLServerHandle;           /**<ConnectionLess Server*/
    void*                           pvCLRemoteClientConnLessHandle; /**<Connection Less Remote Client Handle*/
    uint8_t                         bRemoteSapClEchoOut;
    BOOLEAN                         bDtaInitialized;            /**Flag to indicate DTA Initialization is  Done*/
    BOOLEAN                         blStopCbMsgHandleThrd;      /**< Set to TRUE to stop Integration thread*/
    BOOLEAN                         bLlcpInitialized;           /**Flag to indicate LLCP Initialization is  Done*/
    BOOLEAN                         bIsLlcpCoRemoteServerLinkCongested;       /**Flag to indicate LLCP Particular link is congested or not*/
    uint16_t                        uLlcpLinkHdl;               /**Congested link handle*/
    void*                           qHdlCongestData;
    BOOLEAN                         bIsLlcpCoRemoteClientDisconnected;      /**Flag to indicate the connection status of the remote client*/
    phMwIf_sConfigParams_t          sConfigPrms;                /** Underlying Configuration params */
} phDtaLib_sHandle_t;

typedef union phDtaLib_eEvtType
{
    phMWIf_eEvtType_t eDpEvtType;/**<  Digital Protocol related events */
    phMWIf_eLlcpEvtType_t eLlcpEvtType;/**< LLCP events*/
}phDtaLib_uEvtType_t;

/*The events from Middleware interface. The event values shall be unique for each group*/
typedef union phDtaLib_eEvtInfo
{
    phMWIf_uEvtInfo_t      uDpEvtInfo;/**<  Digital Protocol related events */
    phMwIf_uLlcpEvtInfo_t  uLlcpEvtInfo;/**< LLCP events*/
}phDtaLib_uEvtInfo_t;

/*This data is sent from Callback function to queue which will be handled by the
 * Message handler thread*/
typedef struct phDtaLib_sQueueData
{
    phDtaLib_uEvtType_t uEvtType;
    phDtaLib_uEvtInfo_t uEvtInfo;
}phDtaLib_sQueueData_t;

extern uint32_t gx_status;

#ifdef WIN32

/**
 *\ingroup grp_mwif_snep
 *
 *\brief Defines snep server socket options
 */
typedef struct phMwIf_LlcpTransport_sSocketOptions
{
    uint16_t miu;
    uint8_t rw;
}phMwIf_LlcpTransport_sSocketOptions_t;

/**
 *\ingroup grp_mwif_snep
 *
 *\brief Defines snep server type
 */

typedef enum
{
    phMwIf_SnepServer_Default = 0x00,     /**< Default NFC Forum SNEP server */
    phMwIf_SnepServer_NonDefault          /**< Proprietary SNEP server */

} phMwIf_SnepServer_type_t;

/**
 *\ingroup grp_mwif_snep
 *
 *\brief configuration for snep server used during client and server intialisation
 */
typedef struct phMwIf_SnepServerConfig
{
    phMwIf_SnepServer_type_t SnepServerType;  /**< SNEP Server type to be Initialized */
    phMwIf_sBuffParams_t *SnepServerName;              /**< SNEP Service name for Non Default servers */
    phMwIf_LlcpTransport_sSocketOptions_t sOptions;    /**< LLCP Socket Options */
}phMwIf_SnepConfig_t;
#endif

/**
 * \ingroup grp_dta_lib internal function
 * \brief Checks whether Tag is NDEF compliant or not
 *
 * This function Checks whether Tag is NDEF compliant or not.
 *
 * \param[in] psNdefDetectParams        parameters returned during NDEF tag detection
 *
 * \retval #DTASTATUS_SUCCESS           Ndef TAG detect successfully
 * \retval #DTASTATUS_FAILED            Ndef TAG detect unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_CheckNDEF(phMwIf_uTagOpsParams_t* sTagOpsParams);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Read NDEF data from tag
 *
 * This function Read NDEF data from NDEF compliant tag.
 *
 * \param[in] psTagOpsParams            data parameters returned/sent during tag operations
 *
 * \retval #DTASTATUS_SUCCESS           NDEF Read from tag is successful
 * \retval #DTASTATUS_FAILED            NDEF Read from tag is not successful.
 *
 */
extern DTASTATUS phDtaLibi_ReadNDEF(phMwIf_uTagOpsParams_t* psTagOpsParams);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Write NDEF data to tag
 *
 * This function Writes the NDEF data to NDEF compliant tag.
 *
 * \param[in] psTagOpsParams            data parameters returned/sent during tag operations
 *
 * \retval #DTASTATUS_SUCCESS           NDEF Write to tag is successful
 * \retval #DTASTATUS_FAILED            NDEF Write to tag is not successful.
 *
 */
extern DTASTATUS phDtaLibi_WriteNDEF(phMwIf_uTagOpsParams_t* psTagOpsParams);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Set the Tag as read only
 *
 * This function Set the Tag as read only.
 *
 * \param[in] blReadOnly                Flag to set the tag read only
 * \param[in] psTagOpsParams            data parameters returned/sent during tag operations
 *
 * \retval #DTASTATUS_SUCCESS           NDEF Write to tag is successful
 * \retval #DTASTATUS_FAILED            NDEF Write to tag is not successful.
 *
 */
extern DTASTATUS phDtaLibi_SetTagReadOnly(phMwIf_uTagOpsParams_t* psTagOpsParams);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to print the string or array or group of data
 *
 * This Function to print the string or array or group of data
 *
 * \param[in] pubuf                     8bit Buffer to be printed
 * \param[in] uilen                     length of the Buffer to be printed
 * \param[in] pcsmessage                Message to be printed before printing the Buffer
 *
 * \retval                              None
 *
 */
extern void phDtaLibi_PrintBuffer(uint8_t *pubuf,uint16_t uilen,const uint8_t *pcsmessage);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function To handle NFC_DEP listen mode test cases means LT is polling and is initiator
 *
 * Function To handle NFC_DEP listen mode test cases means LT is polling and is initiator
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           handling of NFC_DEP listen mode test cases is successful
 * \retval #DTASTATUS_FAILED            handling of NFC_DEP listen mode test cases is Unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_NfcdepInitiatorOperations();

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function To handle NFC_DEP poll mode test cases means LT is listening and is polling
 *
 * Function To handle NFC_DEP poll mode test cases means LT is listening and is polling
 *
 * \param[in] bStartDiscReqd            Flag to start the discovery
 * \param[in] bStopDiscReqd             Flag to stop the discovery
 *
 * \retval #DTASTATUS_SUCCESS           handling of NFC_DEP listen mode test cases is successful
 * \retval #DTASTATUS_FAILED            handling of NFC_DEP listen mode test cases is Unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_NfcdepTargetOperations(BOOLEAN* bStartDiscReqd,
        BOOLEAN* bStopDiscReqd);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function To Handle Host card emulation related operations
 *
 * Function To handle Host card emulation related operations
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           Handling of Host card emulation related operations is successful
 * \retval #DTASTATUS_FAILED            Handling of Host card emulation related operations is Unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_HceOperations();

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function To Handle Host card emulation related operations
 *
 * Function To handle Host card emulation for felica related operations
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           Handling of Host card emulation for felica related operations is successful
 * \retval #DTASTATUS_FAILED            Handling of Host card emulation for felica related operations is Unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_HceFOperations();

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function To Handle UICC card emulation related operations
 *
 * Function To handle UICC card emulation related operations
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           Handling of Host card emulation related operations is successful
 * \retval #DTASTATUS_FAILED            Handling of Host card emulation related operations is Unsuccessful.
 *
 */
extern DTASTATUS phDtaLibi_UICCOperations();

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Handle T1T Tag operations(Read/Write)
 *
 * Function To Handle T1T Tag operations(Read/Write) based on the pattern number configured
 *
 * \param[in] sTestProfile              Tag testing required params
 *
 * \retval #DTASTATUS_SUCCESS           T1T Tag read/write operations is successful
 * \retval #DTASTATUS_FAILED            T1T Tag read/write operations is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_T1TOperations(phDtaLib_sTestProfile_t sTestProfile);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Handle T2T Tag operations(Read/Write)
 *
 * Function To Handle T2T Tag operations(Read/Write) based on the pattern number configured
 *
 * \param[in] sTestProfile              Tag testing required params
 *
 * \retval #DTASTATUS_SUCCESS           T2T Tag read/write operations is successful
 * \retval #DTASTATUS_FAILED            T2T Tag read/write operations is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_T2TOperations(phDtaLib_sTestProfile_t sTestProfile);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Handle T3T Tag operations(Read/Write)
 *
 * Function To Handle T3T Tag operations(Read/Write) based on the pattern number configured
 *
 * \param[in] sTestProfile              Tag testing required params
 * \param[in] bStartDiscReqd            Flag to start the discovery
 * \param[in] bStopDiscReqd             Flag to stop the discovery
 * \param[in] psT3TActivationParams     T3T Activation params
 *
 * \retval #DTASTATUS_SUCCESS           T3T Tag read/write operations is successful
 * \retval #DTASTATUS_FAILED            T3T Tag read/write operations is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_T3TOperations(phDtaLib_sTestProfile_t sTestProfile,
                                         phMWIf_sActivatedEvtInfo_t* psT3TActivationParams,BOOLEAN* bStartDiscReqd,
                                         BOOLEAN* bStopDiscReqd);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Handle T4T Tag operations(Read/Write)
 *
 * Function To Handle T4T Tag operations(Read/Write) based on the pattern number configured
 *
 * \param[in] sTestProfile              Tag testing required params
 * \param[in] bStartDiscReqd            Flag to start the discovery
 * \param[in] bStopDiscReqd             Flag to stop the discovery
 *
 * \retval #DTASTATUS_SUCCESS           T4T Tag read/write operations is successful
 * \retval #DTASTATUS_FAILED            T4T Tag read/write operations is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_T4TOperations(phDtaLib_sTestProfile_t sTestProfile,
                                         BOOLEAN* blStartDiscReqd,
                                         BOOLEAN* blStopDiscReqd);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to handle operations related to LLCP starts below
 *
 * Function to handle operations related to LLCP starts below
 *
 * \param[in] psTestProfile              LLCP testing required params
 * \param[in] phMWIf_eLlcpEvtType_t      LLCP events sent to user of MwIf
 * \param[in] phMwIf_uLlcpEvtInfo_t      union of all the LLCP callback parameters
 *
 * \retval #DTASTATUS_SUCCESS           handle operations related to LLCP is successful
 * \retval #DTASTATUS_FAILED            handle operations related to LLCP is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_LlcpOperations(phDtaLib_sTestProfile_t* psTestProfile,
                                          phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                          phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to handle operations related to Connection less LLCP starts below
 *
 * Function to handle operations related to Connection less LLCP starts below
 *
 * \param[in] psTestProfile              LLCP testing required params
 * \param[in] phMWIf_eLlcpEvtType_t      LLCP events sent to user of MwIf
 * \param[in] phMwIf_uLlcpEvtInfo_t      union of all the LLCP callback parameters
 *
 * \retval #DTASTATUS_SUCCESS           handle connection less operations related to LLCP is successful
 * \retval #DTASTATUS_FAILED            handle connection less operations related to LLCP is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_LlcpConnLessOperations(phDtaLib_sTestProfile_t* psTestProfile,
                                                  phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                                  phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo);

/**
 * \ingroup grp_dta_lib internal function
 * \brief Function to Operations to be done after remote client disconnects from local connection oriented server
 *
 * Function to handle Operations to be done after remote client disconnects from local connection oriented server
 *
 * \param[in] psTestProfile              LLCP testing required params
 * \param[in] phMWIf_eLlcpEvtType_t      LLCP events sent to user of MwIf
 * \param[in] phMwIf_uLlcpEvtInfo_t      union of all the LLCP callback parameters
 *
 * \retval #DTASTATUS_SUCCESS           handle connection oriented operations related to LLCP is successful
 * \retval #DTASTATUS_FAILED            handle connection oriented operations related to LLCP is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_LlcpHandleCoClientDisconnect(phDtaLib_sTestProfile_t* psTestProfile,
                                                        phMWIf_eLlcpEvtType_t    eLlcpEvtType,
                                                        phMwIf_uLlcpEvtInfo_t*   puLlcpEvtInfo);

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to handle the call back events
 *
 * Function to handle the middleware call back events
 *
 * \param[in] mwIfHandle              middlware handler
 * \param[in] applHdl                 application handler
 * \param[in] phMWIf_eEvtType_t       Data events sent to user of MwIf
 * \param[in] phMWIf_uEvtInfo_t       Data sent to user of MwIf for callback events
 *
 * \retval                            None
 *
 */
extern void phDtaLibi_EvtCb(void* mwIfHandle,
                            void* applHdl,
                            phMWIf_eEvtType_t evtType,
                            phMWIf_uEvtInfo_t* psEvtData);

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to handle the call back events
 *
 * Function to handle the llcp middleware call back events
 *
 * \param[in] mwIfHandle              middlware handler
 * \param[in] applHdl                 application handler
 * \param[in] phMWIf_eLlcpEvtType_t   LLCP events sent to user of MwIf
 * \param[in] phMwIf_uLlcpEvtInfo_t   union of all the LLCP callback parameters
 *
 * \retval                            None
 *
 */
extern void phDtaLibi_LlcpEvtCb (void*                   pvMwIfHandle,
                                 void*                   pvApplHdl,
                                 phMWIf_eLlcpEvtType_t   eLlcpEvtType,
                                 phMwIf_uLlcpEvtInfo_t*  puLlcpEvtInfo);

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to DeInitialize and Initialize the Middleware
 *
 * Function To DeInitialize and Initialize the Middleware for card emulation in EE mode
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           Restart(Deinitialization and initialization) of middleware is successful
 * \retval #DTASTATUS_FAILED            Restart(Deinitialization and initialization) of middleware is Unsuccessful
 *
 */
extern DTASTATUS phDtaLibi_ReInit();

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to handle the Un-congested event
 *
  * Function to Handle Uncongested event.
 * Congested event is provided by MwIf when the one of the data link connection
 * cannot process to send next data. In this case Data is pushed to the queue.
 * When Uncongested event is received, the data is pulled from the queue and
 * sent to MwIf.
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           All the queue data sent successful
 * \retval #DTASTATUS_FAILED            Data sending Un-succesful
 *
 */
extern DTASTATUS phDtaLibi_LlcpHandleUncongestedEvent();

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to handle the LLCP de-activation
 *
 * Function to handle LLCP de-activation
 * Flushes all the data present in the congested Queue
 *
 * \param[in]                           None
 *
 * \retval #DTASTATUS_SUCCESS           Flushing of Queue is successful
 * \retval #DTASTATUS_FAILED            Flushing of Queue is Un-succesful
 *
 */
extern DTASTATUS phDtaLibi_LlcpHandleDeactivatedEvent();

/**
 * \ingroup grp_dta_lib internal function
 * \brief DTA Library function to Upadte the config params
 *
 * Function to Upadte the config params
 *
 * \param[in]  discParams               Modes and enabled technology details
 *
 * \retval #DTASTATUS_SUCCESS           Flushing of Queue is successful
 * \retval #DTASTATUS_FAILED            Flushing of Queue is Un-succesful
 *
 */
extern MWIFSTATUS phDtaLibi_UpdateConfigPrams(phDtaLib_sDiscParams_t* discParams);

#ifdef WIN32

/**
 * \ingroup grp_mwif_snep
 * \brief  Callback definition to store the received Put message onto the server Inbox
 *
 * \param[in] pDataInbox     the received message from Client(PUT message)
 *
 */
typedef void (*phDta_serverInbox) (phMwIf_sBuffParams_t *pDataInbox);

/**
 * \ingroup grp_mwif_snep
 * \brief  Callback definition to inform the receipt of GET message from client
 *
 * \param[in] recMsg     the received message from Client(GET message)
 *
 */
typedef void (*phDta_getMessage) (phMwIf_sBuffParams_t *recMsg);

/**
 * \ingroup grp_mwif_snep
 * \brief Interface to create and configure a SNEP client over LLCP
 *
 * This function creates and configures a SNEP client over LLCP
 *
 * \param[in] snepConfigInfo      Contains Peer SNEP server configuration
 * \param[in] snepClientHandle    Client handle returned for the session
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP client registered successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP client
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepClient_Init(phMwIf_SnepConfig_t *snepConfigInfo, HANDLE *snepClientHandle);

/**
 * \ingroup grp_mwif_snep
 *
 *  phMwIf_SnepClient_RequestPut() sends a PUT request to the server, along with the data.
 *
 *  \param[in]  pPutData                Data to sent as a part of PUT request.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepClient_RequestPut(phMwIf_sBuffParams_t *pPutData);

/**
 * \ingroup grp_mwif_snep
 * \brief <b>Interface Un-initialize client session</b>.
 *
 * Un-initializes the Client session and closes any open connections.
 *
 * \param[in] snepClientHandle   Client Session Handle received.
 *
 * \retval #MWIFSTATUS_SUCCESS                  Operation successful.
 * \retval #MWIFSTATUS_FAILED                   Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepClient_DeInit(HANDLE *snepClientHandle);

/**
 * \ingroup grp_lib_nfc
 *
 *  phMwIf_SnepClient_RequestGet() requests SNEP Server to return a NDEF message
 *  designated by the NDEF message transmitted with the request.
 *
 *  \param[in]  pGetData                Data sent to server as part of GET request.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepClient_RequestGet(phMwIf_sBuffParams_t *pGetData);

/**
 * \ingroup grp_lib_nfc
 *
 *  The phMwIf_SnepServer_Init initialises a SNEP SERVER over LLCP.
 *
 *  \param[in]  snepConfigInfo          Snep Server config params.
 *  \param[out] snepServerHandle        Connection handle to identify session uniquely.
 *
 *  \retval #MWIFSTATUS_SUCCESS           Operation successful.
 *  \retval #MWIFSTATUS_FAILED            Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepServer_Init(phMwIf_SnepConfig_t *snepConfigInfo, HANDLE *snepServerHandle);

/**
 * \ingroup grp_lib_nfc
 *
 *  The phMwIf_SnepServer_Accept sends the Connection confirm response to the client
 *  connected.
 *
 *  \param[in]  snepServerHandle              Connection handle to identify session uniquely.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepServer_Accept(HANDLE snepServerHandle, phDta_serverInbox notifyCb, \
                                                                             phDta_getMessage notifyGetCb);

/**
 * \ingroup grp_lib_nfc
 *
 *  The phMwIf_SnepServer_GenericWait waits for reception of any request from client
 *  connected to it.
 *  It can be PUT or GET or Link Deactivated notification.
 *
 *  \param[in]  snepServerHandle              Connection handle to identify session uniquely.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *  \retval #MWIFSTATUS_PUT_GET_REQUEST           Recieved a PUT or GET request.
 *  \retval #MWIFSTATUS_LINK_DEACTIVATED          Recieved Link Deactivated notification.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepServer_GenericWait(HANDLE snepServerHandle);

/**
 * \ingroup grp_lib_nfc
 *
 *  The phMwIf_SnepServer_Response Sends a response to the client with appropriate response code
 *
 *  \param[in]  responseStatus              The response code to be sent to the client
 *  \param[in]  responseData                The response data to be sent to the client,
 *                                          which will be NULL in case for PUT response.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepServer_Response(NFCSTATUS responseStatus, \
                                                      phMwIf_sBuffParams_t *responseData);

/**
 * \ingroup grp_lib_nfc
 *
 *  The phMwIf_SnepServer_DeInit de-initialises the Snep Server
 *
 *  \param[in]  ServerHandle            Snep Server handle.
 *
 *  \retval #MWIFSTATUS_SUCCESS                   Operation successful.
 *  \retval #MWIFSTATUS_FAILED                    Operation failed.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SnepServer_DeInit(HANDLE *ServerHandle);

#endif

#ifdef __cplusplus
}
#endif

#endif /*#ifndef __PH_DTATST_H__*/
