/**
 * *****************************************************************************
 * @file         imu_ahrs.c
 * @brief        Automatic Heading Reference System by imu
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "imu_ahrs.h"

#include "ahrs.h"
#include "ca.h"
#include "main.h"
#include "mpu6500.h"
#include "zyx.h"

#include <math.h>

extern imu_t imu;

static float quat[4] = {1.0f, 0.0f, 0.0f, 0.0f};

/**
 * @brief        Gets half of the sampling cycle time
 * @return       float
*/
static float ht_get(void);

#if 0
/**
 * @brief        Gets the sampling cycle time
 * @return       float
*/
static float t_get(void);
#endif

static float ht_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now = HAL_GetTick(); /* ms */
    float ret = (float)(update_now - update_last) / 2000.0f;
    update_last = update_now;

    return ret;
}

#if 0
static float t_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now  = HAL_GetTick(); /* ms */
    float ret   = (float)(update_now - update_last) / 1000.0f;
    update_last = update_now;

    return ret;
}
#endif

/**
 * @brief        Initialize quaternion
 * @param[in]    q[4]: quaternionq[0]+q[1]* i +q[2]* j +q[3]* k
 * @param[in]    hx:   x axis direction of Earth's magnetic field
 * @param[in]    hy:   y axis direction of Earth's magnetic field
*/
void quat_init(float q[4],
               float hx,
               float hy)
{
#ifdef BOARD_IS_DOWN
    if (hx < 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.005f;
            q[1] = -0.199f;
            q[2] = 0.979f;
            q[3] = -0.0089f;
        }
        else
        {
            q[0] = -0.008f;
            q[1] = -0.555f;
            q[2] = 0.83f;
            q[3] = -0.002f;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.005f;
            q[1] = -0.199f;
            q[2] = -0.978f;
            q[3] = 0.012f;
        }
        else
        {
            q[0] = 0.005f;
            q[1] = -0.553f;
            q[2] = -0.83f;
            q[3] = -0.0023f;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.0012f;
            q[1] = -0.978f;
            q[2] = -0.199f;
            q[3] = -0.005f;
        }
        else
        {
            q[0] = 0.0023f;
            q[1] = -0.83f;
            q[2] = -0.553f;
            q[3] = 0.0023f;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.0025f;
            q[1] = 0.978f;
            q[2] = -0.199f;
            q[3] = 0.008f;
        }
        else
        {
            q[0] = 0.0025f;
            q[1] = 0.83f;
            q[2] = -0.56f;
            q[3] = 0.0045f;
        }
    }
#else
    if (hx < 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.195f;
            q[1] = -0.015f;
            q[2] = 0.0043f;
            q[3] = 0.979f;
        }
        else
        {
            q[0] = 0.555f;
            q[1] = -0.015f;
            q[2] = 0.006f;
            q[3] = 0.829f;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.193f;
            q[1] = -0.009f;
            q[2] = -0.006f;
            q[3] = 0.979f;
        }
        else
        {
            q[0] = -0.552f;
            q[1] = -0.0048f;
            q[2] = -0.0115f;
            q[3] = 0.8313f;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.9785f;
            q[1] = 0.008f;
            q[2] = -0.02f;
            q[3] = 0.195f;
        }
        else
        {
            q[0] = -0.9828f;
            q[1] = 0.002f;
            q[2] = -0.0167f;
            q[3] = 0.5557f;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.979f;
            q[1] = 0.0116f;
            q[2] = -0.0167f;
            q[3] = -0.195f;
        }
        else
        {
            q[0] = -0.83f;
            q[1] = 0.014f;
            q[2] = -0.012f;
            q[3] = -0.556f;
        }
    }
#endif /* BOARD_IS_DOWN */
    else
    {
        q[0] = 1;
        q[1] = 0;
        q[2] = 0;
        q[3] = 0;
    }
}

/**
 * @brief        Initialize quaternion
*/
void imu_quat_init(void)
{
    quat_init(quat, imu.mx, imu.my);
}

/**
 * @brief        Update ahrs by imu
*/
void imu_update_ahrs(void)
{
    float g[3] = {imu.wx, imu.wy, imu.wz};
    float a[3] = {imu.ax, imu.ay, imu.az};
    float m[3] = {imu.mx, imu.my, imu.mz};

    //ahrs_madgwick(quat, g, a, m, 2 * ht_get());
    ahrs_mahony(quat, g, a, m, ht_get());
    //ahrs_mahony_imu(quat, g, a, ht_get());
}

/**
 * @brief        update imu attitude
*/
void imu_update_attitude(void)
{
    float a[3];
    zyx_f32_quat_euler(quat, a);
    imu.yaw = a[ZYX_YAW];
    imu.pit = a[ZYX_PITCH];
    imu.rol = a[ZYX_ROLL];
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
