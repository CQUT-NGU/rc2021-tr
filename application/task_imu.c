/**
 * *****************************************************************************
 * @file         task_imu.c/h
 * @brief        The task of imu update
 * @author       ngu
 * @date         20210101
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "task_imu.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "cc.h"
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

static cc_pid_t temp_pid;

static void ctrl_temp_init(void)
{
    static const float temp_pid_k[3] = {
        TEMPERATURE_PID_KP,
        TEMPERATURE_PID_KI,
        TEMPERATURE_PID_KD,
    };
#if 1
    cc_pid_position(&temp_pid,
                    temp_pid_k,
                    TEMPERATURE_PID_MAX_OUT,
                    TEMPERATURE_PID_MAX_IOUT);
#else
    cc_pid_delta(&temp_pid,
                 temp_pid_k,
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

        cc_pid(&temp_pid, imu.temp, 45.0f);
        if (temp_pid.out < 0)
        {
            temp_pid.out = 0;
            imu_pwm_set(0U);
        }
        else
        {
            imu_pwm_set((uint16_t)temp_pid.out);
        }

#if 0
        os_printf("%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
                  mpu.gx,
                  mpu.gy,
                  mpu.gz,
                  mpu.ax,
                  mpu.ay,
                  mpu.az,
                  mpu.mx,
                  mpu.my,
                  mpu.mz);
#elif 1
        os_putf(imu.rol, 3);
        os_printf(",");
        os_putf(imu.pit, 3);
        os_printf(",");
        os_putf(imu.yaw, 3);
        os_printf(",");
        os_putf(imu.temp, 3);
        os_printf("\r\n");
#endif

        osDelay(2U);
    }

    osThreadExit();
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
