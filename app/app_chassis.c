/**
 * *****************************************************************************
 * @file         app_chassis.c
 * @brief        chassis
 * @author       NGU
 * @date         20210509
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_chassis.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

extern imu_t imu;

/* in the beginning of task ,wait a time */
#define CHASSIS_TASK_INIT_TIME 357

/* the channel num of controlling horizontal speed */
#define CHASSIS_X_CHANNEL 0
/* the channel num of controlling vertial speed */
#define CHASSIS_Y_CHANNEL 1
/* in some mode, can use remote control to control rotation speed */
#define CHASSIS_WZ_CHANNEL 2

/* rocker value (max 660) change to horizontal speed (m/s) */
#define CHASSIS_VX_RC_SEN 0.005F
/* rocker value (max 660) change to vertial speed (m/s) */
#define CHASSIS_VY_RC_SEN 0.005F
/* in following yaw angle mode, rocker value add to angle */
#define CHASSIS_ANGLE_Z_RC_SEN 0.000002F
/* in not following yaw angle mode, rocker value change to rotation speed */
#define CHASSIS_WZ_RC_SEN 0.01F
/* vertial speed slowly (dm/s) */
#define CHASSIS_RC_SLOW_SEN 0.1F

#define CHASSIS_ACCEL_X_NUM 0.2F
#define CHASSIS_ACCEL_Y_NUM 0.2F
#define CHASSIS_ACCEL_Z_NUM 0.2F

/* rocker value deadline */
#define CHASSIS_RC_DEADLINE 10

#define MOTOR_SPEED_TO_CHASSIS_SPEED_VX 0.25F
#define MOTOR_SPEED_TO_CHASSIS_SPEED_VY 0.25F
#define MOTOR_SPEED_TO_CHASSIS_SPEED_WZ 0.25F

/* a = 0.7 / 2, b = 0.7 / 2, a + b */
#define MOTOR_DISTANCE_TO_CENTER 0.7F

/* chassis task control time 2ms */
#define CHASSIS_CONTROL_TIME_MS 2
/* chassis task control time 0.002s */
#define CHASSIS_CONTROL_TIME 0.002F
/* chassis control frequence, no use now */
#define CHASSIS_CONTROL_FREQUENCE 500.0F
/* chassis 3508 max motor control current */
#define MAX_MOTOR_CAN_CURRENT 0x4000

/* chassi forward, back, left, right key */
#define CHASSIS_FRONT_KEY KEY_PRESSED_OFFSET_W
#define CHASSIS_BACK_KEY  KEY_PRESSED_OFFSET_S
#define CHASSIS_LEFT_KEY  KEY_PRESSED_OFFSET_A
#define CHASSIS_RIGHT_KEY KEY_PRESSED_OFFSET_D

/* M3508 rmp change to chassis speed */
#define M3508_MOTOR_RPM_TO_VECTOR       0.000415809748903494517209F
#define CHASSIS_MOTOR_RPM_TO_VECTOR_SEN M3508_MOTOR_RPM_TO_VECTOR

/* single chassis motor max speed */
#define MAX_WHEEL_SPEED 4.0F
/* chassis left or right max speed */
#define NORMAL_MAX_CHASSIS_SPEED_X 2.0F
/* chassis forward or back max speed */
#define NORMAL_MAX_CHASSIS_SPEED_Y 2.0F
/* chassis left or right max accelerated speed */
#define NORMAL_MAX_CHASSIS_ACC_X 3.0F
/* chassis forward or back max accelerated speed */
#define NORMAL_MAX_CHASSIS_ACC_Y 3.0F
/* chassis left or right max speed increment */
#define NORMAL_MAX_CHASSIS_SPEED_DELTA_X (CHASSIS_CONTROL_TIME * NORMAL_MAX_CHASSIS_ACC_X)
/* chassis forward or back max speed increment */
#define NORMAL_MAX_CHASSIS_SPEED_DELTA_Y (CHASSIS_CONTROL_TIME * NORMAL_MAX_CHASSIS_ACC_Y)

#define CHASSIS_WZ_SET_SCALE 0.1F

/* when chassis is not set to move, swing max angle */
#define SWING_NO_MOVE_ANGLE 0.7F
/* when chassis is set to move, swing max angle */
#define SWING_MOVE_ANGLE 0.31415926535897932384626433832795F

/* chassis motor speed PID */
#define M3505_MOTOR_SPEED_PID_KP       15000.0F
#define M3505_MOTOR_SPEED_PID_KI       10.0F
#define M3505_MOTOR_SPEED_PID_KD       0.0F
#define M3505_MOTOR_SPEED_PID_MAX_OUT  MAX_MOTOR_CAN_CURRENT
#define M3505_MOTOR_SPEED_PID_MAX_IOUT 2000.0F

