/*
* Copyright (C) 2019 NXP Semiconductors
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
 * \file phDTA_T5TTest.c
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

//static uint8_t gs_T5TBuff[400]  = {0};    /**< Buffer to store the read Tag data */
uint32_t g_dwBlkNumToStart = 0;           /**< Block number to start reading */
uint32_t g_dwBlkNumToEnd = 0;             /**< Block number to end reading */
uint32_t g_dwNumOfBlks = 0;               /**< Number of blocks to be read */

extern phDtaLib_sHandle_t g_DtaLibHdl;

/* The NFC Forum Device SHALL store the NDEF Message retrieved by the NDEF
 * Read procedure in the NDEF message buffer.
 */
#define MAX_T5T_TAG_BUFFER_SIZE   8192    /**< T5T Tag maximum buffer size */
#define T5T_MAX_BLOCK_SIZE        64      /**< T5T Tag maximum block size */
//static uint8_t gs_T5TNdefMsgBuff[MAX_T5T_TAG_BUFFER_SIZE]  = {0};
  /**< T5T NDEF Message buffer usedto store the NDEF message retrieved during Read Procedure */
//static uint32_t gs_T5TNdefBuffLength = 0; /**< Length for Misc array */

#define T5T_REQ_FLAG_NAMS 0x02  /**< Type 5 Tag Request Flag with Non-Addressed Mode*/
#define T5T_REQ_FLAG_SEL  0x12  /**< Type 5 Tag request Flag with T5T in Selected State*/
#define T5T_REQ_FLAG_AMS  0x22  /**< Type 5 Tag Request Flag with Addressed Mode Selector*/
#define T5T_REQ_FLAG_AMS_OPFLAG  0x62  /**< Type 5 Tag Request Flag with Addressed Mode Selector & Option Flag*/

#define T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,blkNum,t5tReqFlag)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_READ_CMD;\
        psTagParams->sBuffParams.pbBuff = gs_T5TBuff;\
        psTagParams->dwBlockNum = blkNum;\
        psTagParams->reqFlag = t5tReqFlag;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_READ_MULTIPLE_BLOCKS(dwMwIfStatus,psTagParams,mwIfHdl,g_dwBlkNumToStart,g_dwNumOfBlks,t5tReqFlag)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_READ_MULTIPLE_CMD;\
        psTagParams->sBuffParams.pbBuff = gs_T5TBuff;\
        psTagParams->dwFirstBlockNum = g_dwBlkNumToStart;\
        psTagParams->dwNumOfBlocks = g_dwNumOfBlks;\
        psTagParams->reqFlag = t5tReqFlag;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,blkNum,tagData,t5tReqFlag)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_WRITE_CMD;\
        psTagParams->dwBlockNum = blkNum;\
        psTagParams->reqFlag = t5tReqFlag;\
        psTagParams->sBuffParams.pbBuff = (uint8_t*)tagData;\
        psTagParams->sBuffParams.dwBuffLength = sizeof(tagData);\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwblkNumToStart,g_dwBlkNumToEnd,t5tReqFlag) {\
        if(g_dwBlkNumToStart > g_dwBlkNumToEnd) {\
          phOsal_LogError((const uint8_t*)"DTALib>T5T_READ:Error Check block number to start and block number to end !! \n");  }\
        else  {\
          gs_T5TNdefBuffLength = 0;\
          for( ; g_dwBlkNumToStart <= g_dwBlkNumToEnd ; ) {\
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwBlkNumToStart,t5tReqFlag)\
            g_dwBlkNumToStart++;\
            for( uint32_t dataCounter = 0; dataCounter <= psTagParams->sBuffParams.dwBuffLength; ) {\
              gs_T5TNdefMsgBuff[gs_T5TNdefBuffLength++] = psTagParams->sBuffParams.pbBuff[dataCounter++]; }\
            phDtaLibi_PrintBuffer((uint8_t *)gs_T5TNdefMsgBuff, gs_T5TNdefBuffLength, (const uint8_t *)"gs_T5TNdefMsgBuff  : ");\
          } \
        } \
      }

