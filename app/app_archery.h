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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ARCHERY_H__
#define __APP_ARCHERY_H__

#include "app.h"

#define ARCHERY_JET_TIME_MS     200
#define ARCHERY_CONTROL_TIME_MS 2
#define ARCHERY_CONTROL_TIME    0.002F

#define ARCHERY_LOAD_NONE (0)       //!< none
#define ARCHERY_CLIP_FL   (1 << 0)  //!< clip arrow device on the far left
#define ARCHERY_CLIP_L    (1 << 1)  //!< clip arrow device on the left
#define ARCHERY_CLIP_M    (1 << 2)  //!< clip arrow device in the middle
#define ARCHERY_CLIP_R    (1 << 3)  //!< clip arrow device on the right
#define ARCHERY_CLIP_FR   (1 << 4)  //!< clip arrow device on the far right
#define ARCHERY_LOAD_L    (1 << 5)  //!< load arrow device on the left
#define ARCHERY_LOAD_M    (1 << 6)  //!< load arrow device in the middle
#define ARCHERY_LOAD_R    (1 << 7)  //!< load arrow device on the right
/* clip arrow device at the edge */
#define ARCHERY_CLIP_F (ARCHERY_CLIP_FL | ARCHERY_CLIP_FR)

#define ARCHERY_AIM_NONE (0)       //!< none
#define ARCHERY_AIM_DO   (1 << 0)  //!< aim do
#define ARCHERY_AIM_DONE (1 << 1)  //!< aim done

#define ARCHERY_JET_NONE   (0)       //!< none
#define ARCHERY_JET_ON     (1 << 0)  //!< open the jet
#define ARCHERY_JET_OFF    (1 << 1)  //!< shot down the jet
#define ARCHERY_JET_CNT    (1 << 2)  //!< jet counting
#define ARCHERY_JET_LEFT   (1 << 4)  //!< jet on the left
#define ARCHERY_JET_MIDDLE (1 << 5)  //!< jet in the middle
#define ARCHERY_JET_RIGHT  (1 << 6)  //!< jet on the right
#define ARCHERY_JET_COUNT  (ARCHERY_JET_TIME_MS / ARCHERY_CONTROL_TIME_MS)

typedef struct
{
    int load;  //!< stats of loading
    int aim;   //!< stats of aiming
    int jet;   //!< stats of jet

    unsigned int jet_count;  //!< count of jet time
    uint32_t tick;           //!< count of run
} archery_t;

__BEGIN_DECLS

__END_DECLS

/* Enddef to prevent recursive inclusion -------------------------------------*/
#endif /* __APP_ARCHERY_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
