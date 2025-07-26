/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart.h"
#include "com_packet.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE		(128)
#define TX_BUFFER_SIZE		(128)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  MX_CRC_Init();
  MX_UART4_Init();
  MX_TIM8_Init();
  MX_UART5_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  uart_handle_t uart4, uart3;
  uart_status_t uart_status = uart_init(&uart4, &huart4);
  (void)uart_init(&uart3, &huart3);
  if(uart_status != UART_OK){
	  // uart init failed
  }

  com_packet_t packet;
  com_packet_init(&packet, &hcrc);
  uint8_t crctest[5] = {'t','e','s','t','\n'};
  com_packet_create(&packet, crctest, 5);
  (void)uart_write(&uart3,crctest,5);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t temp[RX_BUFFER_SIZE] = {0};
  uint8_t debuff[RX_BUFFER_SIZE] = {0};
 /* while(1)  {
    if(uart3.unread_bytes >0)   {
		  memset(temp,'\0', RX_BUFFER_SIZE);
          uint32_t bytes_read = uart_read(&uart3, temp, RX_BUFFER_SIZE);
        temp[bytes_read] = '\0';
          if(bytes_read>0)  {
              memset(debuff,'\0', RX_BUFFER_SIZE);
              bytes_read = (uint32_t)snprintf((char*)debuff, RX_BUFFER_SIZE, "Received: %s\n", (char*)temp);
            (void)uart_write(&uart3, debuff, bytes_read);
          }
    }
  }*/
  volatile uint32_t counter =0;
  uint64_t timer = HAL_GetTick();
  uint64_t tester = HAL_GetTick();
  while (1)
  {

	  uart_update(&uart3);
	  if(uart_update(&uart4) == UART_RX_FULL)	{
		  // restart uart/dma/fifo
		  __asm__("NOP");
	  }
	  if (uart4.unread_bytes > 0 ){//&& HAL_GetTick() - timer >= 1){
		  // handle new uart data
		 /* memset(temp,'\0', RX_BUFFER_SIZE);
          uint32_t bytes_read = uart_read(&uart4, temp, RX_BUFFER_SIZE);
        temp[bytes_read] = '\0';
          if(bytes_read>0)  {
        	  counter++;
              memset(debuff,'\0', RX_BUFFER_SIZE);
              bytes_read = (uint32_t)snprintf((char*)debuff, RX_BUFFER_SIZE, "Received:(%lu) %s\n", counter, (char*)temp);

            bytes_read = uart_write(&uart4, debuff, bytes_read);
            bytes_read = uart_write(&uart3, debuff, bytes_read);
            timer = HAL_GetTick();

          }
        */
		  uint32_t bytes_read =  fifo_peek_continuous(&uart4.rx_fifo, temp, uart4.unread_bytes);

		  if(com_packet_parse(&packet,temp,bytes_read)!=COM_PACKET_FALSE)	{
			  fifo_push_read_index(&uart4.rx_fifo, bytes_read);
			  memset(temp,0, RX_BUFFER_SIZE);
			  packet.payload[packet.payload_length] = '\0';
			  bytes_read = (uint32_t)snprintf((char*)temp, RX_BUFFER_SIZE, "Received: %s\n", (char*)packet.payload);
			  //com_packet_create(&packet, temp, bytes_read);
			  com_packet_create_special(&packet, COM_PACKET_ACK);
			  (void)uart_write_packet(&uart4, &packet);
			  (void)uart_write(&uart3, temp, bytes_read);
			  timer = HAL_GetTick();

		  }
		  /*else {
			  memset(debuff,0, RX_BUFFER_SIZE);
			  temp[bytes_read] = '\0';
			  bytes_read = (uint32_t)snprintf((char*)debuff, RX_BUFFER_SIZE, "Failed check: %s\n", (char*)temp);
			  (void)uart_write(&uart3, debuff, bytes_read);

		  }
		   */
	  }
	/*  if(HAL_GetTick()-tester >= 50)	{
		  memset(temp,0, RX_BUFFER_SIZE);
		  if(counter==80){
			  __asm__("NOP");
		  }
	  	  uint32_t bytes_read = (uint32_t)snprintf((char*)temp, RX_BUFFER_SIZE, "Packet: %lu\n", counter);
	  	  counter++;
	  	  (void)uart_write(&uart3, temp, bytes_read);
	  	  tester = HAL_GetTick();
	  }
	  */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     example: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
