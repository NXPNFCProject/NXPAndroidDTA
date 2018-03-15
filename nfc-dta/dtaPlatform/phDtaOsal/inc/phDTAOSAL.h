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

/**
 * \file  phDTAOSAL.h
 *  \brief OSAL header files related to operating system functionalities.
 */

#ifndef PHOSAL_H
#define PHOSAL_H

/**
 * Generic macros */
#define OSALSTATUS_OK                   0
#define OSALSTATUS_SUCCESS              0
#define OSALSTATUS_INVALID_PARAMS       1
#define OSALSTATUS_FAILED               2
#define OSALSTATUS_SEM_TIMEOUT          3
#define OSALSTATUS_Q_OVERFLOW           4
#define OSALSTATUS_Q_UNDERFLOW          5
#define OSALSTATUS_NOT_SUPPORTED        6
#define OSALSTATUS_NOT_INITIALISED      7
#define OSALSTATUS_INSUFFICIENT_RESOURCES   8
#define OSALSTATUS_ALREADY_INITIALISED  9

/**
 * Windows Specific macros */
#define CID_DTA_NFC_OSAL                 0x0D    /**< DTA OSAL */
#define PH_DTA_LOG_COMP_DTA_LIB          1 /**< Log Group ID For MW If   */
/* Internally required by \ref PHDTA_NFCSTVAL. */
#define PHDTA_NFCSTSHL8                          (8U)
/* Required by \ref PHNFCSTVAL. */
#define PHDTA_NFCSTBLOWER                        ((NFCSTATUS)(0x00FFU))

#define PHDTA_NFCSTVAL(phNfcCompID, phNfcStatus)                                  \
            ( ((phNfcStatus) == (OSALSTATUS_SUCCESS)) ? (OSALSTATUS_SUCCESS) :  \
                ( (((NFCSTATUS)(phNfcStatus)) & (PHDTA_NFCSTBLOWER)) |            \
                    (((uint16_t)(phNfcCompID)) << (PHDTA_NFCSTSHL8)) ) )

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSC_VER
#   include <stdint.h>
#   include <stdio.h>

#ifdef WIN32
#include <phNfcConfig.h>
#endif

#else
#include <stdio.h>
#define snprintf _snprintf

#ifndef linux
/**
 *  \name Basic Type Definitions
 *
 *  Constant-length-type definition ('C99).
 *
 */
/*@{*/

#ifndef __int8_t_defined /* */
#define __int8_t_defined /* */
typedef signed   char   int8_t;         /**<  \ingroup grp_nfc_common
                                               8 bit signed integer */
#endif

#ifndef __int16_t_defined
#define __int16_t_defined
typedef signed   short  int16_t;        /**< \ingroup grp_nfc_common
                                             16 bit signed integer */
#endif

#ifndef __stdint_h
#ifndef __int32_t_defined
#define __int32_t_defined
typedef signed   long   int32_t;        /**< \ingroup grp_nfc_common
                                             32 bit signed integer */
#endif
#endif

#ifndef __uint8_t_defined
#define __uint8_t_defined
typedef unsigned char   uint8_t;        /**<  \ingroup grp_nfc_common
                                              8 bit unsigned integer */
#endif

#ifndef __uint16_t_defined
#define __uint16_t_defined
typedef unsigned short  uint16_t;       /**< \ingroup grp_nfc_common
                                             16 bit unsigned integer */
#endif

#ifndef __stdint_h
#ifndef __uint32_t_defined
#define __uint32_t_defined
typedef unsigned long   uint32_t;       /**< \ingroup grp_nfc_common
                                             32 bit unsigned integer */
#endif
#endif

//typedef bool BOOLEAN;

#endif /* linux */

#endif /* _WIN32 */

#ifndef TRUE
#define TRUE            (0x01)            /**< \ingroup grp_nfc_common
                                              Logical True Value */
#endif

#ifndef FALSE
#define FALSE           (0x00)            /**< \ingroup grp_nfc_common
                                              Logical False Value */
#endif

typedef uint8_t             utf8_t;             /**< \ingroup grp_nfc_common
                                             UTF8 Character String */

