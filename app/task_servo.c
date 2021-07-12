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

    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_serial_t *serial = ctrl_serial_point();

    servo_init();
    servo_start();
    pitch_init(1700);
    fetch_init(1100);

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

        /* down, down */
        if (switch_is_down(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* Pull the lever to the lowest level */
            if (rc->rc.ch[RC_CH_LV] < -650)
            {
                /* Unclip the arrow */
                if (rc->rc.ch[RC_CH_S] < -650)
                {
                    gpio_pin_reset(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
                }
                /**
                 * -550 ~ -110
                 * The steering gear is moved vertically forward and
                 * inserted into the tail of the arrow
                */
                else if (rc->rc.ch[RC_CH_S] < -110)
                {
                    //shiftv_set(PWM_LAST_SHIFTV);
                }

                /* The steering gear moves backwards vertically */
                if (rc->rc.ch[RC_CH_S] > 110 ||
                    rc->rc.ch[RC_CH_RV] > 220)
                {
                    shiftv_set(PWM_INIT_SHIFTV);
                }

                /* Set the Angle of the arrow */
                if (rc->rc.ch[RC_CH_RV] > 0)
                {
                    pitch_set((uint8_t)(PWM_INIT_PITCH - rc->rc.ch[RC_CH_RV]));
                }
                /* Lift the arrow horizontally */
                else if (rc->rc.ch[RC_CH_RV] < -550)
                {
                }
                /* -440 ~ -10, Clip the arrow and lift it */
                else if (rc->rc.ch[RC_CH_RV] < -110)
                {
                    /* Clamp the arrow */
                    gpio_pin_set(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
                    /* Lift it */
                }
            }
        }

        /**
         * Updated arrow steering gear Angle,
         * prevent the PWM from changing too much to
         * cause the steering gear to jam
        */
        fetch_update();
        pitch_update();

        /* Task delay */
        osDelay(4);
    }
}

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
