/**
 * *****************************************************************************
 * @file         task_step.c/h
 * @brief        stepping motor control task
 * @author       ngu
 * @date         20210509
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
    if (switch_is_down(rc->rc.s[RC_SW_L]) &&
        switch_is_down(rc->rc.s[RC_SW_R]))
    {
        int16_t fr = rc->rc.ch[RC_CH_LH];

        if (fr > 10)
        {
            gpio_pin_set(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
        }
        else if (fr < -10)
        {
            gpio_pin_reset(SHIFTH_DIR_GPIO_Port, SHIFTH_DIR_Pin);
            fr = -fr;
        }

        if (-100 < rc->rc.ch[RC_CH_LV] &&
            rc->rc.ch[RC_CH_LV] < 100 &&
            fr > 100)
        {
            shifth_set((int16_t)ca_lpf_f32(&lpf, fr * 3));

            if (!(step_flag_run & FLAG_RUN_SHIFTH))
            {
                SET_BIT(step_flag_run, FLAG_RUN_SHIFTH);
                HAL_TIM_PWM_Start(&SHIFTH_TIM, SHIFTH_CHANNEL);
            }
        }
        else
        {
            ca_lpf_f32_reset(&lpf);
            if (step_flag_run & FLAG_RUN_SHIFTH)
            {
                CLEAR_BIT(step_flag_run, FLAG_RUN_SHIFTH);
                HAL_TIM_PWM_Stop(&SHIFTH_TIM, SHIFTH_CHANNEL);
            }
        }
    }
}

void task_step(void *pvParameters)
{
    (void)pvParameters;

    const ctrl_rc_t *rc = ctrl_rc_point();

    ca_lpf_f32_init(&lpf, 0.1F, 0.002F);

    step_init();

    osDelay(1000);

    while (1)
    {
        shifth_update(rc);

        osDelay(2U);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
