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

#include <math.h>

/* in the beginning of task, wait a time */
#define CHASSIS_TASK_INIT_TIME 357
/* chassis task control time 2ms */
#define CHASSIS_CONTROL_TIME_MS 2
/* chassis task control time 0.002s */
#define CHASSIS_CONTROL_TIME (0.001F * CHASSIS_CONTROL_TIME_MS)
/* chassis control frequence */
#define CHASSIS_CONTROL_FREQUENCE (1000 / CHASSIS_CONTROL_TIME_MS)

/*!
 M3508 rmp change to chassis speed
 \f{aligned}
 V_{m/s} = K V_{rpm}, K = \cfrac {2 \pi r} {60 \cdot 19}, d = 2r = 0.1016 m
 \f}
*/
#define M3508_MOTOR_RPM_TO_VECTOR       0.00027998755579361663F
#define CHASSIS_MOTOR_RPM_TO_VECTOR_SEN M3508_MOTOR_RPM_TO_VECTOR
#define M3508_MOTOR_ECD_TO_DISTANCE     0.000002050690105910278F

#define MOTOR_SPEED_TO_CHASSIS_SPEED_VX (0.25F * (float)M_SQRT2)
#define MOTOR_SPEED_TO_CHASSIS_SPEED_VY (0.25F * (float)M_SQRT2)
#define MOTOR_SPEED_TO_CHASSIS_SPEED_WZ 0.25F
/* a = 0.7 / 2, b = 0.7 / 2, a + b */
#define MOTOR_DISTANCE_TO_CENTER 0.4949747468305833F

/* single chassis motor max speed */
#define MAX_WHEEL_SPEED 4.0F
/* chassis left or right max speed */
#define NORMAL_MAX_CHASSIS_SPEED_X 2.0F
/* chassis forward or back max speed */
#define NORMAL_MAX_CHASSIS_SPEED_Y 2.0F
/* chassis left or right max accelerated speed */
#define NORMAL_MAX_CHASSIS_ACC_X 4.0F
/* chassis forward or back max accelerated speed */
#define NORMAL_MAX_CHASSIS_ACC_Y 4.0F
/* chassis left or right max speed increment */
#define NORMAL_MAX_CHASSIS_SPEED_DELTA_X \
    (CHASSIS_CONTROL_TIME * NORMAL_MAX_CHASSIS_ACC_X)
/* chassis forward or back max speed increment */
#define NORMAL_MAX_CHASSIS_SPEED_DELTA_Y \
    (CHASSIS_CONTROL_TIME * NORMAL_MAX_CHASSIS_ACC_Y)

/* the channel num of controlling horizontal speed */
#define CHASSIS_X_CHANNEL RC_CH_RH
/* the channel num of controlling vertial speed */
#define CHASSIS_Y_CHANNEL RC_CH_RV
/* in some mode, can use remote control to control rotation speed */
#define CHASSIS_WZ_CHANNEL RC_CH_LH
/* rocker value deadline */
#define CHASSIS_RC_DEADLINE RC_DEADLINE
/* chassi forward, back, left, right key */
#define CHASSIS_FRONT_KEY KEY_PRESSED_OFFSET_W
#define CHASSIS_BACK_KEY  KEY_PRESSED_OFFSET_S
#define CHASSIS_LEFT_KEY  KEY_PRESSED_OFFSET_A
#define CHASSIS_RIGHT_KEY KEY_PRESSED_OFFSET_D

/* rocker value (max 660) change to horizontal speed (m/s) */
#define CHASSIS_VX_RC_SEN (NORMAL_MAX_CHASSIS_SPEED_X / RC_ROCKER_MAX)
/* rocker value (max 660) change to vertial speed (m/s) */
#define CHASSIS_VY_RC_SEN (NORMAL_MAX_CHASSIS_SPEED_Y / RC_ROCKER_MAX)
/* in not following yaw angle mode, rocker value change to rotation speed */
#define CHASSIS_WZ_RC_SEN (MAX_WHEEL_SPEED / RC_ROCKER_MAX)
/* vertial speed slowly (m/s) */
#define CHASSIS_RC_SLOW_SEN 0.2F

