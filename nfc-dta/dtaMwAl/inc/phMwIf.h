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

/*!
 *
 * \file  phMwIf.h
 * \brief MWInterface header file.
 *
 * Project:  NFC MWIF LIB
 */

#ifndef phMwIF_H
#define phMwIF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Configuration for extending symbols for different platform */
#ifndef MWIF_LIB_EXTEND
#define MWIF_LIB_EXTEND extern
#endif

/** section MACRO DEFINES */
#define MWIFSTATUS_SUCCESS          0x00
#define MWIFSTATUS_INVALID_PARAM    0x03
#define MWIFSTATUS_FAILED           0x09
/** end section MACRO DEFINES */
typedef uint32_t MWIFSTATUS;

#define PHMWIF_NCI_NFCID1_MAX_LEN           10
#define PHMWIF_NCI_T1T_HR_LEN               2
#define PHMWIF_NCI_MAX_SENSB_RES_LEN        12
#define PHMWIF_NCI_MAX_SENSF_RES_LEN        18
#define PHMWIF_NCI_NFCID2_LEN               8
#define PHMWIF_NCI_MAX_ATS_LEN              60
#define PHMWIF_NCI_MAX_HISTORY_BYTES_LEN    50
#define PHMWIF_NCI_MAX_GEN_BYTES_LEN        48
#define PHMWIF_NCI_MAX_ATTRIB_LEN           (10 + PHMWIF_NCI_MAX_GEN_BYTES_LEN)
#define PHMWIF_NCI_NFCID0_MAX_LEN           4
#define PHMWIF_NCI_MAX_RAW_PARAMS           16
#define PHMWIF_NCI_BITRATE_212              0x02
#define PHMWIF_NCI_BITRATE_424              0x04

/** \ingroup grp_dta_lib
    Definitions for phMwIf_RfTech
    Specifies set of technologies */
#define PHMWIF_RFTECHNOLOGY_MASK_A    0x01    /**< NFC Technology A */
#define PHMWIF_RFTECHNOLOGY_MASK_B    0x02    /**< NFC Technology B */
#define PHMWIF_RFTECHNOLOGY_MASK_F    0x04    /**< NFC Technology F */
typedef uint32_t phMwIf_RfTechMask_t;

#ifndef INT_TO_PTR
# define INT_TO_PTR(P) ((void*)(intptr_t)(P))
#endif

/** \ingroup grp_mwif_lib
    Type of NFC protocol being used*/
typedef enum phMwIf_eNdefTagType
{
    PHMWIF_TAG_NDEF_READ_ONLY        = 0x01,    /**< Tag is read only              */
    PHMWIF_TAG_NDEF_FORMATED         = 0x02,    /**< Tag formated for NDEF         */
    PHMWIF_TAG_NDEF_SUPPORTED        = 0x04,    /**< NDEF supported by the tag     */
    PHMWIF_TAG_NDEF_UNKNOWN          = 0x08,    /**< Unable to find if tag is ndef capable/formated/read only */
    PHMWIF_TAG_NDEF_FORMATABLE       = 0x10,    /**< Tag supports format operation */
    PHMWIF_TAG_NDEF_SOFT_LOCKABLE    = 0x20,    /**< Tag can be soft locked */
    PHMWIF_TAG_NDEF_HARD_LOCKABLE    = 0x40,    /**< Tag can be hard locked */
    PHMWIF_TAG_NDEF_OTP              = 0x80     /**< Tag is one time programmable */
}phMwIf_eNdefTagType_t;

/** \ingroup grp_mwif_lib
    Type of NFC protocol being used*/
typedef enum phMwIf_eProtocolType
{
    PHMWIF_PROTOCOL_UNKNOWN = 0X00,
    PHMWIF_PROTOCOL_T1T     = 0X01,
    PHMWIF_PROTOCOL_T2T     = 0X02,
    PHMWIF_PROTOCOL_T3T     = 0X03,
    PHMWIF_PROTOCOL_ISO_DEP = 0X04,
    PHMWIF_PROTOCOL_NFC_DEP = 0x05,
    PHMWIF_PROTOCOL_INVALID = 0xFF
}phMwIf_eProtocolType_t;

/** \ingroup grp_mwif_lib
    All NDEF related operations common for all tags */
typedef enum phMwIf_eTagOpsType
{
    PHMWIF_TAGOP_CHECK_NDEF     = 0x00, /**<Check if tag is NDEF compliant*/
    PHMWIF_TAGOP_READ_NDEF      = 0X01, /**<Read All (all bytes)*/
    PHMWIF_TAGOP_WRITE_NDEF     = 0X02, /**<Write-no-erase (eight bytes,For Dynamic memory)*/
    PHMWIF_TAGOP_SET_READONLY   = 0x03  /**<Set tag as read only*/
}phMwIf_eTagOpsType_t;

/** \ingroup grp_mwif_lib
    List of commands for Type 1 Tag */
typedef enum phMwIf_eT1TCmd
{
    PHMWIF_T1T_READ_CMD  = 0X00, /**<command reads blocks that belong to the currently selected sector*/
    PHMWIF_T1T_WRITE_CMD = 0X01, /**<programming 4 bytes at once*/
    PHMWIF_T1T_RAW_CMD   = 0X02 /**<RAW command to send to FW. Cmd buffer sent includes reqd headers*/
}phMwIf_eT1TCmd_t;

/** \ingroup grp_mwif_lib
    List of commands for Type 2 Tag */
typedef enum phMwIf_eT2TCmd
{
    PHMWIF_T2T_READ_CMD          = 0X00, /**<command reads blocks that belong to the currently selected sector*/
    PHMWIF_T2T_WRITE_CMD         = 0X01, /**<programming 4 bytes at once*/
    PHMWIF_T2T_SECTOR_SELECT_CMD = 0X02, /**<Selects particular sector*/
    PHMWIF_T2T_RAW_CMD           = 0X03 /**<RAW command to send to FW. Cmd buffer sent includes reqd headers*/
}phMwIf_eT2TCmd_t;

/** \ingroup grp_mwif_lib
    List of commands for Type 3 Tag */
