/**
 * *****************************************************************************
 * @file         app_archery.c
 * @brief        archery application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_archery.h"

#define SHOOT_COUNT      100
#define FLAG_SHOOT_START (1 << 0)
#define FLAG_SHOOT_COUNT (1 << 1)
#define FLAG_SHOOT_STOP  (1 << 2)

#define SIGNAL_SIGNALING_NONE (1 << 0)  //!< none
#define SIGNAL_SIGNALING_DO   (1 << 1)  //!< signal do
#define SIGNAL_SIGNALING_DONE (1 << 2)  //!< signal done

archery_t archery = {
    .jet = ARCHERY_JET_NONE,
    .load = ARCHERY_LOAD_NONE,
    .signal = ARCHERY_SIGNAL_NONE,
    .jet_count = 0,
};

void archery_update(void)
{
    if (READ_BIT(archery.signal, ARCHERY_SIGNAL_DO))
    {
        CLEAR_BIT(archery.signal, ARCHERY_SIGNAL_DO);
        SET_BIT(archery.signal, ARCHERY_SIGNAL_DONE);
        usart_dma_tx(&huart_os, archery.msg, 2);
    }

    if (READ_BIT(archery.jet, ARCHERY_JET_CNT) &&
        archery.jet_count++ == ARCHERY_JET_COUNT)
    {
        archery.jet_count = 0;
        SET_BIT(archery.jet, ARCHERY_JET_OFF);
        if (READ_BIT(archery.jet, ARCHERY_JET_LEFT))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_LEFT);
            gpio_pin_reset(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
        }
        if (READ_BIT(archery.jet, ARCHERY_JET_MIDDLE))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_MIDDLE);
            gpio_pin_reset(POWER2_LD_GPIO_Port, POWER2_LD_Pin);
        }
        if (READ_BIT(archery.jet, ARCHERY_JET_RIGHT))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_RIGHT);
            gpio_pin_reset(POWER4_RD_GPIO_Port, POWER4_RD_Pin);
        }
    }

    shifth_update();
    shiftv_update();

    if (archery.tick % (SERVO_UPDATE_MS / ARCHERY_CONTROL_TIME_MS) == 0)
    {
        /**
         * Updated arrow steering gear Angle,
         * prevent the PWM from changing too much to
         * cause the steering gear to jam
        */
        fetch_update();
        pitch_update();
    }

    l1s_check();

    ++archery.tick;
}

void task_archery(void *pvParameters)
{
    (void)pvParameters;

    osDelay(1);

    ctrl_serial_t *serial = ctrl_serial_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    {
        uint32_t pwm[7] = {
            SERVO_FETCH_PWMMID,
            SERVO_PITCH_PWMMAX - 200,
            SERVO_SHIFTV_PWMMIN,
            SERVO_PITCH_PWMMAX - 200,
            SERVO_SHIFTV_PWMMIN,
            SERVO_PITCH_PWMMAX - 200,
            SERVO_SHIFTV_PWMMIN,
        };
        servo_init();
        servo_start(pwm);
        pitch_update();
        shifth_init();
        shifth_cli(-330000);
    }

    for (;;)
    {
        /* remote control */
        if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* restart control */
            if (rc->rc.ch[RC_CH_LV] < (RC_ROCKER_MIN / 3))
            {
                serial->c = 0;
            }

            /* Start sending signaling signal */
            if (rc->rc.ch[RC_CH_LV] > (RC_ROCKER_MAX / 3))
            {
                signal_on("a\n");
            }

            /* End sending signaling signal */
            else if (rc->rc.ch[RC_CH_LV] < (RC_ROCKER_MAX / 3))
            {
                signal_off("a\n");
            }

            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < RC_ROCKER_MIN + RC_DEADLINE)
            {
                /* It starts to spew out gas */
                signal_on("b\n");
            }
            else
            {
                signal_off("b\n");
                /* Stop ejecting gas */
                jet_off();
            }
        }

        if (switch_is_down(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < RC_ROCKER_MIN + RC_DEADLINE)
            {
                if (!READ_BIT(archery.task, ARCHERY_TASK_ARROW))
                {
                    SET_BIT(archery.task, ARCHERY_TASK_ARROW);
                    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
                    {
                        xTaskNotifyGive(task_arrow_handler);
                    }
                }
            }
        }

        switch (serial->c)
        {
        case 't':
        {
            serial->c = 0;
            if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
            {
                xTaskNotifyGive(task_arrow_handler);
            }
            break;
        }

        case 'D':
        case 'd':
        {
            serial->c = 0;
            uint32_t set = SERVO_PITCHR_PWMMAX - (uint32_t)((1 / 0.18F) * serial->x);
            pitch_set(set);
            do
            {
                pitch_update();
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH) != SERVO_MATCH_PITCH);
            jet_middle_on();
            break;
        }

        default:
            break;
        }

        archery_update();

        osDelay(ARCHERY_CONTROL_TIME_MS);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
