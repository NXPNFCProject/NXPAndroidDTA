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
 * \file  phMwIfLib_NFCAdaptWrap.cpp
 * \brief Middleware Library Interface source file.
 *
 * Project:  NFC DTA LIB
 */

#include "nfc_target.h"
#include "nfc_hal_api.h"
#include "NfcAdaptation.h"
#include "phMwIfLib_NfcAdaptWrap.h"
#include <utils/Log.h>

#ifdef __cplusplus
extern "C"
{
#endif

void* NfcAdaptation_Create()
{
    return static_cast<void*> (& NfcAdaptation::GetInstance());
}

void NfcAdaptation_Destroy( void* AdaptHdl )
{
   delete static_cast<NfcAdaptation*>(AdaptHdl);
}

void NfcAdaptation_Initialize( void* AdaptHdl)
{
   return static_cast<NfcAdaptation*>(AdaptHdl)->Initialize();
}

void NfcAdaptation_DeInitialize( void* AdaptHdl)
{
   return static_cast<NfcAdaptation*>(AdaptHdl)->Finalize();
}

tHAL_NFC_ENTRY* NfcAdaptation_GetHalEntryFuncs( void* AdaptHdl)
{
   return static_cast<NfcAdaptation*>(AdaptHdl)->GetHalEntryFuncs();
}

#ifdef __cplusplus
}
#endif