#define T5T_EXTENDED_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwblkNumToStart,g_dwBlkNumToEnd,t5tReqFlag) {\
        if(g_dwBlkNumToStart > g_dwBlkNumToEnd) {\
          phOsal_LogError((const uint8_t*)"DTALib>T5T_READ:Error Check block number to start and block number to end !! \n");  }\
        else  {\
          gs_T5TNdefBuffLength = 0;\
          for( ; g_dwBlkNumToStart <= g_dwBlkNumToEnd ; ) {\
            T5T_EXTENDED_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwBlkNumToStart,t5tReqFlag)\
            g_dwBlkNumToStart++;\
            for( uint32_t dataCounter = 0; dataCounter <= psTagParams->sBuffParams.dwBuffLength; ) {\
              gs_T5TNdefMsgBuff[gs_T5TNdefBuffLength++] = psTagParams->sBuffParams.pbBuff[dataCounter++]; }\
            phDtaLibi_PrintBuffer((uint8_t *)gs_T5TNdefMsgBuff, gs_T5TNdefBuffLength, (const uint8_t *)"gs_T5TNdefMsgBuff  : ");\
          } \
        } \
      }

#define T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwblkNumToStart,g_dwBlkNumToEnd,blkSize,t5tReqFlag)  {\
        if(g_dwBlkNumToStart > g_dwBlkNumToEnd) {\
          phOsal_LogError((const uint8_t*)"DTALib>T5T_WRITE:Error Check block number to start and block number to end !! \n");  }\
        else  {\
          uint32_t byteCount = 0;\
          uint8_t t5tWriteData[T5T_MAX_BLOCK_SIZE];\
          gs_T5TNdefBuffLength = strlen((const char* const)gs_T5TNdefMsgBuff);\
          for( uint32_t dataCounter = 0; g_dwBlkNumToStart <= g_dwBlkNumToEnd ; ) {\
            byteCount = 0;\
            memset(t5tWriteData, '\0', sizeof(t5tWriteData));\
            for( ; byteCount < blkSize; ) {\
              t5tWriteData[byteCount++] = gs_T5TNdefMsgBuff[dataCounter++]; }\
            phDtaLibi_PrintBuffer((uint8_t *)t5tWriteData, strlen((const char* const)t5tWriteData), (const uint8_t *)"t5tWriteData : ");\
            T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwBlkNumToStart,t5tWriteData,t5tReqFlag)\
            g_dwBlkNumToStart++;\
          } \
         } \
        } \

#define T5T_EXTENDED_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwblkNumToStart,g_dwBlkNumToEnd,blkSize,t5tReqFlag)  {\
        if(g_dwBlkNumToStart > g_dwBlkNumToEnd) {\
          phOsal_LogError((const uint8_t*)"DTALib>T5T_WRITE:Error Check block number to start and block number to end !! \n");  }\
        else  {\
          uint32_t byteCount = 0;\
          uint8_t t5tWriteData[T5T_MAX_BLOCK_SIZE];\
          gs_T5TNdefBuffLength = strlen((const char* const)gs_T5TNdefMsgBuff);\
          for( uint32_t dataCounter = 0; g_dwBlkNumToStart <= g_dwBlkNumToEnd ; ) {\
            byteCount = 0;\
            memset(t5tWriteData, '\0', sizeof(t5tWriteData));\
            for( ; byteCount < blkSize; ) {\
              t5tWriteData[byteCount++] = gs_T5TNdefMsgBuff[dataCounter++]; }\
            phDtaLibi_PrintBuffer((uint8_t *)t5tWriteData, strlen((const char* const)t5tWriteData), (const uint8_t *)"t5tWriteData : ");\
            T5T_EXTENDED_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,g_dwBlkNumToStart,t5tWriteData,t5tReqFlag)\
            g_dwBlkNumToStart++;\
          } \
         } \
        } \

