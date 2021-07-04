/**
 * *****************************************************************************
 * @file         bsp_buzzer.h
 * @brief        buzzer of boards
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * @details      TIM12_CH1(PH6) 2700Hz
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

#include "bsp.h"

#define BUZZER_PWM_MAX   ((180000000 >> 1) / 2700)
#define BUZZER_PWM_DIV1  (BUZZER_PWM_MAX >> 0)
#define BUZZER_PWM_DIV2  (BUZZER_PWM_MAX >> 1)
#define BUZZER_PWM_DIV4  (BUZZER_PWM_MAX >> 2)
#define BUZZER_PWM_DIV8  (BUZZER_PWM_MAX >> 3)
#define BUZZER_PWM_DIV16 (BUZZER_PWM_MAX >> 4)
#define BUZZER_PWM_DIV32 (BUZZER_PWM_MAX >> 5)
#define BUZZER_PWM_DIV64 (BUZZER_PWM_MAX >> 6)

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

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __BSP_BUZZER_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
