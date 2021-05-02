/**
 * *****************************************************************************
 * @file         task_step.c/h
 * @brief        stepping motor control task
 * @author       ngu
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_step.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "ca.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#include <stdint.h>

extern TIM_HandleTypeDef htim8;

/* Private define ------------------------------------------------------------*/

#define SHIFTH_TIM htim8

#define SHIFTH_CHANNEL TIM_CHANNEL_4

#define FLAG_RUN_SHIFTH (1U << 0U)

#define LIMIT_LOW_SHIFTH 100

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t step_flag_run = 0x00U;

static ca_lpf_f32_t lpf;

/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

static void shifth_set(uint32_t hz)
{
    uint16_t x = (uint16_t)ca_sqrt_u32(SystemCoreClock / hz);

    __HAL_TIM_SET_PRESCALER(&SHIFTH_TIM, x - 1U);
    __HAL_TIM_SetAutoreload(&SHIFTH_TIM, x - 1U);
    __HAL_TIM_SetCompare(&SHIFTH_TIM, SHIFTH_CHANNEL, (x >> 1U));
}

static void step_init(void)
{
    HAL_TIM_Base_Start(&SHIFTH_TIM);
}

static void shifth_update(const ctrl_rc_t *rc)
{
    if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
        switch_is_down(rc->rc.s[RC_SW_R]))
    {
        int16_t fr = rc->rc.ch[RC_CH_LV] << 1U;

        if (fr > LIMIT_LOW_SHIFTH)
        {
            gpio_pin_set(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        }
        else if (fr < -LIMIT_LOW_SHIFTH)
        {
            gpio_pin_reset(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
            fr = -fr;
        }

        fr = ca_lpf_f32(&lpf, fr);

        if (fr > LIMIT_LOW_SHIFTH)
        {
            shifth_set(fr);

            if (!(step_flag_run & FLAG_RUN_SHIFTH))
            {
                SET_BIT(step_flag_run, FLAG_RUN_SHIFTH);
                HAL_TIM_PWM_Start(&SHIFTH_TIM, SHIFTH_CHANNEL);
            }
        }
        else
        {
            if (step_flag_run & FLAG_RUN_SHIFTH)
            {
                CLEAR_BIT(step_flag_run, FLAG_RUN_SHIFTH);
                HAL_TIM_PWM_Stop(&SHIFTH_TIM, SHIFTH_CHANNEL);
            }
        }

        /* relay control */
        if (rc->rc.ch[RC_CH_S] > 440)
        {
            gpio_pin_set(RELAY_GPIO_Port, RELAY_Pin);
        }
        else
        {
            gpio_pin_reset(RELAY_GPIO_Port, RELAY_Pin);
        }
    }
}

void task_step(void *pvParameters)
{
    const ctrl_rc_t *rc = ctrl_rc_point();

    step_init();
    ca_lpf_f32_init(&lpf, 0.1, 0.002F);

    osDelay(1000);

    while (1)
    {
        shifth_update(rc);

        osDelay(2U);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