#define CHASSIS_ACCEL_X_NUM 0.2F
#define CHASSIS_ACCEL_Y_NUM 0.2F
#define CHASSIS_ACCEL_Z_NUM 0.2F

// /* in following yaw angle mode, rocker value add to angle */
// #define CHASSIS_ANGLE_Z_RC_SEN 0.000002F
// #define CHASSIS_WZ_SET_SCALE 0.1F
// /* when chassis is not set to move, swing max angle */
// #define SWING_NO_MOVE_ANGLE 0.7F
// /* when chassis is set to move, swing max angle */
// #define SWING_MOVE_ANGLE 0.31415926535897932384626433832795F

/* chassis motor speed PID */
#define M3505_MOTOR_SPEED_PID_KP       16000.0F
#define M3505_MOTOR_SPEED_PID_KI       10.0F
#define M3505_MOTOR_SPEED_PID_KD       0.0F
#define M3505_MOTOR_SPEED_PID_MAX_OUT  MAX_MOTOR_CAN_CURRENT
#define M3505_MOTOR_SPEED_PID_MAX_IOUT 2000.0F

#define LIMIT_RC(x, max) ((x) > -(max) && ((x) < (max)) ? 0 : x)

chassis_move_t move;

static void chassis_omni4(float wheel_speed[4],
                          float vx_set,
                          float vy_set,
                          float wz_set);

static void chassis_rc(void);
static void chassis_serial(void);

static void chassis_update(void);
static void chassis_loop(void);

