/**
 * *****************************************************************************
 * @file         ctrl_step.c
 * @brief        stepping motor control
 * @author       NGU
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "ctrl_step.h"

#include "ca.h"

#define SHIFTH_TIM       htim8
#define COUNT_TIM        htim5
#define COUNT_IRQHandler TIM5_IRQHandler

extern TIM_HandleTypeDef SHIFTH_TIM;
extern TIM_HandleTypeDef COUNT_TIM;

#define SHIFTH_IT_CC   TIM_IT_CC4
#define SHIFTH_CHANNEL TIM_CHANNEL_4

#define SHIFTH_PWM_DIVIDE 6400
#define SHIFTH_PWM_DELTA  500

ctrl_step_t step;

void shifth_init(void)
{
    __HAL_TIM_ENABLE_IT(&COUNT_TIM, SHIFTH_IT_CC);
}

void shifth_set(uint32_t hz)
{
    uint32_t x = (uint32_t)ca_sqrt_u32(SystemCoreClock / hz);

    __HAL_TIM_SET_PRESCALER(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SET_AUTORELOAD(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SetCompare(&SHIFTH_TIM, SHIFTH_CHANNEL, (x >> 1));
}

void shifth_update(void)
{
    if (READ_BIT(step.flag, SHIFTH_FLAG_RUN))
    {
        step.cnt = __HAL_TIM_GET_COUNTER(&COUNT_TIM);
        if (step.cnt < SHIFTH_PWM_DIVIDE)
        {
            step.fr += SHIFTH_PWM_DELTA;
            shifth_set(step.fr);
        }
    }
}

void shifth_index(uint32_t idx)
{
    if (READ_BIT(step.flag, SHIFTH_FLAG_RUN))
    {
        shifth_stop();
    }
    step.set = idx;
    if (step.idx != step.set)
    {
        shifth_start((int32_t)(step.set - step.idx));
    }
}

void shifth_start(int32_t count)
{
    if (count < 0)
    {
        gpio_pin_set(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        SET_BIT(step.flag, SHIFTH_FLAG_REVERSE);
        step.cnt = (uint32_t)-count;
    }
    else
    {
        gpio_pin_reset(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        CLEAR_BIT(step.flag, SHIFTH_FLAG_REVERSE);
        step.cnt = (uint32_t)count;
    }

    CLEAR_REG(step.fr);
    SET_BIT(step.flag, SHIFTH_FLAG_RUN);

    __HAL_TIM_SET_COMPARE(&COUNT_TIM, SHIFTH_CHANNEL, step.cnt);
    __HAL_TIM_ENABLE(&COUNT_TIM);

    HAL_TIM_PWM_Start(&SHIFTH_TIM, SHIFTH_CHANNEL);
    shifth_update();
}

void shifth_stop(void)
{
    step.cnt = __HAL_TIM_GET_COUNTER(&COUNT_TIM);
    __HAL_TIM_DISABLE(&COUNT_TIM);
    if (READ_BIT(step.flag, SHIFTH_FLAG_REVERSE))
    {
        step.idx -= step.cnt;
    }
    else
    {
        step.idx += step.cnt;
    }
    __HAL_TIM_SET_COUNTER(&COUNT_TIM, 0);
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void COUNT_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&COUNT_TIM, SHIFTH_IT_CC))
    {
        __HAL_TIM_CLEAR_FLAG(&COUNT_TIM, SHIFTH_IT_CC);

        HAL_TIM_PWM_Stop_IT(&SHIFTH_TIM, SHIFTH_CHANNEL);
    }

    if (READ_BIT(step.flag, SHIFTH_FLAG_RUN))
    {
        CLEAR_BIT(step.flag, SHIFTH_FLAG_RUN);

        shifth_stop();

        os_printf("idx:%u\r\n", step.idx);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
