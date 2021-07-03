/**
 * *****************************************************************************
 * @file         bsp_led.c
 * @brief        led of boards
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * @details      GPIO PIN
 *               LED_NUM_0 ------> PG1
 *               LED_NUM_1 ------> PG2
 *               LED_NUM_2 ------> PG3
 *               LED_NUM_3 ------> PG4
 *               LED_NUM_4 ------> PG5
 *               LED_NUM_5 ------> PG6
 *               LED_NUM_6 ------> PG7
 *               LED_NUM_7 ------> PG8
 *               LED_NUM_8 ------> PF14
 *               LED_NUM_9 ------> PE11
 * *****************************************************************************
*/

#include "bsp_led.h"

#include "main.h"

#if LED_PWM
#undef htim
#define htim htim1
#undef LED_CHANNEL
#define LED_CHANNEL TIM_CHANNEL_2
#endif /* LED_PWM */

#if LED_PWM
extern TIM_HandleTypeDef htim;
#endif /* LED_PWM */

#undef LED_W
#define LED_W(GPIOx, PIN, STATE)                \
    do                                          \
    {                                           \
        if (STATE == LED_OFF)                   \
        {                                       \
            GPIOx->BSRR = (uint32_t)PIN;        \
        }                                       \
        else                                    \
        {                                       \
            GPIOx->BSRR = (uint32_t)PIN << 16U; \
        }                                       \
    } while (0)

#undef LED_R
#define LED_R(GPIOx, PIN, STATE) \
    do                           \
    {                            \
        if (GPIOx->IDR & PIN)    \
        {                        \
            STATE = LED_OFF;     \
        }                        \
        else                     \
        {                        \
            STATE = LED_ON;      \
        }                        \
    } while (0)

#undef LED_T
#define LED_T(GPIOx, PIN)                       \
    do                                          \
    {                                           \
        if ((GPIOx->ODR & PIN) == PIN)          \
        {                                       \
            GPIOx->BSRR = (uint32_t)PIN << 16U; \
        }                                       \
        else                                    \
        {                                       \
            GPIOx->BSRR = (uint32_t)PIN;        \
        }                                       \
    } while (0)

void led_write(led_e pin,
               led_state_e state)
{
    switch (pin)
    {
    case LED_NUM_0:
    case LED_NUM_1:
    case LED_NUM_2:
    case LED_NUM_3:
    case LED_NUM_4:
    case LED_NUM_5:
    case LED_NUM_6:
    case LED_NUM_7:
    {
        LED_W(LED_PORT0, pin, state);
        break;
    }
    case LED_NUM_8:
    {
        LED_W(LED_PORT1, pin, state);
        break;
    }
#if !LED_PWM
    case LED_NUM_9:
    {
        LED_W(LED_PORT2, pin, state);
        break;
    }
#endif /* LED_PWM */
    default:
        break;
    }
}

led_state_e led_read(led_e pin)
{
    led_state_e state = LED_OFF;

    switch (pin)
    {
    case LED_NUM_0:
    case LED_NUM_1:
    case LED_NUM_2:
    case LED_NUM_3:
    case LED_NUM_4:
    case LED_NUM_5:
    case LED_NUM_6:
    case LED_NUM_7:
    {
        LED_R(LED_PORT0, pin, state);
        break;
    }
    case LED_NUM_8:
    {
        LED_R(LED_PORT1, pin, state);
        break;
    }
#if !LED_PWM
    case LED_NUM_9:
    {
        LED_R(LED_PORT2, pin, state);
        break;
    }
#endif /* LED_PWM */
    default:
        break;
    }

    return state;
}

void led_toggle(led_e pin)
{
    switch (pin)
    {
    case LED_NUM_0:
    case LED_NUM_1:
    case LED_NUM_2:
    case LED_NUM_3:
    case LED_NUM_4:
    case LED_NUM_5:
    case LED_NUM_6:
    case LED_NUM_7:
    {
        LED_T(LED_PORT0, pin);
        break;
    }
    case LED_NUM_8:
    {
        LED_T(LED_PORT1, pin);
        break;
    }
#if !LED_PWM
    case LED_NUM_9:
    {
        LED_T(LED_PORT2, pin);
        break;
    }
#endif /* LED_PWM */
    default:
        break;
    }
}

void led_line(uint8_t state)
{
    /* GPIO port bit set/reset register */
    LED_PORT0->BSRR = ((uint32_t)state << 1U) |
                      (~((uint32_t)state << 1U) << 16U);
}

void led_green(led_state_e state)
{
    LED_W(LED_PORT1, LED_NUM_8, state);
}

#if !LED_PWM

void led_red(led_state_e state)
{
    LED_W(LED_PORT2, LED_NUM_9, state);
}

#else

void led_pwm_start(void)
{
    /* Set the TIM Prescaler on runtime */
    __HAL_TIM_SET_PRESCALER(&htim, LED_PWM_PSC - 1U);
    /* Set the TIM Autoreload Register value on runtime */
    __HAL_TIM_SetAutoreload(&htim, LED_PWM_MAX - 1U);
    /* Set the TIM Clock Division value on runtime */
    __HAL_TIM_SetClockDivision(&htim, TIM_CLOCKDIVISION_DIV1);
    /* Starts the PWM signal generation */
    HAL_TIM_PWM_Start(&htim, LED_CHANNEL);
}

void led_pwm_stop(void)
{
    /* Stops the PWM signal generation */
    HAL_TIM_PWM_Stop(&htim, LED_CHANNEL);
}

void led_pwm_set(uint16_t value)
{
    /* Set the TIM Capture Compare Register value on runtime */
    __HAL_TIM_SetCompare(&htim, LED_CHANNEL, value);
}

#endif /* LED_PWM */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
