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
