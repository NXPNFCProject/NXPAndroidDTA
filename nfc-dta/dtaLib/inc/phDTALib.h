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
 *
 *
 * \file  phDtaLib.h
 * \brief DTA LIB Interface header file.
 *
 * Project:  NFC DTA LIB
 */

/* Preprocessor includes for different platform */

#ifndef PHDTALIB_H
#define PHDTALIB_H

#include <stdlib.h>
#include <string.h>
#include "phDTAOSAL.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration for extending symbols for different platform */
#ifndef DTALIB_LIB_EXTEND
#define DTALIB_LIB_EXTEND extern
#endif

typedef bool_t BOOLEAN;

/*Macro definition*/

/** \ingroup grp_dta_lib
    Indicates successful completion. */
#define DTASTATUS_SUCCESS               0x00
/** \ingroup grp_dta_lib
    Invalid parameter */
#define DTASTATUS_INVALID_PARAM         0x01
/** \ingroup grp_dta_lib
    Status code for failure*/
#define DTASTATUS_FAILED                0xFF
typedef uint32_t DTASTATUS;

/** \ingroup grp_dta_lib
    Data events sent to user of dtaLibIf*/
typedef enum phdtaAppLib_eEvtType
{
    PHDTALIB_TEST_CASE_EXEC_STARTED = 0x00,
    PHDTALIB_TEST_CASE_EXEC_COMPLETED  = 0x01,
    PHDTALIB_TEST_EXEC_DEINIT = 0x02
}phdtaAppLib_eEvtType_t;


/** \ingroup grp_dta_lib
    Data events sent to user of dtaLibIf*/
typedef struct phdtaLib_sEvtData
{
    void *pData;
    uint32_t wDataLength;
}phdtaLib_sEvtData_t;

/** \ingroup grp_dta_lib
    List of secure elements*/
typedef enum phDtaLib_eNfceeDevType
{
    PHDTALIB_NFCEE_UICC    = 0x00,     /**< NFCEE device type UICC*/
    PHDTALIB_NFCEE_ESE     = 0x01     /**< NFCEE device type eSE*/
}phDtaLib_eNfceeDevType_t;

/** \ingroup grp_dta_lib
     LLCP & SNEP selection in P2P*/
typedef enum phDtaLib_eP2PType
{
     PHDTALIB_P2P_DEFAULT                   = 0x00,     /**< Enable default P2P without LLCP or SNEP */
     PHDTALIB_P2P_LLCP_WITH_CONN_PARAMS     = 0x01,     /**< Enable LLCP with Parameters in connect PDU*/
     PHDTALIB_P2P_LLCP_WITHOUT_CONN_PARAMS  = 0x02,     /**< Enable LLCP without parameters in connect PDU*/
     PHDTALIB_P2P_SNEP                      = 0x03      /**< Enable SNEP*/
}phDtaLib_eP2PType_t;

/** \ingroup grp_dta_lib
    Definitions for phDtaLib_RfTech
    Specifies set of technologies */
#define DTALIB_RFTECHNOLOGY_MASK_A    0x01    /**< NFC Technology A */
#define DTALIB_RFTECHNOLOGY_MASK_B    0x02    /**< NFC Technology B */
#define DTALIB_RFTECHNOLOGY_MASK_F    0x04    /**< NFC Technology F */
typedef uint32_t phDtaLib_RfTechMask;

/** \ingroup grp_dta_lib
    RF Discovery Technology and Mode*/
typedef struct phDtaLib_sDiscParams
{
    phDtaLib_RfTechMask dwPollRdWrt; /**< Technologies to enable for Read/Write in Poll Mode*/
    phDtaLib_RfTechMask dwPollP2P;    /**< Technologies to enable for P2P in Poll Mode*/
    phDtaLib_RfTechMask dwListenP2P;  /**< Technologies to enable for P2P in Listen Mode*/
    phDtaLib_RfTechMask dwListenHCE;  /**< Technologies to enable for Host Card Emulation in Listen Mode*/
    phDtaLib_RfTechMask dwListenESE;  /**< Technologies to enable for Card Emulation from Ese in Listen Mode*/
    phDtaLib_RfTechMask dwListenUICC; /**< Technologies to enable for Card Emulation from UICC in Listen Mode*/
}phDtaLib_sDiscParams_t;

