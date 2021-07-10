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

#include "bsp.h"
#include "ca.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#define MIDDLE_TIM htim2
#define BESIDE_TIM htim4

extern TIM_HandleTypeDef MIDDLE_TIM;
extern TIM_HandleTypeDef BESIDE_TIM;

#define SERVO_PSC     90
#define SERVO_PWM_MAX 20000
#define SERVO_PWM_MID 1500

#define FETCH_CHANNEL  TIM_CHANNEL_2
#define PITCH_CHANNEL  TIM_CHANNEL_3
#define SHIFTV_CHANNEL TIM_CHANNEL_4

#define PITCH_L_CHANNEL  TIM_CHANNEL_1
#define SHIFTV_L_CHANNEL TIM_CHANNEL_2
#define PITCH_R_CHANNEL  TIM_CHANNEL_3
#define SHIFTV_R_CHANNEL TIM_CHANNEL_4

#define FLAG_RUN_GETL   (1 << 0)
#define FLAG_RUN_GETR   (1 << 1)
#define FLAG_RUN_SHIFTV (1 << 2)
#define FLAG_RUN_PITCH  (1 << 3)

#define PWM_INIT_PITCH 1275

#define PWM_INIT_SHIFTV 1000
#define PWM_LAST_SHIFTV 2300

__STATIC_INLINE
void fetch_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, FETCH_CHANNEL, pwm);
}

__STATIC_INLINE
void pitch_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, PITCH_CHANNEL, pwm);
}

__STATIC_INLINE
void pitchl_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCH_L_CHANNEL, pwm);
}

__STATIC_INLINE
void pitchr_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCH_R_CHANNEL, pwm);
}

__STATIC_INLINE
void shiftv_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, SHIFTV_CHANNEL, pwm);
}

__STATIC_INLINE
void shiftvl_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, SHIFTV_L_CHANNEL, pwm);
}

__STATIC_INLINE
void shiftvr_set(uint16_t pwm)
{
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, SHIFTV_R_CHANNEL, pwm);
}

void task_servo(void *pvParameters)
{
    (void)pvParameters;

    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_pc_t *pc = ctrl_pc_point();

    {
        __HAL_TIM_SET_PRESCALER(&MIDDLE_TIM, SERVO_PSC - 1);
        __HAL_TIM_SetAutoreload(&MIDDLE_TIM, SERVO_PWM_MAX - 1);
        HAL_TIM_Base_Start(&MIDDLE_TIM);

        __HAL_TIM_SET_PRESCALER(&BESIDE_TIM, SERVO_PSC - 1);
        __HAL_TIM_SetAutoreload(&BESIDE_TIM, SERVO_PWM_MAX - 1);
        HAL_TIM_Base_Start(&BESIDE_TIM);
    }

    {
        HAL_TIM_PWM_Start(&MIDDLE_TIM, FETCH_CHANNEL);
        HAL_TIM_PWM_Start(&MIDDLE_TIM, SHIFTV_CHANNEL);
        HAL_TIM_PWM_Start(&MIDDLE_TIM, PITCH_CHANNEL);

        HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_L_CHANNEL);
        HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_L_CHANNEL);
        HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_R_CHANNEL);
        HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_R_CHANNEL);
    }

    // get_set(PWM_INIT_GET);
    // pitch_set(PWM_INIT_PITCH);
    // shiftv_set(PWM_INIT_SHIFTV);

    osDelay(500);

    uint16_t pwm_fetch_set = 1500;
    uint16_t pwm_fetch = 0;

    while (1)
    {
        switch (pc->c)
        {
        case 'a':
        {
            pwm_fetch_set = (uint16_t)pc->x;
            pitch_set((uint16_t)pc->y);
            shiftv_set((uint16_t)pc->z);

            break;
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
        int32_t delta = pwm_fetch - pwm_fetch_set;
        if (delta < 0)
        {
            pwm_fetch++;
        }
        else if (delta > 0)
        {
            pwm_fetch--;
        }
        if (delta)
        {
            __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, FETCH_CHANNEL, pwm_fetch);
        }

        /* Task delay */
        osDelay(2);
    }
}

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
