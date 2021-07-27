/**
 * *****************************************************************************
 * @file         app_shoot.h
 * @brief        shoot application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_shoot.h"

TaskHandle_t task_shoot_handler;

#define SHOOT_DELAY_MIN  10
#define SHOOT_DELAY_WAIT 500

void task_shoot(void *pvParameters)
{
    (void)pvParameters;

    task_shoot_handler = xTaskGetCurrentTaskHandle();

    for (;;)
    {
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)
        {
        }

        if (READ_BIT(archery.load, ARCHERY_LOAD_ALL))
        {
            CLEAR_BIT(archery.wait, ARCHERY_WAIT_SHOOT);
            do
            {
                osDelay(SHOOT_DELAY_MIN);
            } while (READ_BIT(archery.jet, ARCHERY_JET_COUNT));
            osDelay(SHOOT_DELAY_WAIT);
        }

        if (READ_BIT(archery.load, ARCHERY_CLIP_L))
        {
            shifth_index(SHIFTH_INDEX_LEFT);
        }
        else if (READ_BIT(archery.load, ARCHERY_CLIP_M))
        {
            shifth_index(SHIFTH_INDEX_MIDDLE);
        }
        else if (READ_BIT(archery.load, ARCHERY_CLIP_R))
        {
            shifth_index(SHIFTH_INDEX_RIGHT);
        }
        else
        {
            shifth_index(SHIFTH_INDEX_MIDDLE);
        }

        if (READ_BIT(archery.load, ARCHERY_CLIP_ALL))
        {
            {
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(step.flag, SHIFTH_FLAG_AUTO));
            }
            /* Check whether the archery device is down */
            {
                pitch_set(SERVO_PITCH_PWMMAX);
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(servo.match, SERVO_MATCH_PITCH) != SERVO_MATCH_PITCH);
                osDelay(SHOOT_DELAY_WAIT);
            }
            /* Put the arrow */
            {
                shiftv_set(SERVO_SHIFTV_PWMMAX);
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV) != SERVO_MATCH_SHIFTV);
                osDelay(SHOOT_DELAY_WAIT << 1);
            }
            /* Loosen the arrows */
            {
                if (READ_BIT(archery.load, ARCHERY_CLIP_L))
                {
                    clip_left_off();
                }
                else if (READ_BIT(archery.load, ARCHERY_CLIP_M))
                {
                    clip_middle_off();
                }
                else if (READ_BIT(archery.load, ARCHERY_CLIP_R))
                {
                    clip_right_off();
                }
                osDelay(SHOOT_DELAY_WAIT);
            }
            /* The archery device lifts the arrow */
            {
                pitch_set_pwm(SERVO_PITCH_STD - 200);
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(servo.match, SERVO_MATCH_PITCH) != SERVO_MATCH_PITCH);
                osDelay(SHOOT_DELAY_WAIT);
            }
            /* The archery set back */
            {
                /* Check that the horizontal moving device is in proper position */
                shifth_index(SHIFTH_INDEX_MIDDLE);
                shiftv_set(SERVO_SHIFTV_PWMMIN);
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV) != SERVO_MATCH_SHIFTV);
                osDelay(SHOOT_DELAY_WAIT);
            }
            /* The archery apparatus was raised to the top to load the arrows */
            {
                pitch_set(SERVO_PITCH_STD - 400);
                do
                {
                    osDelay(SHOOT_DELAY_MIN);
                } while (READ_BIT(servo.match, SERVO_MATCH_PITCH) != SERVO_MATCH_PITCH);
                osDelay(SHOOT_DELAY_WAIT << 1);
            }

            if (READ_BIT(archery.wait, ARCHERY_WAIT_SHOOT))
            {
                archery_reday();
            }
            else
            {
                /* The archery device drops an arrow */
                pitch_set(SERVO_PITCH_STD - 150);
                do
                {
                    osDelay(SHOOT_DELAY_WAIT);
                } while (READ_BIT(servo.match, SERVO_MATCH_PITCH) != SERVO_MATCH_PITCH);
            }
        }

        if (!READ_BIT(archery.load, ARCHERY_LOAD_ALL))
        {
            fetch_set(SERVO_FETCH_PWMMAX);
            /* Prompt that the hand arrow clip is empty */
            buzzer_start();
            buzzer_set(NOTEFREQS_A5, BUZZER_PWM_DIV2);
            osDelay(200);
            buzzer_stop();
        }

        CLEAR_BIT(archery.wait, ARCHERY_WAIT_SHOOT);
        CLEAR_BIT(archery.task, ARCHERY_TASK_SHOOT);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