typedef uint8_t             bool_t;             /**< \ingroup grp_nfc_common
                                                boolean data type */

typedef uint16_t        NFCSTATUS;      /**< \ingroup grp_nfc_common
                                             NFC-FRI and HAL return values
                                         \ref phNfcStatus.h for different status
                                          values */

typedef unsigned int BitField_t;        /**<  \ingroup grp_nfc_common
                                               16 bit unsigned integer */

#ifdef WIN32
typedef unsigned long long uint64_t;    /**<  \ingroup grp_nfc_common
                                               64 bit unsigned integer */
#ifndef TimerID_t
typedef uint64_t TimerID_t;            /**<  \ingroup grp_nfc_common
                                               64 bit unsigned integer */
#endif
#endif

#ifndef NULL
#define NULL  ((void *)0)
#endif

#define STATIC static

/* This Macro to be used to resolve Unused and unreference
 * compiler warnings.
 */

#define PHNFC_UNUSED_VARIABLE(x) for((x)=(x);(x)!=(x);)


/** No Logs.
 *
 * Use this level to disable Logging. */
#define PHFL_DTA_LOG_LEVEL_OFF 0

/** Catastrophic/Critical Failure
 *
 * Mostly, this level would be enabled in production code.
 * Use this to alert for failure cases that <b>can not</b> be handled gracefully. */
#define PHFL_DTA_LOG_LEVEL_CRIT 1

/** Unexpected failures that can be safely handled
 *
 * Mostly, this level would be enabled during testing. */
#define PHFL_DTA_LOG_LEVEL_WARN 2

/** General Information
 *
 * Extra debugging information to help understand the program flow. */
#define PHFL_DTA_LOG_LEVEL_INFO 3

/** Data Buffer
 *
 * Data buffer to be dumped
 * */
#define PHFL_DTA_LOG_LEVEL_BUFFER 4

/** Function Logging of Entry and Exit
 *
 * Very verbose logging to understand the flow of called functions. */
#define PHFL_DTA_LOG_LEVEL_FUNC 5

/** Debug Information
 *
 * Debugging informations to understand the program flow
 * */
#define PHFL_DTA_LOG_LEVEL_DEBUG 6





typedef int32_t OSALSTATUS;        /* Return values */

/** \ingroup grp_osal_log
    Types of logging needed.
    If LOGLEVEL is set for 3(Data Buffers) all types of logs are enabled*/
typedef enum phOsal_eLogLevel
{
    PHOSAL_LOGLEVEL_NONE        = 0,
    PHOSAL_LOGLEVEL_ERROR       = 1,
    PHOSAL_LOGLEVEL_INFO        = 2,
    PHOSAL_LOGLEVEL_DEBUG       = 3,
    PHOSAL_LOGLEVEL_DATA_BUFFERS= 4
}phOsal_eLogLevel_t;

/**
 * \internal
 *
 * \def PH_OSAL_TRACE_MALLOC
 *
 * Trace what is being allocated and de-allocated.
 *
 * Set to 1 to trace/log malloc and free. 0 to disable tracing */

#    define PH_OSAL_TRACE_MALLOC                                 0

/** @} */

/** \addtogroup grp_osal_stack
 * @{ */
/**
 * Maximum time a system object can wait to be occupied.
 */
#define PH_OSAL_MAX_WAITTIME                                 (INFINITE)

/**
 * Message Base number at which thread starts storing message queue details */
#define PH_OSAL_MESSAGE_BASE                                 (WM_USER+0x3FF)

/** \internal
 * Debug mode is enabled to analyze and trace the call stack */
#define PH_OSAL_ENABLE_DEBUG 0

#define PH_OSAL_FUNC_ENTRY  phOsal_LogFunctionEntry((const uint8_t*)"Osal",(const uint8_t*)__FUNCTION__)
#define PH_OSAL_FUNC_EXIT   phOsal_LogFunctionExit((const uint8_t*)"Osal",(const uint8_t*)__FUNCTION__)

/** MACRO to declare temporarily unused variables */
#if defined(__QMORE__) && __QMORE__ == 1
#   define UNUSED(X) /* Not used temporarily */
#else
#   define UNUSED(X) (void)X;
#endif