typedef struct phDtaLib_sIUTInfo
{
    uint8_t dtaLibVerMajor;    /**< Major version of DTA Lib*/
    uint8_t dtaLibVerMinor;    /**< Minor version of DTA Lib*/
    uint8_t mwVerMajor;        /**< Major version of NFC MW library*/
    uint8_t mwVerMinor;        /**< Minor version of NFC MW library*/
    uint8_t fwVerMajor;        /**< Major version of NFC FW*/
    uint8_t fwVerMinor;        /**< Minor version of NFC FW*/
}phDtaLib_sIUTInfo_t, *pphDtaLib_sIUTInfo_t;

typedef struct phDtaLib_sTestProfile
{
    uint32_t Pattern_Number;    /**< Pattern Number*/
    char Certification_Release[10];
    BOOLEAN DtaDebugFlag;
    uint32_t TimeSlotNumber;
    uint32_t ConnDeviceLimit;
    /*RFU*/         /**< Test Case ID and other parameters in Auto Mode*/
}phDtaLib_sTestProfile_t;

#ifdef WIN32
/**< Windows macro declarations */
/* Configuration for extending symbols for different platform */
#ifndef DTALIB_LIB_EXTEND
#define DTALIB_LIB_EXTEND extern
#endif

/* Response field values */
#define DTASTATUS_SNEP_INVALID_PROTOCOL_DATA    0x59

/* Client Miu for PUT request */
#define PH_DTA_CLIENT_MIU                       128

/* Client RW for PUT request */
#define PH_DTA_CLIENT_RW                        4

/* First packet offset */
#define PH_DTA_OFFSET_ONE                       1

/* Third Packet OFFSET */
#define PH_DTA_OFFSET_THREE                     3

/* Response field not found */
#define PH_DTA_RESPONSE_NOT_FOUND               0x52

/* Request is rejected */
#define PH_DTA_REQUEST_REJECTED                 0x93

/* Invalid request from Client */
#define PH_DTA_INVALID_REQUEST                  0x59

/** \ingroup grp_dta_snep_lib
    List of all Snep Client test cases */
typedef enum phDtaLib_snepClientTestCases
{
    PH_DTA_TC_C_BIT_BV_01 = 1,
    PH_DTA_TC_C_BIT_BI_01_0,
    PH_DTA_TC_C_BIT_BI_01_1,
    PH_DTA_TC_C_PUT_BV_01,
    PH_DTA_TC_C_PUT_BV_02,
    PH_DTA_TC_C_PUT_BI_01,
    PH_DTA_TC_C_GET_BV_01,
    PH_DTA_TC_C_GET_BV_02,
    PH_DTA_TC_C_GET_BV_03
}phDtaLib_snepClientTestCases;

/** \ingroup grp_dta_snep_lib
    List of all Extended Snep Server test cases */
typedef enum phDtaLib_snepExtServerTestCases
{
    PH_DTA_TC_S_ACC_BV_01 = 1,
    PH_DTA_TC_S_ACC_BV_03,
    PH_DTA_TC_S_ACC_BV_05_0,
    PH_DTA_TC_S_ACC_BV_05_1,
    PH_DTA_TC_S_ACC_BV_05_2,
    PH_DTA_TC_S_RET_BV_01,
    PH_DTA_TC_S_RET_BV_02,
    PH_DTA_TC_S_RET_BV_03,
    PH_DTA_TC_S_RET_BI_01
}phDtaLib_snepExtServerTestCases;

