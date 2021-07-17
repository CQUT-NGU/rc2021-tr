/**
 * *****************************************************************************
 * @file         task_servo.c
 * @brief        steering gear control
 * @author       NGU
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_servo.h"

#include "ca.h"
#include "bsp.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#define FLAG_RUN_GETL   (1 << 0)
#define FLAG_RUN_GETR   (1 << 1)
#define FLAG_RUN_SHIFTV (1 << 2)
#define FLAG_RUN_PITCH  (1 << 3)

#define PWM_INIT_PITCH 1275

#define PWM_INIT_SHIFTV 1000
#define PWM_LAST_SHIFTV 2300

void task_servo(void *pvParameters)
{
    (void)pvParameters;

    osDelay(500);

    ctrl_serial_t *serial = ctrl_serial_point();

    while (1)
    {
        switch (serial->c)
        {
        case 'a':
        {
            uint32_t tmp;
            tmp = (uint32_t)serial->x;
            if (tmp > 400)
            {
                fetch_set(tmp);
            }
            tmp = (uint32_t)serial->y;
            if (tmp > 400)
            {
                pitch_set(tmp);
            }
            tmp = (uint32_t)serial->z;
            if (tmp > 400)
            {
                shiftv_set(tmp);
            }
            break;
        }

        case 'r':
        {
            int tmp = (int)serial->x;
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

        default:
            break;
        }

        /* Task delay */
        osDelay(4);
    }
}

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