/** @} */
/*
***************************Globals,Structure and Enumeration ******************
*/

/** \addtogroup grp_osal_thread
 * @{ */

/**
 * Thread Function Pointer Declaration.
 *
 * This points to the function to be invoked upon creation of thread.
 * It is not the immediate thread procedure since the API of this function
 * depends on the OS.
 *
 * This function shall be called within the body of the thread procedure
 * defined by the underlying, OS-depending OSAL implementation.
 *
 */
/*typedef void (*pphOsal_ThreadFunction_t)(void*);*/

/** @} */

/** \addtogroup grp_osal_stack
 * @{ */

/**
 * Deferred call declaration.
 * This type of API is called from ClientApplication (main thread) to notify
 * specific callback.
 */
typedef  void (*pphOsal_DeferFuncPointer_t) (void*);


/**
 * Deferred message specific info declaration.
 * This type information packed as WPARAM  when windows message
 * is posted to message handler thread.
 */
typedef struct phOsal_DeferedCallInfo
{
        pphOsal_DeferFuncPointer_t   pDeferedCall;/**< pointer to Deferred callback */
        void                            *pParam;    /**< contains timer message specific details*/
}phOsal_DeferedCallInfo_t;

/** @} */

/** \ingroup grp_osal_exp
 *
 * Supported exception types
 *
 * Enum definition contains supported exception types
 */
typedef enum
{
    phOsal_e_NoMemory,                       /**< Memory allocation failed */
    phOsal_e_PrecondFailed,                  /**< precondition wasn't met */
    phOsal_e_InternalErr,                    /**< Unrecoverable error */
    phOsal_e_UnrecovFirmwareErr,             /**< Unrecoverable firmware error */
    phOsal_e_TMLerror                        /**< Unrecoverable TML error */
} phOsal_ExceptionType_t;    /**< Variable indicating type of Exception */

/** \addtogroup grp_osal_stack
 * @{ */

/**
 *  information to configure OSAL
 */
typedef struct phOsal_Config
{
    uint8_t *pLogFile; /**< Log File Name*/
    uint32_t dwCallbackThreadId; /**< Client ID to which message is posted */
    void *pContext;
}phOsal_Config_t, *pphOsal_Config_t /**< Pointer to #phOsal_Config_t */;

typedef enum phOsal_eSlzType
{
    phOsal_eSlzTypeStatic = 0x00,
    phOsal_eSlzTypeDynamic = 0x01
}phOsal_eSlzType_t;

typedef enum phOsal_eSlzVersion
{
    phOsal_eSlzVersionMantis = 0x00,
    phOsal_eSlzVersionVienna_1_0 = 0x00,
    phOsal_eSlzVersionVienna_2_0 = 0x00,
    phOsal_eSlzVersionVienna_2_1 = 0x11
}phOsal_eSlzVersion_t;

/**
 * OSAL initialization.
 * This function initializes Timer queue and Critical section variable.
 * \param[in] pOsalConfig               Osal Configuration file.
 * \retval #OSALSTATUS_SUCCESS            Initialization of OSAL module was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS  Client Thread ID passed is invalid.
 * \retval #OSALSTATUS_ALREADY_INITIALISED   Osal Module is already Initialized.
 * \retval #OSALSTATUS_FAILED             Initialization of OSAL module was not successful.
 *
 */
extern OSALSTATUS phOsal_Init(pphOsal_Config_t pOsalConfig);

/**
 * OSAL initialization.
 * This function De-Initializes the Objects and Memory occupied during
 * Initialization. This function also closes the objects if any of it is still open.
 *
 */
extern void phOsal_DeInit(void );

/** @} */

/** \addtogroup grp_osal_rng
 * @{ */

/**
 * Initializes Random Number Generator seed.
 * \note This function executes successfully without OSAL module Initialization.
 */
void phOsal_RandInit( void );

/**
 * Gets Random number. \ref phOsal_RandInit function shall be invoked prior,
 * in order to get non-repeating random numbers.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \retval Random number generated in the range 0 to 32767
 *
 */
uint16_t phOsal_RandGetNumber( void );

/** @} */

