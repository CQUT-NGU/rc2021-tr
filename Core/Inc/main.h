/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SHIFTH_DIR_Pin GPIO_PIN_7
#define SHIFTH_DIR_GPIO_Port GPIOI
#define SHIFTH_PWM_Pin GPIO_PIN_2
#define SHIFTH_PWM_GPIO_Port GPIOI
#define RELAY3_Pin GPIO_PIN_0
#define RELAY3_GPIO_Port GPIOI
#define POWER1_LU_Pin GPIO_PIN_2
#define POWER1_LU_GPIO_Port GPIOH
#define POWER2_LD_Pin GPIO_PIN_3
#define POWER2_LD_GPIO_Port GPIOH
#define POWER3_RU_Pin GPIO_PIN_4
#define POWER3_RU_GPIO_Port GPIOH
#define LED_G8_Pin GPIO_PIN_8
#define LED_G8_GPIO_Port GPIOG
#define POWER4_RD_Pin GPIO_PIN_5
#define POWER4_RD_GPIO_Port GPIOH
#define LED_G7_Pin GPIO_PIN_7
#define LED_G7_GPIO_Port GPIOG
#define LED_G6_Pin GPIO_PIN_6
#define LED_G6_GPIO_Port GPIOG
#define MPU_NSS_Pin GPIO_PIN_6
#define MPU_NSS_GPIO_Port GPIOF
#define RELAY2_Pin GPIO_PIN_12
#define RELAY2_GPIO_Port GPIOH
#define LED_G5_Pin GPIO_PIN_5
#define LED_G5_GPIO_Port GPIOG
#define LED_G4_Pin GPIO_PIN_4
#define LED_G4_GPIO_Port GPIOG
#define LED_G3_Pin GPIO_PIN_3
#define LED_G3_GPIO_Port GPIOG
#define RELAY1_Pin GPIO_PIN_11
#define RELAY1_GPIO_Port GPIOH
#define RELAY0_Pin GPIO_PIN_10
#define RELAY0_GPIO_Port GPIOH
#define SHIFTVR_Pin GPIO_PIN_15
#define SHIFTVR_GPIO_Port GPIOD
#define LED_G2_Pin GPIO_PIN_2
#define LED_G2_GPIO_Port GPIOG
#define LED_G1_Pin GPIO_PIN_1
#define LED_G1_GPIO_Port GPIOG
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOH
#define PITCHR_Pin GPIO_PIN_14
#define PITCHR_GPIO_Port GPIOD
#define SHIFTVL_Pin GPIO_PIN_13
#define SHIFTVL_GPIO_Port GPIOD
#define FETCH_Pin GPIO_PIN_1
#define FETCH_GPIO_Port GPIOA
#define PITCHL_Pin GPIO_PIN_12
#define PITCHL_GPIO_Port GPIOD
#define PITCH_Pin GPIO_PIN_2
#define PITCH_GPIO_Port GPIOA
#define LED_R_PWM_Pin GPIO_PIN_11
#define LED_R_PWM_GPIO_Port GPIOE
#define SHIFTV_Pin GPIO_PIN_3
#define SHIFTV_GPIO_Port GPIOA
#define LED_G9_Pin GPIO_PIN_14
#define LED_G9_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
