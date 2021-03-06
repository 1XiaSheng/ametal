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
 * \brief ZSL42x PCA 用户配置文件
 * \sa am_hwconf_zsl42x_pca.c
 *
 * \internal
 * \par Modification history
 * - 1.00 19-10-12
 * \endinternal
 */

#include "ametal.h"
#include "am_gpio.h"
#include "am_zsl42x.h"
#include "zsl42x_clk.h"
#include "zsl42x_inum.h"
#include "zsl42x_regbase.h"
#include "am_zsl42x_clk.h"
#include "am_zsl42x_pca.h"

/**
 * \addtogroup am_if_src_hwconf_zsl42x_pca
 * \copydoc am_hwconf_zsl42x_pca.c
 * @{
 */

/** \brief 引脚配置信息列表 */
am_zsl42x_pca_ioinfo_t __g_pca_ioinfo_list[] = {
        {PIOB_4, PIOB_4_PCA_CH0 | PIOB_4_OUT_PP,
                 PIOB_4_PCA_CH0 | PIOB_4_INPUT_PU},
        {PIOB_5, PIOB_5_PCA_CH1 | PIOB_5_OUT_PP,
                 PIOB_5_PCA_CH1 | PIOB_5_INPUT_PU},
        {PIOC_8, PIOC_8_PCA_CH2 | PIOC_8_OUT_PP,
                 PIOC_8_PCA_CH2 | PIOC_8_INPUT_PU},
        {PIOC_9, PIOC_9_PCA_CH3 | PIOC_9_OUT_PP,
                 PIOC_9_PCA_CH3 | PIOC_9_INPUT_PU},
        {PIOC_5, PIOC_5_PCA_CH4 | PIOC_5_OUT_PP,
                 PIOC_5_PCA_CH4 | PIOC_5_INPUT_PU}
};

/**
 * \brief PCA 平台初始化函数
 */
am_local void __zsl42x_pca_plfm_init (void)
{
    am_clk_enable(CLK_PCA);
}

/**
 * \brief PCA 平台解初始化函数
 */
am_local void __zsl42x_pca_plfm_deinit (void)
{
    am_clk_disable(CLK_PCA);
}

/** \brief PCA 设备信息 */
am_local am_const am_zsl42x_pca_devinfo_t __g_zsl42x_pca_devinfo = {
        ZSL42x_PCA_BASE,
        CLK_PCA,
        INUM_PCA,
        ZSL42x_PCA_PCLK32,
        __g_pca_ioinfo_list,
        __zsl42x_pca_plfm_init,
        __zsl42x_pca_plfm_deinit
};

/** \brief PCA 设备实例 */
am_local am_zsl42x_pca_dev_t __g_zsl42x_pca_dev;

/**
 * \brief PCA 实例初始化
 */
am_zsl42x_pca_handle_t am_zsl42x_pca1_inst_init (void)
{
    return am_zsl42x_pca_init(&__g_zsl42x_pca_dev, &__g_zsl42x_pca_devinfo);
}

/**
 * \brief PCA 实例解初始化
 */
void am_zsl42x_pca_inst_deinit (am_zsl42x_pca_handle_t handle)
{
    am_zsl42x_pca_deinit(handle);
}

/**
 * @}
 */

/* end of file */