/** \addtogroup grp_osal_mm
 * @{ */

/**
 * Allocates some memory.
 *
 * \note This function executes successfully without OSAL module initialization.
 *
 * \param[in] dwSize   Size, in uint32_t, to be allocated
 *
 * \retval NON-NULL value:  The memory is successfully allocated ;
 * the return value is a pointer to the allocated memory location
 * \retval NULL:            The operation is not successful.
 *
 */
void * phOsal_GetMemory(uint32_t dwSize);
/**
 * This API allows to free already allocated memory.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] pMem  Pointer to the memory block to be deallocated
 */
#if defined (PH_OSAL_TRACE_MALLOC) && PH_OSAL_TRACE_MALLOC == 0
void   phOsal_FreeMemory(void * pMem);
#else
#   define phOsal_FreeMemory(pMem) phOsal_FreeMemorySTR(pMem,#pMem)
    /* Actual function for #phOsal_GetMemory */
    /* void phOsal_FreeMemoryREAL(void * pMem); */
    /* Run \ref phOsal_FreeMemory and also show what is being freed */
    void phOsal_FreeMemorySTR(void * pMem,const char * What);
#endif

/**
 * This API allows to delay the current thread execution.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] dwDelay  Duration in milliseconds for which thread execution to be halted.
 */
void phOsal_Delay(uint32_t dwDelay);

/**
 * Compares the values stored in the source memory with the
 * values stored in the destination memory.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] pDest     Pointer to the Destination string.
 * \param[in] pSrc      Pointer to the Source string.
 * \param[in] dwSize    Number of bytes to be compared.
 *
 * \retval Zero value:        The comparison is successful,
                    Both the memory areas contains identical values.
 * \retval Non-Zero Value:    The comparison failed, both the memory
 *                  areas are non-identical.
 *
 */
int32_t phOsal_MemCompare(const void *pDest, const void *pSrc, uint32_t dwSize);

/**
 * Sets the given value in the memory locations.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] pMem      Pointer to the memory block to be set to a value
 * \param[in] bVal      Value to be set
 * \param[in] dwSize    Number of bytes to be set.
 *
 */
#if defined (PH_OSAL_TRACE_MALLOC) && PH_OSAL_TRACE_MALLOC == 0
    void phOsal_SetMemory(void *pMem, uint8_t bVal, uint32_t dwSize);