typedef enum phMwIf_eT3TCmd
{
    PHMWIF_T3T_READ_CMD  = 0X00, /**<Command is used to read user data from the memory of a Type 3 Tag*/
    PHMWIF_T3T_WRITE_CMD = 0X01, /**<Command is used to write user data to the memory blocks of a Type 3 Tag*/
    PHMWIF_T3T_RAW_CMD   = 0X02 /**<RAW command to send to FW. Cmd buffer sent includes reqd headers*/
}phMwIf_eT3TCmd_t;

/** \ingroup grp_mwif_lib
    List of commands for Type 4 Tag */
typedef enum phMwIf_eT4TCmd
{
    PHMWIF_T4T_RAW_CMD = 0X00 /**<RAW command to send to FW. Cmd buffer sent includes reqd headers*/
}phMwIf_eT4TCmd_t;

/** \ingroup grp_mwif_lib
    Type of NCI RF interface being used*/
typedef enum phMwIf_eNciRfIntfType
{
    PHMWIF_NCI_INTERFACE_EE_DIRECT_RF = 0x00,
    PHMWIF_NCI_INTERFACE_FRAME        = 0x01,
    PHMWIF_NCI_INTERFACE_ISO_DEP      = 0x02,
    PHMWIF_NCI_INTERFACE_NFC_DEP      = 0x03
}phMwIf_eNciRfIntfType_t;

/** \ingroup grp_mwif_lib
    Type of NCI interface being used*/
typedef enum phMwIf_eNciBitRate
{
    PHMWIF_NCI_BIT_RATE_106  = 0x00,/**< 106 kbit/s */
    PHMWIF_NCI_BIT_RATE_212  = 0x01,/**< 212 kbit/s */
    PHMWIF_NCI_BIT_RATE_424  = 0x02,/**< 424 kbit/s */
    PHMWIF_NCI_BIT_RATE_848  = 0x03,/**< 848 Kbit/s */
    PHMWIF_NCI_BIT_RATE_1696 = 0x04,/**< 1696 Kbit/s*/
    PHMWIF_NCI_BIT_RATE_3392 = 0x05,/**< 3392 Kbit/s*/
    PHMWIF_NCI_BIT_RATE_6784 = 0x06 /**< 6784 Kbit/s*/
}phMwIf_eNciBitRate_t;

/** \ingroup grp_mwif_lib
    Parameters for buffer */
typedef struct phMwIf_sBuffParams
{
    uint8_t*         pbBuff; /**< Read/Write Buffer*/
    uint32_t         dwBuffLength; /**<Read/Write Buffer Length*/
}phMwIf_sBuffParams_t;

/** \ingroup grp_mwif_lib
    Parameters for timer */
typedef struct phMwIf_sTimerParams
{
    phMwIf_sBuffParams_t sBuffParams;
    uint32_t             timeout_ms;
}phMwIf_sTimerParams_t;

/** \ingroup grp_mwif_lib
    parameters returned during NDEF tag detection */
typedef struct phMwIf_sNdefDetectParams
{
    MWIFSTATUS                  dwStatus;            /**< Status of the ndef detecton                             */
    phMwIf_eProtocolType_t      eProtocolType;       /**< protocol used to detect NDEF                         */
    uint32_t                    dwMaxSize;           /**< max number of bytes available for NDEF data              */
    uint32_t                    dwCurSize;           /**< current size of stored NDEF data (in bytes)              */
    phMwIf_eNdefTagType_t       bNdefTypeFlags;   /**< Flags to indicate NDEF capability, is formated, soft/hard lockable, formatable, otp and read only */
}phMwIf_sNdefDetectParams_t;

/** \ingroup grp_mwif_lib
    data parameters returned/sent during tag operations */
typedef struct phMwIf_uTagOpsParams
{
    phMwIf_sBuffParams_t       sBuffParams;
    phMwIf_sNdefDetectParams_t sNdefDetectParams;
}phMwIf_uTagOpsParams_t;

/** \ingroup grp_mwif_lib
    parameters for T1T operations */
typedef struct phMwIf_sT1TParams
{
    phMwIf_eT1TCmd_t     eT1TCmd;
    phMwIf_sBuffParams_t sBuffParams;
}phMwIf_sT1TParams_t;

/** \ingroup grp_mwif_lib
    parameters for T2T operations */
typedef struct phMwIf_sT2TParams
{
    phMwIf_eT2TCmd_t           eT2TCmd;
    uint32_t                   dwSectorNum; /**<To select sector before read*/
    uint32_t                   dwBlockNum; /**<To read particular block*/
    phMwIf_sBuffParams_t       sBuffParams;
}phMwIf_sT2TParams_t;

/** \ingroup grp_mwif_lib
    parameters for T3T operations */
typedef struct phMwIf_sT3TParams
{
    phMwIf_eT3TCmd_t            eT3TCmd;
    phMwIf_sBuffParams_t        sBuffParams;
    uint32_t                    dwPresenceCheckStartDelay; /**<Delay to check presence of tag*/
}phMwIf_sT3TParams_t;

/** \ingroup grp_mwif_lib
    parameters for T4T operations */
typedef struct phMwIf_sT4TParams
{
    phMwIf_eT4TCmd_t        eT4TCmd;
    phMwIf_sBuffParams_t    sBuffParams;
}phMwIf_sT4TParams_t;

/** \ingroup grp_mwif_lib
    parameters for all type of Tag operations */
typedef union phMwIf_uTagParams
{
    phMwIf_sT1TParams_t   sT1TPrms; /**< Parameters for Type 1 Tag*/
    phMwIf_sT2TParams_t   sT2TPrms; /**< Parameters for Type 2 Tag*/
    phMwIf_sT3TParams_t   sT3TPrms; /**< Parameters for Type 3 Tag*/
    phMwIf_sT4TParams_t   sT4TPrms; /**< Parameters for Type 4 Tag*/
}phMwIf_uTagParams_t;

/** \ingroup grp_mwif_lib
    Data events sent to user of MwIf*/
typedef enum phMWIf_eEvtType
{
    PHMWIF_DISCOVERED_EVT,
    PHMWIF_T1T_TAG_ACTIVATED_EVT,
    PHMWIF_T2T_TAG_ACTIVATED_EVT,
    PHMWIF_T3T_TAG_ACTIVATED_EVT,
    PHMWIF_NFCDEP_ACTIVATED_EVT,
    PHMWIF_ISODEP_ACTIVATED_EVT,
    PHMWIF_CE_ACTIVATED_EVT,
    PHMWIF_P2P_DATA_EVT,
    PHMWIF_CE_DATA_EVT,
    PHMWIF_DEACTIVATED_EVT,
    PHMWIF_RESTART_DISCOVERY_EVT,
    PHMWIF_THREAD_DELETE_DUMMY_EVT = 0xFF
}phMWIf_eEvtType_t;

