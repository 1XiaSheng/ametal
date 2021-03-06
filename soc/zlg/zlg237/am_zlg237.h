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
 * \brief ZMF237 芯片特性相关头文件定义
 *
 * \internal
 * \par Modification history
 * - 1.00 19-06-26  yrh, first implementation
 * \endinternal
 */

#ifndef __AM_ZMF237_H
#define __AM_ZMF237_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief 芯片引脚定义 */
#include "../zlg237/zlg237_pin.h"

/** \brief 芯片中断编号定义 */
#include "../zlg237/zlg237_inum.h"

/** \brief 芯片外设寄存器基址定义 */
#include "../zlg237/zlg237_regbase.h"

/** \brief 芯片外设映射 */
#include "../zlg237/zlg237_periph_map.h"

/** \brief 时钟ID定义 */
#include "../zlg237/zlg237_clk.h"

/** \brief DMA通道号定义 */
#include "../zlg237/zlg237_dma_chan.h"

/**
 * \name Cortex-M3 内核处理器和外设设置
 * @{
 */
#ifndef AM_CORTEX_M3
#define AM_CORTEX_M3             1
#endif

#if defined AM_CORTEX_M3

#define __MPU_PRESENT            0  /**< \brief M3 核无 MPU */
#define __NVIC_PRIO_BITS         4  /**< \brief M3 核为 4 */
#define __Vendor_SysTickConfig   0  /**< \brief 设置为 1 可以配置 SysTick */

#include "core_cm3.h"

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_ZMF237_H */

/* end of file */
