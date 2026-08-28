/**
 ******************************************************************************
 * file           : main.c
 * brief          : Main program body
 *                  Calls target system initialization then loop in main.
 ******************************************************************************
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
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "main.h"
#include "mx_adc1.h"
#include "mx_flash.h"
#include "mx_gpio_default.h"
#include "mx_tim2.h"
#include "mx_usart2.h"
#include "stm32c5xx_drivers/hal/stm32c5xx_hal_adc.h"
#include "stm32c5xx_hal_exti.h"
#include "stm32c5xx_hal_flash.h"
#include "stm32c5xx_hal_flash_itf.h"
#include "stm32c5xx_hal_uart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
int __io_putchar(int ch);
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
hal_uart_handle_t* huart2;
hal_adc_handle_t* hadc1;
hal_tim_handle_t* htim2;
hal_exti_handle_t* hexti13;
hal_flash_handle_t* hflash;
uint16_t adc_buffer[8];
uint32_t EDATAStartAddr = (uint32_t)(0x09000000UL);
/* Private functions prototype -----------------------------------------------*/
static void ButtonPressed(hal_exti_handle_t* hexti, hal_exti_trigger_t trigger);
/**
 * brief:  The application entry point.
 * retval: none but we specify int to comply with C99 standard
 */
int main(void)
{
  /** System Init: this code placed in targets folder initializes your system.
   * It calls the initialization (and sets the initial configuration) of the peripherals.
   * You can use STM32CubeMX to generate and call this code or not in this project.
   * It also contains the HAL initialization and the initial clock configuration.
   */
  if (mx_system_init() != SYSTEM_OK)
  {
    return (-1);
  }
  else
  {
    /*
     * You can start your application code here
     */
    huart2 = mx_usart2_uart_gethandle();
    hadc1 = mx_adc1_gethandle();
    htim2 = mx_tim2_gethandle();
    hexti13 = mx_gpio_default_exti13_gethandle();
    hflash = mx_flash_gethandle();

    HAL_EXTI_RegisterTriggerCallback(hexti13, ButtonPressed);
    HAL_EXTI_Enable(hexti13, HAL_EXTI_MODE_INTERRUPT);

    HAL_TIM_OC_StartChannel(htim2, HAL_TIM_CHANNEL_1);
    HAL_TIM_Start(htim2);

    HAL_ADC_Start(hadc1);
    HAL_ADC_Calibrate(hadc1);
    HAL_ADC_REG_StartConv_DMA(hadc1, (uint8_t*)adc_buffer, 8 * sizeof(uint16_t));

    /* Print Hello World on startup */
    printf("\r\n========================================\r\n");
    printf(" Hello World from NUCLEO-C562RE!\r\n");
    printf(" Press User Button (B1) to save ADC data\r\n");
    printf("========================================\r\n");

    while (1)
    {
    }
  }
} /* end main */

int __io_putchar(int ch)
{
  HAL_UART_Transmit(huart2, (uint8_t*)&ch, 1, 0xFFFF);
  return ch;
}

static void ButtonPressed(hal_exti_handle_t* hexti, hal_exti_trigger_t trigger)
{
  HAL_FLASH_ITF_Unlock(HAL_FLASH);
  HAL_FLASH_EDATA_EraseByAddr(hflash, EDATAStartAddr, FLASH_EDATA_PAGE_SIZE, 0xFFFF);
  HAL_FLASH_EDATA_ProgramByAddr(hflash, EDATAStartAddr, (uint32_t*)adc_buffer, 8 * sizeof(uint16_t),
                                0xFFFF);
  printf("ADC Data Saved in Emulated EEPROM\r\n");
}
