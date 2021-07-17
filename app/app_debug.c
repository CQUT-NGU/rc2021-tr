/**
 * *****************************************************************************
 * @file         app_debug.c
 * @brief        debug application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_debug.h"

void task_debug(void *pvParameters)
{
    (void)pvParameters;

    osDelay(1000);

    ctrl_serial_t *serial = ctrl_serial_point();

    while (1)
    {
        switch (serial->c)
        {
        case 'a':
        {
            uint32_t tmp;

            tmp = (uint32_t)serial->x;
            if (tmp > 100)
            {
                pitch_set(tmp);
            }
            tmp = (uint32_t)serial->y;
            if (tmp > 100)
            {
                shiftv_set(tmp);
            }
            tmp = (uint32_t)serial->z;
            if (tmp > 100)
            {
                fetch_set(tmp);
            }

            serial->c = 0;
            break;
        }

        case 'h':
        {
            int32_t tmp;

            tmp = (int32_t)serial->x;
            shifth_start(tmp);

            serial->c = 0;
            break;
        }

        case 'r':
        {
            int tmp;

            tmp = (int)serial->x;
            if (tmp == 1)
            {
                gpio_pin_set(RELAY0_GPIO_Port, RELAY0_Pin);
            }
            else if (tmp == -1 || tmp == 3)
            {
                gpio_pin_reset(RELAY0_GPIO_Port, RELAY0_Pin);
            }
            if (tmp == 1)
            {
                gpio_pin_set(RELAY1_GPIO_Port, RELAY1_Pin);
            }
            else if (tmp == 0 || tmp == 3)
            {
                gpio_pin_reset(RELAY1_GPIO_Port, RELAY1_Pin);
            }

            tmp = (int)serial->y;
            if (tmp == 1)
            {
                gpio_pin_set(RELAY2_GPIO_Port, RELAY2_Pin);
            }
            else if (tmp == 0)
            {
                gpio_pin_reset(RELAY2_GPIO_Port, RELAY2_Pin);
            }

            tmp = (int)serial->z;
            if (tmp == 1)
            {
                gpio_pin_set(RELAY3_GPIO_Port, RELAY3_Pin);
            }
            else if (tmp == 0)
            {
                gpio_pin_reset(RELAY3_GPIO_Port, RELAY3_Pin);
            }

            serial->c = 0;
            break;
        }

        default:
            break;
        }

        /* Task delay */
        osDelay(10);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
