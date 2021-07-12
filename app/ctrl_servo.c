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

void servo_init(void)
{
    __HAL_TIM_SET_PRESCALER(&MIDDLE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&MIDDLE_TIM, SERVO_PWM_MAX - 1);
    HAL_TIM_Base_Start(&MIDDLE_TIM);

    __HAL_TIM_SET_PRESCALER(&BESIDE_TIM, SERVO_PSC - 1);
    __HAL_TIM_SetAutoreload(&BESIDE_TIM, SERVO_PWM_MAX - 1);
    HAL_TIM_Base_Start(&BESIDE_TIM);
}

void servo_start(void)
{
    HAL_TIM_PWM_Start(&MIDDLE_TIM, FETCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, PITCH_CHANNEL);
    HAL_TIM_PWM_Start(&MIDDLE_TIM, SHIFTV_CHANNEL);

    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_L_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_L_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, PITCH_R_CHANNEL);
    HAL_TIM_PWM_Start(&BESIDE_TIM, SHIFTV_R_CHANNEL);
}

void fetch_init(uint32_t pwm)
{
    servo.fetch_set = pwm;
    servo.fetch = servo.fetch_set - 1;
}

void pitch_init(uint32_t pwm)
{
    servo.pitch_set = pwm;
    servo.pitch = servo.pitch_set - 1;
}

void pitchl_init(uint32_t pwm)
{
    servo.pitchl_set = pwm;
    servo.pitchl = servo.pitchl_set - 1;
}

void pitchr_init(uint32_t pwm)
{
    servo.pitchr_set = pwm;
    servo.pitchr = servo.pitchr_set - 1;
}

void fetch_set(uint32_t pwm)
{
    servo.fetch_set = pwm;
}

void pitch_set(uint32_t pwm)
{
    servo.pitch_set = pwm;
}

void pitchl_set(uint32_t pwm)
{
    servo.pitchr_set = pwm;
}

void pitchr_set(uint32_t pwm)
{
    servo.pitchr_set = pwm;
}

void shiftv_set(uint32_t pwm)
{
    __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, SHIFTV_CHANNEL, pwm);
}

void shiftvl_set(uint32_t pwm)
{
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, SHIFTV_L_CHANNEL, pwm);
}

void shiftvr_set(uint32_t pwm)
{
    __HAL_TIM_SET_COMPARE(&BESIDE_TIM, SHIFTV_R_CHANNEL, pwm);
}

void fetch_update(void)
{
    int delta = (int)(servo.fetch - servo.fetch_set);
    if (delta < 0)
    {
        servo.fetch++;
    }
    else if (delta > 0)
    {
        servo.fetch--;
    }
    if (delta)
    {
        __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, FETCH_CHANNEL, servo.fetch);
    }
}

void pitch_update(void)
{
    int delta = (int)(servo.pitch - servo.pitch_set);
    if (delta < 0)
    {
        servo.pitch++;
    }
    else if (delta > 0)
    {
        servo.pitch--;
    }
    if (delta)
    {
        __HAL_TIM_SET_COMPARE(&MIDDLE_TIM, PITCH_CHANNEL, servo.pitch);
    }
}

void pitchl_update(void)
{
    int delta = (int)(servo.pitchl - servo.pitchl_set);
    if (delta < 0)
    {
        servo.pitchl++;
    }
    else if (delta > 0)
    {
        servo.pitchl--;
    }
    if (delta)
    {
        __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCH_L_CHANNEL, servo.pitchl);
    }
}

void pitchr_update(void)
{
    int delta = (int)(servo.pitchr - servo.pitchr_set);
    if (delta < 0)
    {
        servo.pitchr++;
    }
    else if (delta > 0)
    {
        servo.pitchr--;
    }
    if (delta)
    {
        __HAL_TIM_SET_COMPARE(&BESIDE_TIM, PITCH_R_CHANNEL, servo.pitchr);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
