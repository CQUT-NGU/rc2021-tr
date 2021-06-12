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

#define ACC_G_CQ 9.7914F

/* kp of temperature control PID */
#define TEMPERATURE_PID_KP 10000.0F
/* ki of temperature control PID */
#define TEMPERATURE_PID_KI 2.0F
/* kd of temperature control PID */
#define TEMPERATURE_PID_KD 0.0F

/* max out of temperature control PID */
#define TEMPERATURE_PID_MAX_OUT BUZZER_PWM_MAX
/* max iout of temperature control PID */
#define TEMPERATURE_PID_MAX_IOUT 0xFFF

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
    (void)pvParameters;

    ctrl_temp_init();
    imu_pwm_start();

    mpu_data_update();
    imu_quat_init();

    float scale = 0;

    for (uint16_t i = 0U; i != 400U; ++i)
    {
        scale += ca_sqrt_u32(SQ(imu.ax) + SQ(imu.ay) + SQ(imu.az));
        osDelay(2);
    }

    scale /= 400;

    scale = 1 / scale * ACC_G_CQ * 0.002F;

    ca_lpf_f32_t lpf[3] = {0};
    ca_lpf_f32_init(lpf, 0, 0.002F);
    ca_lpf_f32_init(lpf + 1, 0, 0.002F);
    ca_lpf_f32_init(lpf + 2, 0, 0.002F);

    imu.vx = 0;
    imu.vy = 0;
    imu.vz = 0;

    for (;;)
    {
        mpu_data_update();
        imu_update_ahrs();
        imu_update_attitude();

        imu_pwm_set((uint16_t)ca_pid_f32(&pid_temp, imu.temp, 45.0F));

        // imu.vx += ca_lpf_f32(lpf, imu.ax) * scale;
        // imu.vy += ca_lpf_f32(lpf + 1, imu.ay) * scale;
        // imu.vz += ca_lpf_f32(lpf + 2, imu.az) * scale;

        imu.vx += imu.ax * scale;
        imu.vy += imu.ay * scale;
        imu.vz += imu.az * scale;

#if 0
        os_printf("%i,%i,%i,%i,%i,%i,%i,%i,%i,%g\n",
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
#elif 1
        os_printf("%g,%g,%g\r\n", imu.vx, imu.vy, imu.vz);
#elif 0
        os_justfloat(4U, imu.rol, imu.pit, imu.yaw, imu.temp);
#endif

        osDelay(2U);
    }
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
