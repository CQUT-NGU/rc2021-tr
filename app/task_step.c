/**
 * *****************************************************************************
 * @file         task_step.c
 * @brief        stepping motor control task
 * @author       NGU
 * @date         20210509
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_step.h"

#include "bsp.h"
#include "ca.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#include <stdint.h>

#define SHIFTH_TIM       htim8
#define COUNT_TIM        htim5
#define COUNT_IRQHandler TIM5_IRQHandler

extern TIM_HandleTypeDef SHIFTH_TIM;
extern TIM_HandleTypeDef COUNT_TIM;

#define SHIFTH_IT_CC   TIM_IT_CC4
#define SHIFTH_CHANNEL TIM_CHANNEL_4

#define FLAG_STOP_SHIFTH (1 << 0)

static int8_t step_flag = 0;

static void shifth_set(uint32_t hz)
{
    uint32_t x = (uint32_t)ca_sqrt_u32(SystemCoreClock / hz);

    __HAL_TIM_SET_PRESCALER(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SET_AUTORELOAD(&SHIFTH_TIM, x - 1);
    __HAL_TIM_SetCompare(&SHIFTH_TIM, SHIFTH_CHANNEL, (x >> 1));
}

void task_step(void *pvParameters)
{
    (void)pvParameters;

    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_serial_t *serial = ctrl_serial_point();

    osDelay(1000);

    {
        __HAL_TIM_ENABLE_IT(&COUNT_TIM, SHIFTH_IT_CC);
    }

    uint32_t cnt_set = 0;
    uint32_t fr_set = 0;

    for (;;)
    {
        if (serial->c == 'h')
        {
            serial->c = 0;

            uint32_t count = 0;
            if (serial->x < 0)
            {
                gpio_pin_set(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
                count = (uint32_t)(-serial->x);
            }
            else
            {
                gpio_pin_reset(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
                count = (uint32_t)serial->x;
            }

            __HAL_TIM_SET_COMPARE(&COUNT_TIM, SHIFTH_CHANNEL, count);
            __HAL_TIM_ENABLE(&COUNT_TIM);

            fr_set = 1000;
            HAL_TIM_PWM_Start(&SHIFTH_TIM, SHIFTH_CHANNEL);
        }

        uint32_t cnt = __HAL_TIM_GET_COUNTER(&COUNT_TIM);
        if (cnt < 6400)
        {
            fr_set = fr_set + 1000;
            shifth_set(fr_set);
        }

        osDelay(2);
    }
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void COUNT_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&COUNT_TIM, SHIFTH_IT_CC))
    {
        __HAL_TIM_CLEAR_FLAG(&COUNT_TIM, SHIFTH_IT_CC);

        SET_BIT(step_flag, FLAG_STOP_SHIFTH);

        HAL_TIM_PWM_Stop_IT(&SHIFTH_TIM, SHIFTH_CHANNEL);
    }

    if (READ_BIT(step_flag, FLAG_STOP_SHIFTH))
    {
        step_flag = 0;
        __HAL_TIM_DISABLE(&COUNT_TIM);
        __HAL_TIM_SET_COUNTER(&COUNT_TIM, 0);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
