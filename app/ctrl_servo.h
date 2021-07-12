/**
 * *****************************************************************************
 * @file         ctrl_servo.h
 * @brief        steering gear control
 * @author       NGU
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CTRL_SERVO_H__
#define __CTRL_SERVO_H__

#include "ctrl.h"

typedef struct
{
    uint32_t fetch;
    uint32_t fetch_set;
    uint32_t pitch;
    uint32_t pitch_set;
    uint32_t pitchl;
    uint32_t pitchl_set;
    uint32_t pitchr;
    uint32_t pitchr_set;
} ctrl_servo_t;

extern ctrl_servo_t servo;

__BEGIN_DECLS

extern void servo_init(void);
extern void servo_start(void);

extern void shiftv_set(uint32_t pwm);
extern void shiftvl_set(uint32_t pwm);
extern void shiftvr_set(uint32_t pwm);

extern void fetch_init(uint32_t pwm);
extern void fetch_set(uint32_t pwm);
extern void fetch_update(void);

extern void pitch_init(uint32_t pwm);
extern void pitch_set(uint32_t pwm);
extern void pitch_update(void);

extern void pitchl_init(uint32_t pwm);
extern void pitchl_set(uint32_t pwm);
extern void pitchl_update(void);

extern void pitchr_init(uint32_t pwm);
extern void pitchr_set(uint32_t pwm);
extern void pitchr_update(void);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_SERVO_H__ */

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
