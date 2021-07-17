/**
 * *****************************************************************************
 * @file         app_arrow.c
 * @brief        arrow application
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "app_arrow.h"

TaskHandle_t task_arrow_handler;

void task_arrow(void *pvParameters)
{
    (void)pvParameters;

    task_arrow_handler = xTaskGetHandle(pcTaskGetName(NULL));

    for (;;)
    {
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)
        {
        }

        /* Indicates that the operator task is about to begin */
        {
            buzzer_start();
            buzzer_set(NOTEFREQS_A5, BUZZER_PWM_DIV2);
            osDelay(200);
            buzzer_stop();
        }

        /* Check whether the pitching device is in the middle position */
        {
            pitch_set_pwm(SERVO_PITCH_PWMMAX - 200);
            pitchl_set_pwm(SERVO_PITCHL_PWMMAX - 200);
            pitchr_set_pwm(SERVO_PITCHR_PWMMAX - 200);
        }

        /* Check whether the clip arrow device is in the vertical position */
        {
            fetch_set(SERVO_FETCH_PWMMID);
            do
            {
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(1000);
        }

        /* Check that the horizontal moving device is in proper position */
        {
            shifth_index(SHIFTH_INDEX_MIDDLE);
        }

        /* Check whether the clip arrow device is aligned with its arrow */
        {
            fetch_set(SERVO_FETCH_PWMMAX);
            do
            {
                osDelay(SERVO_UPDATE_MS_FETCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(1000);
        }

        /* Clamp all the arrows */
        {
            clip_edge_on();
            clip_left_on();
            clip_middle_on();
            clip_right_on();
            osDelay(500);
        }

        /* Check whether the clip arrow device is in the vertical direction */
        {
            fetch_set(SERVO_FETCH_PWMMID);
            do
            {
                osDelay(SERVO_UPDATE_MS_FETCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(1000);
        }

        /* Check whether the archery device is down */
        {
            pitch_set(SERVO_PITCH_PWMMAX);
            pitchl_set(SERVO_PITCHL_PWMMAX);
            pitchr_set(SERVO_PITCHR_PWMMAX);
        }

        /* The robot move back and lift the arrow */
        {
            // move.vy_set = -0.5F;
            fetch_set(SERVO_FETCH_PWMMIN);
            osDelay(1000);
            move.vy_set = 0;
        }

        /* Check if the clip arrow is horizontal */
        {
            do
            {
                osDelay(SERVO_UPDATE_MS_FETCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(100);
        }

        /* Put the arrow */
        {
            shiftv_set(SERVO_SHIFTV_PWMMAX + 400);
            shiftvl_set(SERVO_SHIFTVL_PWMMAX + 400);
            shiftvr_set(SERVO_SHIFTVR_PWMMAX + 400);
            do
            {
                osDelay(SERVO_UPDATE_MS_SHIFTV);
            } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
            osDelay(1000);
        }

        /* Loosen the arrows on both sides and in the middle */
        {
            clip_edge_off();
            clip_middle_off();
            osDelay(500);
        }

        /* The archery device lifts the arrow */
        {
            pitch_set(SERVO_PITCH_PWMMAX - 200);
            pitchl_set(SERVO_PITCHL_PWMMAX - 200);
            pitchr_set(SERVO_PITCHR_PWMMAX - 200);
            do
            {
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
            osDelay(1000);
        }

        /* The archery set back */
        {
            shiftv_set(SERVO_SHIFTV_PWMMIN);
            shiftvl_set(SERVO_SHIFTV_PWMMIN);
            shiftvr_set(SERVO_SHIFTV_PWMMIN);
            do
            {
                osDelay(SERVO_UPDATE_MS_SHIFTV);
            } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
            osDelay(1000);
        }

        /* The archery apparatus was raised to the top to load the arrows */
        {
            pitch_set(SERVO_PITCH_PWMMAX - 400);
            pitchl_set(SERVO_PITCHL_PWMMAX - 400);
            pitchr_set(SERVO_PITCHR_PWMMAX - 400);
            do
            {
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
            osDelay(1000);
        }

        /* The archery device drops an arrow */
        {
            pitch_set(SERVO_PITCH_PWMMAX - 200);
            pitchl_set(SERVO_PITCHL_PWMMAX - 200);
            pitchr_set(SERVO_PITCHR_PWMMAX - 200);
            do
            {
                osDelay(SERVO_UPDATE_MS_PITCH);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
        }

        /* Indicates that the operator task has been completed */
        {
            buzzer_start();
            buzzer_set(NOTEFREQS_A5, BUZZER_PWM_DIV2);
            osDelay(200);
            buzzer_stop();
        }

        CLEAR_BIT(archery.task, ARCHERY_TASK_ARROW);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