/**
 * @brief        chassis some measure data update
*/
static void chassis_update(void)
{
    for (int i = 0; i != 4; ++i)
    {
        /* update motor speed, accel is differential of speed PID */
        float v = move.mo[i].fb->v_rpm * CHASSIS_MOTOR_RPM_TO_VECTOR_SEN;
        move.mo[i].accel = v - move.mo[i].v;
        move.mo[i].v = v;
        move.mo[i].accel *= CHASSIS_CONTROL_TIME;
    }

    /**
     * calculate horizontal speed, vertical speed, rotation speed,
     * right hand rule
    */
    move.vx = (move.mo[0].v + move.mo[1].v - move.mo[2].v - move.mo[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_VX;
    move.vy = (-move.mo[0].v + move.mo[1].v + move.mo[2].v - move.mo[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_VY;
    move.wz = (-move.mo[0].v - move.mo[1].v - move.mo[2].v - move.mo[3].v) *
              MOTOR_SPEED_TO_CHASSIS_SPEED_WZ / MOTOR_DISTANCE_TO_CENTER;

    /**
     * calculate horizontal offset, vertical offset, rotation offset
    */
    move.x += move.vx * CHASSIS_CONTROL_TIME;
    move.y += move.vy * CHASSIS_CONTROL_TIME;
    move.z += move.wz * CHASSIS_CONTROL_TIME;
}

void position_set(float tx,
                  float x,
                  float ty,
                  float y,
                  float tz,
                  float z)
{
    move.source[0] = 0.001F * (float)xTaskGetTickCount();
    move.target[0] = move.source[0] + tx;
    move.target[0] = move.source[0] + ty;
    move.target[0] = move.source[0] + tz;
    move.target[2] = 0;
    move.source[3] = 0;
    move.target[3] = 0;

    move.x_set = x;
    move.y_set = y;
    move.z_set = z;

    move.source[1] = move.x;
    move.target[1] = move.x_set;
    move.source[2] = move.vx;
    polynomial5_init(move.path + 0, move.source, move.target);
    move.path[0].t[1] = (move.path[0].t[0] + move.path[0].t[1]) * 0.5F;

    move.source[1] = move.y;
    move.target[1] = move.y_set;
    move.source[2] = move.vy;
    polynomial5_init(move.path + 1, move.source, move.target);
    move.path[1].t[1] = (move.path[1].t[0] + move.path[1].t[1]) * 0.5F;

    move.source[1] = move.z;
    move.target[1] = move.z_set;
    move.source[2] = move.wz;
    polynomial5_init(move.path + 2, move.source, move.target);
    move.path[2].t[1] = (move.path[2].t[0] + move.path[2].t[1]) * 0.5F;
}

void position_update(void)
{
    float t = 0.001F * (float)xTaskGetTickCount();
    if (t < move.path[0].t[1])
    {
        move.vx_set = polynomial5_vec(move.path + 0, t);
    }
    else
    {
        move.vx_set = ca_pid_f32(move.pid_offset + 0, move.x, move.x_set);
    }
    if (t < move.path[1].t[1])
    {
        move.vy_set = polynomial5_vec(move.path + 1, t);
    }
    else
    {
        move.vy_set = ca_pid_f32(move.pid_offset + 1, move.y, move.y_set);
    }
    if (t < move.path[2].t[1])
    {
        move.wz_set = polynomial5_vec(move.path + 2, t);
    }
    else
    {
        move.wz_set = ca_pid_f32(move.pid_offset + 2, move.z, move.z_set);
    }
}

void position_setx(float t, float x, float v)
{
    move.source[0] = 0.001F * (float)xTaskGetTickCount();
    move.target[0] = move.source[0] + t;
    move.source[3] = 0;
    move.target[3] = 0;

    move.x_set = x;
    move.source[1] = move.x;
    move.target[1] = move.x_set;
    move.source[2] = move.vx;
    move.target[2] = v;
    polynomial5_init(move.path + 0, move.source, move.target);
    move.path[0].t[1] = (move.path[0].t[0] + move.path[0].t[1]) * 0.5F;
}

void position_updatex(void)
{
    float t = 0.001F * (float)xTaskGetTickCount();
    if (t < move.path[0].t[1])
    {
        move.vx_set = polynomial5_vec(move.path + 0, t);
    }
    else
    {
        move.vx_set = ca_pid_f32(move.pid_offset + 0, move.x, move.x_set);
    }
}

void position_sety(float t, float y, float v)
{
    move.source[0] = 0.001F * (float)xTaskGetTickCount();
    move.target[0] = move.source[0] + t;
    move.source[3] = 0;
    move.target[3] = 0;

    move.y_set = y;
    move.source[1] = move.y;
    move.target[1] = move.y_set;
    move.source[2] = move.vy;
    move.target[2] = v;
    polynomial5_init(move.path + 1, move.source, move.target);
    move.path[1].t[1] = (move.path[1].t[0] + move.path[1].t[1]) * 0.5F;
}

void position_updatey(void)
{
    float t = 0.001F * (float)xTaskGetTickCount();
    if (t < move.path[1].t[1])
    {
        move.vy_set = polynomial5_vec(move.path + 1, t);
    }
    else
    {
        move.vy_set = ca_pid_f32(move.pid_offset + 1, move.y, move.y_set);
    }
}

void position_setz(float t, float z, float v)
{
    move.source[0] = 0.001F * (float)xTaskGetTickCount();
    move.target[0] = move.source[0] + t;
    move.source[3] = 0;
    move.target[3] = 0;

    move.z_set = z;
    move.source[1] = move.z;
    move.target[1] = move.z_set;
    move.source[2] = move.wz;
    move.target[2] = v;
    polynomial5_init(move.path + 2, move.source, move.target);
    move.path[2].t[1] = (move.path[2].t[0] + move.path[2].t[1]) * 0.5F;
}

void position_updatez(void)
{
    float t = 0.001F * (float)xTaskGetTickCount();
    if (t < move.path[2].t[1])
    {
        move.wz_set = polynomial5_vec(move.path + 2, t);
    }
    else
    {
        move.wz_set = ca_pid_f32(move.pid_offset + 2, move.z, move.z_set);
    }
}

int32_t laser_set_wz(float scale)
{
    (void)scale;

    int32_t diff = l1s.dis1.data - l1s.dis0.data;
    if (l1s.dis0.error + l1s.dis1.error == L1S_ERROR_NONE &&
        -400 < diff && diff < 400)
    {
        int32_t sum = (l1s.dis1.data + l1s.dis0.data) >> 1;
        float in = (float)diff / (float)(sum);
        // move.wz_set = ca_pid_f32(move.pid_l1s, 0, tanf(in));
        move.wz_set = scale * in;
    }
    else
    {
        move.wz_set = 0;
    }
    return diff;
}

static void robot_reset(void)
{
    chassis_ctrl(0, 0, 0, 0);
    other_ctrl(0, 0, 0, 0);

    uint32_t pwm[7] = {
        SERVO_FETCH_PWMMAX,
        SERVO_PITCH_STD - 200,
        SERVO_SHIFTV_PWMMIN,
        SERVO_PITCHL_STD - 200,
        SERVO_SHIFTVL_PWMMIN,
        SERVO_PITCHR_STD + 200,
        SERVO_SHIFTVR_PWMMAX,
    };
    servo_start(pwm);

    nvic_reset();
}

/**
 * @brief        get the value of serial
*/
static void chassis_serial(void)
{
    switch (move.serial->c)
    {
    case 'V':
    case 'v':
    {
        move.vx_set = move.serial->x;
        move.vy_set = move.serial->y;
        move.wz_set = move.serial->z;
    }
    break;

    case 'P':
    case 'p':
    {
        move.serial->c = 0;
        position_set(move.serial->x,
                     ABS(move.serial->x),
                     move.serial->y,
                     ABS(move.serial->y),
                     move.serial->z,
                     ABS(move.serial->z));
    }
    break;

    case 'Z':
    case 'z':
    {
        robot_reset();
    }
    break;

    default:
    {
    }
    break;
    }
}

/**
 * @brief        accroding to the channel value of remote control, calculate chassis speed set-point
*/
static void chassis_rc(void)
{
    /**
     * deadline, because some remote control need be calibrated,
     * the value of rocker is not zero in middle place
    */
    int16_t vx_channel =
        LIMIT_RC(move.rc->rc.ch[CHASSIS_X_CHANNEL], CHASSIS_RC_DEADLINE);
    int16_t vy_channel =
        LIMIT_RC(move.rc->rc.ch[CHASSIS_Y_CHANNEL], CHASSIS_RC_DEADLINE);
    int16_t wz_channel =
        LIMIT_RC(move.rc->rc.ch[CHASSIS_WZ_CHANNEL], CHASSIS_RC_DEADLINE);

    move.vx_set = vx_channel * CHASSIS_VX_RC_SEN;
    move.vy_set = vy_channel * CHASSIS_VY_RC_SEN;
    move.wz_set = wz_channel * -CHASSIS_WZ_RC_SEN;

    /* keyboard set speed set-point */
    if (move.rc->key.v & CHASSIS_RIGHT_KEY)
    {
        move.vx_set = NORMAL_MAX_CHASSIS_SPEED_X;
    }
    else if (move.rc->key.v & CHASSIS_LEFT_KEY)
    {
        move.vx_set = -NORMAL_MAX_CHASSIS_SPEED_X;
    }
    if (move.rc->key.v & CHASSIS_FRONT_KEY)
    {
        move.vy_set = NORMAL_MAX_CHASSIS_SPEED_Y;
    }
    else if (move.rc->key.v & CHASSIS_BACK_KEY)
    {
        move.vy_set = -NORMAL_MAX_CHASSIS_SPEED_Y;
    }
}

/**
 * @brief        four omni4 wheels speed is calculated by three param.
 * @param[out]   wheel_speed: four omni4 wheels speed
 * @param[in]    vx_set: horizontal speed
 * @param[in]    vy_set: vertial speed
 * @param[in]    wz_set: rotation speed
*/
static void chassis_omni4(float wheel_speed[4],
                          float vx_set,
                          float vy_set,
                          float wz_set)
{
    vx_set *= (float)M_SQRT1_2;
    vy_set *= (float)M_SQRT1_2;

    wheel_speed[0] = +vx_set - vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[1] = +vx_set + vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[2] = -vx_set + vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
    wheel_speed[3] = -vx_set - vy_set - wz_set * MOTOR_DISTANCE_TO_CENTER;
}

/**
 * @brief        control loop, according to control set-point, calculate
 *               motor current, motor current will be sentto motor
*/
static void chassis_loop(void)
{
    move.vx_set = LIMIT(move.vx_set, -NORMAL_MAX_CHASSIS_SPEED_X, NORMAL_MAX_CHASSIS_SPEED_X);
    move.vy_set = LIMIT(move.vy_set, -NORMAL_MAX_CHASSIS_SPEED_Y, NORMAL_MAX_CHASSIS_SPEED_Y);

    float wheel_speed[4];
    /* omni4 wheel speed calculation */
    chassis_omni4(wheel_speed, move.vx_set, move.vy_set, move.wz_set);

    /* calculate the max speed in four wheels, limit the max speed */
    float vector_max = 0;
    for (int i = 0; i != 4; ++i)
    {
        move.mo[i].v_set = wheel_speed[i];
        float temp = ABS(move.mo[i].v_set);
        if (vector_max < temp)
        {
            vector_max = temp;
        }
    }
    if (vector_max > MAX_WHEEL_SPEED)
    {
        float scale = MAX_WHEEL_SPEED / vector_max;
        for (int i = 0; i != 4; ++i)
        {
            move.mo[i].v_set *= scale;
        }
    }

    /* calculate pid */
    for (int i = 0; i != 4; ++i)
    {
        move.mo[i].i = (int16_t)ca_pid_f32(move.pid_speed + i,
                                           move.mo[i].v,
                                           move.mo[i].v_set);
    }
}

void task_chassis(void *pvParameters)
{
    (void)pvParameters;

    osDelay(CHASSIS_TASK_INIT_TIME);

    can_filter_init();

    /* initialization */
    {
        /* chassis motor speed PID */
        static float kpid_v[3] = {
            M3505_MOTOR_SPEED_PID_KP,
            M3505_MOTOR_SPEED_PID_KI,
            M3505_MOTOR_SPEED_PID_KD,
        };

        static float kpid_p[3][3] = {
            {5.0F, 0.02F, 0.0F}, /* x */
            {5.0F, 0.02F, 0.0F}, /* y */
            {5.0F, 0.02F, 0.0F}, /* z */
        };

        static float kpid_l1s[3] = {
            1.0F,
            0.002F,
            0.0F,
        };

        ca_pid_f32_position(move.pid_offset,
                            kpid_p[0],
                            -NORMAL_MAX_CHASSIS_SPEED_X,
                            NORMAL_MAX_CHASSIS_SPEED_X,
                            NORMAL_MAX_CHASSIS_SPEED_X * CHASSIS_RC_SLOW_SEN);
        ca_pid_f32_position(move.pid_offset + 1,
                            kpid_p[1],
                            -NORMAL_MAX_CHASSIS_SPEED_Y,
                            NORMAL_MAX_CHASSIS_SPEED_Y,
                            NORMAL_MAX_CHASSIS_SPEED_Y * CHASSIS_RC_SLOW_SEN);
        ca_pid_f32_position(move.pid_offset + 2,
                            kpid_p[2],
                            -MAX_WHEEL_SPEED,
                            MAX_WHEEL_SPEED,
                            MAX_WHEEL_SPEED * CHASSIS_RC_SLOW_SEN);

        ca_pid_f32_position(move.pid_l1s,
                            kpid_l1s,
                            -MAX_WHEEL_SPEED,
                            MAX_WHEEL_SPEED,
                            MAX_WHEEL_SPEED * CHASSIS_RC_SLOW_SEN);

        /* in beginning， chassis mode is stop */
        move.mode = CHASSIS_VECTOR_STOP;
        /* get remote control point */
        move.rc = ctrl_rc_point();
        /* get serial control point */
        move.serial = ctrl_serial_point();
        /* get chassis motor data point, initialize motor speed PID */
        for (int i = 0; i != 4; ++i)
        {
            move.mo[i].fb = chassis_point(i);
            ca_pid_f32_position(move.pid_speed + i,
                                kpid_v,
                                -M3505_MOTOR_SPEED_PID_MAX_OUT,
                                M3505_MOTOR_SPEED_PID_MAX_OUT,
                                M3505_MOTOR_SPEED_PID_MAX_IOUT);
        }
        /* first order low-pass filter  replace ramp function */
        ca_lpf_f32_init(move.vx_slow, CHASSIS_ACCEL_X_NUM, CHASSIS_CONTROL_TIME);
        ca_lpf_f32_init(move.vy_slow, CHASSIS_ACCEL_Y_NUM, CHASSIS_CONTROL_TIME);
        ca_lpf_f32_init(move.wz_slow, CHASSIS_ACCEL_Z_NUM, CHASSIS_CONTROL_TIME);
    }

    for (;;)
    {
        move.tick = xTaskGetTickCount();

        /* update chassis measure data */
        chassis_update();

        if (switch_is_up(move.rc->rc.s[RC_SW_L]) &&
            switch_is_up(move.rc->rc.s[RC_SW_R]))
        {
            robot_reset();
        }

        /* chassis mode set */
        if (switch_is_mid(move.rc->rc.s[RC_SW_L]))
        {
            /* middle, up */
            if (switch_is_up(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_PATH;
            }
            /* middle, middle */
            else if (switch_is_mid(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_NORMAL;
            }
            /* middle, down */
            else if (switch_is_down(move.rc->rc.s[RC_SW_R]))
            {
                move.mode = CHASSIS_VECTOR_SLOW;
            }
        }
        else
        {
            move.mode = CHASSIS_VECTOR_AUTO;
        }

        /* chassis loop set */
        switch (move.mode)
        {
        case CHASSIS_VECTOR_STOP:
        {
            move.vx_set = 0;
            move.vy_set = 0;
            move.wz_set = 0;
        }
        break; /* CHASSIS_VECTOR_STOP */

        case CHASSIS_VECTOR_SLOW:
        {
            if (!READ_BIT(move.flag, MOVE_FLAG_NORC))
            {
                chassis_rc();
            }

            /* in slow mode, the speed will decrease */
            move.vx_set *= CHASSIS_RC_SLOW_SEN;
            move.vy_set *= CHASSIS_RC_SLOW_SEN;
            move.wz_set *= CHASSIS_RC_SLOW_SEN;

            if (!READ_BIT(move.flag, MOVE_FLAG_NOSERIAL))
            {
                chassis_serial();
            }
        }
        break;

        case CHASSIS_VECTOR_NORMAL:
        {
            chassis_rc();

            /**
             * first order low-pass replace ramp function,
             * calculate chassis speed set-point to improve control performance
            */
            move.vx_set = ca_lpf_f32(move.vx_slow, move.vx_set);
            move.vy_set = ca_lpf_f32(move.vy_slow, move.vy_set);
            move.wz_set = ca_lpf_f32(move.wz_slow, move.wz_set);
        }
        break; /* CHASSIS_VECTOR_SLOW CHASSIS_VECTOR_NORMAL */

        case CHASSIS_VECTOR_PATH:
        {
            if (!READ_BIT(move.flag, MOVE_FLAG_NORC))
            {
                chassis_rc();
            }

            if (!READ_BIT(move.flag, MOVE_FLAG_NOSERIAL))
            {
                chassis_serial();
            }

            // position_update();
        }
        break; /* CHASSIS_VECTOR_PATH */

        case CHASSIS_VECTOR_AUTO:
        {
            chassis_serial();
        }
        break;

        default:
        {
        }
        break;
        }

        /* control loop */
        chassis_loop();

        chassis_ctrl(move.mo[0].i, move.mo[1].i, move.mo[2].i, move.mo[3].i);

        osDelay(CHASSIS_CONTROL_TIME_MS);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
