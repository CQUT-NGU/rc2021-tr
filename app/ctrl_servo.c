/**
 * *****************************************************************************
 * @file         ctrl_servo.h
 * @brief        steering gear control
 * @author       NGU
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "ctrl_servo.h"

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

ctrl_servo_t servo;

#define SERVO_UPDATE(htim, ch, flag, ref, set)       \
    do                                               \
    {                                                \
        int delta = (int)((set) - (ref));            \
        if (delta > 0)                               \
        {                                            \
            ++(ref);                                 \
        }                                            \
        else if (delta < 0)                          \
        {                                            \
            --(ref);                                 \
        }                                            \
        if (delta)                                   \
        {                                            \
            __HAL_TIM_SET_COMPARE(&htim, ch, (ref)); \
        }                                            \
        else                                         \
        {                                            \
            SET_BIT(servo.match, flag);              \
        }                                            \
    } while (0)

void servo_init(void)
{
    __HAL_TIM_SET_PRESCALER(&MIDDLE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&MIDDLE_TIM, SERVO_PWM_MAX - 1);
    HAL_TIM_Base_Start(&MIDDLE_TIM);

    __HAL_TIM_SET_PRESCALER(&BESIDE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&BESIDE_TIM, SERVO_PWM_MAX - 1);
    HAL_TIM_Base_Start(&BESIDE_TIM);
}

void servo_start(uint32_t pwm[7])
{
    HAL_TIM_PWM_Start(&MIDDLE_TIM, FETCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, PITCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, SHIFTV_CHANNEL);

    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_L_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_L_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_R_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_R_CHANNEL);

    servo.fetch_set = pwm[0];
    servo.fetch = servo.fetch_set - 1;
    servo.pitch_set = pwm[1];
    servo.pitch = servo.pitch_set - 1;
    servo.shiftv_set = pwm[2];
    servo.shiftv = servo.shiftv_set - 1;
    servo.pitchl_set = pwm[3];
    servo.pitchl = servo.pitchl_set - 1;
    servo.shiftvl_set = pwm[4];
    servo.shiftvl = servo.shiftv_set - 1;
    servo.pitchr_set = pwm[5];
    servo.pitchr = servo.pitchr_set - 1;
    servo.shiftvr_set = pwm[6];
    servo.shiftvr = servo.shiftvr_set - 1;
}

void fetch_update(void)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 FETCH_CHANNEL,
                 SERVO_MATCH_FETCH,
                 servo.fetch,
                 servo.fetch_set);
}

void pitch_update(void)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 PITCH_CHANNEL,
                 SERVO_MATCH_PITCH,
                 servo.pitch,
                 servo.pitch_set);
    SERVO_UPDATE(BESIDE_TIM,
                 PITCH_L_CHANNEL,
                 SERVO_MATCH_PITCHL,
                 servo.pitchl,
                 servo.pitchl_set);
    SERVO_UPDATE(BESIDE_TIM,
                 PITCH_R_CHANNEL,
                 SERVO_MATCH_PITCHR,
                 servo.pitchr,
                 servo.pitchr_set);
}

void shiftv_update(void)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 SHIFTV_CHANNEL,
                 SERVO_MATCH_SHIFTV,
                 servo.shiftv,
                 servo.shiftv_set);
    SERVO_UPDATE(BESIDE_TIM,
                 SHIFTV_L_CHANNEL,
                 SERVO_MATCH_SHIFTVL,
                 servo.shiftvl,
                 servo.shiftvl_set);
    SERVO_UPDATE(BESIDE_TIM,
                 SHIFTV_R_CHANNEL,
                 SERVO_MATCH_SHIFTVR,
                 servo.shiftvr,
                 servo.shiftvr_set);
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
