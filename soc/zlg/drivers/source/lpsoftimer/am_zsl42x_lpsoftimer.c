/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief softimer implementation
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-28  sky, first implementation.
 * \endinternal
 */

#include "am_zsl42x_lpsoftimer.h"

/**
 * ��ʱ����ͷ
 */
static am_lpsoftimer_t *__gp_timer_list_head = NULL;

/**
 * \brief �����µ���ͷ��ʱ��
 * \param[in] p_obj : ָ��Ŀ�궨ʱ��
 * \return ��
 */
static void __insert_new_head_timer (am_lpsoftimer_t      *p_obj,
                                     am_lpsoftimer_time_t  current_time)
{
    uint32_t cpu_key;

    am_lpsoftimer_t *p_cur = __gp_timer_list_head;

    /* ����ǰͷ����״̬�л�Ϊ��ֹ */
    if (p_cur != NULL) {
        p_cur->is_running = 0;
    }

    /* Ŀ�궨ʱ���滻Ϊ�µ���ͷ */
    p_obj->p_next        = p_cur;
    p_obj->is_running    = 1;
    __gp_timer_list_head = p_obj;

    cpu_key = am_lpsoftimer_cpu_lock();
    am_lpsoftimer_timeout_set(__gp_timer_list_head->dest_time - current_time);
    am_lpsoftimer_cpu_unlock(cpu_key);
}

/**
 * \brief �����µĶ�ʱ��
 * \param[in] p_obj : ָ��Ŀ�궨ʱ��
 * \return ��
 */
static void __insert_new_timer (am_lpsoftimer_t *p_obj)
{
    am_lpsoftimer_t *p_prev = __gp_timer_list_head;
    am_lpsoftimer_t *p_cur  = __gp_timer_list_head->p_next;

    /* ��ͷ�����ֱ�Ӳ��� */
    if (p_cur == NULL) {

        p_prev->p_next = p_obj;
        p_obj->p_next  = NULL;

    } else {

        while (p_prev != NULL) {

            /* Ŀ�궨ʱ���ȼƵ�����뵽ǰ�棨��ǰ�յ��ȥĿ���յ�С��32λ��ƫ��һ�룩 */
            if ((p_cur->dest_time - p_obj->dest_time) < 0x80000000) {

                p_prev->p_next = p_obj;
                p_obj->p_next  = p_cur;
                break;

            } else {

                p_prev = p_cur;
                p_cur  = p_cur->p_next;

                /* ���뵽��β */
                if (p_cur == NULL) {
                    p_prev->p_next = p_obj;
                    p_obj->p_next  = NULL;
                    break;
                }
            }
        }
    }
}

/**
 * \brief ��鶨ʱ���Ƿ��Ѿ�����
 * \param[in] p_obj : ָ��Ŀ�궨ʱ��
 * \return true of false
 */
static uint8_t __if_timer_exists (am_lpsoftimer_t *p_obj)
{
    am_lpsoftimer_t *p_cur = __gp_timer_list_head;

    while (p_cur != NULL) {
        if (p_cur == p_obj) {
            return 1;
        }
        p_cur = p_cur->p_next;
    }

    return 0;
}

/******************************************************************************/

void am_lpsoftimer_init (am_lpsoftimer_t *p_obj,
                         void           (*pfn_callback)(void *p_arg),
                         void            *p_arg)
{
    if (__if_timer_exists(p_obj)) {
        am_lpsoftimer_stop(p_obj);
    }

    p_obj->dest_time   = 0;
    p_obj->reload_time = 0;
    p_obj->is_running  = 0;
    p_obj->pfn_cb      = pfn_callback;
    p_obj->p_next      = NULL;
    p_obj->p_arg       = p_arg;
}

void am_lpsoftimer_start (am_lpsoftimer_t *p_obj)
{
    uint32_t             cpu_key;
    am_lpsoftimer_time_t current_time;

    cpu_key = am_lpsoftimer_cpu_lock();

    /* ����ʱ���Ѿ������������棬����������Ϊ�Ѿ������� */
    if ((p_obj == NULL) || (__if_timer_exists(p_obj) == 1)) {
        am_lpsoftimer_cpu_unlock(cpu_key);
        return;
    }

    /* ��������ticks */
    current_time      = am_lpsoftimer_current_get();
    p_obj->dest_time  = current_time + p_obj->reload_time;
    p_obj->is_running = 0;

    /* ����ͷ����ͷǰ���滻��ͷ */
    if (__gp_timer_list_head == NULL) {
        __insert_new_head_timer(p_obj, current_time);
    } else {
        /* Ŀ�궨ʱ���ȼƵ�����뵽ǰ�棨��ǰ�յ��ȥĿ���յ�С��32λ��ƫ��һ�룩 */
        if ((__gp_timer_list_head->dest_time - p_obj->dest_time) < 0x80000000) {
            __insert_new_head_timer(p_obj, current_time);
        } else {
            __insert_new_timer(p_obj);
        }
    }

    am_lpsoftimer_cpu_unlock(cpu_key);
}