/** \ingroup grp_mwif_lib
    LLCP events sent to user of MwIf */
typedef enum phMWIf_eLlcpEvtType
{
    PHMWIF_LLCP_ACTIVATED_EVT                     = 100,
    PHMWIF_LLCP_DEACTIVATED_EVT                   = 101,
    PHMWIF_LLCP_SERVER_CONN_REQ_EVT               = 102,/**< Request from a client to accept the connection*/
    PHMWIF_LLCP_ERROR_EVT                         = 103,
    PHMWIF_LLCP_SERVER_CONN_LESS_DATA_EVT         = 104,/**<Data events for Connection less service*/
    PHMWIF_LLCP_CO_REMOTE_CLIENT_DICONNECTED_EVT  = 105,/**<Remote client disconnected from local server*/
    PHMWIF_LLCP_SERVER_CONN_ORIENTED_DATA_EVT     = 106,/**<Data events for Connection Oriented service*/
    PHMWIF_LLCP_P2P_LINK_CONGESTED_EVT            = 107,/**<Congest info event for particular link*/
    PHMWIF_LLCP_P2P_LINK_UNCONGESTED_EVT          = 108 /**<Congest info event for particular link*/
}phMWIf_eLlcpEvtType_t;

typedef enum phMWIf_eDiscType
{
    PHMWIF_DISCOVERY_TYPE_POLL_A            =0x00,
    PHMWIF_DISCOVERY_TYPE_POLL_B            =0x01,
    PHMWIF_DISCOVERY_TYPE_POLL_F            =0x02,
    PHMWIF_DISCOVERY_TYPE_POLL_A_ACTIVE     =0x03,
    PHMWIF_DISCOVERY_TYPE_POLL_F_ACTIVE     =0x05,
    PHMWIF_DISCOVERY_TYPE_POLL_B_PRIME      =0x74,
    PHMWIF_DISCOVERY_TYPE_LISTEN_A          =0x80,
    PHMWIF_DISCOVERY_TYPE_LISTEN_B          =0x81,
    PHMWIF_DISCOVERY_TYPE_LISTEN_F          =0x82,
    PHMWIF_DISCOVERY_TYPE_LISTEN_A_ACTIVE   =0x83,
    PHMWIF_DISCOVERY_TYPE_LISTEN_F_ACTIVE   =0x85
}phMWIf_eDiscType_t;

typedef enum phMWIf_eDeactivateType
{
    PHMWIF_DEACTIVATE_TYPE_IDLE     = 0x00,
    PHMWIF_DEACTIVATE_TYPE_SLEEP    = 0x01,
    PHMWIF_DEACTIVATE_TYPE_SLEEP_AF = 0x02,
    PHMWIF_DEACTIVATE_TYPE_DISCOVERY= 0x03
}phMWIf_eDeactivateType_t;

/** \ingroup grp_mwif_lib
    Data sent to user of MwIf in Data event callback*/
typedef struct phMWIf_sDataEvtInfo
{
    uint32_t dwSize;
    void*    pvDataBuf;
}phMWIf_sDataEvtInfo_t;

/** \ingroup grp_mwif_lib
    TEchnology Type A specific RF parameters sent during activation*/
typedef struct phMwIf_sNciRfTechPollAParams
{
    uint8_t       bSensRes[2];                        /**< SENS_RES Response (ATQA). Available after Technology Detection */
    uint8_t       bNfcId1Len;                         /**< 4, 7 or 10 */
    uint8_t       abNfcId1[PHMWIF_NCI_NFCID1_MAX_LEN];/**< NFCId1 */
    uint8_t       bSelRes;                            /**< SEL_RSP (SAK) Available after Collision Resolution */
    uint8_t       bHrLen;                             /**< 2, if T1T HR0/HR1 is reported */
    uint8_t       abHr[PHMWIF_NCI_T1T_HR_LEN];        /**< T1T HR0 is in hr[0], HR1 is in hr[1] */
}phMwIf_sNciRfTechPollAParams_t;

/** \ingroup grp_mwif_lib
    TEchnology Type B specific RF parameters sent during activation*/
typedef struct phMwIf_sNciRfTechPollBParams
{
    uint8_t       bSensBResLen;                             /**< Length of SENSB_RES Response (Byte 2 - Byte 12 or 13) Available after Technology Detection */
    uint8_t       abSensBRes[PHMWIF_NCI_MAX_SENSB_RES_LEN]; /**< SENSB_RES Response (ATQ) */
}phMwIf_sNciRfTechPollBParams_t;

/** \ingroup grp_mwif_lib
    TEchnology Type F specific RF parameters sent during activation*/
typedef struct phMwIf_sNciRfTechPollFParams
{
    uint8_t       bBitrate;                                /**< NFC_BIT_RATE_212 or NFC_BIT_RATE_424 */
    uint8_t       bSensFResLen;                            /**< Length of SENSF_RES Response (Byte 2 - Byte 17 or 19) Available after Technology Detection */
    uint8_t       abSensFRes[PHMWIF_NCI_MAX_SENSF_RES_LEN];/**< SENSB_RES Response */
    uint8_t       abNfcId2[PHMWIF_NCI_NFCID2_LEN];         /**< NFCID2 generated by the Local NFCC for NFC-DEP Protocol.Available for Frame Interface  */
    uint8_t       bMrtiCheck;
    uint8_t       bMrtiUpdate;
}phMwIf_sNciRfTechPollFParams_t;
typedef phMwIf_sNciRfTechPollFParams_t phMwIf_sNciRfTechListenFParams_t;

/** \ingroup grp_mwif_lib
    TEchnology specific RF parameters sent during activation*/
typedef union phMwIf_uNciRfTechParams
{
    phMwIf_sNciRfTechPollAParams_t   sPollAPrms;
    phMwIf_sNciRfTechPollBParams_t   sPollBPrms;
    phMwIf_sNciRfTechPollFParams_t   sPollFPrms;
    phMwIf_sNciRfTechListenFParams_t sListenFPrms;
}phMwIf_uNciRfTechParams_t;

