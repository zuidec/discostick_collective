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
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "logger.h"
#include "com_packet.h"
#include "stepper.h"
#include "axis_control.h"
#include "input_filter.h"
#include "lfs_conf.h"
#include "dev_id_strings.h"
#include "uart.h"
#include "bus.h"
#include "spi_bus.h"

#define LOG_PREFIX  "[thrust]"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/*
typedef struct uart_port_t  {
    uart_handle_t* uart;
    com_packet_t* cpacket;
    com_packet_t* lpacket;
} uart_port_t;
*/
union collective_data_t	{
	collective_report_t report;
	uint16_t raw[3];
} collective_data;
const uint8_t dev_com_addr = COM_ADDR_COLL;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint16_t adc_buffer[NUM_ADC_CHANNEL] = {0};
volatile bool IRQ_Calibrate_Flag;
volatile bool IRQ_UART_Flag;
volatile uint32_t cdr_debounce_timer = 0;

axis_calibration_factors_t thrust_calibrations = {0};
input_filter_t thrust_filter;
stepper_handle_t thrust_motor;
bus_t f_handle;
spi_bus_ctx_t spi_ctx = { &hspi1, (void*)FLASH_CS_GPIO_Port, (uint16_t)FLASH_CS_Pin, 500 };
com_packet_t packet1, packet2, packet3, packet4, packet5, packet6;
com_packet_t *cyclic_cur_packet, *cyclic_prev_packet, *console_cur_packet, *console_prev_packet, *report_packet, *special_packet = (com_packet_t*)NULL;
static uart_handle_t uart_4,uart_3,uart_2;
uart_handle_t *uart_cyclic, *uart_log, *uart_console = (uart_handle_t*)NULL;
uart_port_t cyclic_port, console_port;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void panic(void);
static void print_boot_msg(void);
uint32_t crc_cb(uint32_t* buffer, uint32_t size);

void init_gpio_state(void);
void init_steppers(void);
void init_filesystem(void);
void init_com_packets(void);
void init_collective_input(void);

void update_collective_input(void);
void update_uart_cyclic(void);
void update_uart_port(uart_port_t*);
void update_stepper(void);

void send_report(com_addr_t);
void send_ack(com_addr_t,uart_handle_t*);
void send_nack(com_addr_t,uart_handle_t*);