#define T5T_INVENTORY_REQ(dwMwIfStatus,psTagParams,mwIfHdl,t5tUidx,t5tReqFlag)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_INVENTORY_REQ_CMD;\
        psTagParams->t5tUid = (uint8_t*)t5tUidx;\
        psTagParams->reqFlag = t5tReqFlag;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_SELECT_TAG(dwMwIfStatus,psTagParams,mwIfHdl,t5tUidx)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_SELECT_UID_CMD;\
        psTagParams->t5tUid = (uint8_t*)t5tUidx;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_STAY_QUIET(dwMwIfStatus,psTagParams,mwIfHdl,t5tUidx)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_STAY_QUIET_CMD;\
        psTagParams->t5tUid = (uint8_t*)t5tUidx;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_LOCK_SINGLE_BLOCK(dwMwIfStatus,psTagParams,mwIfHdl,blkNum)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_LOCK_SINGLE_BLOCK_CMD;\
        psTagParams->dwBlockNum = blkNum;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

#define T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,mwIfHdl,t5tReqFlag)  {\
        psTagParams->eT5TCmd = PHMWIF_T5T_REQ_FLAG_CMD;\
        psTagParams->reqFlag = t5tReqFlag;\
        dwMwIfStatus = phMwIf_TagCmd(mwIfHdl, PHMWIF_PROTOCOL_T5T,(phMwIf_uTagParams_t*)psTagParams);\
        if(dwMwIfStatus != MWIFSTATUS_SUCCESS)\
            break;}

/**
*   T5T Tag read/write operations based on pattern number set by the user.
*   Dynamic Execution refers to dynamically retrieving the data by tag read. Data retrieved is saved
*   and used for tag write operations later as explained in the table 2 of T5T tag test cases
*   specifications.
*/
DTASTATUS phDtaLibi_T5TOperations_DynamicExecution(phDtaLib_sTestProfile_t TestProfile)
{
    MWIFSTATUS dwMwIfStatus = 0;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    phMwIf_uTagParams_t   sTagParams;
    phMwIf_sT5TParams_t*   psTagParams = (phMwIf_sT5TParams_t *)&sTagParams;
    //uint8_t tagReadDataBuf[400]; /**< Misc buffer used for compare */
    DTASTATUS  dwDtaStatus = DTASTATUS_FAILED;
    phMwIf_sNdefDetectParams_t* psNdefDetectParams;
    phMwIf_uTagOpsParams_t sTagOpsParams;

    LOG_FUNCTION_ENTRY;
    memset(psTagParams, 0, sizeof(phMwIf_uTagParams_t));
    //sTagParams.sT5TPrms.sBuffParams.dwBuffLength = sizeof(gs_T5TBuff);
    sTagOpsParams.sBuffParams.pbBuff = gs_ndefReadWriteBuff;

    phOsal_LogDebugString((const uint8_t*)"DTALib> :", (const uint8_t*)__FUNCTION__);
    phOsal_LogDebugU32h((const uint8_t*)"PatternNum = ", TestProfile.Pattern_Number);
    switch(TestProfile.Pattern_Number)
    {

#if 1   /* Added to retian the previous code for now. To implement Tag operations
           using CheckNDEF, ReadNDEF & WriteNDEF */

        // Type 5 Tag Operation NDEF Read Test Functionality. //
        /* The NFC Forum Device SHALL use the non-addressed mode for all commands
         * for all pattern numbers except for #0021.
         */
        case 0x0001:
        case 0x0011:
        case 0x0021:
        {
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation READ Test Functionality");

          /*Send Request Flag Type based on the Pattern Number selected depending the test case*/
          if((TestProfile.Pattern_Number == 0x01) || (TestProfile.Pattern_Number == 0x11))
          {
            T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_NAMS)
          }
          else if ((TestProfile.Pattern_Number == 0x21) || (TestProfile.Pattern_Number == 0x31))
          {
            T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_AMS)
          }

          dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
            phOsal_LogError((const uint8_t*)"DTALib>T5T:Device is not NDEF Compliant\n");
            break;
          }

          psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
          if(!psNdefDetectParams->dwStatus)
          {
            phOsal_LogDebug((const uint8_t*)"DTALib>T5T:Tag is NDEF compliant \n");
            phOsal_LogDebug((const uint8_t*)"DTALib>T5T:Read NDEF \n");
            dwDtaStatus = phDtaLibi_ReadNDEF(&sTagOpsParams);
            if (dwDtaStatus != DTASTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib>T5T:Error Could not read data !!\n");
              break;
            }
            memset(gs_ndefReadWriteBuff, 0, sizeof(gs_ndefReadWriteBuff));
            memcpy(gs_ndefReadWriteBuff,sTagOpsParams.sBuffParams.pbBuff, sTagOpsParams.sBuffParams.dwBuffLength);
            gs_sizeNdefRWBuff = sTagOpsParams.sBuffParams.dwBuffLength;
            phOsal_LogDebugU32d((const uint8_t*)"DTALib>:T5T:NDEF READ Length: ", gs_sizeNdefRWBuff);
          }
        }
        break;

        /* Pattern Numbers to test write functionality*/
        case 0x0002:
        case 0x0012:
        {
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation WRITE Test Functionality");
          /*Send Request Flag Type based on the Pattern Number selected depending the test case*/
          if(TestProfile.Pattern_Number == 0x02)
          {
            T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_NAMS)
          }
