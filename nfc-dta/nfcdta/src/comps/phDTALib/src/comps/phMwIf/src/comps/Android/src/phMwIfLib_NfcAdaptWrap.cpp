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
 * \file  phMwIfLib_NFCAdaptWrap.cpp
 * \brief Middleware Library Interface source file.
 *
 * Project:  NFC DTA LIB
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "nfc_target.h"
#include "nfc_hal_api.h"
#include "NfcAdaptation.h"
#include "phMwIfLib_NfcAdaptWrap.h"
#include <utils/Log.h>

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
