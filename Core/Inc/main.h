/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_OK_Pin GPIO_PIN_2
#define SW_OK_GPIO_Port GPIOE
#define SW_CAL_Pin GPIO_PIN_3
#define SW_CAL_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_6
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_13
#define LED4_GPIO_Port GPIOC
#define UART4_EN_Pin GPIO_PIN_3
#define UART4_EN_GPIO_Port GPIOC
#define UART2_EN_Pin GPIO_PIN_4
#define UART2_EN_GPIO_Port GPIOA
#define COLL_LDL_RET_Pin GPIO_PIN_9
#define COLL_LDL_RET_GPIO_Port GPIOE
#define COLL_LDL_EXT_Pin GPIO_PIN_10
#define COLL_LDL_EXT_GPIO_Port GPIOE
#define COLL_LDL_RIGHT_Pin GPIO_PIN_11
#define COLL_LDL_RIGHT_GPIO_Port GPIOE
#define COLL_LDL_LEFT_Pin GPIO_PIN_12
#define COLL_LDL_LEFT_GPIO_Port GPIOE
#define COLL_TRIM_DOWN_Pin GPIO_PIN_13
#define COLL_TRIM_DOWN_GPIO_Port GPIOE
#define COLL_TRIM_UP_Pin GPIO_PIN_14
#define COLL_TRIM_UP_GPIO_Port GPIOE
#define COLL_TRIM_RIGHT_Pin GPIO_PIN_15
#define COLL_TRIM_RIGHT_GPIO_Port GPIOE
#define COLL_TRIM_LEFT_Pin GPIO_PIN_10
#define COLL_TRIM_LEFT_GPIO_Port GPIOB
#define COLL_HOIST_DN_Pin GPIO_PIN_11
#define COLL_HOIST_DN_GPIO_Port GPIOB
#define COLL_HOIST_UP_Pin GPIO_PIN_12
#define COLL_HOIST_UP_GPIO_Port GPIOB
#define COLL_RPM_100_Pin GPIO_PIN_13
#define COLL_RPM_100_GPIO_Port GPIOB
#define COLL_ENG2_DEC_Pin GPIO_PIN_14
#define COLL_ENG2_DEC_GPIO_Port GPIOB
#define COLL_ENG2_INC_Pin GPIO_PIN_15
#define COLL_ENG2_INC_GPIO_Port GPIOB
#define COLL_ENG1_DEC_Pin GPIO_PIN_8
#define COLL_ENG1_DEC_GPIO_Port GPIOD
#define COLL_ENG1_INC_Pin GPIO_PIN_9
#define COLL_ENG1_INC_GPIO_Port GPIOD
#define COLL_LDL_OVRD_Pin GPIO_PIN_10
#define COLL_LDL_OVRD_GPIO_Port GPIOD
#define COLL_ENG2_MAN_Pin GPIO_PIN_11
#define COLL_ENG2_MAN_GPIO_Port GPIOD
#define COLL_ENG1_MAN_Pin GPIO_PIN_12
#define COLL_ENG1_MAN_GPIO_Port GPIOD
#define COLL_HOIST_CUT_Pin GPIO_PIN_13
#define COLL_HOIST_CUT_GPIO_Port GPIOD
#define COLL_FLOAT_Pin GPIO_PIN_14
#define COLL_FLOAT_GPIO_Port GPIOD
#define COLL_CATA_Pin GPIO_PIN_15
#define COLL_CATA_GPIO_Port GPIOD
#define COLL_TQ_LIM_Pin GPIO_PIN_6
#define COLL_TQ_LIM_GPIO_Port GPIOC
#define COLL_CDR_Pin GPIO_PIN_7
#define COLL_CDR_GPIO_Port GPIOC
#define COLL_GRIP_2_Pin GPIO_PIN_8
#define COLL_GRIP_2_GPIO_Port GPIOC
#define COLL_GRIP_1_Pin GPIO_PIN_9
#define COLL_GRIP_1_GPIO_Port GPIOC
#define COLL_COM_EN_Pin GPIO_PIN_8
#define COLL_COM_EN_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_7
#define FLASH_CS_GPIO_Port GPIOD
#define COLL_STEP_PUL_Pin GPIO_PIN_6
#define COLL_STEP_PUL_GPIO_Port GPIOB
#define COLL_STEP_DIR_Pin GPIO_PIN_7
#define COLL_STEP_DIR_GPIO_Port GPIOB
#define COLL_STEP_EN_Pin GPIO_PIN_8
#define COLL_STEP_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
