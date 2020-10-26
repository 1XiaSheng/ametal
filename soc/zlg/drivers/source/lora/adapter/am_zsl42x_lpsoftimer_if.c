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
 * \brief lpsoftimer timer implementation
 *
 * \internal
 * \par Modification history
 * - 1.00 20-02-13  win, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_int.h"
#include "am_clk.h"
#include "am_softimer.h"

#include "am_zsl42x_lpsoftimer.h"
#include "am_zsl42x_lpsoftimer_if.h"

#include "am_zsl42x.h"
#include "hw/amhw_zsl42x_lptim.h"

#include <math.h>

/**
 * ÿ�����Ӧ��ticks(����)
 */
static uint64_t __g_ticks_per_ms;   /* unit: 1e-6 ������10^6�� */

/**
 * lptmrȫ���豸��Ϣָ��
 */
static am_zsl42x_lptmr_lpsoftimer_devinfo_t *__gp_devinfo;

/**
 * lpsoftimer�δ�ֵ�ṹ
 */
static struct lpsoftimer_ticks {
    uint32_t global_ticks;      /* ȫ�ֵ�ticks */
} volatile __g_lpsoftimer = {
    0,
};

/**
 * lpsoftimer������ƶ�ʱֵ
 */
#define __LPSOFTIMER_AVOID_WRAP_TICK        0xFFFFFFF0

/**
 * \brief ��ʱ���ж�������
 */
static void __timer_irq_handler (void *p_arg);

/**
 * \brief ��ȡ��ǰ����ֵ
 */
static uint16_t __lptimer_ticks_get (void)
{
    return amhw_zsl42x_lptim_cnt16_count_get(__gp_devinfo->p_hw_lptmr1) & 0xFFFF;
}

/**
 * \brief ����ƥ��ֵ
 * \note (val - 1)������tick��������0����������0xFFFF
 */
static void __lptimer_match_ticks_set (uint16_t val)
{

    /* ��ֹ��ʱ�������ж� */
    amhw_zsl42x_lptim_int_disable(__gp_devinfo->p_hw_lptmr0);

    /* �رռ�����(��������) */
    amhw_zsl42x_lptim_disable(__gp_devinfo->p_hw_lptmr0);

    /* �ȴ�ͬ����� */
    while(amhw_zsl42x_lptim_write_into_arr_stat(__gp_devinfo->p_hw_lptmr0) ==
          AM_FALSE);

    /*
     * �������ؼĴ���
     * ��ʱʱ������Ϊ0xFFFF-ARR+0x0001
     */
    amhw_zsl42x_lptim_arr_count_set(__gp_devinfo->p_hw_lptmr0,
                                  0XFFFF - val + 1);

    /* �����־λ */
    amhw_zsl42x_lptim_int_flag_clr(__gp_devinfo->p_hw_lptmr0);

    /* ʹ�ܶ�ʱ��LPTIM������� */
    amhw_zsl42x_lptim_enable(__gp_devinfo->p_hw_lptmr0);

    /* �ж�ʹ�� */
    amhw_zsl42x_lptim_int_enable(__gp_devinfo->p_hw_lptmr0);
}

/**
 * \brief ���µ�ǰʵ�ʼ���ֵ
 * \retval ����һ�θ��»��˶���ticks
 */
static uint16_t __lpsoftimer_current_ticks_update (void)
{
    uint16_t lost_ticks;
    uint32_t cpu_key;

    cpu_key = am_int_cpu_lock();

    /* ����Զ����� */
    lost_ticks = __lptimer_ticks_get() - (uint16_t)__g_lpsoftimer.global_ticks;

    /* �˴�������Խ�磨���ƣ� */

    /* ȫ�ּ�ʱ������ */
    __g_lpsoftimer.global_ticks += lost_ticks;

    am_int_cpu_unlock(cpu_key);

    return lost_ticks;
}

