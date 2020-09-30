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
 * \brief ZSL42X LORA SPI0 �û������ļ�
 * \sa am_hwconf_zsl42x_lora_spi0.c
 *
 * \internal
 * \par Modification history
 * - 1.00 20-02-15  win, first implementation.
 * - 1.01 20-09-23  yg,  adaptation zsl42x.
 * \endinternal
 */

#include "am_gpio.h"
#include "am_clk.h"
#include "am_types.h"

#include "am_zsl42x_lora.h"
#include "am_zsl42x_lora_reg.h"
#include "am_zsl42x_lora_if_spi.h"
#include "am_hwconf_zsl42x_lora_spi0.h"

#include "am_zsl42x.h"
#include "hw/amhw_zsl42x_spi.h"

/** \brief ʹ���ж����� */
#define __ZSL42X_LORA_IRQ_MASK  (AM_ZSL42X_LORA_IRQ_MASK_TX_DONE       | \
                                 AM_ZSL42X_LORA_IRQ_MASK_RX_DONE       | \
                                 AM_ZSL42X_LORA_IRQ_MASK_HEADER_VALID  | \
                                 AM_ZSL42X_LORA_IRQ_MASK_CRC_ERR       | \
                                 AM_ZSL42X_LORA_IRQ_MASK_CAD_DONE      | \
                                 AM_ZSL42X_LORA_IRQ_MASK_CAD_DET       | \
                                 AM_ZSL42X_LORA_IRQ_MASK_TIMEOUT)

/** \brief DIO1�ж����� */
#define __ZSL42X_LORA_DIO1_MASK   (__ZSL42X_LORA_IRQ_MASK)

/** \brief DIO2�ж����� */
#define __ZSL42X_LORA_DIO2_MASK   (0)

/** \brief DIO3�ж����� */
#define __ZSL42X_LORA_DIO3_MASK   (0)

/**
 * \addtogroup am_if_src_hwconf_zsl42x_lora_spi0
 * \copydoc am_hwconf_zsl42x_lora_spi0.c
 * @{
 */

/**
 * \brief ZSL42X LORA SPI0 ƽ̨��ʼ��
 */
am_local void __zsl42x_lora_if_spi0_plfm_init (void)
{
    am_clk_enable(CLK_SPI0);

    am_gpio_pin_cfg(PIOA_15, PIOA_15_SPI0_CS  | PIOA_15_OUT_PP);
    am_gpio_pin_cfg(PIOA_5,  PIOA_5_SPI0_SCK  | PIOA_5_OUT_PP);
    am_gpio_pin_cfg(PIOA_6,  PIOA_6_SPI0_MISO | PIOA_6_INPUT_PU);
    am_gpio_pin_cfg(PIOA_7,  PIOA_7_SPI0_MOSI | PIOA_7_OUT_PP);
}

/**
 * \brief ��� ZSL42X LORA SPI0 ƽ̨��ʼ��
 */
am_local void __zsl42x_lora_if_spi0_plfm_deinit (void)
{
    am_gpio_pin_cfg(PIOA_15, AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOA_5,  AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOA_6,  AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOA_7,  AM_GPIO_INPUT);

    am_clk_disable(CLK_SPI0);
}

//PIOA_15,    /**< \brief Ƭѡ���� */
//PIOC_10,    /**< \brief ��λ���� */
//PIOC_11,    /**< \brief BUSY�ź����� */
//
//PIOC_12,    /**< \brief DIO1���� */
//PIOF_6,     /**< \brief DIO2���� */
//PIOF_7,     /**< \brief DIO3���� */
//
//PIOF_5,     /**< \brief txen���� */
//PIOF_4,     /**< \brief rxen���� */

/**
 * \brief zsl42x lora ƽ̨��ʼ��
 */
am_local void __zsl42x_lora_plfm_init (void *p_arg)
{
    am_zsl42x_lora_dev_t *p_dev = (am_zsl42x_lora_dev_t *)p_arg;

    /* ���ų�ʼ�� */
    am_gpio_pin_cfg(PIOA_15, AM_GPIO_OUTPUT_INIT_HIGH);         // CS
    am_gpio_pin_cfg(PIOC_10, PIOC_10_GPIO | PIOC_10_OUT_PP);    // RST
    am_gpio_pin_cfg(PIOC_11, PIOC_11_GPIO | PIOC_11_INPUT_PD);  // BUSY
    am_gpio_pin_cfg(PIOC_12, PIOC_12_GPIO | PIOC_12_INPUT_PD);  // DIO1
    am_gpio_pin_cfg(PIOF_4,  PIOF_4_GPIO  | PIOF_4_OUT_PP);     // RX_EN
    am_gpio_pin_cfg(PIOF_5,  PIOF_5_GPIO  | PIOF_5_OUT_PP);     // TX_EN
    am_gpio_pin_cfg(PIOF_6,  PIOF_6_GPIO  | PIOF_6_INPUT_PU);   // DIO2
    am_gpio_pin_cfg(PIOF_7,  PIOF_7_GPIO  | PIOF_7_INPUT_PU);   // DIO3

    /* ��λ��Ƭѡ��������ߵ�ƽ */
    am_gpio_set(PIOC_10, 1);
    am_gpio_set(PIOA_15, 1);

    /* �����ش��� */
    am_gpio_trigger_cfg(p_dev->p_devinfo->dio1_pin, AM_GPIO_TRIGGER_RISE);

    /* ����DIO1���жϺ��� */
    am_gpio_trigger_connect(p_dev->p_devinfo->dio1_pin,
                            p_dev->p_dio_irq,
                            &p_dev->lora_dev);
}

