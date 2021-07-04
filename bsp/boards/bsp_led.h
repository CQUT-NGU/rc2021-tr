/**
 * *****************************************************************************
 * @file         bsp_led.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "bsp.h"

#define LED_PWM 1

#ifndef LED_PWM
#define LED_PWM 0
#endif /* LED_PWM */

#define LED_PORT0 GPIOG /* line green */
#define LED_PORT1 GPIOF /* green */
#if !LED_PWM
#define LED_PORT2 GPIOE /* red */
#else
#define LED_PWM_PSC 1
#define LED_PWM_MAX 0x10000
#endif /* LED_PWM */

#undef __BEGIN_DECLS
#undef __END_DECLS

#if defined(__cplusplus)
#define __BEGIN_DECLS \
    extern "C"        \
    {
#define __END_DECLS \
    }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

typedef enum
{
    LED_ON = 0,        /* turn on */
    LED_OFF = !LED_ON, /* turn off */

} led_state_e;

typedef enum
{
    LED_NUM_0 = GPIO_PIN_1,  /* green 0 */
    LED_NUM_1 = GPIO_PIN_2,  /* green 1 */
    LED_NUM_2 = GPIO_PIN_3,  /* green 2 */
    LED_NUM_3 = GPIO_PIN_4,  /* green 3 */
    LED_NUM_4 = GPIO_PIN_5,  /* green 4 */
    LED_NUM_5 = GPIO_PIN_6,  /* green 5 */
    LED_NUM_6 = GPIO_PIN_7,  /* green 6 */
    LED_NUM_7 = GPIO_PIN_8,  /* green 7 */
    LED_NUM_8 = GPIO_PIN_14, /* green */
#if !LED_PWM

    LED_NUM_9 = GPIO_PIN_11, /* red */

#endif /* LED_PWM */
} led_e;

__BEGIN_DECLS

/**
 * @brief        Set the led on or off
 * @param[in]    pin:   led_e { LED_NUM_0 ~ LED_NUM_9 }
 * @param[in]    state: led_state_e { LED_ON LED_OFF }
*/
extern void led_write(led_e pin,
                      led_state_e state);

/**
 * @brief        Read the led state
 * @param[in]    pin: led_e { LED_NUM_0 ~ LED_NUM_9 }
 * @return       led_state_e { LED_ON LED_OFF }
*/
extern led_state_e led_read(led_e pin);

/**
 * @brief        Toggle the led
 * @param[in]    pin: led_e { LED_NUM_0 ~ LED_NUM_9 }
*/
extern void led_toggle(led_e pin);

/**
 * @brief        Set line green led
 * @param[in]    state: 0x00 ~ 0xFF 0(on) 1(off)
*/
extern void led_line(uint8_t state);

/**
 * @brief        Set green led
 * @param[in]    state: led_state_e { LED_ON LED_OFF }
*/
extern void led_green(led_state_e state);

#if !LED_PWM

/**
 * @brief        Set red led
 * @param[in]    state: led_state_e { LED_ON LED_OFF }
*/
extern void led_red(led_state_e state);

#else

/**
 * @brief        Start pwm of the red led
*/
extern void led_pwm_start(void);

/**
 * @brief        Stop pwm of the red led
*/
extern void led_pwm_stop(void);

/**
 * @brief        Set pwm of the red led
 * @param[in]    value: 0 ~ LED_PWM_MAX
*/
extern void led_pwm_set(uint16_t value);

#endif /* LED_PWM */

__END_DECLS

/* Enddef to prevent recursive inclusion ------------------------------------ */
#endif /* __BSP_LED_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
