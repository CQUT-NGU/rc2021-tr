/**
 * *****************************************************************************
 * @file         bsp_buzzer.c/h
 * @brief        buzzer of boards
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * @details      TIM12_CH1(PH6) 2700Hz
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#undef __BEGIN_DECLS
#undef __END_DECLS

#if defined(__cplusplus)
#define __BEGIN_DECLS \
    extern "C"        \
    {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

/* Exported types ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

__BEGIN_DECLS

/**
 * @brief        Start buzzer pwm
*/
extern void buzzer_start(void);

/**
 * @brief        Stop buzzer pwm
*/
extern void buzzer_stop(void);

/**
 * @brief        Set pwm of the buzzer
 * @param[in]    psc: 0x0000 ~ 0xFFF
 * @param[in]    pwm: 0x0000 ~ 0x3FFF
*/
extern void buzzer_set(uint16_t psc,
                       uint16_t pwm);

__END_DECLS

/* Private defines -----------------------------------------------------------*/

/* __BSP_BUZZER_H__ ----------------------------------------------------------*/
#endif /* __BSP_BUZZER_H__ */

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
