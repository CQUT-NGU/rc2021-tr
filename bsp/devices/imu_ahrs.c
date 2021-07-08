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

static float quat[4] = {1.0F, 0.0F, 0.0F, 0.0F};

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
    float ret = (float)(update_now - update_last) / 2000.0F;
    update_last = update_now;

    return ret;
}

#if 0
static float t_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now  = HAL_GetTick(); /* ms */
    float ret   = (float)(update_now - update_last) / 1000.0F;
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
            q[0] = -0.005F;
            q[1] = -0.199F;
            q[2] = 0.979F;
            q[3] = -0.0089F;
        }
        else
        {
            q[0] = -0.008F;
            q[1] = -0.555F;
            q[2] = 0.83F;
            q[3] = -0.002F;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.005F;
            q[1] = -0.199F;
            q[2] = -0.978F;
            q[3] = 0.012F;
        }
        else
        {
            q[0] = 0.005F;
            q[1] = -0.553F;
            q[2] = -0.83F;
            q[3] = -0.0023F;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.0012F;
            q[1] = -0.978F;
            q[2] = -0.199F;
            q[3] = -0.005F;
        }
        else
        {
            q[0] = 0.0023F;
            q[1] = -0.83F;
            q[2] = -0.553F;
            q[3] = 0.0023F;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.0025F;
            q[1] = 0.978F;
            q[2] = -0.199F;
            q[3] = 0.008F;
        }
        else
        {
            q[0] = 0.0025F;
            q[1] = 0.83F;
            q[2] = -0.56F;
            q[3] = 0.0045F;
        }
    }
#else
    if (hx < 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = 0.195F;
            q[1] = -0.015F;
            q[2] = 0.0043F;
            q[3] = 0.979F;
        }
        else
        {
            q[0] = 0.555F;
            q[1] = -0.015F;
            q[2] = 0.006F;
            q[3] = 0.829F;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.193F;
            q[1] = -0.009F;
            q[2] = -0.006F;
            q[3] = 0.979F;
        }
        else
        {
            q[0] = -0.552F;
            q[1] = -0.0048F;
            q[2] = -0.0115F;
            q[3] = 0.8313F;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.9785F;
            q[1] = 0.008F;
            q[2] = -0.02F;
            q[3] = 0.195F;
        }
        else
        {
            q[0] = -0.9828F;
            q[1] = 0.002F;
            q[2] = -0.0167F;
            q[3] = 0.5557F;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabsf(hx / hy) >= 1)
        {
            q[0] = -0.979F;
            q[1] = 0.0116F;
            q[2] = -0.0167F;
            q[3] = -0.195F;
        }
        else
        {
            q[0] = -0.83F;
            q[1] = 0.014F;
            q[2] = -0.012F;
            q[3] = -0.556F;
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

    ahrs_mahony(quat, g, a, m, ht_get());
    //ahrs_mahony_imu(quat, g, a, ht_get());
    //ahrs_madgwick(quat, g, a, m, 2 * ht_get());
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