void calibration_routine(void);
void calibrate_axis(axis_calibration_factors_t* cal, volatile uint16_t* adc_buf);
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
  MX_TIM1_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
    print_boot_msg();
    uart_cyclic = &uart_4;
    uart_log = &uart_3;
    uart_console = &uart_2;

    init_gpio_state();  
    if(UART_OK != uart_init(uart_console, &huart2)){
        uart_deinit(uart_console);
      if(UART_OK != uart_init(uart_console, &huart2))    {
        uart_deinit(uart_console);
          uart_log = NULL;
          log_error("%sFailed to initialize console UART",LOG_PREFIX);
        panic();
      }
    }
    if(UART_OK != uart_init(uart_log, &huart3)){
        uart_deinit(uart_log);
      if(UART_OK != uart_init(uart_log, &huart3))    {
        uart_deinit(uart_log);
          uart_log = NULL;
          log_error("%sFailed to initialize log UART",LOG_PREFIX);
        panic();
      }
    }
    if(UART_OK != uart_init(uart_cyclic, &huart4)){
        uart_deinit(uart_cyclic);
      if(UART_OK != uart_init(uart_cyclic, &huart4))    {
          log_error("%sFailed to initialize cyclic UART",LOG_PREFIX);
        panic();
      }
    }

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, NUM_ADC_CHANNEL);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

    init_steppers();
    init_filesystem();
    init_collective_input();

    cyclic_cur_packet      = &packet1;
    cyclic_prev_packet     = &packet2;
    console_cur_packet     = &packet3;
    console_prev_packet    = &packet4;
    report_packet          = &packet5;
    special_packet         = &packet6;
    init_com_packets();

    console_port.uart = uart_console;
    console_port.cpacket = console_cur_packet;
    console_port.lpacket = console_prev_packet;
    cyclic_port.uart = uart_cyclic;
    cyclic_port.cpacket = cyclic_cur_packet;
    cyclic_port.lpacket = cyclic_prev_packet;

    log_info("%sAll initialization complete, jumping to main program loop",LOG_PREFIX);
    HAL_TIM_Base_Start_IT(&htim11);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    if(IRQ_Calibrate_Flag)  {
        calibration_routine();
    }

    update_uart_port(&cyclic_port);
    update_uart_port(&console_port);
    update_stepper();

    if(IRQ_UART_Flag)  {
       update_collective_input();
       send_report(COM_ADDR_CYCLIC);
       send_report(COM_ADDR_CTRL);
       IRQ_UART_Flag = false;
       HAL_TIM_Base_Start_IT(&htim11);
    }
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)	{
    collective_data.report.thrust = input_filter_update(&thrust_filter, (float)apply_calibration(&thrust_calibrations, adc_buffer[0]));
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)	{
    switch(GPIO_Pin)    {
        case SW_CAL_Pin:
            IRQ_Calibrate_Flag = true;
            break;
        case COLL_CDR_Pin:
     /*   	if(HAL_GetTick()-cdr_debounce_timer >= 250)	{
        	cdr_debounce_timer = HAL_GetTick();
            if(MAG_REL_PRESSED(collective_data.report.buttons))  {
                    stepper_disable(&thrust_motor);
            }
            else    {
                    stepper_enable(&thrust_motor);
            }
        	}
            */
            break;
        default:
            break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)    {
    switch((uint32_t)htim->Instance)    {
        case TIM11_BASE:
            IRQ_UART_Flag = true;
            break;
        default:
            break;
    }
}

void panic(void)	{
	uint64_t time = HAL_GetTick();
	while(1)	{
		while(HAL_GetTick()-time < 250)	{;;}
			if(HAL_GetTick() - time > 500 ){
				HAL_GPIO_TogglePin(LED_4_GPIO_Port, LED_4_Pin);
				time = HAL_GetTick();
			}
		}
}

static void print_boot_msg(void) {
    log_printf("\n");
	log_printf("\033[0;37mBooting from internal flash...\n");
    uint32_t uuid[3] =  {HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()}; 
    uint32_t devid = HAL_GetDEVID();
    log_printf("\033[0;37mDevice family: %s, Device revision: 0x%04x\n", get_stm32_family_str((uint16_t)(devid & 0x0FFF)),(uint16_t)((devid >> 16) & 0xFFFF));
    log_printf("\033[0;37mDevice UID: %lu%lu%lu\n", uuid[0], uuid[1], uuid[2]);
	RCC_ClkInitTypeDef clk;
	uint32_t flashLatency;
	HAL_RCC_GetClockConfig(&clk, &flashLatency);
	uint32_t sysclk = HAL_RCC_GetSysClockFreq();
	uint32_t hclk = HAL_RCC_GetHCLKFreq();
	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
    log_printf("\033[0;37msysclk: %lu, hclk: %lu, pclk1: %lu, pclk2: %lu, flash_latency: %lu\n",
                sysclk, hclk, pclk1, pclk2, flashLatency);
    log_info("%sHAL initialization complete",LOG_PREFIX);
}

uint32_t crc_cb(uint32_t* buffer, uint32_t size)    {
    return HAL_CRC_Calculate(&hcrc, buffer, size);
}

void init_com_packets(void) {
    if(false == com_packet_init(COM_ADDR_COLL, crc_cb, HAL_GetTick))   {
        log_error("%sFailed to initialize com packet interface",LOG_PREFIX);
    }
    else    {
        com_packet_clear(cyclic_cur_packet);
        com_packet_clear(cyclic_prev_packet);
        com_packet_clear(console_cur_packet);
        com_packet_clear(console_prev_packet);
        com_packet_clear(report_packet);
        com_packet_clear(special_packet);
    }
}

void init_filesystem(void)  {
    HAL_Delay(10); // Give flash module some time to come up
    spi_bus_init(&f_handle, &spi_ctx);
    if(init_flashfs(&f_handle)<0)	{
        log_error("%sFailed to initialize filesystem. It may need to be reformatted.\nReformat now? [y/n]:", LOG_PREFIX);
        uint8_t response[1] = {0};
        HAL_UART_AbortReceive(&huart3);
        HAL_UART_Receive(&huart3, response, 1, 10000);
        if(response[0]=='y'||response[0]=='Y')  {
            log_warning("%sReformatting filesystem...",LOG_PREFIX);
            format_flashfs(&f_handle);
            if(init_flashfs(&f_handle)<0)	{
                log_error("%sFailed to initialize filesystem after reformatting. Cannot continue, halting execution.",LOG_PREFIX);
                HARDFAULT_HANDLING_ASM();
            }
        }
        else    {
            log_warning("%sNot reformatting. Attempting to restart filesystem again...",LOG_PREFIX);
            if(init_flashfs(&f_handle)<0)	{
                log_error("%sFailed to initialize filesystem on second attempt. Cannot continue, halting execution.",LOG_PREFIX);
                HARDFAULT_HANDLING_ASM();
            }
        }
    }
    log_info("%sFilesystem initialized",LOG_PREFIX);
}

void init_gpio_state(void)	{
	  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(UART2_EN_GPIO_Port,UART2_EN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(UART4_EN_GPIO_Port,UART4_EN_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_4_GPIO_Port, LED_4_Pin, GPIO_PIN_SET);
}

void init_steppers(void)	{

	  thrust_motor.en_gpio_port     = COLL_STEP_EN_GPIO_Port;
	  thrust_motor.en_gpio_pin      = COLL_STEP_EN_Pin;
	  thrust_motor.dir_gpio_port    = COLL_STEP_DIR_GPIO_Port;
	  thrust_motor.dir_gpio_pin     = COLL_STEP_DIR_Pin;
	  thrust_motor.pul_gpio_port    = COLL_STEP_PUL_GPIO_Port;
	  thrust_motor.pul_gpio_pin     = COLL_STEP_PUL_Pin;
	  thrust_motor.mode             = MICROSTEP_16;
	  thrust_motor.timer            = &htim8;
      thrust_motor.master_enable    = true;

	  stepper_enable(&thrust_motor);
}

void init_collective_input(void)	{
	  collective_data.report.thrust = 0;

	  thrust_calibrations.logical_min = 0;
	  thrust_calibrations.logical_max = AXIS_RANGE;
	  thrust_calibrations.is_symmetric = false;
      uint8_t cal_factor[sizeof(axis_calibration_factors_t)];
      int32_t result = load_calibration(axis_thrust, cal_factor , sizeof(cal_factor));
      if(result == -2)  {   // LFS_ERR_NO_ENT defined in lfs.h
          log_warning("%sNo entry when opening thrust calibration, attempting to create file now...",LOG_PREFIX); 
          result = save_calibration(axis_thrust, (uint8_t*)&cal_factor_template, sizeof(cal_factor_template));
          if(result <0) {
              log_error("%sCreation of calibration file failed with error: %d",LOG_PREFIX, result);
          }
          else  {
              log_warning("%sSuccess!",LOG_PREFIX);
              set_calibration(&thrust_calibrations, (uint8_t*)&cal_factor_template, sizeof(cal_factor_template));
              input_filter_init(&thrust_filter, thrust_calibrations.alpha);
          }
      }
      else if(result < 0)    {
          log_error("%sError opening thrust calibration",LOG_PREFIX);
      }
      else  {
          log_info("%sLoaded thrust calibration file",LOG_PREFIX);
          set_calibration(&thrust_calibrations, cal_factor, sizeof(cal_factor));
          thrust_calibrations.is_symmetric    = 0;
          thrust_calibrations.logical_min     = 0;
          thrust_calibrations.logical_max     = 2048;
          thrust_calibrations.zero            = thrust_calibrations.physical_min;
          if(0.0f == thrust_calibrations.alpha)   {
              thrust_calibrations.alpha = THRUST_ALPHA;
          }
          input_filter_init(&thrust_filter, thrust_calibrations.alpha);
      }

}

void update_collective_input(void)  {
    collective_data.report.buttons = 0;
    // Used GPIOs are sequential, this bitshifts the used pins from each port 
    // into a single variable, 25 bits in total resulting in the following:
    // 31   27   23   19   15   11   7    3  0
    // xxxx xxxB BBBB BCCC CEEE EEEE DDDD DDDD
    collective_data.report.buttons =  ((uint32_t)(GPIOB->IDR & GPIOB_BITMASK) << GPIOB_OFFSET)
                                    | ((uint32_t)(GPIOC->IDR & GPIOC_BITMASK) << GPIOC_OFFSET)
                                    | ((uint32_t)(GPIOD->IDR & GPIOD_BITMASK) >> GPIOD_OFFSET)
                                    | ((uint32_t)(GPIOE->IDR & GPIOE_BITMASK) >> GPIOE_OFFSET);
    collective_data.report.buttons = (collective_data.report.buttons ^ THRUST_BUTTON_MASK);
}

void update_uart_port(uart_port_t* port) {
    /*
     *  This is an ugly nested logic function. I thought about trying to split 
     *  it up and make it easier to read, but I opted instead to just put it all
     *  in this one function to avoid making further abstractions.
     */
    if (uart_update(port->uart) == UART_RX_FAIL) {
        return;
    }
    if(port->uart->unread_bytes > 0)	{
        uint8_t temp[RX_BUFFER_SIZE] = {0};
        uint32_t bytes_read =  fifo_peek_continuous(&port->uart->rx_fifo, temp, port->uart->unread_bytes, sizeof(temp));
        com_packet_result_t result = com_packet_parse(port->cpacket,temp,bytes_read);
        if(result.type == COM_PACKET_FALSE)	{
        	return;
        }
        com_addr_t recv_addr = port->cpacket->src_addr;

        if(port->cpacket->dest_addr == dev_com_addr)    {
            switch(result.type)  {
              case COM_PACKET_NORMAL:
                fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
                send_ack(recv_addr,port->uart);
                break;
              case COM_PACKET_CMD:
                  fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
                  switch(com_packet_get_cmd(port->cpacket)) {
                      case CMD_CDR_EN:
                          send_ack(recv_addr,port->uart);
                          break;
                      case CMD_CDR_DS:
                          send_ack(recv_addr,port->uart);
                          break;
                      case CMD_GET_POS:
                          if(port->cpacket->payload[1] == axis_thrust)    {
                              uint8_t pos[sizeof(int16_t)+1] = {0};
                              pos[0] = axis_thrust;
                              memcpy(&pos[1], &collective_data.report.thrust, sizeof(collective_data.report.thrust));
                              com_packet_create_special(special_packet, recv_addr, COM_PACKET_POS, pos, sizeof(pos));
                              uart_write_packet(port->uart, special_packet);
                          }
                          break;
                      case CMD_GET_CAL: {
                          if(port->cpacket->payload[1] == axis_thrust)  {
                              uint8_t tmp[sizeof(axis_calibration_factors_t)+1] = {0};
                              tmp[0] = axis_thrust;
                              memcpy(&tmp[1],&thrust_calibrations,sizeof(axis_calibration_factors_t));
                              com_packet_create_special(special_packet, recv_addr, COM_PACKET_CAL_FACTOR, tmp, sizeof(tmp));
                              uart_write_packet(port->uart, special_packet);
                          }
                          break;
                      }
                      case CMD_SET_CAL: {
                          if(port->cpacket->payload_length - port->cpacket->padding - 2 >= sizeof(axis_calibration_factors_t))    {
                              axis_calibration_factors_t new_cal;
                              memcpy(&new_cal, &port->cpacket->payload[2], sizeof(axis_calibration_factors_t));
                              if(true == verify_calibration(&new_cal) && port->cpacket->payload[1] == axis_thrust)  {
                                  memcpy(&thrust_calibrations, &new_cal, sizeof(axis_calibration_factors_t));
                                  save_calibration(axis_thrust,(uint8_t*)&thrust_calibrations, sizeof(axis_calibration_factors_t));
                                  send_ack(recv_addr,port->uart);
                              }
                          }
                          else  {
                              send_nack(recv_addr,port->uart);
                          }
                          break;
                      }
                      case CMD_STEP_ON:
                          stepper_master_enable(&thrust_motor);
                          send_ack(recv_addr,port->uart);
                          break;
                      case CMD_STEP_OFF:
                          stepper_master_disable(&thrust_motor);
                          send_ack(recv_addr,port->uart);
                          break;
                      case CMD_GET_DEVS:
                          break;
                      case CMD_BAD_CMD:
                          break;
                      default:
                          break;
                  }
                      break;
                  case COM_PACKET_ACK:
                      fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
                      com_packet_clear(port->cpacket);
                      break;
                  case COM_PACKET_NACK:
                      fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
                      uart_write_packet(port->uart, port->lpacket);
                      break;
                  default:
                      break;
                }
        }
        else if (port->cpacket->dest_addr == COM_ADDR_PEDAL)   {
            fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
            uart_write_packet(uart_cyclic, port->cpacket);

        }
        else if (port->cpacket->dest_addr == COM_ADDR_CYCLIC)   {
            fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
            uart_write_packet(uart_cyclic, port->cpacket);

        }
        else if (port->cpacket->dest_addr == COM_ADDR_CTRL)    {
            fifo_push_read_index(&port->uart->rx_fifo, result.bytes_consumed);
            uart_write_packet(uart_console, port->cpacket);
        }
        memcpy(port->lpacket, port->cpacket, sizeof(com_packet_t));
    }
}
/*
void update_uart_cyclic(void)   {

  uart_update(uart_cyclic);
  if (uart_cyclic->unread_bytes > 0 )  {
      uint8_t temp[RX_BUFFER_SIZE] = {0};
      uint32_t bytes_read =  fifo_peek_continuous(&uart_cyclic->rx_fifo, temp, uart_cyclic->unread_bytes, sizeof(temp));
      packet_type_t result = com_packet_parse(cur_packet,temp,bytes_read); 
      com_addr_t recv_addr = cur_packet->src_addr;
      switch(result)  {
          case COM_PACKET_FALSE:
              break;
          case COM_PACKET_NORMAL:
              fifo_push_read_index(&uart_cyclic->rx_fifo, bytes_read);
              send_ack(recv_addr);
              break;
          case COM_PACKET_CMD:
              fifo_push_read_index(&uart_cyclic->rx_fifo, bytes_read);
              switch(com_packet_get_cmd(cur_packet)) {
                  case CMD_CDR_EN:
                      send_ack(recv_addr);
                      break;
                  case CMD_CDR_DS:
                      send_ack(recv_addr);
                      break;
                  case CMD_GET_POS:
                      send_report(recv_addr);
                      break;
                  case CMD_GET_CAL: {
                      axis_t axis = cur_packet->payload[2];
                      uint8_t tmp[sizeof(axis_calibration_factors_t)+1] = {0};
                      switch(axis)  {
                          case axis_thrust:    {
                              tmp[0] = axis_thrust;
                              memcpy(&tmp[1],&thrust_calibrations,sizeof(axis_calibration_factors_t));
                              break;
                          }
                          default:
                              break;
                      }
                      com_packet_create_special(special_packet, recv_addr, COM_PACKET_CAL_FACTOR, tmp, sizeof(tmp));
                      uart_write_packet(uart_cyclic, special_packet);
                      break;
                  }
                  case CMD_SET_CAL: {
                      axis_t axis = cur_packet->payload[2];
                      if(cur_packet->payload_length - cur_packet->padding - 1 >= sizeof(axis_calibration_factors_t))    {
                          axis_calibration_factors_t new_cal;
                          memcpy(&new_cal, &cur_packet->payload[2], sizeof(axis_calibration_factors_t));
                          if(true == verify_calibration(&new_cal))  {
                              switch(axis) {
                                  case axis_thrust:
                                      memcpy(&thrust_calibrations, &new_cal, sizeof(axis_calibration_factors_t));
                                      save_calibration(axis_thrust,(uint8_t*)&thrust_calibrations, sizeof(axis_calibration_factors_t));
                                      break;
                                  default:
                                      break;
                              }
                              send_ack(recv_addr);
                          }
                          else  {
                              send_nack(recv_addr);
                          }
                      }
                      else  {
                          send_nack(recv_addr);
                      }
                      break;
                  }
                  case CMD_STEP_ON:
                      stepper_master_enable(&thrust_motor);
                      send_ack(recv_addr);
                      break;
                  case CMD_STEP_OFF:
                      stepper_master_disable(&thrust_motor);
                      send_ack(recv_addr);
                      break;
                  case CMD_GET_DEVS:
                      break;
                  case CMD_BAD_CMD:
                      break;
                  default:
                      break;
              }
              // handle the command
              break;
          case COM_PACKET_ACK:
              fifo_push_read_index(&uart_cyclic->rx_fifo, bytes_read);
              com_packet_clear(prev_packet);
              break;
          case COM_PACKET_NACK:
              fifo_push_read_index(&uart_cyclic->rx_fifo, bytes_read);
              uart_write_packet(uart_cyclic, prev_packet);
              break;
          default:
              break;
      }

  }
}
*/

void update_stepper(void)   {
    if(MAG_REL_PRESSED(collective_data.report.buttons))  {
            stepper_disable(&thrust_motor);
    }
    else    {
            stepper_enable(&thrust_motor);
    }
}

void send_report(com_addr_t addr)  {
    if(addr == COM_ADDR_CYCLIC)	{
    	com_packet_clear(report_packet);
    	com_packet_create(report_packet, addr, (uint8_t*)&collective_data.raw, sizeof(collective_data.raw));
    	uart_write_packet(uart_cyclic, report_packet);
    }
    else if(addr == COM_ADDR_CTRL)	{
    	com_packet_clear(report_packet);
    	com_packet_create(report_packet, COM_ADDR_CTRL, (uint8_t*)&collective_data.raw, sizeof(collective_data.raw));
    	uart_write_packet(uart_console, report_packet);
    }
}

void send_ack(com_addr_t addr, uart_handle_t* uart)  {
      com_packet_create_special(special_packet, addr, COM_PACKET_ACK,0,0);
      uart_write_packet(uart, special_packet);
}

void send_nack(com_addr_t addr, uart_handle_t* uart)  {
      com_packet_create_special(special_packet, addr, COM_PACKET_NACK,0,0);
      uart_write_packet(uart, special_packet);
}

void calibration_routine(void)	{
    log_info("%sAxis calibration routine initiated. Beginning thrust calibration...",LOG_PREFIX);
	  calibrate_axis(&thrust_calibrations, &adc_buffer[0]);
	  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);

	  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_4_GPIO_Port, LED_4_Pin, GPIO_PIN_SET);
	  uint64_t time = HAL_GetTick();
	  HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_RESET);
	  while(HAL_GetTick() - time < 100){;;}
	  time = HAL_GetTick();
	  HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_SET);
	  while(HAL_GetTick() - time < 100){;;}
	  time = HAL_GetTick();
	  HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_RESET);
	  while(HAL_GetTick() - time < 100){;;}
	  time = HAL_GetTick();
	  HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_SET);

      log_info("%sCalibration complete, saving results...",LOG_PREFIX);
	  int32_t result = save_calibration(axis_thrust, (uint8_t*)&thrust_calibrations, sizeof(axis_calibration_factors_t));
      if(result <0) {
          log_warning("%sFailed to save thrust calibration with error: %d",LOG_PREFIX,result);
      }
      else  {
        log_info("%sThrust calibration saved",LOG_PREFIX);
      }
      
      log_info("%sCalibration routine complete!",LOG_PREFIX);
	  IRQ_Calibrate_Flag = false;
}

