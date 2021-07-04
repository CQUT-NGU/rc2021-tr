/**
 * *****************************************************************************
 * @file         task_imu.c
 * @brief        The task of imu update
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_imu.h"

#include "bsp.h"
#include "ca.h"
#include "imu_ahrs.h"
#include "main.h"
#include "mpu6500.h"
#include "ctrl.h"

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

/**
 * @brief        aiming signal
*/
typedef enum
{
    SIGNAL_AIMING_NONE = (1 << 0),  //!< none
    SIGNAL_AIMING_DO = (1 << 1),    //!< aim do
    SIGNAL_AIMING_DONE = (1 << 2),  //!< aim done
} signal_aiming_t;

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

    ctrl_pc_t *pc = ctrl_pc_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    static int8_t signal_aiming = SIGNAL_AIMING_NONE;

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

        /* Aiming signal control */
        {
            if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
                switch_is_down(rc->rc.s[RC_SW_R]))
            {
                /* restart control */
                if (rc->rc.ch[RC_CH_S] > 440)
                {
                    pc->c = 0;
                }

                /* Start sending aiming signal */
                if (signal_aiming == SIGNAL_AIMING_NONE &&
                    rc->rc.ch[RC_CH_S] < -440)
                {
                    signal_aiming = SIGNAL_AIMING_DO;
                }

                /* End sending aiming signal */
                if (signal_aiming != SIGNAL_AIMING_NONE &&
                    rc->rc.ch[RC_CH_S] > -440)
                {
                    signal_aiming = SIGNAL_AIMING_NONE;
                }
            }

            /* Send aiming signal */
            if (signal_aiming == SIGNAL_AIMING_DO)
            {
                signal_aiming = SIGNAL_AIMING_DONE;

                usart_dma_tx(&huart_os, (const void *)"a\n", 2);
            }
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
        os_justfloat(4U,
                     imu.rol,
                     imu.pit,
                     imu.yaw,
                     imu.temp);
#endif

        osDelay(2U);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
