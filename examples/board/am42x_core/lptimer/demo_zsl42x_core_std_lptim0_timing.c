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
 * \brief LPTIM0 ��ʱ�� TIMING ���̣�ͨ����׼�ӿ�ʵ��
 *
 * - ʵ������
 *   1. LED0 �� 5Hz ��Ƶ����˸��(����������10Hz��led��ת����Ϊ10/2=5Hz)
 *
 * \note
 *   1.��am_hwconf_zsl42x_lptim.c�ļ��У�__g_lptim0_timing_devinfo�豸��Ϣ�ṹ��ѡ��
 *             AMHW_ZSL42x_LPTIM_FUNCTION_TIMER  ----------- ��ʱ��ģʽ
 *             AMHW_ZSL42x_LPTIM_MODE_RELOAD     ----------- �Զ�����ģʽ
 *             AM_FALSE                          ----------- �ſعر�
 *             AM_TRUE                           ----------- TOG��TOGN���ʹ�ܴ�
 *
 * \par Դ����
 * \snippet demo_zsl42x_std_lptim0_timing.c src_zsl42x_std_lptim0_timing
 *
 * \internal
 * \par Modification history
 * - 1.00 19-09-29  zp, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_zsl42x_std_lptim0_timing
 * \copydoc demo_zsl42x_std_lptim0_timing.c
 */

/** [src_zsl42x_std_lptim0_timing] */

#include "ametal.h"
#include "am_vdebug.h"
#include "demo_std_entries.h"
#include "am_zsl42x_inst_init.h"
#include "demo_am42x_core_entries.h"

/**
 * \brief �������
 */
void demo_zsl42x_core_std_lptim0_timing_entry (void)
{
    AM_DBG_INFO("demo am42x_core std lptim0 timing!\r\n");

    demo_std_timer_timing_entry(am_zsl42x_lptim0_timing_inst_init(), 0);
}
/** [src_zsl42x_std_lptim0_timing] */

/* end of file */