/* chassis follow angle PID */
#define CHASSIS_FOLLOW_GIMBAL_PID_KP       1.0F
#define CHASSIS_FOLLOW_GIMBAL_PID_KI       0.0F
#define CHASSIS_FOLLOW_GIMBAL_PID_KD       0.0F
#define CHASSIS_FOLLOW_GIMBAL_PID_MAX_OUT  (float)M_PI
#define CHASSIS_FOLLOW_GIMBAL_PID_MAX_IOUT 0.1F

#undef LIMIT_RC
#define LIMIT_RC(x, max) ((x) > -(max) && ((x) < (max)) ? 0 : x)

static chassis_move_t move;

static void chassis_omni4(const float vx_set,
                          const float vy_set,
                          const float wz_set,
                          float wheel_speed[4]);

static void chassis_update(void);
static void chassis_rc(float *vx_set,
                       float *vy_set);

/**
 * @brief        chassis some measure data updata,
 *               such as motor speed, euler angle, robot speed
*/
static void chassis_update(void)
{
    for (uint8_t i = 0; i != 4; ++i)
    {
        /* update motor speed, accel is differential of speed PID */
        move.motor[i].v = move.motor[i].measure->v_rpm *
                          CHASSIS_MOTOR_RPM_TO_VECTOR_SEN;

        move.motor[i].accel = (move.pid_speed[i].x[1] - move.pid_speed[i].x[0]) *
                              CHASSIS_CONTROL_FREQUENCE;
    }

    /**
     * calculate horizontal speed, vertical speed, rotation speed,
     * right hand rule
    */
    move.vx = (move.motor[0].v +
               move.motor[1].v -
               move.motor[2].v -
               move.motor[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_VX;

    move.vy = (-move.motor[0].v +
               move.motor[1].v +
               move.motor[2].v -
               move.motor[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_VY;

    move.wz = (-move.motor[0].v -
               move.motor[1].v -
               move.motor[2].v -
               move.motor[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_WZ /
              MOTOR_DISTANCE_TO_CENTER;

    /**
     * calculate horizontal offset, vertical offset, rotation offset
    */
    move.x += move.vx * CHASSIS_CONTROL_TIME;
    move.y += move.vy * CHASSIS_CONTROL_TIME;
    move.z += move.wz * CHASSIS_CONTROL_TIME;

    /**
     * calculate chassis euler angle,
     * if chassis add a new gyro sensor,please change this code
    */
#if 1
    move.yaw = restrict_rad_f32(move.imu->yaw);
    // move.pitch = move.imu->pit;
    // move.roll = restrict_rad_f32(move.imu->rol);
#endif
}

/**
 * @brief        accroding to the channel value of remote control,
 *               calculate chassis horizontal and vertical speed set-point
 * @param[out]   vx_set: horizontal speed set-point
 * @param[out]   vy_set: vertical speed set-point
*/
static void chassis_rc(float *vx_set,
                       float *vy_set)
{
    /**
     * deadline, because some remote control need be calibrated,
     * the value of rocker is not zero in middle place
    */
    int16_t vx_channel = LIMIT_RC(move.rc->rc.ch[CHASSIS_X_CHANNEL],
                                  CHASSIS_RC_DEADLINE);

    int16_t vy_channel = LIMIT_RC(move.rc->rc.ch[CHASSIS_Y_CHANNEL],
                                  CHASSIS_RC_DEADLINE);

    *vx_set = vx_channel * CHASSIS_VX_RC_SEN;
    *vy_set = vy_channel * CHASSIS_VY_RC_SEN;

    /* keyboard set speed set-point */
    if (move.rc->key.v & CHASSIS_RIGHT_KEY)
    {
        *vx_set = move.vx_max;
    }
    else if (move.rc->key.v & CHASSIS_LEFT_KEY)
    {
        *vx_set = move.vx_min;
    }
    if (move.rc->key.v & CHASSIS_FRONT_KEY)
    {
        *vy_set = move.vy_max;
    }
    else if (move.rc->key.v & CHASSIS_BACK_KEY)
    {
        *vy_set = move.vy_min;
    }
}

/**
 * @brief        four omni4 wheels speed is calculated by three param.
 * @param[in]    vx_set: horizontal speed
 * @param[in]    vy_set: vertial speed
 * @param[in]    wz_set: rotation speed
 * @param[out]   wheel_speed: four omni4 wheels speed
*/
static void chassis_omni4(const float vx_set,
                          const float vy_set,
                          const float wz_set,
                          float wheel_speed[4])
{
    wheel_speed[0] = +vx_set - vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[1] = +vx_set + vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[2] = -vx_set + vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[3] = -vx_set - vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
}

void task_chassis(void *pvParameters)
{
    (void)pvParameters;

    can_filter_init();

    osDelay(CHASSIS_TASK_INIT_TIME);

    /* initialization */
    {
        /* chassis motor speed PID */
        const float kpid_v[3] = {
            M3505_MOTOR_SPEED_PID_KP,
            M3505_MOTOR_SPEED_PID_KI,
            M3505_MOTOR_SPEED_PID_KD,
        };

        /* chassis angle PID */
        const float kpid_yaw[3] = {
            CHASSIS_FOLLOW_GIMBAL_PID_KP,
            CHASSIS_FOLLOW_GIMBAL_PID_KI,
            CHASSIS_FOLLOW_GIMBAL_PID_KD,
        };

        const float kpid_p[3] = {
            4.0F,
            0.0F,
            0.0F,
        };

        const float lpf_x = {CHASSIS_ACCEL_X_NUM};
        const float lpf_y = {CHASSIS_ACCEL_Y_NUM};
        const float lpf_z = {CHASSIS_ACCEL_Z_NUM};

        /* in beginning， chassis mode is stop */
        move.mode = CHASSIS_VECTOR_STOP;

        /* get remote control point */
        move.rc = ctrl_rc_point();

        /* get serial control point */
        move.serial = ctrl_serial_point();

        /* get gyro sensor euler angle point */
        move.imu = &imu;

        /* get gimbal motor data point */

        /* get chassis motor data point, initialize motor speed PID */
        for (uint8_t i = 0; i != 4; ++i)
        {
            move.motor[i].measure = chassis_point(i);
            ca_pid_f32_position(&move.pid_speed[i],
                                kpid_v,
                                -M3505_MOTOR_SPEED_PID_MAX_OUT,
                                M3505_MOTOR_SPEED_PID_MAX_OUT,
                                M3505_MOTOR_SPEED_PID_MAX_IOUT);
        }

        /* initialize angle PID */
        ca_pid_f32_position(&move.pid_angle,
                            kpid_yaw,
                            -CHASSIS_FOLLOW_GIMBAL_PID_MAX_OUT,
                            CHASSIS_FOLLOW_GIMBAL_PID_MAX_OUT,
                            CHASSIS_FOLLOW_GIMBAL_PID_MAX_IOUT);

        ca_pid_f32_position(move.pid_offset,
                            kpid_p,
                            -NORMAL_MAX_CHASSIS_SPEED_X,
                            NORMAL_MAX_CHASSIS_SPEED_X,
                            0);
        ca_pid_f32_position(move.pid_offset + 1,
                            kpid_p,
                            -NORMAL_MAX_CHASSIS_SPEED_Y,
                            NORMAL_MAX_CHASSIS_SPEED_Y,
                            0);

        /* first order low-pass filter  replace ramp function */
        ca_lpf_f32_init(&move.vx_slow, lpf_x, CHASSIS_CONTROL_TIME);
        ca_lpf_f32_init(&move.vy_slow, lpf_y, CHASSIS_CONTROL_TIME);
        ca_lpf_f32_init(&move.wz_slow, lpf_z, CHASSIS_CONTROL_TIME);

        /* max and min speed */
        move.vx_max = NORMAL_MAX_CHASSIS_SPEED_X;
        move.vx_min = -NORMAL_MAX_CHASSIS_SPEED_X;

        move.vy_max = NORMAL_MAX_CHASSIS_SPEED_Y;
        move.vy_min = -NORMAL_MAX_CHASSIS_SPEED_Y;
    }

    /* update data */
    chassis_update();

    for (;;)
    {
        /* mode set */
        if (switch_is_mid(move.rc->rc.s[RC_SW_L]))
        {
            /* middle, up */
            if (switch_is_up(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_FOLLOW_CHASSIS_YAW;
            }
            /* middle, middle */
            else if (switch_is_mid(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_NO_FOLLOW_YAW;
            }
            /* middle, down */
            else if (switch_is_down(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_SLOW;
            }
        }
        else
        {
            move.mode = CHASSIS_VECTOR_STOP;
        }

        chassis_update();

        move.yaw_set = move.yaw;

        switch (move.mode)
        {
        case CHASSIS_VECTOR_STOP:
        {
            move.vx_set = 0;
            move.vy_set = 0;
            move.wz_set = 0;

            break;
        }

        case CHASSIS_VECTOR_SLOW:
        case CHASSIS_VECTOR_NO_FOLLOW_YAW:
        {
            chassis_rc(&move.vx_set, &move.vy_set);

            int16_t wz_channel = LIMIT_RC(move.rc->rc.ch[CHASSIS_WZ_CHANNEL],
                                          CHASSIS_RC_DEADLINE);

            move.wz_set = wz_channel * -CHASSIS_WZ_RC_SEN;

            if (move.mode == CHASSIS_VECTOR_SLOW)
            {
                /* in slow mode, the speed will decrease */
                move.vx_set *= CHASSIS_RC_SLOW_SEN;
                move.vy_set *= CHASSIS_RC_SLOW_SEN;
                move.wz_set *= CHASSIS_RC_SLOW_SEN;
            }
            else
            {
                /**
                 * first order low-pass replace ramp function,
                 * calculate chassis speed set-point to improve control performance
                */
                move.vx_set = ca_lpf_f32(&move.vx_slow, move.vx_set);
                move.vy_set = ca_lpf_f32(&move.vy_slow, move.vy_set);
                move.wz_set = ca_lpf_f32(&move.wz_slow, move.wz_set);
            }

            if (move.serial->c == 'V' || move.serial->c == 'v')
            {
                move.vx_set = move.serial->x;
                move.vy_set = move.serial->y;
                move.wz_set = move.serial->z;
            }

            break;
        }

        case CHASSIS_VECTOR_FOLLOW_CHASSIS_YAW:
        {
            chassis_rc(&move.vx_set, &move.vy_set);

            float delta_angle = move.rc->rc.ch[CHASSIS_WZ_CHANNEL] *
                              -CHASSIS_ANGLE_Z_RC_SEN;
            /* set chassis yaw angle set-point */
            move.yaw_set = restrict_rad_f32(move.yaw_set + delta_angle);
            delta_angle = restrict_rad_f32(move.yaw_set - move.yaw);
            /* calculate rotation speed */
            move.wz_set = ca_pid_f32(&move.pid_angle, 0, delta_angle);

            if (move.serial->c == 'P' || move.serial->c == 'p')
            {
                float tmp = move.serial->x - move.x;
                if (ABS(tmp) > 0.001F)
                {
                    move.vx_set = ca_pid_f32(move.pid_offset,
                                             move.x,
                                             move.serial->x);
                }
                /* left or right accelerated speed limit */
                float delta = move.vx_set - move.vx_set;
                if (delta > NORMAL_MAX_CHASSIS_SPEED_DELTA_X)
                {
                    move.vx_set = move.vx_set + NORMAL_MAX_CHASSIS_SPEED_DELTA_X;
                }
                else if (delta < -NORMAL_MAX_CHASSIS_SPEED_DELTA_X)
                {
                    move.vx_set = move.vx_set - NORMAL_MAX_CHASSIS_SPEED_DELTA_X;
                }

                tmp = move.serial->y - move.y;
                if (ABS(tmp) > 0.001F)
                {
                    move.vy_set = ca_pid_f32(move.pid_offset + 1,
                                             move.y,
                                             move.serial->y);
                }
                /* forward accelerated speed limit */
                delta = move.vy_set - move.vy_set;
                if (delta > NORMAL_MAX_CHASSIS_SPEED_DELTA_Y)
                {
                    move.vy_set = move.vy_set + NORMAL_MAX_CHASSIS_SPEED_DELTA_Y;
                }
                else if (delta < -NORMAL_MAX_CHASSIS_SPEED_DELTA_Y)
                {
                    move.vy_set = move.vy_set - NORMAL_MAX_CHASSIS_SPEED_DELTA_Y;
                }

                break;
            }

            break;
        }

        default:
            break;
        }

        /* control loop */
        {
            move.vx_set = LIMIT(move.vx_set, move.vx_min, move.vx_max);
            move.vy_set = LIMIT(move.vy_set, move.vy_min, move.vy_max);

            float wheel_speed[4];
            /* omni4 wheel speed calculation */
            chassis_omni4(move.vx_set,
                          move.vy_set,
                          move.wz_set,
                          wheel_speed);

            /* calculate the max speed in four wheels, limit the max speed */
            float vector_max = 0;
            for (uint8_t i = 0; i != 4; ++i)
            {
                move.motor[i].v_set = wheel_speed[i];

                float temp = ABS(move.motor[i].v_set);
                if (vector_max < temp)
                {
                    vector_max = temp;
                }
            }

            if (vector_max > MAX_WHEEL_SPEED)
            {
                float vector = MAX_WHEEL_SPEED / vector_max;
                for (uint8_t i = 0; i != 4; ++i)
                {
                    move.motor[i].v_set *= vector;
                }
            }

            /* calculate pid */
            for (uint8_t i = 0; i != 4; ++i)
            {
                move.motor[i].i_current = (int16_t)ca_pid_f32(&move.pid_speed[i],
                                                              move.motor[i].v,
                                                              move.motor[i].v_set);
            }
        }

        chassis_ctrl(move.motor[0].i_current,
                     move.motor[1].i_current,
                     move.motor[2].i_current,
                     move.motor[3].i_current);

#if 0
        os_justfloat(4,
                     move.motor[0].v,
                     move.motor[1].v,
                     move.motor[2].v,
                     move.motor[3].v);
#elif 0
        os_printf("p:%g,%g\n", move.x, move.y);
#endif

        osDelay(CHASSIS_CONTROL_TIME_MS);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/