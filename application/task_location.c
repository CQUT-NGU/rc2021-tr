#include "main.h"
#include "mpu6500.h"
#include "task_location.h"
real_position_t real_position;//get the yaw (x ,y) position
extern imu_t imu;
void task_location(void *argument)
{
	float vx,vy;
	real_position.alpha = imu.yaw ; //get xoy yaw
	//��ȡxoyƽ���x��y�ٶ�
	vx = imu.vx;
	vy = imu.vy;
	//ִ�л���,ִ��ʱ��Ϊ10ms
	real_position.x = vx*10;
	real_position.y = vy*10;

}
