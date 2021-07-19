/**
 * *****************************************************************************
 * @file         app_shoot.h
 * @brief        shoot application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SHOOT_H__
#define __APP_SHOOT_H__

#include "app.h"

#define SHOOT_FLAG_RESET (0)
#define SHOOT_FLAG_SHOOT (1 << 0)

extern void shoot_reday(void);

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __APP_SHOOT_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
