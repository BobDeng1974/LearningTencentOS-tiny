/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_SYS_H_
#define  _TOS_SYS_H_

#define K_NESTING_LIMIT_IRQ             (k_nesting_t)250u
#define K_NESTING_LIMIT_SCHED_LOCK      (k_nesting_t)250u

typedef enum knl_state_en {
    KNL_STATE_STOPPED,
    KNL_STATE_RUNNING,
} knl_state_t;

// some kind of magic number, mainly for identifing whether the object is initialized, or whether user pass the correct parameter.
typedef enum knl_obj_type_en {
    KNL_OBJ_TYPE_NONE                   = 0x0000,
    KNL_OBJ_TYPE_TASK                   = 0xDAD1,
    KNL_OBJ_TYPE_TIMER                  = 0xDAD2,
    KNL_OBJ_TYPE_MSG_QUEUE              = 0xDAD3,
    KNL_OBJ_TYPE_MMBLK_POOL             = 0xDAD4,
    KNL_OBJ_TYPE_RING_QUEUE             = 0xDAD5,
    KNL_OBJ_TYPE_BINARY_HEAP            = 0xDAD6,
    KNL_OBJ_TYPE_PRIORITY_QUEUE         = 0xDAD7,
    KNL_OBJ_TYPE_CHAR_FIFO              = 0xDAD8,

    // ipc object
    KNL_OBJ_TYPE_SEMAPHORE              = 0x1BEE,
    KNL_OBJ_TYPE_MUTEX                     = 0x2BEE,
    KNL_OBJ_TYPE_EVENT                     = 0x3BEE,
    KNL_OBJ_TYPE_MAIL_QUEUE                = 0x4BEE,
    KNL_OBJ_TYPE_MESSAGE_QUEUE             = 0x5BEE,
    KNL_OBJ_TYPE_PRIORITY_MAIL_QUEUE       = 0x6BEE,
    KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE    = 0x7BEE,
    KNL_OBJ_TYPE_COUNTDOWNLATCH            = 0x8BEE,
    KNL_OBJ_TYPE_COMPLETION                = 0x9BEE,
} knl_obj_type_t;

typedef enum knl_obj_alloc_type_en {
    KNL_OBJ_ALLOC_TYPE_NONE,
    KNL_OBJ_ALLOC_TYPE_STATIC,
    KNL_OBJ_ALLOC_TYPE_DYNAMIC,
} knl_obj_alloc_type_t;

typedef struct knl_object_st {
    knl_obj_alloc_type_t    alloc_type; /* is dynamic allocated(using tos_mmheap) or static memory? */
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_type_t          type;
#endif
} knl_obj_t;

/**
 * @brief Initialize the kernel.
 * initialize the tos tiny kernel.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  errcode
 * @retval  Non-#K_ERR_NONE               return failed.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_knl_init(void);

/**
 * @brief Start the kernel.
 * get the kernel start to run, which means start the multitask scheduling.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  errcode
 * @retval  #K_ERR_KNL_RUNNING            the kernel is already running.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_knl_start(void);

/**
 * @brief Get the kernel state.
 * whether the kernel is running.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  whether the kernel is running.
 * @retval  Non-0                           the kernel is running.
 * @retval  0                               the kernel is not running.
 */
__API__ int     tos_knl_is_running(void);

/**
 * @brief Kernel enter the interrupt.
 * this function should be called in the entrance of a interrupt handler.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  None
 */
__API__ void    tos_knl_irq_enter(void);

/**
 * @brief Kernel exit the interrupt.
 * this function should be called in the exit of a interrupt handler.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  None
 */
__API__ void    tos_knl_irq_leave(void);

/**
 * @brief Lock the scheduler.
 * prevent the kernel from performing task schedule.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  errcode
 * @retval  K_ERR_KNL_NOT_RUNNING         the kernel is not running.
 * @retval  K_ERR_LOCK_NESTING_OVERFLOW   the schedule lock nesting is overflow.
 * @retval  K_ERR_NONE                    return successfully.
 */
__API__ k_err_t tos_knl_sched_lock(void);

/**
 * @brief Unlock the scheduler.
 * re-enable the task schedule.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  errcode
 * @retval  K_ERR_KNL_NOT_RUNNING         the kernel is not running.
 * @retval  K_ERR_SCHED_NOT_LOCKED        the scheduler is not locked.
 * @retval  K_ERR_NONE                    return successfully.
 */
__API__ k_err_t tos_knl_sched_unlock(void);

#if TOS_CFG_TICKLESS_EN > 0u
__KERNEL__ k_tick_t knl_next_expires_get(void);
#endif

__KERNEL__ void     knl_sched(void);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
__KERNEL__ int      knl_object_verify(knl_obj_t *knl_obj, knl_obj_type_t type);
__KERNEL__ void     knl_object_init(knl_obj_t *knl_obj, knl_obj_type_t type);
__KERNEL__ void     knl_object_deinit(knl_obj_t *knl_obj);
#endif

#if TOS_CFG_MMHEAP_EN > 0u
__KERNEL__ void     knl_object_alloc_reset(knl_obj_t *knl_obj);
__KERNEL__ void     knl_object_alloc_set_dynamic(knl_obj_t *knl_obj);
__KERNEL__ void     knl_object_alloc_set_static(knl_obj_t *knl_obj);
__KERNEL__ int      knl_object_alloc_is_dynamic(knl_obj_t *knl_obj);
__KERNEL__ int      knl_object_alloc_is_static(knl_obj_t *knl_obj);
#endif

__KERNEL__ int      knl_is_sched_locked(void);
__KERNEL__ int      knl_is_inirq(void);
__KERNEL__ int      knl_is_idle(k_task_t *task);
__KERNEL__ int      knl_is_self(k_task_t *task);
__KERNEL__ k_err_t  knl_idle_init(void);

#endif /* _TOS_SYS_H_ */

