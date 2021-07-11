/**
 * *****************************************************************************
 * @file         app_imu.c
 * @brief        imu application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "ca.h"
#include "bsp.h"
#include "ctrl.h"
#include "mpu6500.h"
#include "imu_ahrs.h"

#include <stdint.h>

extern imu_t imu;
extern mpu_t mpu;

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

void task_imu(void *pvParameters)
{
    (void)pvParameters;

    static ca_pid_f32_t pid_temp;

    /* Initialization block */
    {
        static const float kpid_temp[3] = {
            TEMPERATURE_PID_KP,
            TEMPERATURE_PID_KI,
            TEMPERATURE_PID_KD,
        };
        ca_pid_f32_position(&pid_temp,
                            kpid_temp,
                            0,
                            TEMPERATURE_PID_MAX_OUT,
                            TEMPERATURE_PID_MAX_IOUT);

        imu_pwm_start();

        mpu_data_update();
        imu_quat_init();
    }

    for (;;)
    {
        /* imu control */
        {
            mpu_data_update();
            imu_update_ahrs();
            imu_update_attitude();

            uint16_t pwm = (uint16_t)ca_pid_f32(&pid_temp, imu.temp, 45.0F);
            imu_pwm_set(pwm);
        }

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
#elif 0
        os_justfloat(4,
                     imu.rol,
                     imu.pit,
                     imu.yaw,
                     imu.temp);
#endif

        osDelay(2);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
