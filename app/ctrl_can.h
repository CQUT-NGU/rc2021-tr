/**
 * *****************************************************************************
 * @file         ctrl_can.h
 * @brief        can control
 * @author       NGU
 * @date         20210619
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * @details      there is CAN interrupt function to receive motor data, and CAN
 *               send function to send motor current to control motor.
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CTRL_CAN_H__
#define __CTRL_CAN_H__

#include "ctrl.h"

#define CHASSIS_CAN hcan1

/**
 * @brief        CAN send and receive ID
*/
typedef enum
{
    CAN_ID_CHASSIS_ALL = 0x200,
    CAN_ID_3508_M1 = 0x201,
    CAN_ID_3508_M2 = 0x202,
    CAN_ID_3508_M3 = 0x203,
    CAN_ID_3508_M4 = 0x204,
} can_msg_id_e;

/**
 * @brief        motor data
*/
typedef struct
{
    uint16_t ecd_last;
    uint16_t ecd;
    int16_t v_rpm;
    int16_t i_current;
    uint8_t temperate;
} motor_t;

__BEGIN_DECLS

/**
 * @brief        return the chassis 3508 motor data point
 * @param[in]    i: motor number,range [0,3]
 * @return       motor data point
*/
extern const motor_t *chassis_point(uint8_t i);

/**
 * @brief        send control current of motor (0x201, 0x202, 0x203, 0x204)
 * @param[in]    motor1: (0x201) 3508 motor control current, range[-16384,16384]
 * @param[in]    motor2: (0x202) 3508 motor control current, range[-16384,16384]
 * @param[in]    motor3: (0x203) 3508 motor control current, range[-16384,16384]
 * @param[in]    motor4: (0x204) 3508 motor control current, range[-16384,16384]
*/
extern void chassis_ctrl(int16_t motor1,
                         int16_t motor2,
                         int16_t motor3,
                         int16_t motor4);

/**
 * @brief        send CAN packet of ID 0x700,
 *               it will set chassis motor 3508 to quick ID setting
*/
extern void chassis_reset(void);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_CAN_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/