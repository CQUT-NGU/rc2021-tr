/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for led */
osThreadId_t ledHandle;
uint32_t ledBuffer[ 128 ];
osStaticThreadDef_t ledControlBlock;
const osThreadAttr_t led_attributes = {
  .name = "led",
  .stack_mem = &ledBuffer[0],
  .stack_size = sizeof(ledBuffer),
  .cb_mem = &ledControlBlock,
  .cb_size = sizeof(ledControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for chassis */
osThreadId_t chassisHandle;
uint32_t chassisBuffer[ 512 ];
osStaticThreadDef_t chassisControlBlock;
const osThreadAttr_t chassis_attributes = {
  .name = "chassis",
  .stack_mem = &chassisBuffer[0],
  .stack_size = sizeof(chassisBuffer),
  .cb_mem = &chassisControlBlock,
  .cb_size = sizeof(chassisControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for imu */
osThreadId_t imuHandle;
uint32_t imuBuffer[ 512 ];
osStaticThreadDef_t imuControlBlock;
const osThreadAttr_t imu_attributes = {
  .name = "imu",
  .stack_mem = &imuBuffer[0],
  .stack_size = sizeof(imuBuffer),
  .cb_mem = &imuControlBlock,
  .cb_size = sizeof(imuControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for servo */
osThreadId_t servoHandle;
uint32_t servoBuffer[ 128 ];
osStaticThreadDef_t servoControlBlock;
const osThreadAttr_t servo_attributes = {
  .name = "servo",
  .stack_mem = &servoBuffer[0],
  .stack_size = sizeof(servoBuffer),
  .cb_mem = &servoControlBlock,
  .cb_size = sizeof(servoControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for step */
osThreadId_t stepHandle;
uint32_t stepBuffer[ 128 ];
osStaticThreadDef_t stepControlBlock;
const osThreadAttr_t step_attributes = {
  .name = "step",
  .stack_mem = &stepBuffer[0],
  .stack_size = sizeof(stepBuffer),
  .cb_mem = &stepControlBlock,
  .cb_size = sizeof(stepControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task_led(void *argument);
extern void task_chassis(void *argument);
extern void task_imu(void *argument);
extern void task_servo(void *argument);
extern void task_step(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of led */
  ledHandle = osThreadNew(task_led, NULL, &led_attributes);

  /* creation of chassis */
  chassisHandle = osThreadNew(task_chassis, NULL, &chassis_attributes);

  /* creation of imu */
  imuHandle = osThreadNew(task_imu, NULL, &imu_attributes);

  /* creation of servo */
  servoHandle = osThreadNew(task_servo, NULL, &servo_attributes);

  /* creation of step */
  stepHandle = osThreadNew(task_step, NULL, &step_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_task_led */
/**
  * @brief  Function implementing the led thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_task_led */
__weak void task_led(void *argument)
{
  /* USER CODE BEGIN task_led */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_led */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
