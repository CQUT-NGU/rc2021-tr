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
            serial->c = 0;
            uint32_t pwm;
            switch ((uint32_t)serial->x)
            {
            case 0:
            {
                pwm = (uint32_t)serial->y;
                if (pwm > 100)
                {
                    fetch_set(pwm);
                }
            }
            break;

            case 1:
            {
                pwm = (uint32_t)serial->y;
                if (pwm > 100)
                {
                    pitchl_set_pwm(pwm);
                }
                pwm = (uint32_t)serial->z;
                if (pwm > 100)
                {
                    shiftvl_set_pwm(pwm);
                }
            }
            break;

            case 2:
            {
                pwm = (uint32_t)serial->y;
                if (pwm > 100)
                {
                    pitch_set_pwm(pwm);
                }
                pwm = (uint32_t)serial->z;
                if (pwm > 100)
                {
                    shiftv_set_pwm(pwm);
                }
            }
            break;

            case 3:
            {
                pwm = (uint32_t)serial->y;
                if (pwm > 100)
                {
                    pitchr_set_pwm(pwm);
                }
                pwm = (uint32_t)serial->z;
                if (pwm > 100)
                {
                    shiftvr_set_pwm(pwm);
                }
            }
            break;

            default:
            {
            }
            break;
            }
        }
        break;

        case 'h':
        {
            serial->c = 0;
            int32_t count = (int32_t)serial->x;
            if (count)
            {
                shifth_start(count);
            }
            uint32_t index = (uint32_t)serial->y;
            if (index)
            {
                shifth_index(index);
            }
        }
        break;

        case 'j':
        {
            serial->c = 0;
            uint32_t set = SERVO_PITCH_STD - 200;
            int match = SERVO_MATCH_PITCH;
            void (*pitch)(uint32_t) = pitch_set;
            void (*jet_on)(void) = jet_middle_on;
            if (serial->y > 0)
            {
                set = SERVO_PITCH_STD - (uint32_t)((1 / 0.18F) * serial->y);
            }
            else if (serial->x > 0)
            {
                set = SERVO_PITCHL_STD - (uint32_t)((1 / 0.18F) * serial->x);
                match = SERVO_MATCH_PITCHL;
                pitch = pitchl_set;
                jet_on = jet_left_on;
            }
            else if (serial->z > 0)
            {
                set = SERVO_PITCHR_STD + (uint32_t)((1 / 0.18F) * serial->z);
                match = SERVO_MATCH_PITCHR;
                pitch = pitchr_set;
                jet_on = jet_right_on;
            }
            pitch(set);
            do
            {
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, match) != match);
            osDelay(500);
            jet_on();
        }
        break;

        case 'r':
        {
            serial->c = 0;
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
        }
        break;

        case 't':
        {
            serial->c = 0;
            switch ((int)serial->x)
            {
            case 1:
            {
                archery_arrow();
            }
            break;

            case 2:
            {
                if (serial->y > 0)
                {
                    archery.angle = serial->y;
                    archery_shoot();
                }
            }
            break;

            default:
            {
            }
            break;
            }
        }
        break;

        default:
        {
        }
        break;
        }

        /* Task delay */
        osDelay(2);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
