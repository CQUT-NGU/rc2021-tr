/**
 * *****************************************************************************
 * @file         ctrl_can.c/h
 * @brief        can control
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * @details      there is CAN interrupt function to receive motor data, and CAN 
 *               send function to send motor current to control motor.
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "ctrl_can.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;
#if CAN2_IS_ENABLE
extern CAN_HandleTypeDef hcan2;
#endif /* CAN2_IS_ENABLE */

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/**
 * @brief motor data read 
*/
#define MOTOR_MEASURE(ptr, data)                                    \
                                                                    \
    do                                                              \
    {                                                               \
        (ptr)->ecd_last  = (ptr)->ecd;                              \
        (ptr)->ecd       = (uint16_t)((data)[0] << 8U | (data)[1]); \
        (ptr)->v_rpm     = (int16_t)((data)[2] << 8U | (data)[3]);  \
        (ptr)->i_current = (int16_t)((data)[4] << 8U | (data)[5]);  \
        (ptr)->temperate = (data)[6];                               \
    } while (0)

/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static motor_t motor_chassis[4];

static CAN_TxHeaderTypeDef chassis_tx_message;

static uint8_t chassis_tx_can_data[8];

/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

/**
 * @brief        hal CAN fifo call back, receive motor data
 * @param[in]    hcan: the point to CAN handle
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef header_rx  = {0};
    static uint8_t      data_rx[8] = {0};

    (void)HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header_rx, data_rx);

    switch (header_rx.StdId)
    {
    case CAN_ID_3508_M1:
    case CAN_ID_3508_M2:
    case CAN_ID_3508_M3:
    case CAN_ID_3508_M4:
    {
        MOTOR_MEASURE(&motor_chassis[(header_rx.StdId - CAN_ID_3508_M1)],
                      data_rx); /*!< Get motor id */
        break;
    }

    default:
        break;
    }
}

const motor_t *chassis_point(uint8_t i)
{
    return &motor_chassis[(i & 0x07U)];
}

void chassis_ctrl(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    /*!< the TxMailbox used to store the Tx message */
    uint32_t send_mail_box;
    /*!< Specifies the type of identifier for the message */
    chassis_tx_message.IDE = CAN_ID_STD;
    /*!< Specifies the type of frame for the message */
    chassis_tx_message.RTR = CAN_RTR_DATA;
    /*!< Specifies the standard identifier */
    chassis_tx_message.StdId = CAN_ID_CHASSIS_ALL;
    /*!< Specifies the length of the frame that will be transmitted */
    chassis_tx_message.DLC = 0x08U;

    chassis_tx_can_data[0] = (motor1 >> 8);
    chassis_tx_can_data[1] = (motor1 & 0xFFU);
    chassis_tx_can_data[2] = (motor2 >> 8);
    chassis_tx_can_data[3] = (motor2 & 0xFFU);
    chassis_tx_can_data[4] = (motor3 >> 8);
    chassis_tx_can_data[5] = (motor3 & 0xFFU);
    chassis_tx_can_data[6] = (motor4 >> 8);
    chassis_tx_can_data[7] = (motor4 & 0xFFU);

    /**
     * Add a message to the first free Tx mailbox and
     * activate the corresponding transmission request 
    */
    (void)HAL_CAN_AddTxMessage(&CHASSIS_CAN,
                               &chassis_tx_message,
                               chassis_tx_can_data,
                               &send_mail_box);
}

void chassis_reset(void)
{
    /*!< the TxMailbox used to store the Tx message */
    uint32_t send_mail_box;
    /*!< Specifies the type of identifier for the message */
    chassis_tx_message.IDE = CAN_ID_STD;
    /*!< Specifies the type of frame for the message */
    chassis_tx_message.RTR = CAN_RTR_DATA;
    /*!< Specifies the standard identifier */
    chassis_tx_message.StdId = 0x700;
    /*!< Specifies the length of the frame that will be transmitted */
    chassis_tx_message.DLC = 0x08U;

    chassis_tx_can_data[0] = 0;
    chassis_tx_can_data[1] = 0;
    chassis_tx_can_data[2] = 0;
    chassis_tx_can_data[3] = 0;
    chassis_tx_can_data[4] = 0;
    chassis_tx_can_data[5] = 0;
    chassis_tx_can_data[6] = 0;
    chassis_tx_can_data[7] = 0;

    /**
     * Add a message to the first free Tx mailbox and
     * activate the corresponding transmission request 
    */
    (void)HAL_CAN_AddTxMessage(&CHASSIS_CAN,
                               &chassis_tx_message,
                               chassis_tx_can_data,
                               &send_mail_box);
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
