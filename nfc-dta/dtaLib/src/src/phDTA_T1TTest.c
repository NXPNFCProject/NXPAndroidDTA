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
 * \file phDTA_T1TTest.c
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

extern phDtaLib_sHandle_t g_DtaLibHdl;


/**< Constant T1T Data Arrays for predefined memory layouts*/
static const uint8_t gs_T1TStaticLayoutZeroLenTagData[] = { 0x01, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x2E, 0x63, 0x6F, 0x6D };

static const uint8_t gs_T1TStaticLayoutNonZeroLenTagData[] = { 0x01, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x2E, 0x63,
        0x6F, 0x6D };

static const uint8_t gs_T1TDynamicLayoutZeroLenTagData[] = { 0x01, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x2E, 0x63, 0x6F, 0x6D };

static const uint8_t gs_T1TDynamicLayoutNonZeroLenTagData[] = { 0x01, 0x61, 0x62,
        0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
        0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
        0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
        0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74,
        0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
        0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72,
        0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62,
        0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
        0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
        0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
        0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72, 0x73, 0x74,
        0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
        0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72,
        0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62,
        0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
        0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
        0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
        0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
        0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
        0X6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70, 0x71, 0x72,
        0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62,
        0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
        0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
        0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0X6F, 0x70,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x2E, 0x63, 0x6F,
        0x6D };
#if 0
static const uint8_t gs_gs_T1TDynamicLayout4NonZeroLenTagData[]={ 0x01, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x2E, 0x63, 0x6F, 0x6D };
#endif

/**< Dynamic Memory layout 6 for WRITE_BV_5 test case */
static const uint8_t gs_T1TDynamicLayout6NonZeroLenTagData[]={ 0x01, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62, 0x63,
        0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73,
        0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x2E, 0x63,
        0x6F, 0x6D };

/**
 * Handle T1T Tag operations(Read/Write) based on the pattern number configured
 */