#if(ENABLE_CR12_SUPPORT == FALSE) /* CR12_ON_AR12_CHANGE */
          else if(TestProfile.Pattern_Number == 0x12)
          {
            T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_AMS_OPFLAG)
          }
#endif /* CR12_ON_AR12_CHANGE */

          phOsal_LogDebug ((const uint8_t*)"DTALib>T5T:Perform NDEF Check \n");
          dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
            phOsal_LogError((const uint8_t*)"DTALib>T5T:Device is not NDEF Compliant\n");
            break;
          }

          psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
          if(!psNdefDetectParams->dwStatus)
          {
            phOsal_LogDebug((const uint8_t*)"DTALib>T5T:Tag is NDEF compliant \n");
            phOsal_LogDebug((const uint8_t*)"DTALib>T5T:Write NDEF Message \n");
            sTagOpsParams.sBuffParams.dwBuffLength = gs_sizeNdefRWBuff;
            phOsal_LogDebugU32d((const uint8_t*)"DTALib>:T5T:NDEF WRITE Length: ", sTagOpsParams.sBuffParams.dwBuffLength);
            dwDtaStatus = phDtaLibi_WriteNDEF(&sTagOpsParams);
            if(dwDtaStatus != DTASTATUS_SUCCESS)
            {
              phOsal_LogError((const uint8_t*)"DTALib>T5T:Device is not NDEF Complaint\n");
              break;
            }
          }
        }
        break;

        case 0x0003:
        {
          // TC_T5T_NIA_TRANS_BV_1_x test case
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation READ / WRITE to READ-ONLY Test Functionality");
          T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_NAMS)
          phOsal_LogDebug ((const uint8_t*)"DTALib>T5T:Perform NDEF Check \n");
          dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
            phOsal_LogError((const uint8_t*)"DTALib>T5T:Device is not NDEF Compliant\n");
            break;
          }
          dwDtaStatus = phDtaLibi_SetTagReadOnly(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
            phOsal_LogError((const uint8_t*)"DTALib> T5T:Error Could not set tag readonly !!\n");
            break;
          }
        }
        break;

        case 0x0031:
        {
          //TC_T5T_CS_SLPSEL_BV_1 test case
          uint8_t t5tUid1[] = {0xF0, 0x10, 0x32, 0x54, 0x76, 0x98, 0xAB, 0xE0}; /** T5T UID1*/
          uint8_t t5tUid2[] = {0xF1, 0x10, 0x32, 0x54, 0x76, 0x99, 0xAB, 0xE0}; /** T5T UID2*/
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0031, Read with 1 tag in QUIET state and 1 tag in SELECTED state");
#if(ENABLE_CR12_SUPPORT == FALSE) /* CR12_ON_AR12_CHANGE */
          T5T_REQ_FLAG_TYPE(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl, T5T_REQ_FLAG_SEL)
#endif /* CR12_ON_AR12_CHANGE */
          //The sequence to be followed is Type V Sleep -> RF_DEACTIVATE_CMD(Sleep) -> RF_DISCOVER_SELECT_CMD -> Type V Select
          T5T_STAY_QUIET(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid2) // T5T Stay Quiet UID1
          phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_SLEEP); // RF Deactivate with Sleep
          phMwIf_RfDiscoverySelectNextDevice(dtaLibHdl->mwIfHdl); //  RF Discovery Select next device
          T5T_SELECT_TAG(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid1) // T5T Select Tag with UID2
          dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
          if (dwDtaStatus != DTASTATUS_SUCCESS)
          {
            phOsal_LogError((const uint8_t*)"DTALib>T5T:Device is not NDEF Compliant\n");
            break;
          }
        }

        default:
          phOsal_LogError((const uint8_t*)"DTALib>T5T:Error Pattern Number not valid for T5T !! \n");
        break;
      }