/** \ingroup grp_mwif_lib
    ISODEP RF Interface  parameters for tech type Poll A type sent during activation*/
typedef struct phMwIf_sNciRfIntfIsoDepPollAParams
{
    uint8_t       bAtsResLen;                                      /**< Length of ATS RES                */
    uint8_t       abAtsRes[PHMWIF_NCI_MAX_ATS_LEN];                /**< ATS RES                          */
    uint8_t       blNadUsed;                                       /**< NAD is used or not               */
    uint8_t       bFwi;                                            /**< Frame Waiting time Integer       */
    uint8_t       bSfgi;                                           /**< Start-up Frame Guard time Integer*/
    uint8_t       bLenOfHistoryBytes;                              /**< len of historical bytes          */
    uint8_t       abHistoryBytes[PHMWIF_NCI_MAX_HISTORY_BYTES_LEN];/**< historical bytes             */
}phMwIf_sNciRfIntfIsoDepPollAParams_t;

/** \ingroup grp_mwif_lib
    ISODEP RF Interface  parameters for Tech type Listen A  sent during activation*/
typedef struct phMwIf_sNciRfIntfIsoDepListenAParams
{
    uint8_t       bRats;  /**< RATS */
}phMwIf_sNciRfIntfIsoDepListenAParams_t;

typedef struct phMwIf_sNciRfIntfIsoDepPollBParams
{
    uint8_t       bAttribResLen;                                  /**< Length of ATTRIB RES      */
    uint8_t       abAttribRes[PHMWIF_NCI_MAX_ATTRIB_LEN];         /**< ATTRIB RES                */
    uint8_t       bHigherLayerInfoLen;                            /**< len of Higher layer Info  */
    uint8_t       abHigherLayerInfo[PHMWIF_NCI_MAX_GEN_BYTES_LEN];/**< Higher layer Info         */
    uint8_t       bMaxBufferLen;                                  /**< Maximum buffer length.    */
}phMwIf_sNciRfIntfIsoDepPollBParams_t;

typedef struct phMwIf_sNciRfIntfIsoDepListenBParams
{
    uint8_t       bAttribReqLen;                                  /**< Length of ATTRIB REQ      */
    uint8_t       abAttribReq[PHMWIF_NCI_MAX_ATTRIB_LEN];         /**< ATTRIB REQ (Byte 2 - 10+k)*/
    uint8_t       bHigherLayerInfoLen;                            /**< len of Higher layer Info  */
    uint8_t       abHigherLayerInfo[PHMWIF_NCI_MAX_GEN_BYTES_LEN];/**< Higher layer Info         */
    uint8_t       bNfcid0[PHMWIF_NCI_NFCID0_MAX_LEN];             /**< NFCID0                    */
}phMwIf_sNciRfIntfIsoDepListenBParams_t;

/** \ingroup grp_mwif_lib
    NFCDEP RF Interface  parameters for Tech type Poll A  sent during activation*/
typedef struct phMwIf_sNciRfIntfNfcDepPollAParams
{
    uint8_t       bAtrResLen;                              /**< Length of ATR_RES            */
    uint8_t       abAtrRes[PHMWIF_NCI_MAX_ATS_LEN];        /**< ATR_RES (Byte 3 - Byte 17+n) */
    uint8_t       bMaxPayloadSize;                         /**< 64, 128, 192 or 254          */
    uint8_t       bGenBytesLen;                            /**< len of general bytes         */
    uint8_t       abGenBytes[PHMWIF_NCI_MAX_GEN_BYTES_LEN];/**< general bytes           */
    uint8_t       bWaitingTime;                            /**< WT -> Response Waiting Time RWT = (256 x 16/fC) x 2WT */
}phMwIf_sNciRfIntfNfcDepPollAParams_t;
typedef phMwIf_sNciRfIntfNfcDepPollAParams_t phMwIf_sNciRfIntfNfcDepPollFParams_t;

/** \ingroup grp_mwif_lib
    NFCDEP RF Interface  parameters for Tech type Listen A  sent during activation*/
typedef struct phMwIf_sNciRfIntfNfcDepListenAParams
{
    uint8_t       bAtrReqLen;                              /**< Length of ATR_REQ            */
    uint8_t       abAtrReq[PHMWIF_NCI_MAX_ATS_LEN];        /**< ATR_REQ (Byte 3 - Byte 18+n) */
    uint8_t       bMaxPayloadSize;                         /**< 64, 128, 192 or 254          */
    uint8_t       bGenBytesLen;                            /**< len of general bytes         */
    uint8_t       abGenBytes[PHMWIF_NCI_MAX_GEN_BYTES_LEN];/**< general bytes           */
}phMwIf_sNciRfIntfNfcDepListenAParams_t;
typedef phMwIf_sNciRfIntfNfcDepListenAParams_t phMwIf_sNciRfIntfNfcDepListenFParams_t;

/** \ingroup grp_mwif_lib
    Raw Frame RF Interface  parameters*/
typedef struct phMwIf_sNciRfIntfRawFrameParams
{
    uint8_t       bParamLen;
    uint8_t       abParam[PHMWIF_NCI_MAX_RAW_PARAMS];
}phMwIf_sNciRfIntfRawFrameParams_t;

/** \ingroup grp_mwif_lib
    TEchnology specific RF parameters sent during activation*/
typedef union phMwIf_uNciRfIntfParams
{
    phMwIf_sNciRfIntfIsoDepPollAParams_t   sIsoDepPollAPrms;
    phMwIf_sNciRfIntfIsoDepListenAParams_t sIsoDepLisAPrms;
    phMwIf_sNciRfIntfIsoDepPollBParams_t   sIsoDepPollBPrms;
    phMwIf_sNciRfIntfIsoDepListenBParams_t sIsoDepLisBPrms;
    phMwIf_sNciRfIntfNfcDepPollAParams_t   sNfcDepPollAPrms;
    phMwIf_sNciRfIntfNfcDepListenAParams_t sNfcDepLisAPrms;
    phMwIf_sNciRfIntfNfcDepPollFParams_t   sNfcDepPollFPrms;
    phMwIf_sNciRfIntfNfcDepListenFParams_t sNfcDepLisFPrms;
    phMwIf_sNciRfIntfRawFrameParams_t      sRawFramePrms;
}phMwIf_uNciRfIntfParams_t;