/**
 * \brief zsl42x lora ��ƽ̨��ʼ��
 */
am_local void __zsl42x_lora_plfm_deinit (void *p_arg)
{
    (void)p_arg;

    am_gpio_pin_cfg(PIOA_15, PIOA_15_GPIO | PIOA_15_INPUT_PU);
    am_gpio_pin_cfg(PIOC_10, PIOC_10_GPIO | PIOC_10_INPUT_PU);
    am_gpio_pin_cfg(PIOC_11, PIOC_11_GPIO | PIOC_11_INPUT_PU);
    am_gpio_pin_cfg(PIOC_12, PIOC_12_GPIO | PIOC_12_INPUT_PU);
    am_gpio_pin_cfg(PIOF_4,  PIOF_4_GPIO  | PIOF_4_INPUT_PU);
    am_gpio_pin_cfg(PIOF_5,  PIOF_5_GPIO  | PIOF_5_INPUT_PU);
    am_gpio_pin_cfg(PIOF_6,  PIOF_6_GPIO  | PIOF_6_INPUT_PU);
    am_gpio_pin_cfg(PIOF_7,  PIOF_7_GPIO  | PIOF_7_INPUT_PU);
}

/**
 * \brief zsl42x lora �����ж�ʹ��
 */
am_local int __zsl42x_lora_dio_irq_enable (int pin)
{
    return am_gpio_trigger_on(pin);
}

/**
 * \brief zsl42x lora �����жϽ���
 */
am_local int __zsl42x_lora_dio_irq_disable (int pin)
{
    return am_gpio_trigger_off(pin);
}

/**
 * \brief LORA SPI0 �豸��Ϣ
 */
am_const struct am_zsl42x_lora_if_spi_devinfo __g_zsl42x_lora_spi0_devinfo = {

    {
        AM_ZSL42X_LORA_CORE_TYPE_SX1268, /**< \brief оƬ������� */

        PIOA_15,                         /**< \brief Ƭѡ���� */
        PIOC_10,                         /**< \brief ��λ���� */
        PIOC_11,                         /**< \brief BUSY�ź����� */

        PIOC_12,                         /**< \brief DIO1���� */
        PIOF_6,                          /**< \brief DIO2���� */
        PIOF_7,                          /**< \brief DIO3���� */

        PIOF_5,                          /**< \brief txen���� */
        PIOF_4,                          /**< \brief rxen���� */

        32000000,                        /**< \brief ZSL42X_LORAʹ�õľ���Ƶ�� */

        /**
         * \brief ����470 ~ 510Ƶ��У��
         *        У׼���̽�����4ms���ҵĵȴ����̣��������ϴ�ĵ�����ֵ
         */
        AM_ZSL42X_LORA_CALIBRATE_IMAGE_470MHz_510MHz,

        __ZSL42X_LORA_IRQ_MASK,
        __ZSL42X_LORA_DIO1_MASK,
        __ZSL42X_LORA_DIO2_MASK,
        __ZSL42X_LORA_DIO3_MASK,

        __zsl42x_lora_plfm_init,         /**< \brief ������ų�ʼ������ */
        __zsl42x_lora_plfm_deinit,       /**< \brief �������ȥ��ʼ������ */

        am_gpio_set,                     /**< \brief ���ŵ�ƽ���ù��ܺ���ָ�� */
        am_gpio_get,                     /**< \brief ���ŵ�ƽ��ȡ���ܺ���ָ�� */

        __zsl42x_lora_dio_irq_enable,    /**< \brief �����ж�ʹ�� */
        __zsl42x_lora_dio_irq_disable    /**< \brief �����жϽ��� */
    },

    ZSL42x_SPI0_BASE,                      /**< \brief SPI0 �Ĵ���ָ��   */

    __zsl42x_lora_if_spi0_plfm_init,     /**< \brief ZSL42X LORA SPI0 ƽ̨��ʼ������ */
    __zsl42x_lora_if_spi0_plfm_deinit    /**< \brief ZSL42X LORA SPI0 ƽ̨���ʼ������ */
};

/**
 * \brief ZSL42X LORA SPI0 �豸ʵ��
 */
am_local am_zsl42x_lora_if_spi_dev_t __g_zsl42x_lora_spi0_dev;

/**
 * \brief ZSL42X LORA SPI0 ʵ����ʼ��
 */
am_zsl42x_lora_handle_t am_zsl42x_lora_spi0_inst_init (void)
{
    return am_zsl42x_lora_if_spi_init(&__g_zsl42x_lora_spi0_dev,
                                      &__g_zsl42x_lora_spi0_devinfo);
}

/**
 * \brief ZSL42X LORA SPI0 ʵ�����ʼ��
 */
void am_zsl42x_lora_spi0_inst_deinit (am_zsl42x_lora_handle_t handle)
{
    am_zsl42x_lora_if_spi_deinit(handle);
}

/**
 * @}
 */

/* end of file */
