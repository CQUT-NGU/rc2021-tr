/**
 * *****************************************************************************
 * @file         task_servo.c/h
 * @brief        steering gear control
 * @author       tqfx
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_servo.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "ca.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

extern TIM_HandleTypeDef htim4;

/* Private define ------------------------------------------------------------*/

#define SERVO_PSC     90U
#define SERVO_PWM_MAX 20000U
#define SERVO_PWM_MID 1500U

#define SERVO_TIM      htim4
#define GETL_CHANNEL   TIM_CHANNEL_1
#define GETR_CHANNEL   TIM_CHANNEL_2
#define SHIFTV_CHANNEL TIM_CHANNEL_3
#define PITCH_CHANNEL  TIM_CHANNEL_4

#define FLAG_RUN_GETL   (1U << 0U)
#define FLAG_RUN_GETR   (1U << 1U)
#define FLAG_RUN_SHIFTV (1U << 2U)
#define FLAG_RUN_PITCH  (1U << 3U)

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//static uint8_t flag_servo_run = 0x00U;

/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

static void servo_init(void)
{
    __HAL_TIM_SET_PRESCALER(&SERVO_TIM, SERVO_PSC - 1U);
    __HAL_TIM_SetAutoreload(&SERVO_TIM, SERVO_PWM_MAX - 1U);
    HAL_TIM_Base_Start(&SERVO_TIM);
}

static void servo_start(void)
{
    HAL_TIM_PWM_Start(&SERVO_TIM, GETL_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, GETR_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, SHIFTV_CHANNEL);
    HAL_TIM_PWM_Start(&SERVO_TIM, PITCH_CHANNEL);
}

static inline void getl_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, pwm);
}

static inline void getr_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, pwm);
}

static inline void get_set(int16_t x)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, GETL_CHANNEL, 1500 - x);
    __HAL_TIM_SetCompare(&SERVO_TIM, GETR_CHANNEL, 1500 + x);
}

static inline void shiftv_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, SHIFTV_CHANNEL, pwm);
}

static inline void pitch_set(uint16_t pwm)
{
    __HAL_TIM_SetCompare(&SERVO_TIM, PITCH_CHANNEL, pwm);
}

void task_servo(void *pvParameters)
{
    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_pc_t *pc = ctrl_pc_point();

    ca_lpf_f32_t lpf;

    ca_lpf_f32_init(&lpf, 0.7F, 0.002F);

    osDelay(999);

    servo_init();
    servo_start();

    get_set(0);
    pitch_set(1280);
    shiftv_set(1000);

    int16_t pwm_get_set = 0;

    int16_t pwm_get = 0;

    while (1)
    {
        switch (pc->c)
        {
        case 'a':
        {
            pwm_get_set = (int16_t)pc->x;
            pitch_set((int16_t)pc->y);
            shiftv_set((int16_t)pc->z);

            int16_t delta = pwm_get + pwm_get_set;
            if (delta < 0)
            {
                pwm_get++;
                get_set(pwm_get);
            }
            else if (delta > 0)
            {
                pwm_get--;
                get_set(pwm_get);
            }
            os_printf("%i\r\n", pwm_get);

            break;
        }

        default:
            break;
        }

        if (switch_is_down(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            if (rc->rc.ch[RC_CH_LV] < -650)
            {
                if (rc->rc.ch[RC_CH_S] < -550)
                {
                    gpio_pin_reset(POWER3_GPIO_Port, POWER3_Pin);
                }
                else if (rc->rc.ch[RC_CH_S] < -110)
                {
                    shiftv_set(2300);
                }

                if (rc->rc.ch[RC_CH_S] > 110 ||
                    rc->rc.ch[RC_CH_RV] > 220)
                {
                    shiftv_set(1000);
                }

                if (rc->rc.ch[RC_CH_RV] > 10)
                {
                    pitch_set(1280 - rc->rc.ch[RC_CH_RV]);
                }
                else if (rc->rc.ch[RC_CH_RV] < -440)
                {
                    pwm_get_set = 750;
                }
                else if (rc->rc.ch[RC_CH_RV] < -10)
                {
                    gpio_pin_set(POWER3_GPIO_Port, POWER3_Pin);
                }
            }
            else if (rc->rc.ch[RC_CH_LV] > 650)
            {
                pwm_get_set = 0;
            }
            else
            {
                /* relay control */
                if (rc->rc.ch[RC_CH_S] < -440)
                {
                    gpio_pin_set(POWER1_GPIO_Port, POWER1_Pin);
                    gpio_pin_set(RELAY_GPIO_Port, RELAY_Pin);
                }
                else
                {
                    gpio_pin_reset(POWER1_GPIO_Port, POWER1_Pin);
                    gpio_pin_reset(RELAY_GPIO_Port, RELAY_Pin);
                }
            }
        }

        int16_t delta = pwm_get - pwm_get_set;
        if (delta < 0)
        {
            pwm_get++;
            get_set(pwm_get);
        }
        else if (delta > 0)
        {
            pwm_get--;
            get_set(pwm_get);
        }
        osDelay(4);
    }
}

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