/** \ingroup grp_mwif_lib
    Data sent to user of MwIf activated event callback*/
typedef struct phMWIf_sActivatedEvtInfo
{
    uint8_t                     bRfDiscId;
    phMwIf_eNciRfIntfType_t     eRfInterfaceType;
    phMwIf_eProtocolType_t      eProtocolType;
    phMWIf_eDiscType_t          eActivatedRfTechTypeAndMode;
    phMwIf_uNciRfTechParams_t   uRfTechParams;              /**<RfTech Params for Activated RfTechType and mode*/
    phMWIf_eDiscType_t          eDataXchgRfTechTypeAndMode;
    phMwIf_eNciBitRate_t        eTxBitRate;
    phMwIf_eNciBitRate_t        eRxBitRate;
    phMwIf_uNciRfIntfParams_t   uRfIntfParams;              /**<Interface Parameters for eRfInterface Type*/
}phMWIf_sActivatedEvtInfo_t;

/** \ingroup grp_mwif_lib
    Data sent to user of MwIf for callback events*/
typedef union phMWIf_uEvtInfo
{
    phMWIf_sActivatedEvtInfo_t  sActivationPrms;/**<Info passed along with ACTIVATED event*/
    phMWIf_sDataEvtInfo_t       sData;          /**<Info passed along with DATA event*/
    phMWIf_eDeactivateType_t    eDeactivateType;/**<Info passed along with DEACTIVATED event*/
}phMWIf_uEvtInfo_t;

/** \ingroup grp_mwif_lib
    Callback type to be registered by user to MwIf*/
typedef void (*phMWIf_EvtCb_t)( void*              pvMwIfHandle,
                                void*              pvApplHdl,
                                phMWIf_eEvtType_t  eEvtType,
                                phMWIf_uEvtInfo_t* puEvtInfo);

/** \ingroup grp_mwif_lib
    parameters passed along with PHMWIF_LLCP_SERVER_CONN_REQ_EVT callback to user of mwif*/
typedef struct phMwIf_sLlcpConnReqParams
{
    void*        wServerHandle;/**< Server Handle for which conn is requested*/
    void*        wConnHandle;  /**< Connection Handle for which conn is requested*/
    uint8_t      bRemoteSap;   /**< Remote Source Access Point*/
    uint16_t     wRemoteMiu;   /**< Remote Maximum Information Unit supported*/
    uint8_t      bRemoteRw;    /**< Remote receive window supported*/
} phMwIf_sLlcpConnReqParams_t;

/** \ingroup grp_mwif_lib
    parameters passed along with PHMWIF_LLCP_CONN_LESS_DATA_EVT callback to user of mwif
    Actual data can be received by calling  phMwIf_LlcpConnLessRecvData*/
typedef struct phMwIf_sLlcpConnLessData
{
    uint16_t     wConnLessHandle;  /**<  Handle for which connection less Link*/
    uint8_t      bRemoteSap;       /**< Remote Source Access Point*/
} phMwIf_sLlcpConnLessData_t;

/** \ingroup grp_mwif_lib
    parameters passed along with PHMWIF_LLCP_CONN_ORIENTED_DATA_EVT callback to user of mwif
    Actual data can be received by calling  phMwIf_LlcpConnOrientedRecvData*/

typedef struct phMwIf_sLlcpConnOrientedData
{
    uint16_t     wConnOrientedHandle;  /**<  Handle for which connection Oriented Link*/
    uint8_t      bRemoteSap;       /**< Remote Source Access Point*/
} phMwIf_sLlcpConnOrientedData_t;

/** \ingroup grp_mwif_lib
    parameters passed along with PHMWIF_LLCP_CO_REMOTE_CLIENT_DICONNECTED_EVT callback to user of mwif
 */
typedef struct phMwIf_sLlcpDisconnectInfo
{
    uint16_t     wConnHandle;                /**< connection handle for which disconnect is called*/
    uint8_t      bReasonForDisconnect;       /**< Reason for remote client to get disconnected*/
} phMwIf_sLlcpDisconnectInfo_t;

/** \ingroup grp_mwif_lib
    union of all the LLCP callback parameters*/
typedef union phMwIf_uLlcpEvtInfo
{
    phMwIf_sLlcpConnReqParams_t sConnReq;          /**<Info passed along with CONN_REQ event*/
    phMwIf_sLlcpConnLessData_t  sLlcpConnLessData; /**<Info passed along with LLCP P2P DATA event*/
    phMwIf_sLlcpDisconnectInfo_t sDisconnect;      /**<Info passed along with DISCONNECT event*/
    phMwIf_sLlcpConnOrientedData_t  sLlcpConnOrientedData; /**<Info passed along with LLCP P2P DATA event*/
    uint16_t                        uLlcpLinkHdl;               /**Congested link handle*/
}phMwIf_uLlcpEvtInfo_t;

/** \ingroup grp_mwif_lib
    LLCP Callback type to be registered by user to MwIf*/
typedef void (*phMWIf_LlcpEvtCb_t) (void*                   pvMwIfHandle,
                                    void*                   pvApplHdl,
                                    phMWIf_eLlcpEvtType_t   eLlcpEvtType,
                                    phMwIf_uLlcpEvtInfo_t*  puLlcpEvtInfo);

/** \ingroup grp_mwif_lib
    RF Discovery Technology and Mode*/
typedef struct phMwIf_sDiscParams
{
    phMwIf_RfTechMask_t dwPollRdWrt;  /**< Technologies to enable for Read/Write in Poll Mode*/
    phMwIf_RfTechMask_t dwPollP2P;    /**< Technologies to enable for P2P in Poll Mode*/
    phMwIf_RfTechMask_t dwListenP2P;  /**< Technologies to enable for P2P in Listen Mode*/
    phMwIf_RfTechMask_t dwListenHCE;  /**< Technologies to enable for Host Card Emulation in Listen Mode*/
    phMwIf_RfTechMask_t dwListenESE;  /**< Technologies to enable for Card Emulation from Ese in Listen Mode*/
    phMwIf_RfTechMask_t dwListenUICC; /**< Technologies to enable for Card Emulation from UICC in Listen Mode*/
}phMwIf_sDiscParams_t;

/** \ingroup grp_mwif_lib
    Device type where Card Emulation is present*/
