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

#define FETCH_CHANNEL  TIM_CHANNEL_2
#define PITCH_CHANNEL  TIM_CHANNEL_3
#define SHIFTV_CHANNEL TIM_CHANNEL_4

#define PITCHL_CHANNEL  TIM_CHANNEL_1
#define SHIFTVL_CHANNEL TIM_CHANNEL_2
#define PITCHR_CHANNEL  TIM_CHANNEL_3
#define SHIFTVR_CHANNEL TIM_CHANNEL_4

ctrl_servo_t servo = {
    .match = SERVO_MATCH_FETCH | SERVO_MATCH_PITCH_ALL | SERVO_MATCH_SHIFTV_ALL,
};

#define SERVO_UPDATE(htim, ch, flag, ref, set, inc) \
    do                                              \
    {                                               \
        int delta = (int)((set) - (ref));           \
        if (delta >= (int)(inc))                    \
        {                                           \
            (ref) = (ref) + (inc);                  \
        }                                           \
        else if (delta <= -(int)(inc))              \
        {                                           \
            (ref) = (ref) - (inc);                  \
        }                                           \
        else                                        \
        {                                           \
            (ref) = (set);                          \
            delta = 0;                              \
        }                                           \
        if (delta)                                  \
        {                                           \
            CLEAR_BIT(servo.match, flag);           \
        }                                           \
        else                                        \
        {                                           \
            SET_BIT(servo.match, flag);             \
        }                                           \
        __HAL_TIM_SET_COMPARE(&htim, ch, (ref));    \
    } while (0)

void servo_init(void)
{
    __HAL_TIM_SET_PRESCALER(&MIDDLE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&MIDDLE_TIM, SERVO_PWM_MAX - 1);

    __HAL_TIM_SET_PRESCALER(&BESIDE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&BESIDE_TIM, SERVO_PWM_MAX - 1);
}

void servo_start(uint32_t pwm[7])
{
    HAL_TIM_PWM_Start(&MIDDLE_TIM, FETCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, PITCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, SHIFTV_CHANNEL);

    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCHL_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTVL_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCHR_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTVR_CHANNEL);

    fetch_set_pwm(pwm[0]);
    pitch_set_pwm(pwm[1]);
    shiftv_set_pwm(pwm[2]);

    pitchl_set_pwm(pwm[3]);
    shiftvl_set_pwm(pwm[4]);
    pitchr_set_pwm(pwm[5]);
    shiftvr_set_pwm(pwm[6]);
}

void fetch_set_pwm(uint32_t pwm)
{
    fetch_set(pwm);
    servo.fetch = pwm;
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, FETCH_CHANNEL, pwm);
}

void fetch_update(uint32_t inc)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 FETCH_CHANNEL,
                 SERVO_MATCH_FETCH,
                 servo.fetch,
                 servo.fetch_set,
                 inc);
}

void pitch_set_pwm(uint32_t pwm)
{
    pitch_set(pwm);
    servo.pitch = pwm;
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, PITCH_CHANNEL, pwm);
}

void pitchl_set_pwm(uint32_t pwm)
{
    pitchl_set(pwm);
    servo.pitchl = pwm;
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCHL_CHANNEL, pwm);
}

void pitchr_set_pwm(uint32_t pwm)
{
    pitchr_set(pwm);
    servo.pitchr = pwm;
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCHR_CHANNEL, pwm);
}

void pitch_update(uint32_t inc)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 PITCH_CHANNEL,
                 SERVO_MATCH_PITCH,
                 servo.pitch,
                 servo.pitch_set,
                 inc);
    SERVO_UPDATE(BESIDE_TIM,
                 PITCHL_CHANNEL,
                 SERVO_MATCH_PITCHL,
                 servo.pitchl,
                 servo.pitchl_set,
                 inc);
    SERVO_UPDATE(BESIDE_TIM,
                 PITCHR_CHANNEL,
                 SERVO_MATCH_PITCHR,
                 servo.pitchr,
                 servo.pitchr_set,
                 inc);
}

void shiftv_set_pwm(uint32_t pwm)
{
    shiftv_set(pwm);
    servo.shiftv = pwm;
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, SHIFTV_CHANNEL, pwm);
}

void shiftvl_set_pwm(uint32_t pwm)
{
    shiftvl_set(pwm);
    servo.shiftvl = pwm;
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, SHIFTVL_CHANNEL, pwm);
}

void shiftvr_set_pwm(uint32_t pwm)
{
    shiftvr_set(pwm);
    servo.shiftvr = pwm;
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, SHIFTVR_CHANNEL, pwm);
}

void shiftv_update(uint32_t inc)
{
    SERVO_UPDATE(MIDDLE_TIM,
                 SHIFTV_CHANNEL,
                 SERVO_MATCH_SHIFTV,
                 servo.shiftv,
                 servo.shiftv_set,
                 inc);
    SERVO_UPDATE(BESIDE_TIM,
                 SHIFTVL_CHANNEL,
                 SERVO_MATCH_SHIFTVL,
                 servo.shiftvl,
                 servo.shiftvl_set,
                 inc);
    SERVO_UPDATE(BESIDE_TIM,
                 SHIFTVR_CHANNEL,
                 SERVO_MATCH_SHIFTVR,
                 servo.shiftvr,
                 servo.shiftvr_set,
                 inc);
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
