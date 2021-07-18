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

    for (;;)
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
            if (tmp)
            {
                shifth_start(tmp);
            }

            tmp = (int32_t)serial->y;
            if (tmp)
            {
                shifth_index((uint32_t)tmp);
            }

            serial->c = 0;
            break;
        }

        case 'd':
        {
            uint32_t set = 200;

            int match = SERVO_MATCH_PITCH;
            void (*pitch)(uint32_t) = pitch_set;
            void (*jet_on)(void) = jet_middle_on;

            if (serial->y > 0)
            {
                set = (uint32_t)((1 / 0.18F) * serial->y);
            }
            else if (serial->x > 0)
            {
                set = (uint32_t)((1 / 0.18F) * serial->x);
                match = SERVO_MATCH_PITCHL;
                pitch = pitchl_set;
                jet_on = jet_left_on;
            }
            else if (serial->z > 0)
            {
                set = (uint32_t)((1 / 0.18F) * serial->z);
                match = SERVO_MATCH_PITCHR;
                pitch = pitchr_set;
                jet_on = jet_right_on;
            }

            pitch(2000 - set);
            do
            {
                pitch_update(2);
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, match) != match);
            osDelay(400);
            jet_on();

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

        case 't':
        {
            serial->c = 0;

            int tmp = (int)serial->x;
            if (tmp == 1)
            {
                if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
                {
                    xTaskNotifyGive(task_arrow_handler);
                }
            }
            else if (tmp == 2)
            {
                if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
                {
                    xTaskNotifyGive(task_shoot_handler);
                }
            }

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