typedef enum phMwIf_eCeDevType
{
     PHMWIF_UICC_CE  = 0x01,     /**< Card emulation at UICC*/
     PHMWIF_SE_CE    = 0x02,     /**< Card Emulation at eSE*/
     PHMWIF_HOST_CE  = 0x04      /**< Card Emulation at Host*/
}phMwIf_eCeDevType_t;

/** \ingroup grp_mwif_lib
    Device type where Card Emulation is present*/
typedef enum phMwIf_eServerConnectType
{
     PHMWIF_SERVER_CONNECT_BY_NAME  = 0x01,     /**< Connect to server using service name*/
     PHMWIF_SERVER_CONNECT_BY_SAP   = 0x02     /**< Connect to server using SAP*/
}phMwIf_eServerConnectType_t;

/** \ingroup grp_mwif_lib
    Middleware and Firmware versions used*/
typedef struct phMwIf_sVersionInfo
{
    uint8_t mwVerMajor;
    uint8_t mwVerMinor;
    uint8_t fwVerMajor;
    uint8_t fwVerMinor;
}phMwIf_sVersionInfo_t;

/** \ingroup grp_mwif_lib
    Discovery configuration which includes technology types and card emulation
    features to be enabled*/
typedef struct phMwIf_sDiscCfgPrms
{
    phMwIf_sDiscParams_t discParams;
    char Certification_Release[8];
}phMwIf_sDiscCfgPrms_t;

/** \ingroup grp_mwif_lib
    Event group type for which the callbacks are registered*/
typedef enum phMWIf_eMwEvtType
{
    PHMWIF_ACTIVATION_EVT=0, /**<Device/CE/SE/LLCP Activation events */
    PHMWIF_HCE_EVT,
    PHMWIF_LLCP_EVT,
}phMWIf_eMwEvtType_t;

/** \ingroup grp_mwif_lib
    LLCP initialization parameters*/
typedef struct phMwIf_sLlcpInitParams
{
    phMwIf_sBuffParams_t     sGenBytesInitiator;/**< General bytes Data Buffer which sent with ATR_REQ*/
    phMwIf_sBuffParams_t     sGenBytesTarget;   /**< General bytes Data Buffer which sent with ATR_RESP*/
    phMWIf_LlcpEvtCb_t       pfMwIfLlcpCb;      /**< Callback function to pass to MwIf for LLCP events*/
    void*                    pvApplHdl;         /**<Application Handle which needs to be passed back in LLCP callback*/
} phMwIf_sLlcpInitParams_t;

/** \ingroup grp_mwif_lib
    LLCP initialization parameters*/
typedef struct phMwIf_sLlcpSrvrRegnParams
{
    uint8_t               bSap;
    uint8_t*              pbServiceName;
    phMWIf_LlcpEvtCb_t    pfMwIfLlcpServerCb;/**< Callback function to pass to MwIf for LLCP server events*/
    void*                 pvApplHdl;/**<Application Hdl which needs to be passed back in LLCP callback*/
} phMwIf_sLlcpSrvrRegnParams_t;

/** \ingroup grp_mwif_lib
    LLCP initialization parameters*/
typedef struct phMwIf_sLlcpClientConnectParams
{
    phMwIf_eServerConnectType_t eConnectType; /**< Connect by SAP or Service name */
    uint8_t                     bSap;
    uint8_t*                    pbServerServiceName;
    phMWIf_LlcpEvtCb_t          pfMwIfLlcpClientCb;/**< Callback function to pass to MwIf for LLCP server events*/
    void*                       pvApplHdl;/**<Application Hdl which needs to be passed back in LLCP callback*/
    uint16_t                    wClientMiu;
    uint8_t                     bClientRw;
} phMwIf_sLlcpClientConnectParams_t;

/** \ingroup grp_mwif_lib
    Parameters for Configuration */
typedef struct phMwIf_sConfigParams
{
    char aCertRelease[10];                    /**< Certification Configuration details*/
    uint8_t bPollBitRateTypeF;                /**< Configure TypeF Polling Bitrate to 424 as per DTA*/
    uint8_t bNfcdepPollBitRateHigh;           /**< Poll NFC-DEP : Select Highest Available Bit Rates(PSL_REQ)*/
    uint8_t bEnableAnalog;                    /**< For Analog Mode. TRUE= enable, FALSE=disable*/
    uint8_t bEnableLlcp;                      /**< Enable P2P on LLCP. TRUE= enable, FALSE=disable*/
    phMwIf_sDiscCfgPrms_t sMwIfDiscCfgParams; /**< RF Discovery Technology and Mode */
    uint32_t dwTimeSlotNumber;                /**< Configure the slot number*/
    uint32_t dwConnDeviceLimit;               /**< Configure the number of devices*/
}phMwIf_sConfigParams_t;

