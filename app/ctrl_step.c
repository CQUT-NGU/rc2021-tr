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

ctrl_step_t step;

void shifth_init(void)
{
    shifth_zero_cli(SHIFTH_INDEX_CLI);
}

uint32_t shifth_set_dir(int32_t offset)
{
    if (offset < 0)
    {
        gpio_pin_set(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        SET_BIT(step.flag, SHIFTH_FLAG_REVERSE);
        step.cnt = (uint32_t)-offset;
    }
    else
    {
        gpio_pin_reset(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        CLEAR_BIT(step.flag, SHIFTH_FLAG_REVERSE);
        step.cnt = (uint32_t)offset;
    }

    return step.cnt;
}

void shifth_set_freq(uint32_t hz)
{
    uint32_t x = (uint32_t)ca_sqrt_u32(SystemCoreClock / hz);

    __HAL_TIM_SET_PRESCALER(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SET_AUTORELOAD(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SET_COMPARE(&SHIFTH_TIM, SHIFTH_CHANNEL, (x >> 1));
}

void shifth_update(uint32_t inc, uint32_t cnt)
{
    if (READ_BIT(step.flag, SHIFTH_FLAG_AUTO))
    {
        step.cnt = __HAL_TIM_GET_COUNTER(&COUNT_TIM);
        if (step.cnt < cnt)
        {
            step.fr += inc;
            shifth_set_freq(step.fr);
        }
    }
}

void shifth_zero_cli(int32_t idx)
{
    if (!READ_BIT(step.flag, SHIFTH_FLAG_ZERO))
    {
        SET_BIT(step.flag, SHIFTH_FLAG_ZERO);

        shifth_start(idx);
    }
}

void shifth_index(uint32_t idx)
{
    step.set = idx;

    if (READ_BIT(step.flag, SHIFTH_FLAG_AUTO))
    {
        shifth_stop();
    }

    if (step.idx != step.set)
    {
        int32_t delta = (int32_t)(step.set - step.idx);
        shifth_start(delta);
    }
}

void shifth_start(int32_t offset)
{
    step.fr = 0;
    SET_BIT(step.flag, SHIFTH_FLAG_AUTO);

    __HAL_TIM_SET_COMPARE(&COUNT_TIM, SHIFTH_CHANNEL, shifth_set_dir(offset));
    __HAL_TIM_ENABLE_IT(&COUNT_TIM, SHIFTH_IT_CC);
    __HAL_TIM_ENABLE(&COUNT_TIM);

    HAL_TIM_PWM_Start(&SHIFTH_TIM, SHIFTH_CHANNEL);

    shifth_update(SHIFTH_PWM_DELTA, SHIFTH_PWM_DIVIDE);
}

void shifth_stop(void)
{
    HAL_TIM_PWM_Stop(&SHIFTH_TIM, SHIFTH_CHANNEL);

    __HAL_TIM_DISABLE(&COUNT_TIM);
    step.cnt = __HAL_TIM_GET_COUNTER(&COUNT_TIM);
    __HAL_TIM_SET_COUNTER(&COUNT_TIM, 0);
    if (READ_BIT(step.flag, SHIFTH_FLAG_REVERSE))
    {
        step.idx -= step.cnt;
    }
    else
    {
        step.idx += step.cnt;
    }
}

/**
  * @brief This function handles TIM global interrupt.
  */
void COUNT_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&COUNT_TIM, SHIFTH_IT_CC))
    {
        __HAL_TIM_CLEAR_FLAG(&COUNT_TIM, SHIFTH_IT_CC);
        __HAL_TIM_DISABLE_IT(&COUNT_TIM, SHIFTH_IT_CC);
    }

    if (READ_BIT(step.flag, SHIFTH_FLAG_AUTO))
    {
        CLEAR_BIT(step.flag, SHIFTH_FLAG_AUTO);

        shifth_stop();
    }

    if (READ_BIT(step.flag, SHIFTH_FLAG_ZERO))
    {
        CLEAR_BIT(step.flag, SHIFTH_FLAG_ZERO);

        shifth_stop();

        step.idx = 0;
        step.set = 0;

        shifth_index(SHIFTH_INDEX_MIDDLE);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
