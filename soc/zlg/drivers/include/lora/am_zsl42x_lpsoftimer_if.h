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
 * \brief LPTMRʵ�� lowpower softimer �Ľӿ�ʵ��
 *
 * \internal
 * \par Modification history
 * - 1.00 20-02-13  ebi, first implementation.
 * - 1.01 20-09-23  yg,  adaptation zsl42x.
 * \endinternal
 */

#ifndef __AM_ZSL42X_LPSOFTIMER_PORTING_H
#define __AM_ZSL42X_LPSOFTIMER_PORTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_int.h"
#include "am_delay.h"

#include "am_zsl42x.h"
#include "hw/amhw_zsl42x_rcc.h"
#include "hw/amhw_zsl42x_lptim.h"

/**
 * @addtogroup am_if_lpsoftimer_if
 * @copydoc am_zsl42x_lpsoftimer_if.h
 * @{
 */

/**
 * \brief softimer ʱ������
 */
typedef uint32_t am_lpsoftimer_time_if_t;

/**
 * \brief LPTMR��ʱ������ص��豸��Ϣ
 */
typedef struct am_zsl42x_lptmr_lpsoftimer_devinfo {
    amhw_zsl42x_lptim_t  *p_hw_lptmr0;       /**< \brief ָ��LPTIMER�Ĵ������ָ�� */
    amhw_zsl42x_lptim_t  *p_hw_lptmr1;       /**< \brief ָ��LPTIMER�Ĵ�����ָ�� */
    uint8_t             lptmr0_inum;       /**< \brief LPTMR0�жϺ�  */
    uint32_t            xtl_clk;           /**< \brief �ⲿ����ʱ��(һ��Ϊ32.768kHz)*/

    float               temp_coef;         /**< \brief �¶�������ϵ�� */
    float               temp_coef_sub;     /**< \brief �¶�������ϵ����Χ */
    float               temp_turnover;     /**< \brief �����߷�ת�¶ȵ� */
    float               temp_turnover_sub; /**< \brief �����߷�ת�¶ȵ���Χ */

    double              timer_offset_coef; /**< \brief ��ʱ�����ϵ�� */

    /** \brief ƽ̨��ʼ�����������ʱ�ӣ��������ŵȹ��� */
    void              (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void              (*pfn_plfm_deinit)(void);

} am_zsl42x_lptmr_lpsoftimer_devinfo_t;

/**
 * \brief lpsoftimer��ʼ��
 * \param[in] p_devinfo : ָ��lpsoftimer�豸��Ϣ��ָ��
 * \retval AM_OK        : �����ɹ�
 * \retval -AM_EINVAL   : ��������
 */
int am_zsl42x_lptmr_lpsoftimer_init (
    const am_zsl42x_lptmr_lpsoftimer_devinfo_t *p_devinfo);

/**
 * \brief lpsoftimerȥ��ʼ��
 * \param[in] ��
 * \return ��
 */
void am_zsl42x_lptmr_lpsoftimer_deinit (void);

/**
 * \brief lpsoftimer �¶Ȳ���
 *
 * \param[in] period      : ʱ������
 * \param[in] temperature : �¶�
 *
 * \return �������ʱ������
 */
uint32_t am_lpsoftimer_temp_compensation (uint32_t period, float temperature);

/**
 * \brief softimer ������һ�γ�ʱ
 * \param[in] timeout : �����ڿ�ʼ���֮����붨ʱ���ж�
 * \return ��
 */
void am_lpsoftimer_timeout_set (am_lpsoftimer_time_if_t timeout);

/**
 * \brief softimer ��ȡ��ǰ�����ʱֵ(��ϵͳ��������)
 * \param[in] ��
 * \return ��ʱֵ
 */
am_lpsoftimer_time_if_t am_lpsoftimer_current_get (void);

/**
 * @}
 */

void am_lora_if_lpsoftimer_init (void);

#ifdef __cplusplus
}
#endif

#endif /* __AM_ZSL42X_LPSOFTIMER_PORTING_H */

/* end of file */
