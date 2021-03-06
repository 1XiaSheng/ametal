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
 * \brief TOF传感器 TMF8801 例程，通过标准接口实现(通过查询方式获取数据)
 *
 * - 实验现象：
 *   1. 每过一秒，通过串口打印
 *
 * \par 源代码
 * \snippet demo_stm32f103rbt6_core_sensor_tmf8801.c src_stm32f103rbt6_core_sensor_tmf8801
 *
 * \internal
 * \par Modification history
 * - 1.00 20-04-26  fzb, first implementation
 * \endinternal
 */

#include "am_gpio.h"
#include "am_delay.h"
#include "am_stm32f103rbt6.h"
#include "am_sensor.h"
#include "am_hwconf_sensor_tmf8801.h"
#include "demo_std_entries.h"
#include "demo_stm32f103rbt6_core_entries.h"

/**
 * \brief 传感器例程入口
 */
void demo_stm32f103rbt6_core_sensor_tmf8801_entry (void)
{
    am_sensor_handle_t handle = am_sensor_tmf8801_inst_init();

    demo_std_tmf8801_entry(handle);
}


/* end of file */
