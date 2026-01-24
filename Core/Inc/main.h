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
#ifdef USE_HAL_ERRORHANDLER
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#endif

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_OK_Pin GPIO_PIN_2
#define SW_OK_GPIO_Port GPIOE
#define SW_CAL_Pin GPIO_PIN_3
#define SW_CAL_GPIO_Port GPIOE
#define SW_CAL_EXTI_IRQn EXTI3_IRQn
#define LED_1_Pin GPIO_PIN_4
#define LED_1_GPIO_Port GPIOE
#define LED_2_Pin GPIO_PIN_5
#define LED_2_GPIO_Port GPIOE
#define LED_3_Pin GPIO_PIN_6
#define LED_3_GPIO_Port GPIOE
#define LED_4_Pin GPIO_PIN_13
#define LED_4_GPIO_Port GPIOC
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
#define COLL_CDR_EXTI_IRQn EXTI9_5_IRQn
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
#define NUM_ADC_CHANNEL             (1)
#define NUM_AXIS                    (1)
#define UINT16_T_MAX				(65536)
#define THRUST_ALPHA 				(0.3f)
#define THRUST_BUTTON_MASK			((uint32_t)0x001FEFFFF)
#define AXIS_RANGE					(2048)
#define SAMPLE_TIME_MS              (3)

//                                    00000000 00000000 00000000 00000000
#define GPIOB_BITMASK               (0x0000FC00)    //0b11111100 00000000
#define GPIOC_BITMASK               (0x000003C0)    //0b00000011 11000000
#define GPIOD_BITMASK               (0x0000FF00)    //0b11111111 00000000
#define GPIOE_BITMASK               (0x0000FE00)    //0b11111110 00000000
#define GPIOB_OFFSET                (9)
#define GPIOC_OFFSET                (9)
#define GPIOD_OFFSET                (8)
#define GPIOE_OFFSET                (1)
#define MAG_REL_PRESSED(x)          (x & 0x00010000)

#define RX_BUFFER_SIZE      (1024)
#define UART_BUFFER_SIZE RX_BUFFER_SIZE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
