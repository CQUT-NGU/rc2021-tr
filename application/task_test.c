/**
 * *****************************************************************************
 * @file         task_test.c
 * @brief        test task
 * @author       NGU
 * @date         20210101
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_test.h"

#include "bsp.h"
#include "bsp_delay.h"
#include "cmsis_os.h"
#include "ctrl.h"

#include <stdint.h>

extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim5;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5)
    {
        static uint8_t flagStop[4] = {0};

        if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC4) != RESET)
        {
            if (__HAL_TIM_GET_IT_SOURCE(&htim8, TIM_IT_CC4) != RESET)
            {
                __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_CC4);

                if (HAL_TIM_PWM_Stop_IT(&htim8, TIM_CHANNEL_4) == HAL_OK)
                {
                    HAL_TIM_OC_Stop_IT(&htim5, TIM_CHANNEL_4);
                    flagStop[3] = 1;
                }
            }
        }

        if (flagStop[3])
        {
            flagStop[3] = 0;

            __HAL_TIM_SET_COUNTER(&htim5, 0);
        }
    }
}

void task_test(void *pvParameters)
{
    (void)pvParameters;

    TIM_SET_CAPTUREPOLARITY(&htim8, TIM_CHANNEL_4, TIM_ICPOLARITY_RISING);

    ctrl_pc_t *ctrl = ctrl_pc_point();

    for (;;)
    {
        if (ctrl->c == 't')
        {
            ctrl->c = 0;

            uint16_t count = 0;
            if (ctrl->y < 0)
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
                count = (uint16_t)(-ctrl->y);
            }
            else
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
                count = (uint16_t)ctrl->y;
            }

            __HAL_TIM_SET_PRESCALER(&htim8, (uint16_t)ctrl->x);
            __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, count);
            HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_4);
            HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_4);
        }
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
