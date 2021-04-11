/**
 * *****************************************************************************
 * @file         imu_ahrs.c/h
 * @brief        Automatic Heading Reference System by imu
 * @author       tqfx
 * @date         20210101
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "imu_ahrs.h"

/* Private includes ----------------------------------------------------------*/
#include "ahrs.h"
#include "cc.h"
#include "main.h"
#include "mpu6500.h"

#include <math.h>

extern imu_t imu;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

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

/* Private user code ---------------------------------------------------------*/

static float ht_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now  = HAL_GetTick(); /* ms */
    float ret   = (float)(update_now - update_last) / 2000.0f;
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
*/
void imu_quaternion_init(void)
{
    ahrs_quaternion_init(imu.mx, imu.my);
}

/**
 * @brief        Update ahrs by imu
*/
void imu_update_ahrs(void)
{
    volatile float g[3] = {imu.wx, imu.wy, imu.wz};
    volatile float a[3] = {imu.ax, imu.ay, imu.az};
    volatile float m[3] = {imu.mx, imu.my, imu.mz};

    ahrs_mahony(g, a, m, ht_get());
}

/**
 * @brief        update imu attitude
*/
void imu_update_attitude(void)
{
    ahrs_euler_angle(&imu.rol, &imu.pit, &imu.yaw);
}

/************************ (C) COPYRIGHT tqfx *******************END OF FILE****/