void am_lpsoftimer_stop (am_lpsoftimer_t *p_obj)
{
    uint32_t cpu_key;

    am_lpsoftimer_t *p_prev = __gp_timer_list_head;
    am_lpsoftimer_t *p_cur  = __gp_timer_list_head;

    cpu_key = am_lpsoftimer_cpu_lock();

    // List is empty or the Obj to stop does not exist
    if ((__gp_timer_list_head == NULL) || (p_obj == NULL)) {
        am_lpsoftimer_cpu_unlock(cpu_key);
        return;
    }

    // Stop the Head
    if (__gp_timer_list_head == p_obj) {

        // The head is already running
        if (__gp_timer_list_head->is_running == 1) {

            if (__gp_timer_list_head->p_next != NULL) {
                __gp_timer_list_head->is_running  = 0;
                __gp_timer_list_head              = __gp_timer_list_head->p_next;
                __gp_timer_list_head->is_running  = 1;
                am_lpsoftimer_timeout_set(__gp_timer_list_head->dest_time - am_lpsoftimer_current_get());
            } else {
                __gp_timer_list_head = NULL;
            }

        // Stop the head before it is started
        } else {

            if (__gp_timer_list_head->p_next != NULL) {
                __gp_timer_list_head = __gp_timer_list_head->p_next;
            } else {
                __gp_timer_list_head = NULL;
            }
        }

    // Stop an object within the list
    } else {
        while (p_cur != NULL) {
            if (p_cur == p_obj) {
                if (p_cur->p_next != NULL) {
                    p_cur          = p_cur->p_next;
                    p_prev->p_next = p_cur;
                } else {
                    p_cur          = NULL;
                    p_prev->p_next = p_cur;
                }
                break;
            } else {
                p_prev = p_cur;
                p_cur  = p_cur->p_next;
            }
        }
    }

    am_lpsoftimer_cpu_unlock(cpu_key);
}

void am_lpsoftimer_restart (am_lpsoftimer_t *p_obj)
{
    am_lpsoftimer_stop(p_obj);
    am_lpsoftimer_start(p_obj);
}

uint8_t am_lpsoftimer_list_isempty (void)
{
    return (uint8_t)(__gp_timer_list_head == NULL);
}

void am_lpsoftimer_value_set (am_lpsoftimer_t *p_obj, uint32_t time)
{
    /* ֹͣ��ʱ�� */
    am_lpsoftimer_stop(p_obj);

    /* ��ʱֵ��Ч��У�� */
    if (0 == time) {
        time = 1;
    }

    /* ��ֵ��tick���� */
    p_obj->reload_time = time;
}

am_lpsoftimer_time_t am_lpsoftimer_elapsed_get (am_lpsoftimer_time_t saved_time)
{
    uint32_t cpu_key;

    am_lpsoftimer_time_t current_time;

    cpu_key = am_lpsoftimer_cpu_lock();
    current_time = am_lpsoftimer_current_get();
    am_lpsoftimer_cpu_unlock(cpu_key);

    return current_time - saved_time;
}

void am_lpsoftimer_isr (void)
{
    uint32_t cpu_key;

    am_lpsoftimer_time_t current_time;

    /* Early out when __gp_timer_list_head is null to prevent null pointer */
    if (__gp_timer_list_head == NULL) {
        return;
    }

    /* �л�ͷ��ʱ������״̬ */
    __gp_timer_list_head->is_running = 0;

    cpu_key = am_lpsoftimer_cpu_lock();
    current_time = am_lpsoftimer_current_get();
    am_lpsoftimer_cpu_unlock(cpu_key);

    /* ɨ����ϻص������Ķ�ʱ������ǰʱ���ȥĿ���յ�С��32λ��ƫ��һ�뼴�Ƶ��� */
    while ((__gp_timer_list_head != NULL) &&
           ((current_time - __gp_timer_list_head->dest_time) < 0x80000000)) {

        am_lpsoftimer_t *p_elapsed_timer;

        p_elapsed_timer      = __gp_timer_list_head;
        __gp_timer_list_head = __gp_timer_list_head->p_next;

        if (p_elapsed_timer->pfn_cb != NULL) {
            p_elapsed_timer->pfn_cb(p_elapsed_timer->p_arg);
        }

        cpu_key = am_lpsoftimer_cpu_lock();
        current_time = am_lpsoftimer_current_get();
        am_lpsoftimer_cpu_unlock(cpu_key);
    }

    /* ������º����ͷ��ʱ�������������� */
    if (__gp_timer_list_head != NULL) {
        if (__gp_timer_list_head->is_running != 1) {
            __gp_timer_list_head->is_running = 1;
            cpu_key = am_lpsoftimer_cpu_lock();
            am_lpsoftimer_timeout_set(__gp_timer_list_head->dest_time -
                                      current_time);
            am_lpsoftimer_cpu_unlock(cpu_key);
        }
    }
}

/* end of file */