/**
 * \brief ��ȡ��ǰʵ�ʼ���ֵ
 * \note 16λӲ����ʱ����Ҫ�������㣬32λӲ����ʱ������ֱ�ӻ�ȡCNT
 */
static uint32_t __lpsoftimer_current_ticks_get (void)
{
    __lpsoftimer_current_ticks_update();
    return __g_lpsoftimer.global_ticks;
}

/**
 * \brief lptimer��ʼ��
 */
static void __lptimer_init (const am_zsl42x_lptmr_lpsoftimer_devinfo_t *p_info)
{
    uint32_t lptmr_clock = p_info->xtl_clk;
    uint32_t pre_reg     = 1;
    uint32_t pre_real    = 0;
    uint32_t temp        = 0;

    /* ����/��ʱ����ѡ�� */
    amhw_zsl42x_lptim_function_sel(p_info->p_hw_lptmr0,
                                   AMHW_ZSL42x_LPTIM_FUNCTION_TIMER);
    amhw_zsl42x_lptim_function_sel(p_info->p_hw_lptmr1,
                                   AMHW_ZSL42x_LPTIM_FUNCTION_TIMER);

    /* ���Զ����� */
    amhw_zsl42x_lptim_mode_sel(p_info->p_hw_lptmr0,
                               AMHW_ZSL42x_LPTIM_MODE_NORELOAD);
    amhw_zsl42x_lptim_mode_sel(p_info->p_hw_lptmr1,
                               AMHW_ZSL42x_LPTIM_MODE_RELOAD);

    /* ʱ��ԴС��256kHz�� ��Ƶ��1kHz */
    if (lptmr_clock <= 256000) {

        /* ��Ƶ��1kHz */
        temp = lptmr_clock / 1000;

        /* �����Ƶֵ 2^n */
        for (pre_real = 2; pre_real < temp;) {
            pre_reg++;
            pre_real  = pre_real << 1;
        }
        /* lptmr_clock / pre_real Ϊ1���tick */
        __g_ticks_per_ms = (uint64_t)(lptmr_clock / pre_real) * 1000000 / 1000;

    } else {
        pre_reg          =  AMHW_ZSL42x_LPTIM_CLK_DIV256;
        /* lptmr_clock / pre_real Ϊ1���tick */
        __g_ticks_per_ms = (uint64_t)(lptmr_clock / 256) * 1000000 / 1000;
    }

    /* ����Ԥ��Ƶֵ */
    amhw_zsl42x_lptim_clk_div_set(p_info->p_hw_lptmr0,
                                (amhw_zsl42x_lptim_clk_div_t)pre_reg);
    amhw_zsl42x_lptim_clk_div_set(p_info->p_hw_lptmr1,
                                (amhw_zsl42x_lptim_clk_div_t)pre_reg);

    /* �����ſ�  */
    amhw_zsl42x_lptim_gate_disable(p_info->p_hw_lptmr0);
    amhw_zsl42x_lptim_gate_disable(p_info->p_hw_lptmr1);

    /* TOG��TOGN�ź��������  */
    amhw_zsl42x_lptim_tog_disable(p_info->p_hw_lptmr0);
    amhw_zsl42x_lptim_tog_disable(p_info->p_hw_lptmr1);

    /* ��ʱ���ж� */
    amhw_zsl42x_lptim_int_enable(p_info->p_hw_lptmr0);
    amhw_zsl42x_lptim_int_enable(p_info->p_hw_lptmr1);

    /* ʹ�ܶ�ʱ��LPTIM������� */
    amhw_zsl42x_lptim_disable(p_info->p_hw_lptmr0);
    amhw_zsl42x_lptim_enable(p_info->p_hw_lptmr1);

    /* �ȴ�ͬ����� */
    while(amhw_zsl42x_lptim_write_into_arr_stat(__gp_devinfo->p_hw_lptmr1) ==
          AM_FALSE);

    /*
     * �������ؼĴ���
     * ��ʱʱ������Ϊ0xFFFF-ARR+0x0001
     */
    amhw_zsl42x_lptim_arr_count_set(__gp_devinfo->p_hw_lptmr1, 1);

#if 0
    while(amhw_zsl42x_lptim_write_into_arr_stat(__gp_devinfo->p_hw_lptmr1 ) ==
          AM_FALSE);

    /* ʹ�ܶ�ʱ��LPTIM������� */
    amhw_zsl42x_lptim_disable(p_info->p_hw_lptmr0);
    amhw_zsl42x_lptim_enable(p_info->p_hw_lptmr1);
#endif

    /* ʹ���ж�  */
    am_int_connect(p_info->lptmr0_inum, __timer_irq_handler, (void *)0);
    am_int_enable(p_info->lptmr0_inum);
}

