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

#define SHIFTH_INDEX_ZERO   0
#define SHIFTH_INDEX_LEFT   2000
#define SHIFTH_INDEX_MIDDLE 155000
#define SHIFTH_INDEX_RIGHT  311000
#define SHIFTH_INDEX_CLI    -330000

#define SHIFTH_PWM_DIVIDE 6400
#define SHIFTH_PWM_DELTA  400

#define SHIFTH_FLAG_REVERSE (1 << 0)
#define SHIFTH_FLAG_AUTO    (1 << 1)
#define SHIFTH_FLAG_ZERO    (1 << 2)

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
extern uint32_t shifth_set_dir(int32_t offset);
extern void shifth_set_freq(uint32_t hz);
extern void shifth_zero_cli(int32_t idx);
extern void shifth_index(uint32_t idx);
extern void shifth_start(int32_t offset);
extern void shifth_stop(void);
extern void shifth_update(uint32_t inc, uint32_t cnt);

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __CTRL_STEP_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
