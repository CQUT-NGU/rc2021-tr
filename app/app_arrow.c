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

#define ARROW_DELAY_MIN  10
#define ARROW_DELAY_WAIT 1000

void task_arrow(void *pvParameters)
{
    (void)pvParameters;

    task_arrow_handler = xTaskGetCurrentTaskHandle();

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
            pitch_set(SERVO_PITCH_PWMMAX - 200 * 3 / 2);
            pitchl_set(SERVO_PITCHL_PWMMAX - 200);
            pitchr_set(SERVO_PITCHR_PWMMIN + 200);
        }

        /* The archery set back */
        {
            shiftv_set_pwm(SERVO_SHIFTV_PWMMIN);
            shiftvl_set_pwm(SERVO_SHIFTV_PWMMIN);
            shiftvr_set_pwm(SERVO_SHIFTV_PWMMAX);
        }

        // /* Check whether the clip arrow device is in the vertical position */
        // {
        //     fetch_set(SERVO_FETCH_PWMMID);
        //     do
        //     {
        //         osDelay(ARROW_DELAY_MIN);
        //     } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
        //     /* Check that the horizontal moving device is in proper position */
        //     osDelay(ARROW_DELAY_WAIT);
        // }

        /* Check whether the clip arrow device is aligned with its arrow */
        {
            shifth_index(SHIFTH_INDEX_LEFT);
            fetch_set(SERVO_FETCH_PWMMAX);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* Clamp all the arrows */
        {
            clip_edge_on();
            clip_left_on();
            clip_middle_on();
            clip_right_on();
            osDelay(ARROW_DELAY_WAIT);
        }

        /* Check whether the clip arrow device is in the vertical direction */
        {
            fetch_set(SERVO_FETCH_PWMMID);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(ARROW_DELAY_WAIT);
        }

        do
        {
            osDelay(ARROW_DELAY_MIN);
        } while (READ_BIT(step.flag, SHIFTH_FLAG_AUTO));

        /* Check whether the archery device is down */
        {
            pitch_set_pwm(SERVO_PITCH_PWMMAX);
            pitchl_set_pwm(SERVO_PITCHL_PWMMAX);
            pitchr_set_pwm(SERVO_PITCHR_PWMMIN);
        }

        /* The robot move back and lift the arrow */
        {
            move.vy_set = -0.5F;
            fetch_set(SERVO_FETCH_PWMMIN);
            osDelay(1000);
            move.vy_set = 0;
        }

        /* Check if the clip arrow is horizontal */
        {
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* Put the arrow */
        {
            shiftv_set(SERVO_SHIFTV_PWMMAX);
            shiftvl_set(SERVO_SHIFTVL_PWMMAX);
            shiftvr_set(SERVO_SHIFTVR_PWMMIN);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* Loosen the arrows on both sides and in the middle */
        {
            clip_edge_off();
            clip_left_off();
            osDelay(ARROW_DELAY_WAIT >> 1);
        }

        /* The archery device lifts the arrow */
        {
            pitch_set_pwm(SERVO_PITCH_PWMMAX - 200 * 3 / 2);
            pitchl_set_pwm(SERVO_PITCHL_PWMMAX - 200);
            pitchr_set_pwm(SERVO_PITCHR_PWMMIN + 200);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* The archery set back */
        {
            shiftv_set(SERVO_SHIFTV_PWMMIN);
            shiftvl_set(SERVO_SHIFTV_PWMMIN);
            shiftvr_set(SERVO_SHIFTV_PWMMAX);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* The archery apparatus was raised to the top to load the arrows */
        {
            pitch_set(SERVO_PITCH_PWMMAX - 400 * 3 / 2);
            pitchl_set(SERVO_PITCHL_PWMMAX - 400);
            pitchr_set(SERVO_PITCHR_PWMMIN + 400);
            do
            {
                osDelay(ARROW_DELAY_MIN);
            } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
            osDelay(ARROW_DELAY_WAIT);
        }

        /* Check that the horizontal moving device is in proper position */
        {
            shifth_index(SHIFTH_INDEX_MIDDLE);
        }

        /* The archery device drops an arrow */
        {
            pitch_set(SERVO_PITCH_PWMMAX - 150 * 3 / 2);
            pitchl_set(SERVO_PITCHL_PWMMAX - 150);
            pitchr_set(SERVO_PITCHR_PWMMIN + 150);
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
