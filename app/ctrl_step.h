/**
 * *****************************************************************************
 * @file         ctrl_step.c
 * @brief        stepping motor control
 * @author       NGU
 * @date         20210502
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CTRL_STEP_H__
#define __CTRL_STEP_H__

#include "ctrl.h"

#define SHIFTH_FLAG_RUN     (1 << 0)
#define SHIFTH_FLAG_REVERSE (1 << 1)
#define SHIFTH_FLAG_CLI     (1 << 2)

typedef struct
{
    uint32_t fr;
    uint32_t cnt;
    uint32_t idx;
    uint32_t set;
    int8_t flag;
} ctrl_step_t;

extern ctrl_step_t step;

__BEGIN_DECLS

extern void shifth_init(void);
extern void shifth_cli(int32_t idx);
extern void shifth_set(uint32_t hz);
extern void shifth_index(uint32_t idx);
extern void shifth_start(int32_t count);
extern void shifth_stop(void);
extern void shifth_update(void);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_STEP_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
