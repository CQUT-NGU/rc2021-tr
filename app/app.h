/**
 * *****************************************************************************
 * @file         app.h
 * @brief        application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_H__
#define __APP_H__

#define NOTEFREQS_PROPORT ((180000000 >> 1) / BUZZER_PWM_MAX)
#define NOTEFREQS_TYPE    unsigned int

#include "ca.h"
#include "bsp.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#include <stdint.h>

#include "app_led.h"
#include "app_chassis.h"
#include "app_archery.h"
#include "app_arrow.h"
#include "app_shoot.h"

extern TaskHandle_t task_arrow_handler;
extern TaskHandle_t task_shoot_handler;

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __APP_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
