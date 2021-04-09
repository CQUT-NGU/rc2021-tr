/**
 * *****************************************************************************
 * @file         task_led.c/h
 * @brief        led task
 * @author       ngu
 * @date         20210101
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_led.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "cmsis_os.h"

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
    delay_init();

    int8_t count = 0;

    for (;;)
    {
        uint8_t tmp = 0xFCU;

        if (count < 0)
        {
            for (uint8_t i = 0U; i < 8U; i++)
            {
                led_line(tmp);
                osDelay(100);
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
            for (uint8_t i = 0U; i < 8U; i++)
            {
                led_line(tmp);
                osDelay(100);
                tmp <<= 1U;
                if (i < 6U)
                {
                    tmp |= 0x01U;
                }
                count++;
            }
        }
    }

    osThreadExit();
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
