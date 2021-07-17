/**
 * *****************************************************************************
 * @file         app_archery.h
 * @brief        archery application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ARCHERY_H__
#define __APP_ARCHERY_H__

#include "app.h"

#define ARCHERY_JET_TIME_MS     250
#define ARCHERY_CONTROL_TIME_MS 2
#define ARCHERY_CONTROL_TIME    0.002F

#define ARCHERY_LOAD_RESET (0)       //!< reset
#define ARCHERY_CLIP_FL    (1 << 0)  //!< clip arrow device on the far left
#define ARCHERY_CLIP_L     (1 << 1)  //!< clip arrow device on the left
#define ARCHERY_CLIP_M     (1 << 2)  //!< clip arrow device in the middle
#define ARCHERY_CLIP_R     (1 << 3)  //!< clip arrow device on the right
#define ARCHERY_CLIP_FR    (1 << 4)  //!< clip arrow device on the far right
#define ARCHERY_LOAD_L     (1 << 5)  //!< load arrow device on the left
#define ARCHERY_LOAD_M     (1 << 6)  //!< load arrow device in the middle
#define ARCHERY_LOAD_R     (1 << 7)  //!< load arrow device on the right
/* clip arrow device at the edge */
#define ARCHERY_CLIP_F (ARCHERY_CLIP_FL | ARCHERY_CLIP_FR)

#define ARCHERY_SIGNAL_RESET (0)       //!< reset
#define ARCHERY_SIGNAL_DO    (1 << 0)  //!< signal do
#define ARCHERY_SIGNAL_DONE  (1 << 1)  //!< signal done

#define ARCHERY_JET_RESET  (0)       //!< reset
#define ARCHERY_JET_ON     (1 << 0)  //!< open the jet
#define ARCHERY_JET_OFF    (1 << 1)  //!< shot down the jet
#define ARCHERY_JET_CNT    (1 << 2)  //!< jet counting
#define ARCHERY_JET_LEFT   (1 << 4)  //!< jet on the left
#define ARCHERY_JET_MIDDLE (1 << 5)  //!< jet in the middle
#define ARCHERY_JET_RIGHT  (1 << 6)  //!< jet on the right
#define ARCHERY_JET_COUNT  (ARCHERY_JET_TIME_MS / ARCHERY_CONTROL_TIME_MS)

#define ARCHERY_TASK_RESET (0)       //!< reset
#define ARCHERY_TASK_ARROW (1 << 0)  //!< run arrow

typedef struct
{
    int jet;     //!< state of jet
    int task;    //!< state of task
    int load;    //!< state of loading
    int signal;  //!< state of signaling

    unsigned int jet_count;  //!< count of jet time
    uint32_t tick;           //!< count of run
    char msg[2];             //!< content of message
} archery_t;

extern archery_t archery;

__BEGIN_DECLS

__END_DECLS

__STATIC_INLINE
void clip_edge_on(void)
{
    gpio_pin_set(RELAY0_GPIO_Port, RELAY0_Pin);
    SET_BIT(archery.load, ARCHERY_CLIP_F);
}

__STATIC_INLINE
void clip_edge_off(void)
{
    gpio_pin_reset(RELAY0_GPIO_Port, RELAY0_Pin);
    CLEAR_BIT(archery.load, ARCHERY_CLIP_F);
}

__STATIC_INLINE
void clip_left_on(void)
{
    gpio_pin_set(RELAY1_GPIO_Port, RELAY1_Pin);
    SET_BIT(archery.load, ARCHERY_CLIP_L);
}

__STATIC_INLINE
void clip_left_off(void)
{
    gpio_pin_reset(RELAY1_GPIO_Port, RELAY1_Pin);
    CLEAR_BIT(archery.load, ARCHERY_CLIP_L);
}

__STATIC_INLINE
void clip_middle_on(void)
{
    gpio_pin_set(RELAY2_GPIO_Port, RELAY2_Pin);
    SET_BIT(archery.load, ARCHERY_CLIP_M);
}

__STATIC_INLINE
void clip_middle_off(void)
{
    gpio_pin_reset(RELAY2_GPIO_Port, RELAY2_Pin);
    CLEAR_BIT(archery.load, ARCHERY_CLIP_M);
}

__STATIC_INLINE
void clip_right_on(void)
{
    gpio_pin_set(RELAY3_GPIO_Port, RELAY3_Pin);
    SET_BIT(archery.load, ARCHERY_CLIP_R);
}

__STATIC_INLINE
void clip_right_off(void)
{
    gpio_pin_reset(RELAY3_GPIO_Port, RELAY3_Pin);
    CLEAR_BIT(archery.load, ARCHERY_CLIP_R);
}

__STATIC_INLINE
void jet_left_on(void)
{
    if (!READ_BIT(archery.jet, ARCHERY_JET_ON))
    {
        CLEAR_BIT(archery.load, ARCHERY_LOAD_L);
        gpio_pin_set(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
        SET_BIT(archery.jet, ARCHERY_JET_ON | ARCHERY_JET_CNT | ARCHERY_JET_LEFT);
    }
}

__STATIC_INLINE
void jet_middle_on(void)
{
    if (!READ_BIT(archery.jet, ARCHERY_JET_ON))
    {
        CLEAR_BIT(archery.load, ARCHERY_LOAD_M);
        gpio_pin_set(POWER2_LD_GPIO_Port, POWER2_LD_Pin);
        SET_BIT(archery.jet, ARCHERY_JET_ON | ARCHERY_JET_CNT | ARCHERY_JET_MIDDLE);
    }
}

__STATIC_INLINE
void jet_right_on(void)
{
    if (!READ_BIT(archery.jet, ARCHERY_JET_ON))
    {
        CLEAR_BIT(archery.load, ARCHERY_LOAD_R);
        gpio_pin_set(POWER4_RD_GPIO_Port, POWER4_RD_Pin);
        SET_BIT(archery.jet, ARCHERY_JET_ON | ARCHERY_JET_CNT | ARCHERY_JET_RIGHT);
    }
}

__STATIC_INLINE
void jet_off(void)
{
    if (READ_BIT(archery.jet, ARCHERY_JET_OFF))
    {
        CLEAR_BIT(archery.jet, ARCHERY_JET_ON | ARCHERY_JET_OFF);
    }
}

__STATIC_INLINE
void signal_on(const void *data)
{
    if (!READ_BIT(archery.signal, ARCHERY_SIGNAL_DONE))
    {
        SET_BIT(archery.signal, ARCHERY_SIGNAL_DO);
        const char *msg = (const char *)data;
        archery.msg[0] = msg[0];
        archery.msg[1] = msg[1];
    }
}

__STATIC_INLINE
void signal_off(const void *data)
{
    if (READ_BIT(archery.signal, ARCHERY_SIGNAL_DONE))
    {
        const char *msg = (const char *)data;
        if (archery.msg[0] == msg[0])
        {
            CLEAR_BIT(archery.signal, ARCHERY_SIGNAL_DONE);
        }
    }
}

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __APP_ARCHERY_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
