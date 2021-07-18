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

void task_shoot(void *pvParameters)
{
    (void)pvParameters;

    task_shoot_handler = xTaskGetHandle(pcTaskGetName(NULL));

    for (;;)
    {
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)
        {
        }

        if (READ_BIT(archery.load, ARCHERY_LOAD_M))
        {
        }
        else
        {
        }

        if (READ_BIT(archery.load, ARCHERY_LOAD_L))
        {
        }
        else if (READ_BIT(archery.load, ARCHERY_LOAD_R))
        {
        }
        else
        {
        }

        CLEAR_BIT(archery.task, ARCHERY_TASK_SHOOT);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
