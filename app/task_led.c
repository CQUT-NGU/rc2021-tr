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

void task_led(void *pvParameters)
{
    (void)pvParameters;

    delay_init();

    buzzer_start();
    buzzer_set(0, BUZZER_PWM_DIV64);
    osDelay(1000);
    buzzer_set(0, 0);
    buzzer_stop();

    int8_t count = 0;

    for (;;)
    {
        uint8_t tmp = 0xFC;

        if (count < 0)
        {
            for (uint8_t i = 0; i != 8; ++i)
            {
                led_line(tmp);
                osDelay(100);
                tmp >>= 1;
                if (i > 1)
                {
                    tmp |= 0x80;
                }
                count++;
            }
        }
        else
        {
            for (uint8_t i = 0; i != 8; ++i)
            {
                led_line(tmp);
                osDelay(100);
                tmp = (uint8_t)(tmp << 1);
                if (i < 6)
                {
                    tmp |= 0x01;
                }
                count++;
            }
        }
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
