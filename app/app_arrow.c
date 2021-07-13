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

        pitch_set(SERVO_PITCH_PWMMAX);
        pitchl_set(SERVO_PITCHL_PWMMAX);
        pitchr_set(SERVO_PITCHR_PWMMAX);

        /* clip is in the vertical position */
        fetch_set(SERVO_FETCH_PWMMID);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
        osDelay(1000);

        /* clip to the low */
        fetch_set(SERVO_FETCH_PWMMAX);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
        osDelay(1000);

        /* clip on all */
        clip_edge_on();
        clip_left_on();
        clip_middle_on();
        clip_right_on();
        osDelay(500);

        /* lift up */
        fetch_set(SERVO_FETCH_PWMMID);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
        osDelay(1000);

        /* backward */
        // osDelay(1000);

        /*  */
        fetch_set(SERVO_FETCH_PWMMIN);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_FETCH) != SERVO_MATCH_FETCH);
        osDelay(1000);

        shiftv_set(SERVO_SHIFTV_PWMMIN);
        shiftvl_set(SERVO_SHIFTVL_PWMMIN);
        shiftvr_set(SERVO_SHIFTVR_PWMMIN);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
        osDelay(1000);

        /* clip off in the middle and at edge */
        clip_edge_off();
        clip_middle_off();
        osDelay(500);

        pitch_set(SERVO_PITCH_PWMMAX - 200);
        pitchl_set(SERVO_PITCHL_PWMMAX - 200);
        pitchr_set(SERVO_PITCHR_PWMMAX - 200);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
        osDelay(1000);

        shiftv_set(SERVO_SHIFTV_PWMMAX);
        shiftvl_set(SERVO_SHIFTV_PWMMAX);
        shiftvr_set(SERVO_SHIFTV_PWMMAX);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_SHIFTV_ALL) != SERVO_MATCH_SHIFTV_ALL);
        osDelay(1000);

        pitch_set(SERVO_PITCH_PWMMAX - 500);
        pitchl_set(SERVO_PITCHL_PWMMAX - 500);
        pitchr_set(SERVO_PITCHR_PWMMAX - 500);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);
        osDelay(2000);

        pitch_set(SERVO_PITCH_PWMMAX - 200);
        pitchl_set(SERVO_PITCHL_PWMMAX - 200);
        pitchr_set(SERVO_PITCHR_PWMMAX - 200);
        do
        {
            osDelay(10);

        } while (READ_BIT(servo.match, SERVO_MATCH_PITCH_ALL) != SERVO_MATCH_PITCH_ALL);

        buzzer_start();
        buzzer_set(0, BUZZER_PWM_DIV2);
        osDelay(200);
        buzzer_stop();
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