void calibrate_axis(axis_calibration_factors_t* cal, volatile uint16_t* adc_buf)	{

	clear_calibration(cal);

	uint64_t time = HAL_GetTick();
	while(HAL_GetTick()-time < 250)	{;;}
	while(HAL_GPIO_ReadPin(SW_OK_GPIO_Port, SW_OK_Pin))	{
		if(HAL_GetTick() - time > 500 ){
			HAL_GPIO_TogglePin(LED_4_GPIO_Port, LED_4_Pin);
			time = HAL_GetTick();
		}
	}

	int16_t val1 = (int16_t)adc_buf[0];
	while(HAL_GetTick()-time < 50)	{;;}
	int16_t val2 = (int16_t)adc_buf[0];
	while(HAL_GetTick()-time < 50)	{;;}
	int16_t val3 = (int16_t)adc_buf[0];

	cal->physical_max = (val1+val2+val3)/3;
	time = HAL_GetTick();
	while(HAL_GetTick()-time < 250)	{;;}
	while(HAL_GPIO_ReadPin(SW_OK_GPIO_Port, SW_OK_Pin))	{
			if(HAL_GetTick() - time > 150 ){
				HAL_GPIO_TogglePin(LED_4_GPIO_Port, LED_4_Pin);
				time = HAL_GetTick();
			}
		}

	val1 = (int16_t)adc_buf[0];
	while(HAL_GetTick()-time < 5)	{;;}
	val2 = (int16_t)adc_buf[0];
	while(HAL_GetTick()-time < 5)	{;;}
	val3 = (int16_t)adc_buf[0];

	cal->physical_min = (val1+val2+val3)/3;
	recalculate_calibration(cal);
	HAL_GPIO_WritePin(LED_4_GPIO_Port, LED_4_Pin, GPIO_PIN_SET);
}

