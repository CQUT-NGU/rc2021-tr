/**
 * *****************************************************************************
 * @file         ctrl_l1s.c
 * @brief        contrl l1s
 * @author       NGU
 * @date         20210707
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CTRL_L1S_H__
#define __CTRL_L1S_H__

#include "bsp.h"

#include "main.h"

#include <stdint.h>

typedef enum l1s_error_t
{
    L1S_ERROR_NONE = 0,
    L1S_ERROR_PROTOCOL_FUNCTION = 0x8C,    //!< protocol function code error
    L1S_ERROR_PROTOCOL_VALIDATION = 0x8E,  //!< protocol validation error
    L1S_ERROR_PROTOCOL_PARAMETER = 0x8D,   //!< protocol parameter error
    L1S_ERROR_TEMPERATURE_LOW = 0xFC,      //!< temperature is too low
    L1S_ERROR_TEMPERATURE_HIGH = 0xFD,     //!< temperature is too high
    L1S_ERROR_FAILURE = 0xFF,              //!< weak reflection or calculation failure
    L1S_ERROR_BRIGHT = 0x100,              //!< bright reflection
    L1S_ERROR_NOTOSCALE = 0x102,           //!< notoscale
    L1S_ERROR_PHOTOSENSITIVE = 0x11D,      //!< abnormal photosensitive device
    L1S_ERROR_LASER_TUBE = 0x11E,          //!< laser tube anomaly
    L1S_ERROR_HARDWARE = 0x122,            //!< hardware abnormal
} l1s_error_t;

typedef struct l1s_dis_t
{
    uint32_t data;      //!< distance
    l1s_error_t error;  //!< error stats
} l1s_dis_t;

typedef struct l1s_t
{
    l1s_dis_t dis0;
    l1s_dis_t dis1;
    l1s_dis_t dis2;
    // l1s_dis_t dis3;
    int8_t flag;
} l1s_t;

#define huart_l1s0 huart7 /* uart, connect to l1s 0 */
#undef L1S0_IRQHandler
#define L1S0_IRQHandler UART7_IRQHandler

#define huart_l1s1 huart8 /* uart, connect to l1s 1 */
#undef L1S1_IRQHandler
#define L1S1_IRQHandler UART8_IRQHandler

#define huart_l1s2 huart3 /* usart, connect to l1s 2 */
#undef L1S2_IRQHandler
#define L1S2_IRQHandler USART3_IRQHandler

// #define huart_l1s3 huart5 /* usart, connect to l1s 3 */
// #undef L1S3_IRQHandler
// #define L1S3_IRQHandler UART5_IRQHandler

extern UART_HandleTypeDef huart_l1s0;
extern UART_HandleTypeDef huart_l1s1;
extern UART_HandleTypeDef huart_l1s2;
// extern UART_HandleTypeDef huart_l1s3;

#define BUFSIZ_L1S  0x10
#define BUFSIZ_L1S0 BUFSIZ_L1S
#define BUFSIZ_L1S1 BUFSIZ_L1S
#define BUFSIZ_L1S2 BUFSIZ_L1S
#define BUFSIZ_L1S3 BUFSIZ_L1S

#define FLAG_L1S0 (1 << 0)
#define FLAG_L1S1 (1 << 1)
#define FLAG_L1S2 (1 << 2)
#define FLAG_L1S3 (1 << 3)

extern l1s_t l1s;

__BEGIN_DECLS

extern void l1s_config(UART_HandleTypeDef *huart);
extern void l1s_start(UART_HandleTypeDef *huart);
extern void l1s_stop(UART_HandleTypeDef *huart);
extern void l1s_init(void);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_L1S_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