/** \ingroup grp_dta_snep_lib
    List of all Default Snep Server test cases */
typedef enum phDtaLib_snepDefServerTestCases
{
    PH_DTA_TC_S_BIT_BV_01 = 1,
    PH_DTA_TC_S_BIT_BV_02,
    PH_DTA_TC_S_BIT_BV_03,
    PH_DTA_TC_S_BIT_BI_01_0,
    PH_DTA_TC_S_BIT_BI_01_1,
    PH_DTA_TC_S_BIT_BI_01_2,
    PH_DTA_TC_S_BIT_BI_02,
    PH_DTA_TC_S_ACC_BV_02,
    PH_DTA_TC_S_ACC_BV_04,
    PH_DTA_TC_S_ACC_BV_06_0,
    PH_DTA_TC_S_ACC_BV_06_1,
    PH_DTA_TC_S_ACC_BV_06_2,
    PH_DTA_TC_S_ACC_BV_07,
    PH_DTA_TC_S_RET_BV_04
}phDtaLib_snepDefServerTestCases;
#endif
/**< End */


/**<Callback interface between DTALIB and JNI layer*/
typedef void (*phdtaLib_EvtCb_t) (void* dtaApplHdl, phdtaAppLib_eEvtType_t eEvtType, phdtaLib_sEvtData_t*  sEvtData);

/**
 * \ingroup grp_dta_lib
 * \brief Initializes the DTA and NFC library
 *
 * This function initializes DTA library and NFC including underlying layers.
 * A session with NFC hardware will be established.
 *
 * \note Before successful initialization other NFC functionalities are not enabled.
 * this function doesn't initialise NFCEE.
 * \param[in]                           None
 * \retval #DTASTATUS_SUCCESS           DTA LIB Initialised successfully
 * \retval #DTASTATUS_FAILED            DTA LIB failed to initialise.
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_Init();

/**
 * \ingroup grp_dta_lib
 * \brief DeInitializes the DTA and NFC library
 *
 * This function DeInitializes DTA library and NFC including underlying layers.
 * A session with NFC hardware will be closed.
 *
 * \param[in]                           None
 * \retval #DTASTATUS_SUCCESS           DTA LIB De-Initialised successfully
 * \retval #DTASTATUS_FAILED            DTA LIB failed to De-initialise.
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_DeInit();

/**
 * \ingroup grp_dta_lib
 * \brief Initializes the DTA and NFC library
 *
 * This function initializes NFCEE(UICC or eSE) needed for DTA.
 *
 * \param[in] DevType                   NFCEE type #phDtaLib_NfceeDevType_t
 * \retval #DTASTATUS_SUCCESS           NFCEE initialised successfully
 * \retval #DTASTATUS_FAILED            DTA LIB failed to De-initialise.
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_EeInit(phDtaLib_eNfceeDevType_t DevType);

/**
 * \ingroup grp_dta_lib
 * \brief Enable discovery(Poll + Listen)
 *
 * This function configures enables NFCC to run discovery wheel.
 *
 * \param[in] DiscMask                  Technology types for poll and listen modes #phDtaLib_DiscMask_t
                                        DiscMask = 0 (Disable Discovery)
 * \retval #DTASTATUS_SUCCESS           Discovery enabled successfully
 * \retval #DTASTATUS_FAILED            Failed to enable discovery
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_EnableDiscovery(phDtaLib_sDiscParams_t* DiscParams);

/**
 * \ingroup grp_dta_lib
 * \brief Disable discovery(Poll + Listen)
 *
 * This function Disables NFCC to run discovery wheel.
 *
 * \param[in] DiscMask                  Technology types for poll and listen modes #phDtaLib_DiscMask_t
                                        DiscMask = 0 (Disable Discovery)
 * \retval #DTASTATUS_SUCCESS           Discovery disabled successfully
 * \retval #DTASTATUS_FAILED            Failed to disable discovery
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_DisableDiscovery();

/**
 * \ingroup grp_dta_lib
 * \brief Get IUT Information.
 *
 * This function Retrieves Version information of DTA LIB, MW and FW.
 *
 * \param[in] IUTInfo                  Pointer to #phDtaLib_IUTInfo_t, Memory shall be allocated by caller.
 * \retval #DTASTATUS_SUCCESS           IUT information retrieved successfully.
 * \retval #DTASTATUS_FAILED            Failed to get IUT information
 * \retval #DTASTATUS_INVALID_PARAM     Invalid parameter to function
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_GetIUTInfo(phDtaLib_sIUTInfo_t* psIUTInfo);

/**
 * \ingroup grp_dta_lib
 * \brief Set Test Profile
 *
 * This function sets the test profile to configure DTA for execution.
 *
 * \param[in] IUTInfo                  Pointer to #phDtaLib_IUTInfo_t, Memory shall be allocated by caller.
 * \retval #DTASTATUS_SUCCESS           IUT information retrieved successfully.
 * \retval #DTASTATUS_FAILED            Failed to get IUT information
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_SetTestProfile(phDtaLib_sTestProfile_t TestProfile);

/**
 * \ingroup grp_dta_lib
 * \brief Configure type of P2P: LLCP or SNEP or defaultP2P.
 *
 * This function configures P2P Type: LLCP/SNEP/DefaultP2P.
 * This P2P type configuration is valid only when dwPollP2P/dwListenP2P is configured
 * during discovery
 *
 * \param[in] p2pType                   P2P type #phDtaLib_eP2pDevType_t
 *
 * \retval #DTASTATUS_SUCCESS           P2P type configured successfully
 * \retval #DTASTATUS_FAILED            P2P type configuration failed
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLib_ConfigureP2p(phDtaLib_eP2PType_t p2pType);

/**
 * \ingroup grp_dta_lib
 * \brief Register callbacks which will be invoked when required data is available.
 *
 * This function is used to register callback from application to dtalib, which will be invoked when required data is available.
 *
 * \param[in] dtaLibIfHdl               DTALIB Handler
 * \param[in] dtaJNIHdl                 JNI Handler
 *
 * \retval                              None
 *
 *
 */