DTASTATUS phDtaLibi_T1TOperations(phDtaLib_sTestProfile_t TestProfile)
{
    phMwIf_uTagOpsParams_t sTagOpsParams;
    phMwIf_sBuffParams_t*  psBuffParams;
    phMwIf_sNdefDetectParams_t* psNdefDetectParams;
    phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
    uint8_t* pBuffer;
    uint32_t dwSizeOfBuffer;
    DTASTATUS dwDtaStatus;
    uint8_t outBuff[512]={0};
    LOG_FUNCTION_ENTRY;

    sTagOpsParams.sBuffParams.pbBuff = outBuff;
    phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Perform NDEF Check");
    dwDtaStatus = phDtaLibi_CheckNDEF((&sTagOpsParams));
    if (dwDtaStatus != DTASTATUS_SUCCESS)
    {
        phOsal_LogError((const uint8_t*)"DTALib>Device is not NDEF Compliant");
        return dwDtaStatus;
    }
    psNdefDetectParams = (&sTagOpsParams.sNdefDetectParams);
    /**< Select operation based on pattern number */
    phOsal_LogDebugU32h((const uint8_t*)"DTALib>T1T:pattern number ", TestProfile.Pattern_Number);
    switch (TestProfile.Pattern_Number)
    {
        case 0x0001:
        {
            /* Update length based on the memory layout*/

            sTagOpsParams.sBuffParams.dwBuffLength = psNdefDetectParams->dwMaxSize;

            phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Perform NDEF Read \n");
            dwDtaStatus = phDtaLibi_ReadNDEF(&sTagOpsParams);
            if (dwDtaStatus != DTASTATUS_SUCCESS)
            {
                phOsal_LogError((const uint8_t*)"DTALib> Device is not NDEF Compliant");
                return dwDtaStatus;
            }
        }
        break;
        case 0x0002:
        {
            /* Perform NDEF Message generation based on the sizes*/
            if (psNdefDetectParams->dwMaxSize > 0x5A) /*Dynamic Layout*/
            {
                if (psNdefDetectParams->dwCurSize == 0)/*Zero Size*/
                {
                    phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Dynamic Zero Len \n");
                    pBuffer = (uint8_t *)gs_T1TDynamicLayoutZeroLenTagData;
                    dwSizeOfBuffer = sizeof(gs_T1TDynamicLayoutZeroLenTagData);

                }
                else if(psNdefDetectParams->dwCurSize == 0xFE)
                {
                    phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Dynamic Memory layout 6 \n");
                    pBuffer = (uint8_t *)gs_T1TDynamicLayout6NonZeroLenTagData;
                    dwSizeOfBuffer = sizeof(gs_T1TDynamicLayout6NonZeroLenTagData);
                }
                else /*Non Zero Size*/
                {
                    phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Dynamic Non Zero Len \n");
                    pBuffer = (uint8_t *) gs_T1TDynamicLayoutNonZeroLenTagData;
                    dwSizeOfBuffer = sizeof(gs_T1TDynamicLayoutNonZeroLenTagData);
                }
            }
            else /*Static Layout*/
            {
                if (psNdefDetectParams->dwCurSize == 0) /*Zero Size*/
                {
                    phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Static Zero Len \n");
                    pBuffer = (uint8_t *) gs_T1TStaticLayoutZeroLenTagData;
                    dwSizeOfBuffer = sizeof(gs_T1TStaticLayoutZeroLenTagData);
                }
                else /*Non Zero Size*/
                {
                    phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Static Non Zero Len \n");
                    pBuffer = (uint8_t *) gs_T1TStaticLayoutNonZeroLenTagData;
                    dwSizeOfBuffer = sizeof(gs_T1TStaticLayoutNonZeroLenTagData);
                }
            }
            /* Write the NDEF Message */
            phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Perform NDEF Write \n");
            psBuffParams = (&sTagOpsParams.sBuffParams);
            psBuffParams->pbBuff = pBuffer;
            psBuffParams->dwBuffLength = dwSizeOfBuffer;
            dwDtaStatus = phDtaLibi_WriteNDEF(&sTagOpsParams);
            if (dwDtaStatus != DTASTATUS_SUCCESS)
            {
                phOsal_LogError((const uint8_t*)"DTALib> T1T:Error Could not form the NDEF message !!\n");
                break;
            }
        }
        break;
        case 0x0003:
        phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Perform Read-Only conversion \n");
        dwDtaStatus = phDtaLibi_SetTagReadOnly(&sTagOpsParams);
        if (dwDtaStatus != DTASTATUS_SUCCESS)
        {
            phOsal_LogError((const uint8_t*)"DTALib> T1T:Error Could not set tag readonly !!\n");
            break;
        }
        break;
        default:
        phOsal_LogError((const uint8_t*)"DTALib>T1T: Error  Pattern Number not valid for T1T !! \n");
        break;
    }
    phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl, PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
    LOG_FUNCTION_EXIT;
    return dwDtaStatus;
}

