#include "task_location.h"

#include "cmsis_os.h"
#include "main.h"
#include "mpu6500.h"

extern imu_t imu;

real_position_t real_position;  //get the yaw (x ,y) position

void task_location(void *argument)
{
    float vx, vy;
    while (1)
    {
        real_position.alpha = imu.yaw;  //get xoy yaw
        //获取xoy平面的x，y速度
        vx = imu.vx;
        vy = imu.vy;
        //执行换算,执行时间为10ms
        real_position.x = vx * 10;
        real_position.y = vy * 10;
        osDelay(4);
    }
}