void phDtaLib_RegisterCallback(void *dtaApplHdl, phdtaLib_EvtCb_t dtaApplCb);

/**
 * \ingroup grp_dta_lib
 * \brief Set configuration for NFC controller before starting the discovery loop.
 *
 * This function Set configuration for NFC controller before starting the discovery loop.
 *
 * \param[in]                  None
 *
 * \retval #DTASTATUS_SUCCESS  DTA LIB Configuration is successful
 * \retval #DTASTATUS_FAILED   DTA LIB Failed to configure
 *
 */
DTASTATUS phDtaLib_SetConfig();

/**
 * \ingroup grp_dta_lib
 * \brief Get configuration for NFCC.
 *
 * This function will Get the configuration of NFCC.
 *
 * \param[in] bnumIDs            NCI Configurations ID's
 * \param[in] pbparamIDs         Configuration ID data buffer
 * \param[in] pvOutBuff          Output Buffer required data handling
 * \param[in] dwLenOutBuff       Length of OutBuffer

 *
 * \retval #DTASTATUS_SUCCESS    If successfully initiated
 * \retval #DTASTATUS_FAILED     Otherwise
 *
 */
DTASTATUS phDtaLib_GetConfig(uint8_t bnumIDs, uint8_t *pbparamIDs, uint8_t *pvOutBuff,uint32_t *dwLenOutBuff);

/**
* windows declarations
*/