/**
*   T1T Tag read/write operations based on pattern number set by the user.
*   Dynamic Execution refers to dynamically retrieving the data by tag read. Data retrieved is saved
*   and used for tag write operations later as explained in the table of T1T tag test cases
*   specifications.
*/
DTASTATUS  phDtaLibi_T1TOperations_DynamicExecution(phDtaLib_sTestProfile_t TestProfile)
{
  MWIFSTATUS dwMwIfStatus = MWIFSTATUS_FAILED;
  DTASTATUS  dwDtaStatus = DTASTATUS_FAILED;
  phDtaLib_sHandle_t *dtaLibHdl = &g_DtaLibHdl;
  phMwIf_uTagParams_t   sTagParams;
  phMwIf_sT2TParams_t*   psTagParams = (phMwIf_sT2TParams_t *)&sTagParams;
  phMwIf_uTagOpsParams_t sTagOpsParams;
  phMwIf_sNdefDetectParams_t* psNdefDetectParams;
  uint8_t t1tBuff[512]  = {0};

  LOG_FUNCTION_ENTRY;
  memset(psTagParams, 0, sizeof(phMwIf_uTagParams_t));
  sTagParams.sT1TPrms.sBuffParams.dwBuffLength = sizeof(t1tBuff);

  phOsal_LogDebugString((const uint8_t*)"DTALib> :", (const uint8_t*)__FUNCTION__);
  phOsal_LogDebugU32h((const uint8_t*)"PatternNum = ", TestProfile.Pattern_Number);
  sTagOpsParams.sBuffParams.pbBuff = gs_ndefReadWriteBuff;
  switch(TestProfile.Pattern_Number)
  {

    /* Pattern Numbers to test READ functionality */
    case 0x0001:
    {
      phOsal_LogDebug ((const uint8_t*)"DTALib>T1T:Perform NDEF Check \n");
      dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
      if (dwDtaStatus != DTASTATUS_SUCCESS)
      {
        phOsal_LogError((const uint8_t*)"DTALib>T1T:Device is not NDEF Compliant\n");
        break;
      }
      psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
      if(!psNdefDetectParams->dwStatus)
      {
        phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Tag is NDEF compliant \n");
        phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Read NDEF \n");
        dwDtaStatus = phDtaLibi_ReadNDEF(&sTagOpsParams);
        if (dwDtaStatus != DTASTATUS_SUCCESS)
        {
          phOsal_LogError((const uint8_t*)"DTALib> T1T:Error Could not read data !!\n");
          break;
        }
        memset(gs_ndefReadWriteBuff, 0, sizeof(gs_ndefReadWriteBuff));
        memcpy(gs_ndefReadWriteBuff,sTagOpsParams.sBuffParams.pbBuff, sTagOpsParams.sBuffParams.dwBuffLength);
        gs_sizeNdefRWBuff = sTagOpsParams.sBuffParams.dwBuffLength;
        phOsal_LogDebugU32d((const uint8_t*)"DTALib>:T1T:NDEF READ Length: ", gs_sizeNdefRWBuff);
      }
    }
    break;

    /* Pattern Numbers to test WRITE functionality */
    case 0x0002:
    {
      phOsal_LogDebug ((const uint8_t*)"DTALib>T1T:Perform NDEF Check \n");
      dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
      if (dwDtaStatus != DTASTATUS_SUCCESS)
      {
        phOsal_LogError((const uint8_t*)"DTALib>T1T:Device is not NDEF Compliant\n");
        break;
      }

      psNdefDetectParams = &sTagOpsParams.sNdefDetectParams;
      if(!psNdefDetectParams->dwStatus)
      {
        phOsal_LogDebug((const uint8_t*)"DTALib> T1T:Tag is NDEF compliant \n");
        phOsal_LogDebug((const uint8_t*)"DTALib>T1T:Write NDEF Message \n");
        sTagOpsParams.sBuffParams.dwBuffLength = gs_sizeNdefRWBuff;
        phOsal_LogDebugU32d((const uint8_t*)"DTALib>:T1T:NDEF WRITE Length: ", sTagOpsParams.sBuffParams.dwBuffLength);
        dwDtaStatus = phDtaLibi_WriteNDEF(&sTagOpsParams);
        if(dwDtaStatus != DTASTATUS_SUCCESS)
        {
          phOsal_LogError((const uint8_t*)"DTALib>T1T:Device is not NDEF Complaint\n");
          break;
        }
      }
    }
    break;

    /* Pattern Number to test Read/Write to Read Only Test functionality */
    case 0x0003:
    {
      phOsal_LogDebug ((const uint8_t*)"DTALib>T1T:Perform NDEF Check \n");
      dwDtaStatus = phDtaLibi_CheckNDEF(&sTagOpsParams);
      if (dwDtaStatus != DTASTATUS_SUCCESS)
      {
        phOsal_LogError((const uint8_t*)"DTALib>T1T:Device is not NDEF Compliant\n");
        break;
      }
      dwDtaStatus = phDtaLibi_SetTagReadOnly(&sTagOpsParams);
      if (dwDtaStatus != DTASTATUS_SUCCESS)
      {
        phOsal_LogError((const uint8_t*)"DTALib> T1T:Error Could not set tag readonly !!\n");
        break;
      }
    }
    break;

    default:
      phOsal_LogError((const uint8_t*)"DTALib>T1T:Error Pattern Number not valid for T1T !! \n");
    break;
  }
#ifdef WIN32
  Sleep(4000);
#endif
  phMwIf_NfcDeactivate(dtaLibHdl->mwIfHdl,PHMWIF_DEACTIVATE_TYPE_DISCOVERY);
  LOG_FUNCTION_EXIT;
  return dwMwIfStatus;
}

#ifdef __cplusplus
}
#endif
