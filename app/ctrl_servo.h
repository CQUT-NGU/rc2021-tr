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

#define SERVO_UPDATE_MS        2
#define SERVO_UPDATE_MS_PITCH  SERVO_UPDATE_MS
#define SERVO_UPDATE_MS_FETCH  SERVO_UPDATE_MS
#define SERVO_UPDATE_MS_SHIFTV 2

#define SERVO_PWMMID 1500

#define SERVO_FETCH_PWMMAX 1780
#define SERVO_FETCH_PWMMID 1660
#define SERVO_FETCH_PWMMIN 1050

#define SERVO_PITCH_PWMMID 1500
#define SERVO_PITCH_PWMMAX 2220
#define SERVO_PITCH_PWMMIN 1000

#define SERVO_PITCHL_PWMMID 1500
#define SERVO_PITCHL_PWMMAX 2150
#define SERVO_PITCHL_PWMMIN 1000

#define SERVO_PITCHR_PWMMID 1500
#define SERVO_PITCHR_PWMMAX 2000
#define SERVO_PITCHR_PWMMIN 1020

#define SERVO_SHIFTV_PWMMID 1500
#define SERVO_SHIFTV_PWMMAX 2200
#define SERVO_SHIFTV_PWMMIN 1000

#define SERVO_SHIFTVL_PWMMID 1500
#define SERVO_SHIFTVL_PWMMAX 2150
#define SERVO_SHIFTVL_PWMMIN 900

#define SERVO_SHIFTVR_PWMMID 1500
#define SERVO_SHIFTVR_PWMMAX 2100
#define SERVO_SHIFTVR_PWMMIN 900

#define SERVO_MATCH_FETCH   (1 << 0)  //!< servo match state for fetch
#define SERVO_MATCH_PITCH   (1 << 1)  //!< servo match state for pitch in the middle
#define SERVO_MATCH_PITCHL  (1 << 2)  //!< servo match state for pitch on the left
#define SERVO_MATCH_PITCHR  (1 << 3)  //!< servo match state for pitch on the right
#define SERVO_MATCH_SHIFTV  (1 << 4)  //!< servo match state for vertical shift in the middle
#define SERVO_MATCH_SHIFTVL (1 << 5)  //!< servo match state for vertical shift on the left
#define SERVO_MATCH_SHIFTVR (1 << 6)  //!< servo match state for vertical shift on the right

#define SERVO_MATCH_PITCH_ALL  (SERVO_MATCH_PITCH | SERVO_MATCH_PITCHL | SERVO_MATCH_PITCHR)
#define SERVO_MATCH_SHIFTV_ALL (SERVO_MATCH_SHIFTV | SERVO_MATCH_SHIFTVL | SERVO_MATCH_SHIFTVR)

typedef struct
{
    int match;
    uint32_t fetch;
    uint32_t fetch_set;
    uint32_t pitch;
    uint32_t pitch_set;
    uint32_t pitchl;
    uint32_t pitchl_set;
    uint32_t pitchr;
    uint32_t pitchr_set;
    uint32_t shiftv;
    uint32_t shiftv_set;
    uint32_t shiftvl;
    uint32_t shiftvl_set;
    uint32_t shiftvr;
    uint32_t shiftvr_set;
} ctrl_servo_t;

extern ctrl_servo_t servo;

__BEGIN_DECLS

extern void servo_init(void);

/**
 * @brief        Start generating PWM
 * @param[in]    pwm: An array of seven PWM values
 * @arg          0 fetch
 * @arg          1 pitch in the middle
 * @arg          2 vertical shift in the middle
 * @arg          3 pitch on the left
 * @arg          4 vertical shift on the left
 * @arg          5 pitch on the right
 * @arg          6 vertical shift on the right
*/
extern void servo_start(uint32_t pwm[7]);

extern void fetch_set_pwm(uint32_t pwm);
extern void pitch_set_pwm(uint32_t pwm);
extern void pitchl_set_pwm(uint32_t pwm);
extern void pitchr_set_pwm(uint32_t pwm);
extern void shiftv_set_pwm(uint32_t pwm);
extern void shiftvr_set_pwm(uint32_t pwm);
extern void shiftvl_set_pwm(uint32_t pwm);

extern void fetch_update(uint32_t inc);
extern void pitch_update(uint32_t inc);
extern void shiftv_update(uint32_t inc);

__END_DECLS

__STATIC_INLINE
void fetch_set(uint32_t pwm)
{
    if (servo.fetch != pwm)
    {
        servo.fetch_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_FETCH);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_FETCH);
    }
}

__STATIC_INLINE
void pitch_set(uint32_t pwm)
{
    if (servo.pitch != pwm)
    {
        servo.pitch_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_PITCH);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_PITCH);
    }
}

__STATIC_INLINE
void pitchl_set(uint32_t pwm)
{
    if (servo.pitchl != pwm)
    {
        servo.pitchl_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_PITCHL);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_PITCHL);
    }
}

__STATIC_INLINE
void pitchr_set(uint32_t pwm)
{
    if (servo.pitchr != pwm)
    {
        servo.pitchr_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_PITCHR);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_PITCHR);
    }
}

__STATIC_INLINE
void shiftv_set(uint32_t pwm)
{
    if (servo.shiftv != pwm)
    {
        servo.shiftv_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTV);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_SHIFTV);
    }
}

__STATIC_INLINE
void shiftvl_set(uint32_t pwm)
{
    if (servo.shiftvl != pwm)
    {
        servo.shiftvl_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTVL);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_SHIFTVL);
    }
}

__STATIC_INLINE
void shiftvr_set(uint32_t pwm)
{
    if (servo.shiftvr != pwm)
    {
        servo.shiftvr_set = pwm;
        CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTVR);
    }
    else
    {
        SET_BIT(servo.match, SERVO_MATCH_SHIFTVR);
    }
}

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_SERVO_H__ */

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