#else
        // Type 5 Tag Operation NDEF Read Test Functionality. //

        /* The NFC Forum Device SHALL use the non-addressed mode for all commands
         * for all pattern numbers except for #0021.
         */

        /* Pattern Number to verify minor and major Mapping Version */
        case 0x0001:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation Mapping Version Test Functionality");

          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_NAMS)

          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          //MemLayout = G/Config 0
          if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x73)) //TC_T5T_NIA_READ_BV_1_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x73)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 2;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = G/Config 1
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x73)) //TC_T5T_NIA_READ_BV_1_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x73)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 261;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = G/Config 2
          else if((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0xC3)) //TC_T5T_NIA_READ_BV_2_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0xC3)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 12;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = G/Config 3
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0xC3)) //TC_T5T_NIA_READ_BV_2_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0xC3)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 261;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = A/Config 0
          else if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x0B)) //TC_T5T_MSM_READ_BV_1_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x0B)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 16;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = B/Config 0
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[6] == 0x01)) //TC_T5T_MSM_READ_BV_1_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[6] == 0x01)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 9;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = C/Config 0
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[6] == 0x02)) //TC_T5T_MSM_READ_BV_1_x for x=2
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[1] == 0x02)");
            memcpy(gs_T5TNdefMsgBuff,psTagParams->sBuffParams.pbBuff, (psTagParams->sBuffParams.dwBuffLength));
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 4;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = D/Config 0
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[6] == 0x04)) //TC_T5T_MSM_READ_BV_1_x for x=3
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[1] == 0x04)");
            memcpy(gs_T5TNdefMsgBuff,psTagParams->sBuffParams.pbBuff, ((psTagParams->sBuffParams.dwBuffLength)));
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 2;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = E/Config 0 for x = 0
          //MemLayout = E/Config 1 for x = 1 DataBlock12[4] contains Terminator TLV
          //MemLayout = E/Config 2 for x = 2 DataBlock12[4] contains NDEF TLV
          else if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x06)) //TC_T5T_MSM_READ_BV_2_x for x=0, 1 & 2
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x0B)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 12;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          else if ((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43)) //TC_T5T_MSM_READ_BV_2_x for x=3, 4 & 5
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
            if (tagReadDataBuf[3] == 0x82)
            {
              T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,T5T_REQ_FLAG_NAMS)
              memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
              //MemLayout = F/Config 0 for x = 3
              if((tagReadDataBuf[2] == 0x04) && (tagReadDataBuf[3] == 0x0C))
              {
                g_dwBlkNumToStart = 3;
                g_dwBlkNumToEnd = 261;
              }
              //MemLayout = F/Config 1 for x = 4 DataBlock257[4] contains Terminator TLV
              else if((tagReadDataBuf[2] == 0x03) && (tagReadDataBuf[3] == 0xF9))
              {
                g_dwBlkNumToStart = 3;
                g_dwBlkNumToEnd = 257;
              }
            }
            //MemLayout = F/Config 2 for x = 5 DataBlock272[4] contains NDEF TLV
            else if (tagReadDataBuf[3] == 0x88)
            {
              T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,T5T_REQ_FLAG_NAMS)
              g_dwBlkNumToStart = 256;
              g_dwBlkNumToEnd = 272;
            }
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_NAMS)
          }
          else
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : Else with PN = 01");
          }
        }
        break;

        /* If the pattern number is equal to #0011 and the Capability Container (CC)
         * field byte 3 bit 0 equals to 1b, the NFC Forum Device SHALL use either
         * the READ_MULTIPLE_BLOCK or EXTENDED_READ_MULTIPLE_BLOCK command during
         * the NDEF Read procedure except while reading the CC.
         */
        case 0x0011:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0011 Read, Read multiple blocks or extended read multiple blocks");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_NAMS)
          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          //MemLayout = E/Config 0b
          if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43)) //TC_T5T_NIA_READ_BV_4_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43)");
            g_dwBlkNumToStart = 1;
            g_dwNumOfBlks = 12;
            T5T_READ_MULTIPLE_BLOCKS(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwNumOfBlks,T5T_REQ_FLAG_NAMS)
            memcpy(gs_T5TNdefMsgBuff,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          }
          //MemLayout = F/Config 5b
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40)) //TC_T5T_NIA_READ_BV_4_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,T5T_REQ_FLAG_NAMS)
            g_dwBlkNumToStart = 254;
            g_dwNumOfBlks = 19;
            T5T_READ_MULTIPLE_BLOCKS(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwNumOfBlks,T5T_REQ_FLAG_NAMS)
            memcpy(gs_T5TNdefMsgBuff,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          }
          else
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : Else with PN = 11");
          }
        }
        break;

        /* If the pattern number is equal to #0021, the NFC Forum Device SHALL use
         * the addressed mode for all commands during the NDEF Read procedure.
         */
        case 0x0021:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0021 Read, addressed mode for NDEF Read Procedure");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_AMS)
          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          //MemLayout = E/Config 0
          if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43)) //TC_T5T_NIA_READ_BV_3_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43)");
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 12;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_AMS)
          }
          //MemLayout = F/Config 2
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43)) //TC_T5T_NIA_READ_BV_3_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x43)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
            g_dwBlkNumToStart = 2;
            g_dwBlkNumToEnd = 272;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_AMS)
          }
          //MemLayout = F/Config 4b
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40)) //TC_T5T_NIA_READ_BV_3_x for x=2
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
            g_dwBlkNumToStart = 2;
            g_dwBlkNumToEnd = 261;
            T5T_READ_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,T5T_REQ_FLAG_AMS)
          }
          else
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : Else with PN = 21");
          }
        }
        break;

        /* If the pattern number is equal to #0031 and the collision resolution activity
         * resolved two UIDs, the NFC Forum Device SHALL at first put one tag to QUIET state,
         * then the second tag SHALL be put to SELECTED state. Afterwards the NFC Forum Device
         * SHALL perform the NDEF Read procedure for the tag in SELECTED state.
         */
        case 0x0031:
        {
          //TC_T5T_CS_SLPSEL_BV_1 test case
          uint8_t t5tUid1[] = {0xE0, 0xAB, 0x98, 0x76, 0x54, 0x32, 0x10, 0xF0}; /** T5T UID1*/
          uint8_t t5tUid2[] = {0xE0, 0xAB, 0x99, 0x76, 0x54, 0x32, 0x10, 0xF1}; /** T5T UID2*/

          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0031, Read with 1 tag in QUIET state and 1 tag in SELECTED state");
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          T5T_INVENTORY_REQ(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid1,T5T_REQ_FLAG_AMS) // Inventory Request with UID1
          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          T5T_INVENTORY_REQ(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid2,T5T_REQ_FLAG_AMS) // Inventory Request with UID2
          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          T5T_INVENTORY_REQ(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,NULL,T5T_REQ_FLAG_AMS) // Inventory Request with UID2
          T5T_STAY_QUIET(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid1) // T5T Stay Quiet UID1
          T5T_SELECT_TAG(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,t5tUid2) // T5T Select Tag with UID2
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_SEL)
        }
        break;

        // Type 5 Tag Operation NDEF Write Test Functionality //
        /*
         * The NDEF Message stored in the NDEF message buffer SHALL be used for multiple
         * NDEF Write procedures if sequentially executed without an NDEF Read procedure
         * in between.
         *
         * If the pattern number is equal to #0012, the NFC Forum Device SHALL use
         * the addressed mode for all commands during the NDEF Write procedure.
         */
        case 0x0002:
        {
          uint32_t blkSize = 4;
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0002 Write Test Functionality");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_NAMS)

          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          //MemLayoutDest = E/Config 4; MemLayoutSrc = E/Config 0b
          if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x06)) //TC_T5T_NIA_WRITE_BV_1_x for x=0
          {
            uint8_t WriteData[10]; /**< Local T5T Write Data Buffer  */
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x06)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            /*The following is to reset the L-Field as specificed in T5T test case spec */
            memset(WriteData, '\0', sizeof(WriteData));
            WriteData[0] = gs_T5TNdefMsgBuff[0];
            WriteData[1] = 0x00;
            WriteData[2] = gs_T5TNdefMsgBuff[2];
            WriteData[3] = gs_T5TNdefMsgBuff[3];
            T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,WriteData,T5T_REQ_FLAG_NAMS)
            g_dwBlkNumToStart = 1;
            g_dwBlkNumToEnd = 12;
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayoutDest = F/Config 4; MemLayoutSrc = F/Config 5b for //TC_T5T_NIA_WRITE_BV_1_x for x=1
          //MemLayoutDest = F/Config 4; MemLayoutSrc = F/Config 0 for //TC_T5T_MSM_WRITE_BV_2_x for x = 4
          //MemLayoutDest = F/Config 4; MemLayoutSrc = F/Config 1 for //TC_T5T_MSM_WRITE_BV_2_x for x = 5
          //MemLayoutDest = F/Config 4; MemLayoutSrc = F/Config 2 for //TC_T5T_MSM_WRITE_BV_2_x for x = 6
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x00))
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x00)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
            if((tagReadDataBuf[0] == 0x00) && (tagReadDataBuf[1] == 0x00) && (tagReadDataBuf[3] == 0x82))
            {
              T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,T5T_REQ_FLAG_NAMS)
              g_dwBlkNumToStart = 3;
              g_dwNumOfBlks = 273;
            }
            else
            {
              g_dwBlkNumToStart = 2;
              g_dwNumOfBlks = 261;
            }
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayout = E/Config 0
          else if((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x06)) //TC_T5T_NIA_WRITE_BV_2
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[2] == 0x06)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            //No Write
          }
          //MemLayoutDest = A/Config 1; MemLayoutSrc = A/Config 0
          else if((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x0B)) //TC_T5T_MSM_WRITE_BV_1_x for x = 0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x0B)");
            g_dwBlkNumToStart = 1;
            g_dwNumOfBlks = 17;
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayoutDest = B/Config 1; MemLayoutSrc = B/Config 0
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x01)) //TC_T5T_MSM_WRITE_BV_1_x for x = 1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x01)");
            g_dwBlkNumToStart = 1;
            g_dwNumOfBlks = 9;
            blkSize = 8;
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayoutDest = C/Config 1; MemLayoutSrc = C/Config 0
          else if((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x02)) //TC_T5T_MSM_WRITE_BV_1_x for x = 2
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x02)");
            g_dwBlkNumToStart = 0;
            g_dwNumOfBlks = 4;
            blkSize = 16;
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayoutDest = D/Config 1; MemLayoutSrc = D/Config 0
          else if((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x43) && (tagReadDataBuf[6] == 0x04)) //TC_T5T_MSM_WRITE_BV_1_x for x = 3
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x04)");
            g_dwBlkNumToStart = 0;
            g_dwNumOfBlks = 2;
            blkSize = 32;
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
          //MemLayoutDest = E/Config 4; MemLayoutSrc = E/Config 0 //TC_T5T_MSM_WRITE_BV_2_x for x = 0
          //MemLayoutDest = E/Config 4; MemLayoutSrc = E/Config 1 //TC_T5T_MSM_WRITE_BV_2_x for x = 1
          else if((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[2] == 0x06))
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40) && (tagReadDataBuf[6] == 0x04)");
            T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_NAMS)
            memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
            if((tagReadDataBuf[0] == 0x03) && (tagReadDataBuf[1] == 0x08))
            {
              g_dwBlkNumToStart = 1;
              g_dwNumOfBlks = 12;
            }
            //MemLayoutDest = E/Config 6; MemLayoutSrc = E/Config 2
            else if((tagReadDataBuf[0] == 0x02) && (tagReadDataBuf[1] == 0x2A)) //TC_T5T_MSM_WRITE_BV_2_x for x = 3
            {
              g_dwBlkNumToStart = 1;
              g_dwNumOfBlks = 12;
            }
            T5T_WRITE_BLOCK_BY_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,g_dwBlkNumToStart,g_dwBlkNumToEnd,blkSize,T5T_REQ_FLAG_NAMS)
          }
        }
        break;

        case 0x0012:
        {
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0012 Write Test Functionality");
        }
        break;

        // TRANSITION test cases
        /* The NFC Forum Device SHALL execute Transition from READ/WRITE to READ-ONLY
         * according to the Type 5 Tag Operation (see [T5T], Section 7.6.5).
        */
        case 0x0003:
        {
          phOsal_LogDebug((const uint8_t*)"DTALib> : T5T Pattern 0x0003, Transition from READ/WRITE to READ-ONLY");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_NAMS)
          memcpy(tagReadDataBuf,psTagParams->sBuffParams.pbBuff, psTagParams->sBuffParams.dwBuffLength);
          //MemLayout = E/Config 7
          if ((tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40)) //TC_T5T_NIA_TRANS_BV_1_x for x=0
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40)");
            tagReadDataBuf[1] = 0x43; //sets bits b1b0 of byte 1 of the CC to 11b
            T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,tagReadDataBuf,T5T_REQ_FLAG_NAMS)
            T5T_LOCK_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00)
          }
          //MemLayout = F/Config 7
          else if ((tagReadDataBuf[0] == 0xE2) && (tagReadDataBuf[1] == 0x40)) //TC_T5T_NIA_TRANS_BV_1_x for x=1
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : (tagReadDataBuf[0] == 0xE1) && (tagReadDataBuf[1] == 0x40)");
            tagReadDataBuf[1] = 0x43; //sets bits b1b0 of byte 1 of the CC to 11b
            T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,tagReadDataBuf,T5T_REQ_FLAG_NAMS)
            T5T_LOCK_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00)
          }
          else
          {
            phOsal_LogDebug((const uint8_t*)"DTALib> : Else with PN = 03");
          }
        }
        break;

        case 0x000B:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 00");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_AMS)
        }
        break;

        /** For Test Case: TC_DP21_POL_NFCV_UND_BI_4 & TC_DP21_POL_NFCV_UND_BI_5 */
        case 0x000C:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 00");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 01");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
        }
        break;

        /** For Test Case: TC_DP21_POL_NFCV_T5T_BV_1_x for x = 0 & 1 */
        case 0x000D:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 00");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 01");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 02");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,T5T_REQ_FLAG_AMS)
        }
        break;

        /** For Test Case: TC_DP21_POL_NFCV_T5T_BV_2_x for x = 0 & 1 */
        case 0x000E:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 00");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x00,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 01");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution WriteSingleBlock 01");
          T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,tagReadDataBuf,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution WriteSingleBlock 02");
          T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,tagReadDataBuf,T5T_REQ_FLAG_AMS)
        }
        break;

        /** For Test Case: TC_DP21_POL_NFCV_T5T_BV_2_x for x = 2 & 3 */
        case 0x000F:
        {
          memset(gs_T5TNdefMsgBuff, 0, sizeof(gs_T5TNdefMsgBuff));
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution ReadSingleBlock 01");
          T5T_READ_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,T5T_REQ_FLAG_AMS)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution WriteSingleBlock 01");
          T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x01,tagReadDataBuf,T5T_REQ_FLAG_AMS_OPFLAG)
          phOsal_LogDebug((const uint8_t*)"DTALib> : Type 5 Tag Operation : NFCV test case execution WriteSingleBlock 02");
          T5T_WRITE_SINGLE_BLOCK(dwMwIfStatus,psTagParams,dtaLibHdl->mwIfHdl,0x02,tagReadDataBuf,T5T_REQ_FLAG_AMS_OPFLAG)
        }
        break;

        default:
          phOsal_LogError((const uint8_t*)"DTALib>T5T:Error Pattern Number not valid for T5T !! \n");
          break;
      }
#endif
    usleep(4000000);
    /*
     * If there is an error, m/w will move to discovery state
     * In such case, DEACTIVATE_EVT will get queued to MwIf queue
     * which has to be consumed otherwise next test case
     * will consume resulting test case failure
     */
    phMwIf_ConsumeDeactivatedEvent(dtaLibHdl->mwIfHdl, 100);
    phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
    LOG_FUNCTION_EXIT;
    return dwMwIfStatus;
}

#ifdef __cplusplus
}
#endif