/******************************************************************************/
int am_zsl42x_lptmr_lpsoftimer_init (
    const am_zsl42x_lptmr_lpsoftimer_devinfo_t *p_devinfo)
{
    /* ������Ч�Լ�� */
    if (NULL == p_devinfo) {
        return -AM_EINVAL;
    }

    /* ����ȫ���豸��Ϣָ�� */
    __gp_devinfo = (am_zsl42x_lptmr_lpsoftimer_devinfo_t *)p_devinfo;

    /* ƽ̨��ʼ�� */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* �ж�Ƶ�ʷ�Ƶ���ӽ�1000Hz�Ա�֤����������������� */
    __lptimer_init(p_devinfo);

    return 0;
}

/******************************************************************************/
void am_zsl42x_lptmr_lpsoftimer_deinit (void)
{
    /* ƽ̨ȥ��ʼ�� */
    __gp_devinfo->pfn_plfm_deinit();
}

/******************************************************************************/
// adapter

float __lpsoftimer_temp_compensation(float period, float temp)
{
#if 1       //ִ�д˶δ�����Ҫ47.298us
    float k       = __gp_devinfo->temp_coef;         //  RTC_TEMP_COEFFICIENT
    float k_dev   = __gp_devinfo->temp_coef_sub;     //  RTC_TEMP_DEV_COEFFICIENT
    float t       = __gp_devinfo->temp_turnover;     //  RTC_TEMP_TURNOVER
    float t_dev   = __gp_devinfo->temp_turnover_sub; //  RTC_TEMP_DEV_TURNOVER
    float interim = 0.0;
    float ppm     = 0.0;

    if (k < 0.0) {
        ppm = (k - k_dev);
    } else {
        ppm = ( k + k_dev );
    }

    interim = (temp - (t - t_dev));
    ppm    *=  interim * interim;

    // Calculate the drift in time
    interim = (period * ppm) / 1000000;
    // Calculate the resulting time period
    interim += period;

    // Calculate the resulting period
    if (interim < 0.0) {
        return period;
    } else {
        return interim;
    }
#else
    return period;
#endif
}

/**
 * Lora timer �¶Ȳ���
 */
uint32_t am_lpsoftimer_temp_compensation (uint32_t period, float temperature)
{
    return (uint32_t)__lpsoftimer_temp_compensation((float)period, temperature);
}

/**
 * \brief softimer Msʱ��ת��Ϊϵͳtickʱ��
 */
uint32_t am_lpsoftimer_ms_to_tick (am_lpsoftimer_time_t ms, float temp)
{
    float tick = __gp_devinfo->timer_offset_coef *
                 __lpsoftimer_temp_compensation(ms, temp);

    return (uint32_t)floor(tick);
}

/**
 * \brief softimer ϵͳtickת��ΪMsʱ��
 */
