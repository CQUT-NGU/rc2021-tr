/**
 * *****************************************************************************
 * @file         task_step.c
 * @brief        stepping motor control task
 * @author       NGU
 * @date         20210509
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "task_step.h"

#include "ca.h"
#include "bsp.h"
#include "ctrl.h"

#if USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

#include <stdint.h>

void task_step(void *pvParameters)
{
    (void)pvParameters;

    osDelay(1000);

    const ctrl_rc_t *rc = ctrl_rc_point();

    ctrl_serial_t *serial = ctrl_serial_point();

    for (;;)
    {
        if (serial->c == 'h')
        {
            serial->c = 0;
            shifth_start((int32_t)serial->x);
        }

        osDelay(2);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
