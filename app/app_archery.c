/**
 * *****************************************************************************
 * @file         app_archery.c
 * @brief        archery application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_archery.h"

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

#define SHOOT_COUNT      100
#define FLAG_SHOOT_START (1 << 0)
#define FLAG_SHOOT_COUNT (1 << 1)
#define FLAG_SHOOT_STOP  (1 << 2)

/**
 * @brief        aiming signal
*/
typedef enum
{
    SIGNAL_AIMING_NONE = (1 << 0),  //!< none
    SIGNAL_AIMING_DO = (1 << 1),    //!< aim do
    SIGNAL_AIMING_DONE = (1 << 2),  //!< aim done
} signal_aiming_t;

void task_archery(void *pvParameters)
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

    ctrl_serial_t *serial = ctrl_serial_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    static int8_t signal_aiming = SIGNAL_AIMING_NONE;
    static int8_t shoot_flag = 0;
    static uint16_t shoot_count = 0;

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

        /* remote control */
        if (switch_is_mid(rc->rc.s[RC_SW_L]) &&
            switch_is_down(rc->rc.s[RC_SW_R]))
        {
            /* restart control */
            if (rc->rc.ch[RC_CH_LV] < -220)
            {
                serial->c = 0;
            }

            /* Start sending aiming signal */
            if (signal_aiming == SIGNAL_AIMING_NONE &&
                rc->rc.ch[RC_CH_LV] > 220)
            {
                signal_aiming = SIGNAL_AIMING_DO;
            }

            /* End sending aiming signal */
            if (signal_aiming != SIGNAL_AIMING_NONE &&
                rc->rc.ch[RC_CH_LV] < 220)
            {
                signal_aiming = SIGNAL_AIMING_NONE;
            }

            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < -650)
            {
                /* It starts to spew out gas */
                if (!READ_BIT(shoot_flag, FLAG_SHOOT_START))
                {
                    SET_BIT(shoot_flag, FLAG_SHOOT_START);
                    SET_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                    gpio_pin_set(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
                    usart_dma_tx(&huart_os, (const void *)"b\n", 2);
                }
            }
            else
            {
                /* Stop ejecting gas */
                if (READ_BIT(shoot_flag, FLAG_SHOOT_STOP))
                {
                    CLEAR_BIT(shoot_flag, FLAG_SHOOT_STOP);
                    CLEAR_BIT(shoot_flag, FLAG_SHOOT_START);
                }

                gpio_pin_reset(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
            }
        }

        /* Send aiming signal */
        if (signal_aiming == SIGNAL_AIMING_DO)
        {
            signal_aiming = SIGNAL_AIMING_DONE;

            usart_dma_tx(&huart_os, (const void *)"a\n", 2);
        }

        /* Control injection time */
        if (READ_BIT(shoot_flag, FLAG_SHOOT_COUNT))
        {
            if (shoot_count++ == SHOOT_COUNT)
            {
                shoot_count = 0;
                CLEAR_BIT(shoot_flag, FLAG_SHOOT_COUNT);

                gpio_pin_reset(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
                SET_BIT(shoot_flag, FLAG_SHOOT_STOP);
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