am_lpsoftimer_time_t am_lpsoftimer_tick_to_ms (uint32_t tick, float temp)
{
    float  k       = __gp_devinfo->temp_coef;
    float  k_dev   = __gp_devinfo->temp_coef_sub;
    float  t       = __gp_devinfo->temp_turnover;
    float  t_dev   = __gp_devinfo->temp_turnover_sub;
    float  interim = 0.0;
    float  ppm     = 0.0;
    double ms      = 0;

    if (k < 0.0) {
        ppm = (k - k_dev);
    } else {
        ppm = ( k + k_dev );
    }

    interim = (temp - (t - t_dev));
    ppm    *=  interim * interim;
    ms      =  tick / (ppm / 1000000 + 1);

    return (uint32_t)floor((2 - __gp_devinfo->timer_offset_coef) * ms);
}

uint32_t __fsl_lptmr_lpsoftimer_current_get (void)
{
    return ((uint64_t)__lpsoftimer_current_ticks_get() *
            (uint64_t)1000000) / __g_ticks_per_ms;
}

void __fsl_lptmr_lpsoftimer_timeout_set (uint32_t timeout)
{
    uint32_t remain_ticks;
    uint32_t cpu_key;

    __lpsoftimer_current_ticks_update();

    cpu_key = am_int_cpu_lock();

    /* �޷����Զ����ƣ�δ�������ڣ���ʹ���Ҳ�ܻ�þ���ֵ */
    remain_ticks = ((uint64_t)timeout * __g_ticks_per_ms) / (uint64_t)1000000;
    if (remain_ticks <= ((uint16_t)__LPSOFTIMER_AVOID_WRAP_TICK)) {
        __lptimer_match_ticks_set(remain_ticks);
    } else {
        __lptimer_match_ticks_set((uint16_t)__LPSOFTIMER_AVOID_WRAP_TICK);
    }

    am_int_cpu_unlock(cpu_key);
}

void am_lpsoftimer_timeout_set (am_lpsoftimer_time_if_t timeout)
{
    __fsl_lptmr_lpsoftimer_timeout_set(timeout);
}

am_lpsoftimer_time_if_t am_lpsoftimer_current_get (void)
{
    return __fsl_lptmr_lpsoftimer_current_get();
}

/**
 * \brief �ر�CPU��
 */
uint32_t am_lpsoftimer_cpu_lock (void)
{
    return am_int_cpu_lock();
}

/**
 * \brief ��CPU��
 */
void am_lpsoftimer_cpu_unlock (uint32_t lock)
{
    am_int_cpu_unlock(lock);
}

/******************************************************************************/

/**
 * \brief LPTIMER0 �жϷ���
 */
static void __lptimer0_irq_handler (void)
{
    /* ����ȫ��tick */
    __lpsoftimer_current_ticks_update();

    /* �ص������ʱ���жϷ����� */
    am_lpsoftimer_isr();

    /* ����ȫ��tick */
    __lpsoftimer_current_ticks_update();

    /* �����־λ */
    amhw_zsl42x_lptim_int_flag_clr(__gp_devinfo->p_hw_lptmr0);
}

/**
 * \brief LPTIMER1 �жϷ���
 */
static void __lptimer1_irq_handler (void)
{
    /* ����ȫ��tick */
    __lpsoftimer_current_ticks_update();

    /* �����־λ */
    amhw_zsl42x_lptim_int_flag_clr(__gp_devinfo->p_hw_lptmr1);
}

/**
 * \brief ÿ�����Ӧ 1.024��tick => ÿ��tick 0.976563ms
 *        ֤ʵ������Ҫtick��Ҫ-1���ܴﵽ��Ҫ�ļ���
 *        ����0ʱ����һ��tick���ж�
 *       (��ʱ��ʹ�ܡ�����ֵ����Ҽ���ֵ����ʱ��TCF����)
 */
static void __timer_irq_handler (void *p_arg)
{
    (void)p_arg;

    if (amhw_zsl42x_lptim_int_flag_check(__gp_devinfo->p_hw_lptmr0)) {
        __lptimer0_irq_handler();
    }

    if (amhw_zsl42x_lptim_int_flag_check(__gp_devinfo->p_hw_lptmr1)) {
        __lptimer1_irq_handler();
    }
}

/* end of file */
