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
 * \file  phMwIfLib_NFCAdaptWrap.h
 * \brief Middleware Library Interface header file. Used as static interface from C
 *        to C++ NFCAdaptation class
 *
 * Project:  NFC DTA LIB
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "nfc_target.h"
#include "nfc_hal_api.h"

/**
 * \ingroup grp_mwif_lib
 * \brief Creates NFC Adaptation Object
 *
 * This function creates NFCAdaptation Object
  *
 * \param[in]                           None
 * \retval AdaptHdl                     Handle for Adaptation object created earlier
 * \retval NULL                         MWIF LIB failed to create NFC Adaptation Object
 *
 */
void* NfcAdaptation_Create();

/**
 * \ingroup grp_mwif_lib
 * \brief deletes NFC Adaptation Object
 *
 * This function deletes NFCAdaptation Object
 *
 * \param[in] AdaptHdl                  Handle for Adaptation object created earlier
 * \retval #MWIFSTATUS_SUCCESS          NFCAdaptation Object destroyed successfully
 * \retval #MWIFSTATUS_FAILED           MWIF LIB failed to delete NFC Adaptation Object
 *
 */
void  NfcAdaptation_Destroy( void* AdaptHdl );

/**
 * \ingroup grp_mwif_lib
 * \brief Initializes NFC Adaptation Object
 *
 * This function initializes NFCAdaptation Object
 *
 * \param[in] AdaptHdl                  Handle for Adaptation object created earlier
 * \retval                              None
  */
void  NfcAdaptation_Initialize( void* AdaptHdl);

/**
 * \ingroup grp_mwif_lib
 * \brief DeInitializes NFC Adaptation Object
 *
 * This function Deinitializes NFCAdaptation Object
 *
 * \param[in] AdaptHdl                  Handle for Adaptation object created earlier
 * \retval                              None
  */

void NfcAdaptation_DeInitialize( void* AdaptHdl);

/**
 * \ingroup grp_mwif_lib
 * \brief Provides the registered HAL function pointers
 *
 * List of HAL function pointers are returned by this function
 *
 * \param[in] AdaptHdl                  Handle for Adaptation object created earlier
 * \retval                              None
 */
tHAL_NFC_ENTRY* NfcAdaptation_GetHalEntryFuncs( void* AdaptHdl);

#ifdef __cplusplus
}
#endif