#else
#   define phOsal_SetMemory(pMem, bVal, dwSize) \
    phOsal_SetMemorySTR(pMem, #pMem, bVal, dwSize,#dwSize)

    void phOsal_SetMemoryREAL(void *pMem, uint8_t bVal, uint32_t dwSize);
    void phOsal_SetMemorySTR(void *pMem, const char *WhatMem,uint8_t bVal, uint32_t dwSize,const char *WhatSize);
#endif
/**
 * Copies the values stored in the source memory to the
 * values stored in the destination memory.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] pDest     Pointer to the Destination Memory
 * \param[in] pSrc      Pointer to the Source Memory
 * \param[in] dwSize    Number of bytes to be copied.
 *
 */
void phOsal_MemCopy( void *pDest, const void *pSrc, uint32_t dwSize );

/** @} */


/** \addtogroup grp_osal_exp
 * @{ */

/**
 * Raises exception on a severe error
 *
 * The program jumps out of the current execution flow, i.e. this function doesn't return.
 * The given exception contains information on what has happened and how severe the error is.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] eExceptiontype  exception Type.
 * \param[in] wReason     This is an additional reason value that gives a vendor specific reason code
 */

void phOsal_RaiseException(phOsal_ExceptionType_t     eExceptiontype,
                              uint16_t                      wReason);


/** \addtogroup grp_osal_thread
 * @{ */

/**
 * Thread Function Pointer Declaration.
 *
 * This points to the function to be invoked upon creation of thread.
 * It is not the immediate thread procedure since the API of this function
 * depends on the OS.
 *
 * This function shall be called within the body of the thread procedure
 * defined by the underlying, OS-depending OSAL implementation.
 *
 */
typedef void (*pphOsal_ThreadFunction_t)(void*);


/** \ingroup grp_osal_thread
 *
 * @{ */

/**
 * Thread Creation.
 *
 * This function creates a thread in the underlying system. To delete the
 * created thread use the phOsal_Thread_Delete function.
 *
 *
 * \param[in,out] hThread    The Thread handle: The caller has to prepare a void pointer
 *                           that need not to be initialized.
 *                           The value (content) of the pointer is set by the function.
 *
 * \param[in] pThreadFunction Pointer to a function within the
 *                           implementation that shall be called by the Thread
 *                           procedure. This represents the Thread main function.
 *                           When this function exits the thread exits.
 * \param[in] pParam          A pointer to a user-defined location the thread
 *                           function receives.
 *
 * \retval #OSALSTATUS_SUCCESS                    The operation was successful.
 * \retval #OSALSTATUS_INSUFFICIENT_RESOURCES     At least one parameter value is invalid.
 * \retval #PH_OSAL_THREAD_CREATION_ERROR     A new Thread could not be created due to system error.
 * \retval #OSALSTATUS_NOT_INITIALISED            Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_ThreadCreate(void                          **hThread,
                                 pphOsal_ThreadFunction_t    pThreadFunction,
                                 void                           *pParam);

/**
 * Get the task identifier of the caller thread.
 *
 * \retval 0 OSAL is not Initialized.
 * \retval NonZero Thread ID of the caller thread
 */
uint32_t phOsal_ThreadGetTaskId(void);

/**
 * Terminates the thread.
 *
 * This function Terminates the thread passed as a handle.
 *
 * \param[in] hThread The handle of the system object.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS      At least one parameter value is invalid.
 * \retval #PH_OSAL_THREAD_DELETE_ERROR   Thread could not be deleted due to system error.
 * \retval #OSALSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_ThreadDelete(void *hThread);
#ifdef ENABLE_ADVANCED_FUNCS
/**
 * Suspend the specified thread.
 *
 * \param[in] hThread The handle of the system object.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS      At least one parameter value is invalid.
 * \retval #PH_OSAL_THREAD_SUSPEND_ERROR  If the thread is not able to be suspended.
 * \retval #OSALSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 * Note : there is no direct function for suspend thread in linux.
 */
OSALSTATUS phOsal_ThreadSuspend(void *hThread);

/**
 * Resumes the specified thread.
 *
 * \param[in] hThread The handle of the system object.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS      At least one parameter value is invalid.
 * \retval #PH_OSAL_THREAD_RESUME_ERROR   If the thread is not able to be resume.
 * \retval #OSALSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 * Note : there is no direct function for wakeup thread in linux.
 */
OSALSTATUS phOsal_ThreadWakeUp(void *hThread);
#endif
/**
 * Set priority for the specified thread.
 *
 * \param[in] hThread       The handle of the system object.
 * \param[in] sdwPriority   Priority of the thread.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS      At least one parameter value is invalid.
 * \retval #PH_OSAL_THREAD_SETPRIORITY_ERROR If the priority is not able to be set.
 * \retval #OSALSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 */
OSALSTATUS phOsal_ThreadSetPriority(void *hThread,int32_t sdwPriority);

/**
 * OSAL Message structure
 *
 * Contains message specific details such as message type ,message specific data block details
 */
typedef struct phOsal_Message
{
    uint32_t eMsgType;  /**< Type of the message to be posted */
    void   * pMsgData;  /**< Pointer to message specific data block (in case any) */
    uint16_t Size;      /**< Size of the datablock */
} phOsal_Message_t,*pphOsal_Message_t; /**< pointer to \ref phOsal_Message */


/**
 * Create a queue by allocating specified memory.
 *
 * \param[in,out] msgQHdl    The handle: The caller has to prepare a void pointer
 *                                       that needs not to be initialized.
 *                                       The value (content) of the pointer is set by the function.
 * \param[in] dwQueueLength              Length of the message Queue required.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_INVALID_PARAMS      Any caller provided parameter is invalid.
 * \retval #OSALSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_MsgQCreate(void **msgQHdl, uint32_t dwQueueLength);

/**
 * This API allows to delete a Message Queue.
 * \param[in] msgQHdl    Handle of the message queue which needs to be deleted.
 *
 * \retval #OSALSTATUS_SUCCESS            The operation was successful.
 * \retval #OSALSTATUS_NOT_INITIALISED   Osal Module is not Initialized.
 * \retval #OSALSTATUS_INVALID_PARAMS  At least one parameter value is invalid.
 */
OSALSTATUS phOsal_MsgQDelete( void *msgQHdl );

/**
 * Function used to post a message to a thread's message queue.
 *
 * \param[in] dwSourceID             The Thread identifier of the sending thread.
 * \param[in] msgQHdl    The handle of the MessageQueue.
 * \param[in] pMsg                   Pointer to the message structure to be posted to the queue.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_NOT_INITIALISED       Osal Module is not Initialized.
 * \retval #OSALSTATUS_INVALID_PARAMS      Any caller provided parameter is invalid.
 * \retval #OSALSTATUS_INSUFFICIENT_RESOURCES The operation could not complete because of lack of resources.
 */
OSALSTATUS phOsal_MsgQPostMsg( void *msgQHdl,uint32_t dwSourceID,phOsal_Message_t *pMsg );

/**
 * Function used to receive a message from calling thread's message queue.
 *
 * \param[out] pSourceID            Pointer where Thread ID of poster thread is stored.
 * \param[in]  msgQHdl  The handle of the MessageQueue.
 * \param[out] pMsg                 The message to be filled with the received data from the message queue.
 *
 * \retval #OSALSTATUS_SUCCESS                The operation was successful.
 * \retval #OSALSTATUS_NOT_INITIALISED       Osal Module is not Initialized.
 * \retval #OSALSTATUS_INVALID_PARAMS      Any caller provided parameter is invalid.
 * \retval #OSALSTATUS_INSUFFICIENT_RESOURCES The operation could not complete because of lack of resources.
 */
OSALSTATUS phOsal_MsgQReceiveMsg( void *msgQHdl, uint32_t *pSourceID,phOsal_Message_t * pMsg );

/**
 * Semaphore Creation.
 * This function creates a semaphore in the underlying system.
 *
 * \param[in,out] hSemaphore The handle: The caller has to prepare a void
 *                           pointer where the handle of semaphore shall be returned.
 *
 * \param[in] bInitialValue   The initial value of the Semaphore.
 * \param[in] bMaxValue       The maximum value of the Semaphore.
 *
 * \retval #NFCSTATUS_SUCCESS                    The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER          Parameter passed to the function is not Correct.
 * \retval #NFCSTATUS_INSUFFICIENT_RESOURCES     All semaphores are occupied by other threads.
 * \retval #PH_OSAL_SEMAPHORE_CREATION_ERROR  A new semaphore could not be created due to system error.
 * \retval #NFCSTATUS_NOT_INITIALISED           Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_SemaphoreCreate(void       **hSemaphore,
                                 uint8_t     bInitialValue,
                                 uint8_t     bMaxValue);

/**
 * Semaphore-Produce (never waiting).
 *
 * Increment the value of the semaphore.
 * The purpose is to enable a waiting thread ("consumer") to continue
 * if it has been waiting because of the Semaphore value set to zero.
 *
 * \param[in] hSemaphore The handle of the Semaphore.
 *
 * \retval #NFCSTATUS_SUCCESS                    The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER          Parameter passed to the function is not Correct.
 * \retval #PH_OSAL_SEMAPHORE_PRODUCE_ERROR   The semaphore cannot be produced due to a system
 *                                              error or invalid handle .
 * \retval #NFCSTATUS_NOT_INITIALISED           Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_SemaphorePost(void *hSemaphore);

/**
 * Semaphore Consumption (waiting if value is zero).
 *
 * Decrement the value of the semaphore. When the internal value is
 * non-zero, the function continues. If the value is zero however, the
 * function blocks till the semaphore is released.
 *
 * \param[in] hSemaphore The handle of the Semaphore.
 *
 * \retval #NFCSTATUS_SUCCESS                    The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER          Parameter passed to the function is not Correct.
 * \retval #PH_OSAL_SEMAPHORE_CONSUME_ERROR   The semaphore can not be consumed
 *                                              due to a system error or invalid handle .
 * \retval #NFCSTATUS_NOT_INITIALISED           Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_SemaphoreWait(void *hSemaphore,
                                uint32_t timeout_ms);
/**
 * Semaphore Deletion.
 * This function deletes the Semaphore in the underlying OS.
 *
 * \param[in] hSemaphore                    The handle of the Semaphore.
 *
 * \retval #NFCSTATUS_SUCCESS                    The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER          Parameter passed to the function is not Correct.
 * \retval #PH_OSAL_SEMAPHORE_DELETE_ERROR    The semaphore can not be deleted
 *                                              due to a system error or invalid handle .
 * \retval #NFCSTATUS_NOT_INITIALISED           Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_SemaphoreDelete(void *hSemaphore);

/**
 * Mutex Creation.
 * This function creates a Mutex in the underlying system.
 *
 * \param[in,out] hMutex     The handle: The caller has to prepare a void
 *                           pointer where the handle of mutex shall be returned.
 *
 * \retval #NFCSTATUS_SUCCESS The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER          Parameter passed to the function is not Correct.
 * \retval #NFCSTATUS_INSUFFICIENT_RESOURCES     All Mutexes are occupied by other threads.
 * \retval #PH_OSAL_MUTEX_CREATION_ERROR      A new mutex could not be created due to system error.
 * \retval #NFCSTATUS_NOT_INITIALISED           Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_MutexCreate(void **hMutex);

/**
 * Mutex Locking.
 * This function locks a mutex used for handling critical section of code.
 * However the function blocks till the Mutex is available to be occupied.
 *
 * \param[in] hMutex                    The handle of the Mutex.
 *
 * \retval #NFCSTATUS_SUCCESS            The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER  Parameter passed to the function is not Correct.
 * \retval #PH_OSAL_MUTEX_LOCK_ERROR  The mutex cannot be locked due to a system error or invalid handle.
 * \retval #NFCSTATUS_NOT_INITIALISED   Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_MutexLock(void *hMutex);

/**
 * Mutex Unlocking.
 * This function unlocks a mutex after updating critical section of code.
 *
 * \param[in] hMutex        The handle of the Mutex.
 *
 * \retval #NFCSTATUS_SUCCESS                The operation was successful.
 * \retval #NFCSTATUS_INVALID_PARAMETER      Parameter passed to the function is not Correct.
 * \retval #PH_OSAL_MUTEX_UNLOCK_ERROR    The mutex cannot be locked due to a system error or invalid handle.
 * \retval #NFCSTATUS_NOT_INITIALISED       Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_MutexUnlock(void *hMutex);

/**
 * Mutex Deletion.
 * This function deletes a Mutex in the underlying system.
 *
 * \param[in] hMutex        The handle of the Mutex.
 *
 * \retval #NFCSTATUS_SUCCESS                The operation was successful.
 * \retval #PH_OSAL_MUTEX_DELETE_ERROR    The mutex cannot be deleted due to a system error or invalid handle.
 * \retval #NFCSTATUS_NOT_INITIALISED       Osal Module is not Initialized.
 *
 */
OSALSTATUS phOsal_MutexDelete(void *hMutex);

/**
 * Logging levels
 * This function set the type of logs to be enabled
 *
 * \param[in] eLogLevel  Type of logs to be enabled(info,errors,debug,data)
 *
 */
void phOsal_SetLogLevel(phOsal_eLogLevel_t eLogLevel);

/**
 * Log Error
 * This function prints the error message specified
 * appropriate log level for error needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines Error log message to be printed
 *
 */
void phOsal_LogError(const uint8_t* pbMsg);

/**
 * Log Error
 * This function prints the error message specified along with 32bit value in Hex
 * appropriate log level for error needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines Error log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogErrorU32h(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Error
 * This function prints the error message specified along with 32bit value in decimal
 * appropriate log level for error needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines Error log message to be printed
 * \param[in] wValue defines value to be printed
 */
void phOsal_LogErrorU32d(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Error String
 * This function prints the error message specified along with string
 * appropriate log level for error needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines Error log message to be printed
 * \param[in] pbString defines pointer to string to be printed
 */
void phOsal_LogErrorString(const uint8_t* pbMsg, const uint8_t* pbString);

/**
 * Log Information
 * This function prints the information /warning specified
 * appropriate log level for info needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg  defines log message to be printed
 *
 */
void phOsal_LogInfo(const uint8_t* pbMsg);

/**
 * Log Information
 * This function prints the information /warning specified along with 32bit value in Hex
 * appropriate log level for info needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg  defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogInfoU32h(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Information
 * This function prints the information /warning specified along with 32bit value in decimal
 * appropriate log level for info needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg  defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogInfoU32d(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Information and String
 * This function prints the information specified along with string
 * appropriate log level for info needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] pbString defines pointer to string to be printed
 */
void phOsal_LogInfoString(const uint8_t* pbMsg, const uint8_t* pbString);

/**
 * Log Debug Information
 * This function prints the Debug log message specified
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 *
 */
void phOsal_LogDebug(const uint8_t* pbMsg);

/**
 * Log Debug Information
 * This function prints the Debug log message specified along with 32bit value in hex
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogDebugU32h(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Debug Information
 * This function prints the Debug log message specified along with 32bit value in decimal
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogDebugU32d(const uint8_t* pbMsg, uint32_t wValue);

/**
 * Log Debug Information
 * This function prints the Debug log message specified along with pointer value in hex
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] pValue defines value to be printed
 *
 */
void phOsal_LogDebugPtrh(const uint8_t* pbMsg, void* pValue);

/**
 * Log Debug Information specified along with the string
 * This function prints the Debug log message specified along with 32bit value in decimal
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogDebugString(const uint8_t* pbMsg, const uint8_t* pbString);

/**
 * Log function entry point
 * This function prints the function name specified prefixed by Module name
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbModuleName defines log message to be printed
 * \param[in] pbFuncName defines value to be printed
 *
 */
void phOsal_LogFunctionEntry(const uint8_t* pbModuleName, const uint8_t* pbFuncName);

/**
 * Log function exit point
 * This function prints the function name specified prefixed by Module name
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbMsg defines log message to be printed
 * \param[in] wValue defines value to be printed
 *
 */
void phOsal_LogFunctionExit(const uint8_t* pbModuleName, const uint8_t* pbFuncName);

/**
 * Log Buffer data
 * This function prints the data in the buffers provided in Hex
 * appropriate log level for debug needs to be enabled from phOsal_SetLogLevel
 *
 * \param[in] pbBuffer              Buffer data  to be logged
 * \param[in] dwSizeOfBuffer        Size of buffer data.
 * \param[in] pbMsg                 defines message to be printed before printing the buffer data.
 *
 */
void phOsal_LogBuffer(const uint8_t*     pbBuffer,
                            uint32_t     dwSizeOfBuffer,
                      const uint8_t*     pbMsg);

#ifdef WIN32
/**
 * This API is used to open a file and write the data file.
 * \note This function opens the file and write the content in to the file.
 *
 * \param[in] *phFileName   Name of the File.
 * \param[in] *PhData[]     Data content.
 * \param[in] NoOfArgument  NUmber of lines to be write in to the file.
 * \retval #DTACLISTATUS_SUCCESS           successfully opens the file
 * \retval #DTACLISTATUS_FAILED            File open fails
 */
OSALSTATUS phOsal_FileOpenAndWrite(const int8_t *phFileName, const int8_t *PhData[], int8_t NoOfArgument);

/**
 * This API is used to the file name.
 * \note This function delete the file from the specified file name.
 *
 * \param[in] *phFileName   Name of the File.
 * \retval #DTACLISTATUS_SUCCESS           successfully delete the file.
 * \retval #DTACLISTATUS_FAILED            File delet fails.
 */
OSALSTATUS phOsal_FileDelete(const int8_t *phFileName);

/**
 * This API is used to get string input from.
 * \note This function executes successfully without OSAL module Initialization.
 *
 * \param[in] dwDelay  Duration in milliseconds for which thread execution to be halted.
 */
void phOsal_Getstring(int8_t *pString);

#endif

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */

#endif
