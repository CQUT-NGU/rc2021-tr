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

#define SHOOT_COUNT      100
#define FLAG_SHOOT_START (1 << 0)
#define FLAG_SHOOT_COUNT (1 << 1)
#define FLAG_SHOOT_STOP  (1 << 2)

#define SIGNAL_AIMING_NONE (1 << 0)  //!< none
#define SIGNAL_AIMING_DO   (1 << 1)  //!< aim do
#define SIGNAL_AIMING_DONE (1 << 2)  //!< aim done

archery_t archery = {
    .load = ARCHERY_LOAD_NONE,
    .aim = ARCHERY_AIM_NONE,
    .jet = ARCHERY_JET_NONE,
    .jet_count = 0,
};

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
        gpio_pin_set(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
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
        gpio_pin_set(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
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
void aim_on(void)
{
    if (!READ_BIT(archery.aim, ARCHERY_AIM_DONE))
    {
        SET_BIT(archery.aim, ARCHERY_AIM_DO);
    }
}

__STATIC_INLINE
void aim_off(void)
{
    if (READ_BIT(archery.aim, ARCHERY_AIM_DONE))
    {
        CLEAR_BIT(archery.aim, ARCHERY_AIM_DONE);
    }
}

void archery_update(void)
{
    if (READ_BIT(archery.aim, ARCHERY_AIM_DO))
    {
        CLEAR_BIT(archery.aim, ARCHERY_AIM_DO);
        SET_BIT(archery.aim, ARCHERY_AIM_DONE);
        usart_dma_tx(&huart_os, (const void *)"a\n", 2);
    }

    if (READ_BIT(archery.jet, ARCHERY_JET_CNT) &&
        archery.jet_count++ == ARCHERY_JET_COUNT)
    {
        archery.jet_count = 0;
        SET_BIT(archery.jet, ARCHERY_JET_OFF);
        if (READ_BIT(archery.jet, ARCHERY_JET_LEFT))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_LEFT);
            gpio_pin_reset(POWER1_LU_GPIO_Port, POWER1_LU_Pin);
        }
        if (READ_BIT(archery.jet, ARCHERY_JET_MIDDLE))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_MIDDLE);
            gpio_pin_reset(POWER2_LD_GPIO_Port, POWER2_LD_Pin);
        }
        if (READ_BIT(archery.jet, ARCHERY_JET_RIGHT))
        {
            CLEAR_BIT(archery.jet, ARCHERY_JET_CNT | ARCHERY_JET_RIGHT);
            gpio_pin_reset(POWER3_RU_GPIO_Port, POWER3_RU_Pin);
        }
        usart_dma_tx(&huart_os, (const void *)"b\n", 2);
    }

    shifth_update();
    if (archery.tick % 2 == 0)
    {
        /**
         * Updated arrow steering gear Angle,
         * prevent the PWM from changing too much to
         * cause the steering gear to jam
        */
        fetch_update();
        pitch_update();
    }

    ++archery.tick;
}

void task_archery(void *pvParameters)
{
    (void)pvParameters;

    osDelay(1);

    ctrl_serial_t *serial = ctrl_serial_point();
    const ctrl_rc_t *rc = ctrl_rc_point();

    {
        shifth_init();
        servo_init();
        servo_start();
    }

    {
        pitch_init(1700);
        fetch_init(1100);
    }

    for (;;)
    {
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
            if (rc->rc.ch[RC_CH_LV] > 220)
            {
                aim_on();
            }

            /* End sending aiming signal */
            else if (rc->rc.ch[RC_CH_LV] < 220)
            {
                aim_off();
            }

            /* relay control, shoot an arrow */
            if (rc->rc.ch[RC_CH_S] < -650)
            {
                /* It starts to spew out gas */
                jet_left_on();
            }
            else
            {
                /* Stop ejecting gas */
                jet_off();
            }
        }

        archery_update();

        osDelay(2);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
