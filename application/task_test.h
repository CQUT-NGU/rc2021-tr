/**
 * *****************************************************************************
 * @file         task_test.h
 * @brief        test task
 * @author       NGU
 * @date         20210101
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASK_TEST_H__
#define __TASK_TEST_H__

#include <stdint.h>

#undef __BEGIN_DECLS
#undef __END_DECLS

#if defined(__cplusplus)
#define __BEGIN_DECLS \
    extern "C"        \
    {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

__BEGIN_DECLS

extern void task_test(void *pvParameters);

__END_DECLS

/* Terminate definition to prevent recursive inclusion -----------------------*/
#endif /* __TASK_TEST_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
