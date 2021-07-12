/**
 * *****************************************************************************
 * @file         app_archery.c
 * @brief        archery application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_archery.h"

#define SHOOT_COUNT      100
#define FLAG_SHOOT_START (1 << 0)
#define FLAG_SHOOT_COUNT (1 << 1)
#define FLAG_SHOOT_STOP  (1 << 2)

#define SIGNAL_AIMING_NONE (1 << 0)  //!< none
#define SIGNAL_AIMING_DO   (1 << 1)  //!< aim do
#define SIGNAL_AIMING_DONE (1 << 2)  //!< aim done

void task_archery(void *pvParameters)
{
    (void)pvParameters;

    osDelay(800);

    ctrl_serial_t *serial = ctrl_serial_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    static int8_t signal_aiming = SIGNAL_AIMING_NONE;
    static int8_t shoot_flag = 0;
    static uint16_t shoot_count = 0;

    for (;;)
    {
        /* remote control */
        if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* restart control */
            if (rc->rc.ch[RC_CH_LV] < -220)
            {
                serial->c = 0;
            }

            /* Start sending aiming signal */
            if (signal_aiming == SIGNAL_AIMING_NONE &&
                rc->rc.ch[RC_CH_LV] > 220)
            {
                signal_aiming = SIGNAL_AIMING_DO;
            }

            /* End sending aiming signal */
            if (signal_aiming != SIGNAL_AIMING_NONE &&
                rc->rc.ch[RC_CH_LV] < 220)
            {
                signal_aiming = SIGNAL_AIMING_NONE;
            }

            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < -650)
            {
                /* It starts to spew out gas */
                if (!READ_BIT(shoot_flag, FLAG_SHOOT_START))
                {
                    SET_BIT(shoot_flag, FLAG_SHOOT_START);
                    SET_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                    gpio_pin_set(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
                    usart_dma_tx(&huart_os, (const void *)"b\n", 2);
                }
            }
            else
            {
                /* Stop ejecting gas */
                if (READ_BIT(shoot_flag, FLAG_SHOOT_STOP))
                {
                    CLEAR_BIT(shoot_flag, FLAG_SHOOT_STOP);
                    CLEAR_BIT(shoot_flag, FLAG_SHOOT_START);
                }

                gpio_pin_reset(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
            }
        }

        /* Send aiming signal */
        if (signal_aiming == SIGNAL_AIMING_DO)
        {
            signal_aiming = SIGNAL_AIMING_DONE;

            usart_dma_tx(&huart_os, (const void *)"a\n", 2);
        }

        /* Control injection time */
        if (READ_BIT(shoot_flag, FLAG_SHOOT_COUNT))
        {
            if (shoot_count++ == SHOOT_COUNT)
            {
                shoot_count = 0;
                CLEAR_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                gpio_pin_reset(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
                SET_BIT(shoot_flag, FLAG_SHOOT_STOP);
            }
        }

        {
        }

        osDelay(2);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
