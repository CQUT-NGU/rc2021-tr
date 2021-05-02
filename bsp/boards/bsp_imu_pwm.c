/**
 * *****************************************************************************
 * @file         bsp_imu_pwm.c/h
 * @brief        The IMU temperature is controlled by PWM
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bsp_imu_pwm.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#undef htim
#define htim htim3
#undef IMU_CHANNEL
#define IMU_CHANNEL TIM_CHANNEL_2

/* Private includes ----------------------------------------------------------*/
extern TIM_HandleTypeDef htim;

/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

void imu_pwm_start(void)
{
    /* Set the TIM Prescaler on runtime */
    __HAL_TIM_SET_PRESCALER(&htim, IMU_PWM_PSC - 1U);
    /* Set the TIM Autoreload Register value on runtime */
    __HAL_TIM_SetAutoreload(&htim, IMU_PWM_MAX - 1U);
    /* Set the TIM Clock Division value on runtime */
    __HAL_TIM_SetClockDivision(&htim, TIM_CLOCKDIVISION_DIV1);
    /* Starts the PWM signal generation */
    HAL_TIM_PWM_Start(&htim, IMU_CHANNEL);
}

void imu_pwm_stop(void)
{
    /* Stops the PWM signal generation */
    HAL_TIM_PWM_Stop(&htim, IMU_CHANNEL);
}

void imu_pwm_set(uint16_t pwm)
{
    /* Set the TIM Capture Compare Register value on runtime */
    __HAL_TIM_SetCompare(&htim, IMU_CHANNEL, pwm & IMU_PWM_MAX);
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
