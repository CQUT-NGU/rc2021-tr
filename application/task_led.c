/**
 * *****************************************************************************
 * @file         task_led.c
 * @brief        led task
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_led.h"

#include "bsp.h"

#include <stdint.h>

void task_led(void *pvParameters)
{
    (void)pvParameters;

    delay_init();

    buzzer_start();
    buzzer_set(0, BUZZER_PWM_DIV64);
    osDelay(1000U);
    buzzer_set(0, 0);
    buzzer_stop();

    int8_t count = 0;

    for (;;)
    {
        uint8_t tmp = 0xFCU;

        if (count < 0)
        {
            for (uint8_t i = 0U; i != 8U; ++i)
            {
                led_line(tmp);
                osDelay(100U);
                tmp >>= 1U;
                if (i > 1U)
                {
                    tmp |= 0x80U;
                }
                count++;
            }
        }
        else
        {
            for (uint8_t i = 0U; i != 8U; ++i)
            {
                led_line(tmp);
                osDelay(100U);
                tmp <<= 1U;
                if (i < 6U)
                {
                    tmp |= 0x01U;
                }
                count++;
            }
        }
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
