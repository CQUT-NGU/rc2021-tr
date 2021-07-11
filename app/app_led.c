/**
 * *****************************************************************************
 * @file         app_led.h
 * @brief        led application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_led.h"

void task_led(void *pvParameters)
{
    (void)pvParameters;

    buzzer_start();
    {
        uint32_t i = 0;
        while (i != 8)
        {
            buzzer_set(i++, BUZZER_PWM_DIV2);
            osDelay(0x80);
        }
        while (i)
        {
            buzzer_set(--i, BUZZER_PWM_DIV2);
            osDelay(0x80);
        }
    }
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
                osDelay(0x40);
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
                osDelay(0x40);
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
