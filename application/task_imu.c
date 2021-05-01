/**
 * *****************************************************************************
 * @file         task_imu.c/h
 * @brief        The task of imu update
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_imu.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "ca.h"
#include "imu_ahrs.h"
#include "main.h"
#include "mpu6500.h"

#include <stdint.h>

extern imu_t imu;
extern mpu_t mpu;

/* Private define ------------------------------------------------------------*/

/* kp of temperature control PID */
#define TEMPERATURE_PID_KP 10000.0f
/* ki of temperature control PID */
#define TEMPERATURE_PID_KI 2.0f
/* kd of temperature control PID */
#define TEMPERATURE_PID_KD 0.0f

/* max out of temperature control PID */
#define TEMPERATURE_PID_MAX_OUT 0x3fff
/* max iout of temperature control PID */
#define TEMPERATURE_PID_MAX_IOUT 0xfff

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

static ca_pid_f32_t pid_temp;

static void ctrl_temp_init(void)
{
    static const float kpid_temp[3] = {
        TEMPERATURE_PID_KP,
        TEMPERATURE_PID_KI,
        TEMPERATURE_PID_KD,
    };
#if 1
    ca_pid_f32_position(&pid_temp,
                        kpid_temp,
                        0,
                        TEMPERATURE_PID_MAX_OUT,
                        TEMPERATURE_PID_MAX_IOUT);
#else
    ca_pid_delta(&pid_temp,
                 kpid_temp,
                 TEMPERATURE_PID_MAX_OUT);
#endif
}

void task_imu(void *pvParameters)
{
    ctrl_temp_init();
    imu_pwm_start();

    mpu_data_update();
    imu_quat_init();

    for (;;)
    {
        mpu_data_update();
        imu_update_ahrs();
        imu_update_attitude();

        imu_pwm_set((uint16_t)ca_pid_f32(&pid_temp, imu.temp, 45.0f));

#if 0
        os_justfloat(10U,
                     mpu.gx,
                     mpu.gy,
                     mpu.gz,
                     mpu.ax,
                     mpu.ay,
                     mpu.az,
                     mpu.mx,
                     mpu.my,
                     mpu.mz,
                     mpu.temp);
#elif 0
        os_printf("a:");
        os_putf(imu.rol, 5);
        os_printf(",");
        os_putf(imu.pit, 5);
        os_printf(",");
        os_putf(imu.yaw, 5);
        os_printf(",");
        os_putf(imu.temp, 3);
        os_printf("\r\n");
#elif 0
        os_pushf(imu.rol);
        os_pushf(imu.pit);
        os_pushf(imu.yaw);
        os_pushf(imu.temp);
        os_tail();
#elif 1
        os_justfloat(4U, imu.rol, imu.pit, imu.yaw, imu.temp);
#endif

        osDelay(2U);
    }

    osThreadExit();
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
