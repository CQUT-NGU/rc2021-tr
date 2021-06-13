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
	//平均的参数数组
		uint8_t index = 0;
		float ort_x_value[10];
		float ort_y_value[10];
    float scale = 0;
		//x，y当前的坐标，板倾斜时效果更佳
		float vx_value;
		float vy_value;
		
	//平均值缓解误差
    for (uint16_t i = 0U; i != 200U; ++i)
    {
        scale += ca_sqrt_u32(SQ(imu.ax) + SQ(imu.ay)+ SQ(imu.az));//get the accel victor now
        osDelay(1);// task switch in 2ms
    }

    scale /= 200;

    scale = 1 / scale * ACC_G_CQ * 0.001F;

		//低通滤波器缓解误差
    ca_lpf_f32_t lpf[2] = {0};
    ca_lpf_f32_init(lpf, 0, 0.001F);
    ca_lpf_f32_init(lpf + 1, 0, 0.001F);
		//ca_lpf_f32_init(lpf + 2, 0, 0.002F);
		
		//reset the velocit
    imu.vx = 0;
    imu.vy = 0;
    imu.vz = 0;

    for (;;)
    {
        mpu_data_update();
        imu_update_ahrs();
        imu_update_attitude();

        imu_pwm_set((uint16_t)ca_pid_f32(&pid_temp, imu.temp, 45.0F));

        //imu.vx += ca_lpf_f32(lpf, imu.ax) * scale;
        //imu.vy += ca_lpf_f32(lpf + 1, imu.ay) * scale;
        // imu.vz += ca_lpf_f32(lpf + 2, imu.az) * scale;
				
					imu.vx += imu.ax * scale;
					imu.vy += imu.ay * scale;
				//平均N=10时数据
					ort_x_value[index] = imu.vx;
					ort_y_value[index] = imu.vy;
					index ++;

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
				if(index == 10)
				{
					for(index = 0 ;index<10;index++)
					{
						 vx_value += ort_x_value[index];
						 vy_value += ort_y_value[index];
					}
					vx_value =((vx_value/10)-0.1);
					vy_value =((vy_value/10)-0.1);
					index = 0;
          os_printf("%g,%g\r\n", vx_value*0.4, vy_value*0.4);
				}
#elif 0
        os_justfloat(4U, imu.rol, imu.pit, imu.yaw, imu.temp);
#endif

        osDelay(2U);
    }
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
