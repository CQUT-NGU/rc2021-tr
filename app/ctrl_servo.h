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

#define SERVO_MATCH_FETCH   (1 << 0)  //!< servo match state for fetch
#define SERVO_MATCH_PITCH   (1 << 1)  //!< servo match state for pitch in the middle
#define SERVO_MATCH_PITCHL  (1 << 2)  //!< servo match state for pitch on the left
#define SERVO_MATCH_PITCHR  (1 << 3)  //!< servo match state for pitch on the right
#define SERVO_MATCH_SHIFTV  (1 << 4)  //!< servo match state for vertical shift in the middle
#define SERVO_MATCH_SHIFTVL (1 << 5)  //!< servo match state for vertical shift on the left
#define SERVO_MATCH_SHIFTVR (1 << 6)  //!< servo match state for vertical shift on the right

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

extern void fetch_update(void);
extern void pitch_update(void);
extern void shiftv_update(void);

__END_DECLS

__STATIC_INLINE
void fetch_set(uint32_t pwm)
{
    servo.fetch_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_FETCH);
}

__STATIC_INLINE
void pitch_set(uint32_t pwm)
{
    servo.pitch_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_PITCH);
}

__STATIC_INLINE
void pitchl_set(uint32_t pwm)
{
    servo.pitchr_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_PITCHL);
}

__STATIC_INLINE
void pitchr_set(uint32_t pwm)
{
    servo.pitchr_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_PITCHR);
}

__STATIC_INLINE
void shiftv_set(uint32_t pwm)
{
    servo.shiftv_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTV);
}

__STATIC_INLINE
void shiftvl_set(uint32_t pwm)
{
    servo.shiftvl_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTVL);
}

__STATIC_INLINE
void shiftvr_set(uint32_t pwm)
{
    servo.shiftvr_set = pwm;
    CLEAR_BIT(servo.match, SERVO_MATCH_SHIFTVR);
}

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_SERVO_H__ */

/************************ (C) COPYRIGHT TQFX *******************END OF FILE****/