/*
void load_calibrations(axis_calibration_factors_t* cal[], uint8_t calibration_count)	{
	uint8_t buffer_size = 0;
	for(uint8_t i=0; i < calibration_count; i++)	{
		buffer_size += CALIBRATION_FACTOR_SIZE;
	}
	uint8_t size_offset = buffer_size / calibration_count;
	uint8_t data_buffer[buffer_size];

	w25q16_read(&flash_handle, FLASH_CALIBRATION_ADDRESS, data_buffer, buffer_size);
	for(uint8_t i=0; i< calibration_count; i++){
		set_calibration(cal[i], &data_buffer[i*size_offset], size_offset);
	}
}

void save_calibrations(axis_calibration_factors_t* cal[], uint8_t calibration_count)	{
	uint8_t buffer_size = 0;
	for(uint8_t i=0; i < calibration_count; i++)	{
		buffer_size += CALIBRATION_FACTOR_SIZE;
	}
	uint8_t size_offset = buffer_size / calibration_count;
	uint8_t data_buffer[buffer_size];

	for(uint8_t i=0; i< calibration_count; i++){
		get_calibration(cal[i], &data_buffer[i*size_offset], size_offset);
	}

	w25q16_sector_erase_4k(&flash_handle, FLASH_CALIBRATION_ADDRESS);
	uint8_t busy_flag = w25q16_read_SR1(&flash_handle)&0x01;
	uint64_t timer = HAL_GetTick();
	while(busy_flag && HAL_GetTick()-timer<200)	{
		busy_flag = w25q16_read_SR1(&flash_handle)&0x01;
	}
	w25q16_write(&flash_handle, FLASH_CALIBRATION_ADDRESS, data_buffer, buffer_size);
}
*/

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart3, (uint8_t*) &ch, 1, 0xFFFF);

	return ch;
}

int _write(int fd, char *ptr, int len) {

    if(uart_log!=NULL)  {
        uart_write(uart_log, (uint8_t*)ptr, len);
    }
    else    {
        HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    }
	return len;
}

#ifdef USE_HAL_ERRORHANDLER
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
#endif
  /* USER CODE END Error_Handler_Debug */

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