/**
 * \ingroup grp_mwif_lib
 * \brief Initializes the Middleware Interface Library
 *
 * This function initializes Middleware Interface library and
 * and underlying resources(threads,semaphores)required
 * A session with NFC hardware will be established.
 *
 * \note Before successful initialization other NFC functionalities are not enabled.
 * this function doesn't initialise NFCEE.
 * \param[out] mwIfHandle       Middleware Interface Handle
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Initialised successfully
 * \retval #MWIFSTATUS_FAILED   MWIF LIB failed to initialise.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_Init(void** mwIfHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief DeInitializes the Middleware Interface Library
 *
 * This function Deinitializes Middleware Interface library and
 * and releases resources(threads,semaphores)
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB DeInitialised successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to Deinit.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_DeInit(void* mwIfHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief Callback function from user of MwIf is registered
 *
 * This callback function for data events are registered by Mwif users
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] ApplHdl             Application handle which will be passed back when
 *                                callback is invoked
 * \param[in] mwIfCb              Callback function to be registered
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Registered CB successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to RegisterCallback.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_RegisterCallback(void* mwIfHandle,
                                          void* applHdl,
                                          phMWIf_EvtCb_t mwIfCb);

/**
 * \ingroup grp_mwif_lib
 * \brief Configures the Middleware Interface Library
 *
 * This function Configures Middleware Interface library and
 * and underlying layer
 *
 * \param[in] mwIfHandle       Middleware Interface Handle
 * \param[in] sConfigParams    Details of configuration
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Initialised successfully
 * \retval #MWIFSTATUS_FAILED   MWIF LIB failed to initialise.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_ConfigParams( void* mwIfHandle,
                                                phMwIf_sConfigParams_t *sConfigParams);

/**
 * \ingroup grp_mwif_lib
 * \brief Get version of underlying Middleware and Firmware
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[out] versionInfo        MiddleWare and Firmware versions used will be returned
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Get Version Info successful
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to get version.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_VersionInfo(void* mwIfHandle,
                                     phMwIf_sVersionInfo_t* versionInfo);

/**
 * \ingroup grp_mwif_lib
 * \brief Configure and start the discovery
 *
 * This function configures the discovery loop as provided in the configuration
 * and starts the discovery
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Discovery successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to enable discovery.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_EnableDiscovery( void* mwIfHandle);
/**
 * \ingroup grp_mwif_lib
 * \brief Disable the discovery loop
 *
 * This function stops the discovery loop if its not stopped before
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Discovery stopped successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to stop Discovery.
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_DisableDiscovery(void* mwIfHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief read/write and other operations common for all tags
 *
 * This function is for operations on tag. Type of tag is assumed to be the previouly activated one
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] eTagOpsType         Type of Tag Operation
 * \param[in/out] psTagOpsParams  Data/Buffers for required tag operation to be sent/returned based on the operation type
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Tag operations is successful
 * \retval #MWIFSTATUS_FAILED     MWIF LIB Tag operations is unsuccessful
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_TagOperation( void* mwIfHandle,
                                                phMwIf_eTagOpsType_t       eTagOpsType,
                                                phMwIf_uTagOpsParams_t*    psTagOpsParams);
/**
 * \ingroup grp_mwif_lib
 * \brief command for particular tag type
 *
 * This function is for specific command for particular tag type
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] eProtocolType       Type of Tag indicated by protocol
 * \param[in/out] psTagParams     Buffer and other data returned/sent for Tag commands based on cmd type
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB command sent successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB command sending Unsuccessful
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_TagCmd(void* mwIfHandle,
                                  phMwIf_eProtocolType_t eProtocolType,
                                  phMwIf_uTagParams_t* psTagParams);

/**
 * \ingroup grp_mwif_lib
 * \brief Initializes and Configures for Card Emulation
 *
 * This function is for Initializing and Configuring for Card Emulation
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] eDevType            Card Emulation device type
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB command sent successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB command sending Unsuccessful
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_CeConfigure( void *mwIfHandle,    phMwIf_eCeDevType_t eDevType );

/**
 * \ingroup grp_mwif_lib
 * \brief De-Initializes and De-Configures for Card Emulation
 *
 * This function is for De-Initializing and De-Configuring for Card Emulation
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] eDevType            Card Emulation device type
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB command sent successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB command sending Unsuccessful
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_CeDeInit( void *mwIfHandle,   phMwIf_eCeDevType_t eDevType );

/**
 * \ingroup grp_mwif_lib
 * \brief Send a raw frame over the activated interface with the NFCC.
 *
 * This function Send a raw frame over the activated interface and
 *      can only be called after NFC link is activated.
 *
 * \param[in] pvInBuff          Input Buffer required data handling
 * \param[in] dwLenInBuff       Length of input buffer
 * \param[in] mwIfHandle        Middleware Interface Handle
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Sending of data is successful
 * \retval #MWIFSTATUS_FAILED   MWIF LIB Failed to send raw data
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_SendRawFrame( void* mwIfHandle,
                                                void* pvInBuff,
                                                uint32_t dwLenInBuff);

/**
 * \ingroup grp_mwif_lib
 * \brief Send/receive a raw frame over the activated interface with the NFCC.
 *
 * This function Send/receive a raw frame over the activated interface and
 *      can only be called after NFC link is activated.
 *
 * \param[in] pvInBuff          Input Buffer required data handling
 * \param[in] dwLenInBuff       Length of input buffer
 * \param[out] pvOutBuff        Output Buffer required data handling
 * \param[out] dwLenOutBuff     Length of OutBuffer
 * \param[in] mwIfHandle        Middleware Interface Handle
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Sending/receiving of data is successful
 * \retval #MWIFSTATUS_FAILED   MWIF LIB Failed to send/receive raw data
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_Transceive(void* mwIfHandle,
                                   void* pvInBuff,
                                   uint32_t dwLenInBuff,
                                   void* pvOutBuff,
                                   uint32_t* dwLenOutBuff);

/**
 * \ingroup grp_mwif_lib
 * \brief Set configuration for NFC controller before starting the discovery loop.
 *
 * This function Set configuration for NFC controller before starting the discovery loop.
 *
 * \param[in] abnumIDs           Number of NCI Configurations ID's
 * \param[in] abparamIDs         Configuration ID data buffer
 * \param[in] mwIfHandle         Middleware Interface Handle
 * \param[in] pvOutBuff          Output Buffer required data handling
 * \param[in] dwLenOutBuff       Length of OutBuffer
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Get Configuration is successful
 * \retval #MWIFSTATUS_FAILED   MWIF LIB Failed to Get configuration
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_GetConfig(void* mwIfHandle,
                                    uint8_t abnumIDs,
                                    uint8_t *abparamIDs,
                                    uint8_t *pvOutBuff,
                                    uint32_t *dwLenOutBuff);

/**
 * \ingroup grp_mwif_lib
 * \brief Sends Deactivate command to NFCC
 *
 * This function Sends Deactivate command to NFCC.
 *
 * \param[in] phMWIf_eDeactivateType_t   Deactivate type
 * \param[in] mwIfHandle                 Middleware Interface Handle
 *
 * \retval #MWIFSTATUS_SUCCESS  MWIF LIB Deactivation of NFCC is successful
 * \retval #MWIFSTATUS_FAILED   MWIF LIB FAiled to Deactivate NFCC
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_NfcDeactivate(void*                    mwIfHandle,
                                                phMWIf_eDeactivateType_t eDeactType);

/**
 * \ingroup grp_mwif_lib
 * \brief Receive P2P Data from middleware
 *
 * This function receives P2P/CE loopback data from middleware with default parameters
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvOutBuff           Output Buffer required data handling
 * \param[in] dwLenOutBuff        Length of OutBuffer
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB Data received successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed receive data
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_ReceiveData(void *mwIfHandle,
                                    void* pvOutBuff,
                                    uint32_t* dwLenOutBuff);

/**
 * \ingroup grp_mwif_lib
 * \brief Initialize LLCP stack
 *
 * This function initializes LLCP stack to set general bytes for ATR_REQ/ATR_RES
 * with default parameters required for MAC Link activation procedure
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] psLlcpInitPrms      Initialization parameters required for LLCP stack initialization.
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP stack initialized successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to initiailize LLCP stack
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpInit(void*                     pvMwIfHandle,
                                  phMwIf_sLlcpInitParams_t* psLlcpInitPrms);

/**
 * \ingroup grp_mwif_lib
 * \brief LLCP Server registration
 *
 * This function registers Connection Oriented LLCP server for SAP value and service name
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] psLlcpSrvrPrms      Server parameters required for registration
 * \param[out] ppvServerHandle    Server Handle registered
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB registered LLCP server successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP server
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS  phMwIf_LlcpRegisterServerConnOriented(void*                         pvMwIfHandle,
                                                                  phMwIf_sLlcpSrvrRegnParams_t* psLlcpSrvrPrms,
                                                                  void**                        ppvServerHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief LLCP Server registration
 *
 * This function registers Connection Oriented LLCP server for SAP value and service name
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] psLlcpSrvrPrms      Server parameters required for registration
 * \param[out] ppvServerHandle    Server Handle registered
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB registered LLCP server successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP server
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS  phMwIf_LlcpRegisterServerConnLess(void*                         pvMwIfHandle,
                                                              phMwIf_sLlcpSrvrRegnParams_t* psLlcpSrvrPrms,
                                                              void**                        ppvServerHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief Accept Client connection requested in event LLCP_CONN_REQ_EVT
 *
 * This function accepts  LLCP client connection request and returns the connection handle
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvServerHandle      Server handle returned during registration
 * \param[in] wServerMiu          Max Information Unit supported by Server
 * \param[in] bServerRw           Server Receive Window
 * \param[in] pvConnHandle         Connection handle sent during connection request
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP client registered successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP client
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpServerAcceptClientConn( void*       pvMwIfHandle,
                                                              void*       pvServerHandle,
                                                              uint16_t    wServerMiu,
                                                              uint8_t     bServerRw,
                                                              void*       pvConnHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief Receive data from LLCP connection
 *
 * This function receives data from llcp connection as provided in connection handle
 * This can be used for both server and client
 * with default parameters required for MAC Link activation procedure
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvConnHandle        Connection handle provided when connection is accepted by server
 * \param[out] psData             LLCP Data Received on the connection specified by conn handle
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP stack initialized successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to initiailize LLCP stack
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnOrientedRecvData( void*                     pvMwIfHandle,
                                                            void*                     pvConnHandle,
                                                            phMwIf_sBuffParams_t*     psData);

/**
 * \ingroup grp_mwif_lib
 * \brief Receive data from LLCP connectionless link
 *
 * This function receives data from llcp connectionless link as provided in connection handle
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvConnHandle        Connection handle provided when P2P_DATA_EVT is received
 * \param[out] psData             LLCP Data Received on the connection specified by conn handle
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP stack initialized successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to initiailize LLCP stack
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnLessRecvData( void*                     pvMwIfHandle,
                                                        void*                     pvConnHandle,
                                                        uint8_t*                  pbRemoteSap,
                                                        phMwIf_sBuffParams_t*     psData);

/**
 * \ingroup grp_mwif_lib
 * \brief Send data over Data link(Connection oriented service
 *
 * This function sends data from llcp connection as provided in connection handle
 * This can be used for both server and client
 * with default parameters required for MAC Link activation procedure
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvConnHandle        Connection handle provided when connection is accepted by server
 * \param[out] psData             LLCP Data to be sent on the connection specified by conn handle
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP stack initialized successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to initiailize LLCP stack
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnOrientedSendData( void*                     pvMwIfHandle,
                                                            void*                     pvConnHandle,
                                                            phMwIf_sBuffParams_t*     psData);

/**
 * \ingroup grp_mwif_lib
 * \brief Send  data over logical link(connection less)
 *
 * This function sends data to remote SAP
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] bRemoteSap          Connection handle provided when connection is accepted by server
 * \param[out] psData             LLCP Data to be sent on the connection specified by conn handle
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP stack initialized successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to initiailize LLCP stack
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnLessSendData( void*                     pvMwIfHandle,
                                                        uint8_t                   bRemoteSap,
                                                        phMwIf_sBuffParams_t*     psData);
/**
 * \ingroup grp_mwif_lib
 * \brief Connection Oriented client to server
 *
 * This function register LLCP client
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] psConnectPrms       Connection parameters. If server service name is Null, then server SAP value
 *                                is used  to connect to server
 * \param[out] ppvRemoteServerConnHandle      Connection Handle to be returned after connection
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP client registered successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP client
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnOrientedClientConnect( void*                               pvMwIfHandle,
                                                     phMwIf_sLlcpClientConnectParams_t*  psConnectPrms,
                                                     void**                              ppvRemoteServerConnHandle);

/**
 * \ingroup grp_mwif_lib
 * \brief Disconnect connection associated with connection handle
 * \for Connection Oriented client/remote server OR server/remote client
 *
 * This function register LLCP client
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pvConnHandle        Connection Handle
 *
 * \retval #MWIFSTATUS_SUCCESS    Disconnected connection successfully
 * \retval #MWIFSTATUS_FAILED     failed to Disconnect the connection
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpConnOrientedClientDisconnect( void*     pvMwIfHandle,
                                                          void*     pvConnHandle);
/**
 * \ingroup grp_mwif_lib
 * \brief Connect client to server
 *
 * This function register LLCP client
 *
 * \param[in] mwIfHandle          Middleware Interface Handle provided during init
 * \param[in] pbServiceName       Service name for which the SAP value is requested
 * \param[out] pbSAP              SAP value of the requested service name returned
 *
 * \retval #MWIFSTATUS_SUCCESS    MWIF LIB LLCP client registered successfully
 * \retval #MWIFSTATUS_FAILED     MWIF LIB failed to register LLCP client
 *
 */
MWIF_LIB_EXTEND MWIFSTATUS phMwIf_LlcpServiceDiscovery( void*     pvMwIfHandle,
                                                        uint8_t*  pbServiceName,
                                                        uint8_t*  pbSAP);

#ifdef __cplusplus
}
#endif

#endif/* phMwIF_H */
