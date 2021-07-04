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
#ifndef __TASK_CHASSIS_H__
#define __TASK_CHASSIS_H__

#include "ca.h"
#include "bsp.h"
#include "ctrl.h"
#include "mpu6500.h"

typedef enum
{
    /* chassis will stop */
    CHASSIS_VECTOR_STOP,
    /* chassis will have yaw angle(chassis_yaw) close-looped control */
    CHASSIS_VECTOR_FOLLOW_CHASSIS_YAW,
    /* chassis will have rotation speed control */
    CHASSIS_VECTOR_NO_FOLLOW_YAW,
    /* chassis will have rotation speed control slowly */
    CHASSIS_VECTOR_SLOW,
} chassis_mode_e;

typedef struct
{
    const motor_t *measure;

    float accel; /* accelerated speed */
    float v;     /* velocity */
    float v_set; /* velocity set-point */

    int16_t i_current; /* current value */
} chassis_motor_t;

typedef struct
{
    /* the point to remote control */
    const ctrl_rc_t *data_rc;

    /* the point to pc */
    ctrl_pc_t *data_pc;

    /* the point to the euler angle of gyro sensor */
    imu_t *imu;

    chassis_mode_e mode;      /* state machine */
    chassis_motor_t motor[4]; /* chassis motor data */

    ca_pid_f32_t pid_offset[2]; /* offset PID */
    ca_pid_f32_t pid_speed[4];  /* motor speed PID */
    ca_pid_f32_t pid_angle;     /* follow angle PID */

    /* chassis horizontal offset, positive means letf,unit m/s */
    float x;
    /* chassis vertical offset, positive means forward,unit m/s */
    float y;

    /* use first order filter to slow set-point */
    ca_lpf_f32_t vx_slow;
    /* use first order filter to slow set-point */
    ca_lpf_f32_t vy_slow;
    /* use first order filter to slow set-point */
    ca_lpf_f32_t wz_slow;

    /* chassis horizontal speed, positive means letf,unit m/s */
    float vx;
    /* chassis vertical speed, positive means forward,unit m/s */
    float vy;
    /* chassis rotation speed, positive means counterclockwise,unit rad/s */
    float wz;

    /* chassis set horizontal speed,positive means left,unit m/s */
    float vx_set;
    /* chassis set vertical speed,positive means forward,unit m/s */
    float vy_set;
    /* chassis set rotation speed,positive means counterclockwise,unit rad/s */
    float wz_set;

    /* the relative angle between chassis and gimbal */
    float angle;
    /* the set relative angle */
    float angle_set;

    float yaw_set;

    float vx_max; /* max forward speed, unit m/s */
    float vx_min; /* max backward speed, unit m/s */
    float vy_max; /* max letf speed, unit m/s */
    float vy_min; /* max right speed, unit m/s */

    /* the yaw angle calculated by gyro sensor and gimbal motor */
    float yaw;
    /* the pitch angle calculated by gyro sensor and gimbal motor */
    float pitch;
    /* the roll angle calculated by gyro sensor and gimbal motor */
    float roll;

} chassis_move_t;

__BEGIN_DECLS

extern void task_chassis(void *pvParameters);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __TASK_CHASSIS_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
