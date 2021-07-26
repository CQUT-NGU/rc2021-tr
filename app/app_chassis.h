/**
 * *****************************************************************************
 * @file         task_chassis.h
 * @brief        task chassis
 * @author       NGU
 * @date         20210509
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_CHASSIS_H__
#define __APP_CHASSIS_H__

#include "app.h"

typedef enum
{
    /* chassis will stop */
    CHASSIS_VECTOR_STOP,
    /* chassis will auto run */
    CHASSIS_VECTOR_AUTO,
    /* chassis will have rotation speed control slowly */
    CHASSIS_VECTOR_SLOW,
    /* chassis will have rotation speed control */
    CHASSIS_VECTOR_NORMAL,
    /* chassis will speed trajectory */
    CHASSIS_VECTOR_PATH,
} chassis_mode_e;

typedef struct
{
    const motor_t *fb; /* feedback */

    int16_t i;   /* current value */
    float v;     /* velocity */
    float v_set; /* velocity set-point */
    float accel; /* accelerated speed */
} chassis_motor_t;

typedef struct
{
    TickType_t tick;
    /* the point to serial */
    ctrl_serial_t *serial;
    /* the point to remote control */
    const ctrl_rc_t *rc;

    chassis_mode_e mode;   /* state machine */
    chassis_motor_t mo[4]; /* chassis motor data */

    ca_pid_f32_t pid_speed[4];  /* motor speed PID */
    ca_pid_f32_t pid_offset[3]; /* offset PID */

    /* chassis horizontal offset, positive means letf, unit m */
    float x;
    /* chassis vertical offset, positive means forward, unit m */
    float y;
    /* chassis rotation offset, positive means counterclockwise, unit rad */
    float z;

    /* chassis set horizontal offset, positive means letf, unit m */
    float x_set;
    /* chassis set vertical offset, positive means forward, unit m */
    float y_set;
    /* chassis set rotation offset, positive means counterclockwise, unit rad */
    float z_set;

    /* chassis horizontal speed, positive means letf, unit m/s */
    float vx;
    /* chassis vertical speed, positive means forward, unit m/s */
    float vy;
    /* chassis rotation speed, positive means counterclockwise, unit rad/s */
    float wz;

    /* chassis set horizontal speed, positive means left, unit m/s */
    float vx_set;
    /* chassis set vertical speed, positive means forward, unit m/s */
    float vy_set;
    /* chassis set rotation speed, positive means counterclockwise, unit rad/s */
    float wz_set;

    /* use first order filter to slow set-point */
    ca_lpf_f32_t vx_slow[1];
    /* use first order filter to slow set-point */
    ca_lpf_f32_t vy_slow[1];
    /* use first order filter to slow set-point */
    ca_lpf_f32_t wz_slow[1];

    ca_pid_f32_t pid_l1s[1];
    float yaw_set;

    float source[4]; /* source speed trajectory */
    float target[4]; /* target speed trajectory */

    polynomial5_t path[3]; /* speed trajectory */
} chassis_move_t;

extern chassis_move_t move;

extern void position_set(float x,
                         float tx,
                         float y,
                         float ty,
                         float z,
                         float tz);
extern void position_setx(float x, float t);
extern void position_sety(float y, float t);
extern void position_setz(float z, float t);
extern void position_update(void);
extern void position_updatex(void);
extern void position_updatey(void);
extern void position_updatez(void);
extern int32_t laser_set_wz(float scale);

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __APP_CHASSIS_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
