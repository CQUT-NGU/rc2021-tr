/**
 * *****************************************************************************
 * @file         task_led.c/h
 * @brief        led task
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_led.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"

#include <stdint.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

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

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
