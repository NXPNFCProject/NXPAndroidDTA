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


#ifndef __PH_OSAL_QUEUE_H__
#define __PH_OSAL_QUEUE_H__

#ifdef __cplusplus
extern "C" {  /* Assume C declarations for C++ */
#endif  /* __cplusplus */

typedef enum  phOsal_eQueueOverwriteMode
{
    PHOSAL_QUEUE_NO_OVERWRITE,
    PHOSAL_QUEUE_OVERWRITE_OLDEST,
    PHOSAL_QUEUE_OVERWRITE_NEWEST
}phOsal_eQueueOverwriteMode_t;

typedef struct phOsal_QueueCreateParams_tag
{
    void*    memHdl;
    void*                (*MemAllocCb)(void* memHdl,
                                     uint32_t Size);
    int32_t                (*MemFreeCb)(void* memHdl,
                                      void* ptrToMem);
    uint32_t wQLength;
    phOsal_eQueueOverwriteMode_t eOverwriteMode;

}phOsal_QueueCreateParams_t;


/**
 * \ingroup grp_osal_lib
 * \brief creates queue
 *
 * This function creates resources for handling queue
 * \param[out] pvQueueHandle       Queue Handle to be filled
 * \param[in]  psQueueCreatePrms  structure contatinng params to create linkedlist
 * \retval #OSALSTATUS_SUCCESS    OSAL LIB Queue created successfully
 * \retval #OSALSTATUS_FAILED     OSAL LIB failed to create queue
 *
 */
extern OSALSTATUS phOsal_QueueCreate(void**                        pvQueueHandle,
                                     phOsal_QueueCreateParams_t* psQueueCreatePrms);
/**
 * \ingroup grp_osal_lib
 * \brief Destroys queue
 *
 * This function destroys resources used for handling queue
 * \param[out] pvQueueHandle       Queue Handle to be filled
 * \retval #OSALSTATUS_SUCCESS    OSAL LIB Queue created successfully
 * \retval #OSALSTATUS_FAILED     OSAL LIB failed to create queue
 *
 */
extern OSALSTATUS phOsal_QueueDestroy(void* pvQueueHandle);

/**
 * \ingroup grp_osal_lib
 * \brief Inserts object into queue
 *
 * This function inserts objects into queue

 * \param[in] pvQueueHandle       Queue Handle to be filled
 * \param[out] pvQueueObj         Queue object ot be inserted
 * \retval #OSALSTATUS_SUCCESS    OSAL LIB Queue created successfully
 * \retval #OSALSTATUS_FAILED     OSAL LIB failed to create queue
 *
 */
extern OSALSTATUS phOsal_QueuePush(void*    pvQueueHandle,
                                         void     *pvQueueObj,
                                         uint32_t u4_time_out_ms);

/**
 * \ingroup grp_osal_lib
 * \brief retreive objects from queue
 *
 * This function retrieves objects from queue
 * \param[in] pvQueueHandle       Queue Handle to be filled
 * \param[out] ppvQueueObj        Queue object pulled
 * \param[in] u4_time_out_ms      time to wait until an object is found in queue
 * \retval #OSALSTATUS_SUCCESS    OSAL LIB Queue Pulled successfully
 * \retval #OSALSTATUS_FAILED     OSAL LIB failed to Pull object from queue
 * \retval #OSALSTATUS_Q_UNDERFLOW  No objects in Q even after timeout
 */
extern OSALSTATUS phOsal_QueuePull(void*    pvQueueHandle,
                                        void    **ppvQueueObj,
                                        uint32_t u4_time_out_ms);

/**
 * \ingroup grp_osal_lib
 * \brief retreive objects from queue
 *
 * This function retrieves objects from queue
 * \param[in] pvQueueHandle       Queue Handle to be filled
 * \retval #OSALSTATUS_SUCCESS    OSAL LIB Queue created successfully
 * \retval #OSALSTATUS_FAILED     OSAL LIB failed to create queue
 *
 */
extern OSALSTATUS phOsal_QueueFlush(void* pvQueueHandle);

#ifdef __cplusplus
}  /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#endif /* __PH_OSAL_QUEUE_H__ */