#ifdef WIN32
/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Snep Client Basic Interconnection Tests
 *
 * Function To Handle all the following Snep Client Basic Interconnection Test cases.
 *
 *      TC_C_BIT_BV_01
 *      TC_C_BIT_BI_01_0
 *      TC_C_BIT_BI_01_1
 *
 * \param[in] testCaseID              Tag testing required params
 *
 * \retval #DTASTATUS_SUCCESS           Client basic test case is successful
 * \retval #DTASTATUS_FAILED            Client basic test case is Unsuccessful
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLibi_snepClient_basic(phDtaLib_snepClientTestCases clientCaseID);

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Snep Client Put Tests
 *
 * Function To Handle all the Snep Client Put Test cases.
 *
 *      TC_C_PUT_BV_01
 *      TC_C_PUT_BV_02
 *      TC_C_PUT_BI_01
 *
 * \param[in] testCaseID              Tag testing required params
 *
 * \retval #DTASTATUS_SUCCESS           Client basic test case is successful
 * \retval #DTASTATUS_FAILED            Client basic test case is Unsuccessful
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLibi_snepClient_put(phDtaLib_snepClientTestCases clientCaseID);

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Snep Client Get Tests
 *
 * Function To Handle all the Snep Client Get Test cases.
 *
 *          TC_C_GET_BV_01
 *          TC_C_GET_BV_02
 *          TC_C_GET_BV_03
 *
 * \param[in] testCaseID              Tag testing required params
 *
 * \retval #DTASTATUS_SUCCESS           Client basic test case is successful
 * \retval #DTASTATUS_FAILED            Client basic test case is Unsuccessful
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLibi_snepClient_get(phDtaLib_snepClientTestCases clientCaseID);

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Default Snep Server Tests
 *
 * Function To intialise pre requisitesfor Snep Server
 *
 * \retval #DTASTATUS_SUCCESS           Server test case success
 * \retval #DTASTATUS_FAILED            Server test case failed
 *
 */
DTASTATUS phDtaLib_snepServerSetUp();

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Client Tests
 *
 * Function To intialise pre requisites for Snep client
 *
 * \retval #DTASTATUS_SUCCESS           Server test case success
 * \retval #DTASTATUS_FAILED            Server test case failed
 *
 */
DTASTATUS phDtaLib_snepClientSetUp();

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Default Snep Server Tests
 *
 * Function To Handle all Default Snep Server Test cases as mentioned below
 *
 *      TC_S_BIT_BV_01
 *      TC_S_BIT_BV_02
 *      TC_S_BIT_BV_03
 *      TC_S_BIT_BI_01_0
 *      TC_S_BIT_BI_01_1
 *      TC_S_BIT_BI_01_2
 *      TC_S_BIT_BI_02
 *      TC_S_ACC_BV_02
 *      TC_S_ACC_BV_04
 *      TC_S_ACC_BV_07
 *
 * \retval #DTASTATUS_SUCCESS           Server test case success
 * \retval #DTASTATUS_FAILED            Server test case failed
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLibi_snepServerDefault(phDtaLib_snepDefServerTestCases serverCaseID);

/**
 * \ingroup grp_dta_snep_lib internal function
 * \brief Extended Snep Server Tests
 *
 * Function To Handle all Extended Snep Server Test cases as mentioned below
 *
 *      TC_S_ACC_BV_01
 *      TC_S_ACC_BV_03
 *      TC_S_ACC_BV_05_0
 *      TC_S_ACC_BV_05_1
 *      TC_S_ACC_BV_05_2
 *      TC_S_RET_BV_01
 *      TC_S_RET_BV_02
 *      TC_S_RET_BV_03
 *      TC_S_RET_BI_01
 *
 * \retval #DTASTATUS_SUCCESS           Server test case success
 * \retval #DTASTATUS_FAILED            Server test case failed
 *
 */
DTALIB_LIB_EXTEND DTASTATUS phDtaLibi_snepServerExtended(phDtaLib_snepExtServerTestCases serverCaseID);
#endif

/**
* End
*/


#ifdef __cplusplus
}
#endif

#endif
