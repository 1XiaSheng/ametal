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
 * \brief OPA不同模式下输入输出关系例程，通过 HW 层接口实现
 *
 * - 实验现象：
 *
 *    OPA_UintMode_Test
 *    此时通过示波器观察PC06和PC07信号，PC07输出信号是PC06信号通信幅值是一致的。
 *    OPA_ForWardMode_Test
 *    此时通过示波器观察PC06和PC07信号，PC07输出信号是PC06信号通信幅值是其两倍。
 *    OPA_GpMode_Test
 *    PC06输入VCOR1.5V，PB15和PC07接电阻22K，PB15接电阻10K对地，
 *    此时通过示波器观察PC06和PC07信号，PC07输出信号是PC06信号通信幅值是其两倍。
 *
 *
 * \note
 *    1. 如需观察串口打印的调试信息，需要将 PIOA_10 引脚连接 PC 串口的 TXD，
 *       PIOA_9 引脚连接 PC 串口的 RXD；
 *    2. 如果调试串口使用与本例程相同，则不应在后续继续使用调试信息输出函数
 *      （如：AM_DBG_INFO()）。
 *
 * \par 源代码
 * \snippet demo_hc32l13x_core_opa_one.c src_hc32l13x_core_opa_one
 *
 *
 * \internal
 * \par Modification History
 * - 1.00 19-10-10  ly, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_hc32l13x_core_opa_one
 * \copydoc demo_hc32l13x_core_opa_one.c
 */

/** [src_hc32l13x_core_opa_one] */
#include "ametal.h"
#include "am_hc32.h"
#include "am_gpio.h"
#include "hc32x3x_pin.h"
#include "am_hc32x3x_opa.h"
#include "demo_hc32_entries.h"

/**
 * \brief OPA通道
 */
#define OPA_CH    AM_HC32_OPA_CH1

/**
 * \brief OPA模式
 */
#define OPA_MODE    AM_HC32_OPA_MODE_UNITY_GAIN  /**< \brief OPA单位增益模式*/
//#define OPA_MODE    AM_HC32_OPA_MODE_FORWARD_IN  /**< \biref 正向输入模式 */
//#define OPA_MODE    AM_HC32_OPA_MODE_UNIVERSAL   /**< \biref 通用模式 */


/**
 * \brief 例程入口
 */
void demo_hc32l13x_core_hw_opa_entry (void)
{

    AM_DBG_INFO("demo aml13x_core hw opa one test!\r\n");

    /* 开启OPA时钟 */
    am_clk_enable (CLK_OPA);

    /* 开启BGR时钟 */
    am_clk_enable (CLK_ADC_BGR);

    /* OPA1 P N OUT端 */
    am_gpio_pin_cfg (PIOC_6,  PIOC_6_AIN);
    am_gpio_pin_cfg (PIOB_15, PIOB_15_AIN);
    am_gpio_pin_cfg (PIOC_7,  PIOC_7_AOUT);

    //    /* OPA2 P N OUT端 */
    //    am_gpio_pin_cfg (PIOB_13, PIOB_13_AIN);
    //    am_gpio_pin_cfg (PIOB_12, PIOB_12_AIN);
    //    am_gpio_pin_cfg (PIOB_14, PIOB_14_AOUT);
    //
    //    /* OPA3 P N OUT端 */
    //    am_gpio_pin_cfg (PIOB_10, PIOB_10_AIN);
    //    am_gpio_pin_cfg (PIOB_2,  PIOB_2_AIN);
    //    am_gpio_pin_cfg (PIOB_11, PIOB_11_AOUT);

    demo_hc32x3x_hw_opa_entry(HC32_OPA, OPA_MODE, OPA_CH);
}

/* end of file */
