/**
 * *****************************************************************************
 * @file         task_servo.c
 * @brief        steering gear control
 * @author       tqfx
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

extern TIM_HandleTypeDef htim4;

#define SERVO_PSC     90U
#define SERVO_PWM_MAX 20000U
#define SERVO_PWM_MID 1500U

#define SERVO_TIM      htim4
#define GETL_CHANNEL   TIM_CHANNEL_1
#define GETR_CHANNEL   TIM_CHANNEL_2
#define SHIFTV_CHANNEL TIM_CHANNEL_3
#define PITCH_CHANNEL  TIM_CHANNEL_4

#define FLAG_RUN_GETL   (1U << 0U)
#define FLAG_RUN_GETR   (1U << 1U)
#define FLAG_RUN_SHIFTV (1U << 2U)
#define FLAG_RUN_PITCH  (1U << 3U)

#define PWM_INIT_GET 0
#define PWM_LIFT_GET 100
#define PWM_LAST_GET 750

#define PWM_INIT_PITCH 1275

#define PWM_INIT_SHIFTV 1000
#define PWM_LAST_SHIFTV 2300

#define SHOOT_COUNT      50U
#define FLAG_SHOOT_START (1U << 0U)
#define FLAG_SHOOT_COUNT (1U << 1U)
#define FLAG_SHOOT_STOP  (1U << 2U)

static void servo_init(void)
{
    __HAL_TIM_SET_PRESCALER(&SERVO_TIM, SERVO_PSC - 1U);
    __HAL_TIM_SetAutoreload(&SERVO_TIM, SERVO_PWM_MAX - 1U);
    HAL_TIM_Base_Start(&SERVO_TIM);
}

static void servo_start(void)
{
    HAL_TIM_PWM_Start(&SERVO_TIM, GETL_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, GETR_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, SHIFTV_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, PITCH_CHANNEL);
}

static inline void getl_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, pwm);
}

static inline void getr_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, pwm);
}

static inline void get_set(int16_t x)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, 1500 - x);
    __HAL_TIM_SetCompare(&SERVO_TIM, GETR_CHANNEL, 1500 + x);
}

static inline void shiftv_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, SHIFTV_CHANNEL, pwm);
}

static inline void pitch_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, PITCH_CHANNEL, pwm);
}

void task_servo(void *pvParameters)
{
    (void)pvParameters;

    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_pc_t *pc = ctrl_pc_point();

    servo_init();
    servo_start();

    get_set(PWM_INIT_GET);
    pitch_set(PWM_INIT_PITCH);
    shiftv_set(PWM_INIT_SHIFTV);

    osDelay(500);

    int16_t pwm_get_set = 0;

    int16_t pwm_get = 0;

    uint8_t shoot_flag = 0x00U;
    uint8_t shoot_count = 0x00U;

    while (1)
    {
        switch (pc->c)
        {
        case 'a':
        {
            pwm_get_set = (int16_t)pc->x;
            pitch_set((int16_t)pc->y);
            shiftv_set((int16_t)pc->z);

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
                    gpio_pin_reset(POWER3_GPIO_Port, POWER3_Pin);
                }
                /**
                 * -550 ~ -110
                 * The steering gear is moved vertically forward and
                 * inserted into the tail of the arrow
                */
                else if (rc->rc.ch[RC_CH_S] < -110)
                {
                    shiftv_set(PWM_LAST_SHIFTV);
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
                    pitch_set(PWM_INIT_PITCH - rc->rc.ch[RC_CH_RV]);
                }
                /* Lift the arrow horizontally */
                else if (rc->rc.ch[RC_CH_RV] < -550)
                {
                    pwm_get_set = PWM_LAST_GET;
                }
                /* -440 ~ -10, Clip the arrow and lift it */
                else if (rc->rc.ch[RC_CH_RV] < -110)
                {
                    /* Clamp the arrow */
                    gpio_pin_set(POWER3_GPIO_Port, POWER3_Pin);
                    /* Lift it */
                    if (pwm_get_set == PWM_INIT_GET)
                    {
                        pwm_get_set = PWM_LIFT_GET;
                    }
                }
            }
            /* Pull lever up to maximum */
            else if (rc->rc.ch[RC_CH_LV] > 650)
            {
                /* Return to initial position */
                pwm_get_set = PWM_INIT_GET;
            }
            /* The tie rod is in the normal position */
            else
            {
                /* relay control, shoot an arrow */
                if (rc->rc.ch[RC_CH_S] < -650)
                {
                    /* It starts to spew out gas */
                    if (!READ_BIT(shoot_flag, FLAG_SHOOT_START))
                    {
                        SET_BIT(shoot_flag, FLAG_SHOOT_START);
                        SET_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                        gpio_pin_set(POWER1_GPIO_Port, POWER1_Pin);
                    }
                }
                else
                {
                    /* Stop ejecting gas */
                    if (READ_BIT(shoot_flag, FLAG_SHOOT_STOP))
                    {
                        CLEAR_BIT(shoot_flag, FLAG_SHOOT_STOP);
                        CLEAR_BIT(shoot_flag, FLAG_SHOOT_START);
                    }

                    gpio_pin_reset(POWER1_GPIO_Port, POWER1_Pin);
                }
            }
        }

        /**
         * Updated arrow steering gear Angle,
         * prevent the PWM from changing too much to
         * cause the steering gear to jam
        */
        int16_t delta = pwm_get - pwm_get_set;
        if (delta < 0)
        {
            pwm_get++;
        }
        else if (delta > 0)
        {
            pwm_get--;
        }
        if (delta)
        {
            get_set(pwm_get);
        }

        /* Control injection time */
        if (READ_BIT(shoot_flag, FLAG_SHOOT_COUNT))
        {
            if (shoot_count++ == SHOOT_COUNT)
            {
                shoot_count = 0U;
                CLEAR_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                gpio_pin_reset(POWER1_GPIO_Port, POWER1_Pin);
                SET_BIT(shoot_flag, FLAG_SHOOT_STOP);
            }
        }

        /* Task delay */
        osDelay(4);
    }
}

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
