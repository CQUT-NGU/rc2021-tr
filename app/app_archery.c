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

archery_t archery = {
    .jet = ARCHERY_JET_RESET,
    .task = ARCHERY_TASK_RESET,
    .load = ARCHERY_LOAD_ALL,
    .signal = ARCHERY_SIGNAL_RESET,
    .tick = 0,
    .jet_count = 0,
};

void archery_arrow(void)
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

void archery_reday(void)
{
    archery.angle = (1 / 0.18F) * archery.angle;

    uint32_t angle = (uint32_t)(archery.angle);

    if (READ_BIT(archery.load, ARCHERY_LOAD_M))
    {
        archery.jet_on = jet_middle_on;
        shifth_index(SHIFTH_INDEX_MIDDLE);
        pitch_set(SERVO_PITCH_PWMMAX - angle);
        do
        {
            archery_update();
            osDelay(ARCHERY_CONTROL_TIME_MS);
        } while (READ_BIT(step.flag, SHIFTH_FLAG_AUTO) ||
                 !READ_BIT(servo.match, SERVO_MATCH_PITCH));
        osDelay(500);
    }
    else if (READ_BIT(archery.load, ARCHERY_LOAD_L))
    {
        archery.jet_on = jet_left_on;
        pitchl_set(SERVO_PITCHL_PWMMAX - angle);
        do
        {
            archery_update();
            osDelay(ARCHERY_CONTROL_TIME_MS);
        } while (READ_BIT(servo.match, SERVO_MATCH_PITCHL) != SERVO_MATCH_PITCHL);
        osDelay(500);
    }
    else if (READ_BIT(archery.load, ARCHERY_LOAD_R))
    {
        archery.jet_on = jet_right_on;
        pitchr_set(SERVO_PITCHR_PWMMIN + angle);
        do
        {
            archery_update();
            osDelay(ARCHERY_CONTROL_TIME_MS);
        } while (READ_BIT(servo.match, SERVO_MATCH_PITCHR) != SERVO_MATCH_PITCHR);
        osDelay(500);
    }
    else
    {
        archery.jet_on = 0;
    }
}

void archery_shoot(void)
{
    archery_reday();

    if (!READ_BIT(archery.task, ARCHERY_TASK_SHOOT))
    {
        SET_BIT(archery.task, ARCHERY_TASK_SHOOT);
        SET_BIT(archery.wait, ARCHERY_WAIT_SHOOT);
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            xTaskNotifyGive(task_shoot_handler);
        }
    }

    /* Wait for archery inspection to complete */
    do
    {
        archery_update();
        osDelay(ARCHERY_CONTROL_TIME_MS);
    } while (READ_BIT(archery.wait, ARCHERY_WAIT_SHOOT));
    if (archery.jet_on)
    {
        archery.jet_on();
        archery.jet_on = 0;
    }
}

void archery_update(void)
{
    if (xTaskGetCurrentTaskHandle() == task_archery_handler)
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
                pitchl_set(SERVO_PITCHL_PWMMAX - 100);
            }
            if (READ_BIT(archery.jet, ARCHERY_JET_MIDDLE))
            {
                CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_MIDDLE);
                gpio_pin_reset(POWER2_LD_GPIO_Port, POWER2_LD_Pin);
                pitch_set(SERVO_PITCH_PWMMAX - 100);
            }
            if (READ_BIT(archery.jet, ARCHERY_JET_RIGHT))
            {
                CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_RIGHT);
                gpio_pin_reset(POWER4_RD_GPIO_Port, POWER4_RD_Pin);
                pitchr_set(SERVO_PITCHR_PWMMIN + 100);
            }
        }

        shifth_update(SHIFTH_PWM_DELTA, SHIFTH_PWM_DIVIDE);
        shiftv_update(16);
        pitch_update(1);
        fetch_update(1);

        if (archery.tick % ARCHERY_CONTROL_TIME_MS == 0)
        {
            /**
             * Updated arrow steering gear Angle,
             * prevent the PWM from changing too much to
             * cause the steering gear to jam
            */
        }

        l1s_check();

        ++archery.tick;
    }
}

TaskHandle_t task_archery_handler;

void task_archery(void *pvParameters)
{
    (void)pvParameters;

    osDelay(1);

    ctrl_serial_t *serial = ctrl_serial_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    {
        uint32_t pwm[7] = {
            SERVO_FETCH_PWMMAX,
            SERVO_PITCH_PWMMAX - 200,
            SERVO_SHIFTV_PWMMIN,
            SERVO_PITCHL_PWMMAX - 200,
            SERVO_SHIFTVL_PWMMIN,
            SERVO_PITCHR_PWMMIN + 200,
            SERVO_SHIFTVR_PWMMAX,
        };
        servo_init();
        servo_start(pwm);
        shifth_init();
    }

    task_archery_handler = xTaskGetCurrentTaskHandle();

    for (;;)
    {
        /* restart control */
        if (rc->rc.ch[RC_CH_LV] < (RC_ROCKER_MIN / 3))
        {
            serial->c = 0;
        }
        else if (rc->rc.ch[RC_CH_LV] < (RC_ROCKER_MAX / 3))
        {
            signal_off("a\n");
        }
        if (rc->rc.ch[RC_CH_S] > RC_ROCKER_MIN + RC_DEADLINE)
        {
            signal_off("b\n");
            jet_off();
        }
        if (rc->rc.ch[RC_CH_S] < RC_ROCKER_MAX - RC_DEADLINE)
        {
            signal_off("c\n");
            if (READ_BIT(archery.signal, ARCHERY_SIGNAL_SHOOT))
            {
                CLEAR_BIT(archery.signal, ARCHERY_SIGNAL_SHOOT);
            }
        }

        /* remote control */
        if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* Start sending signaling signal */
            if (rc->rc.ch[RC_CH_LV] > (RC_ROCKER_MAX / 3))
            {
                signal_on("a\n");
            }

            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < RC_ROCKER_MIN + RC_DEADLINE)
            {
                /* It starts to spew out gas */
                signal_on("b\n");
            }
            else if (rc->rc.ch[RC_CH_S] > RC_ROCKER_MAX - RC_DEADLINE)
            {
                /* It starts to spew out gas */
                signal_on("c\n");
            }
        }

        if (switch_is_down(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* take arrow */
            if (rc->rc.ch[RC_CH_S] < RC_ROCKER_MIN + RC_DEADLINE)
            {
                archery_arrow();
            }
            else if (rc->rc.ch[RC_CH_S] > RC_ROCKER_MAX - RC_DEADLINE)
            {
                if (!READ_BIT(archery.signal, ARCHERY_SIGNAL_SHOOT))
                {
                    SET_BIT(archery.signal, ARCHERY_SIGNAL_SHOOT);

                    archery.angle = 60;
                    archery_shoot();
                }
            }
        }

        switch (serial->c)
        {
        case 'D':
        {
            serial->c = 0;
            archery.angle = serial->x;
            archery_shoot();
        }
        break;

        default:
        {
        }
        break;
        }

        archery_update();
        osDelay(ARCHERY_CONTROL_TIME_MS);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
