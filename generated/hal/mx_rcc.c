
/**
  ******************************************************************************
  * @file           : mx_rcc.c
  * @brief          : STM32 RCC program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the mx_stm32c5xx_hal_drivers_license.md file
  * in the same directory as the generated code.
  * If no mx_stm32c5xx_hal_drivers_license.md file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mx_rcc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/

/******************************************************************************/
/* Exported functions for RCC in HAL layer */
/******************************************************************************/

/**
  * Configure the system core clock only and activate it using the HAL RCC unitary APIs (footprint optimization)
  *         The system Clock is configured as follow :
  *            System Clock source            = PSIS
  *            SYSCLK(Hz)                     = 144000000
  *            HCLK(Hz)                       = 144000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB3 Prescaler                 = 1
  *            Flash Latency(WS)              = 4
  */
/**
  * @brief  Explicitly configure and switch the system core clock.
  * @param  profile  Target clock profile (e.g. CLOCK_PROFILE_160MHZ, CLOCK_PROFILE_6MHZ)
  * @retval system_status_t (SYSTEM_OK or SYSTEM_CLOCK_ERROR)
  */
system_status_t mx_rcc_set_clock(clock_profile_t profile)
{
  switch (profile)
  {
    case CLOCK_PROFILE_160MHZ:
    case CLOCK_PROFILE_144MHZ:
    case CLOCK_PROFILE_100MHZ:
    {
      if (HAL_RCC_HSE_Enable(HAL_RCC_HSE_ON) != HAL_OK)
      {
        return SYSTEM_CLOCK_ERROR;
      }

      hal_rcc_psi_config_t config_psi;
      config_psi.psi_source = HAL_RCC_PSI_SRC_HSE;
      config_psi.psi_ref    = HAL_RCC_PSI_REF_24MHZ;

      if (profile == CLOCK_PROFILE_160MHZ)
      {
        config_psi.psi_out = HAL_RCC_PSI_OUT_160MHZ; // 160 MHz
      }
      else if (profile == CLOCK_PROFILE_144MHZ)
      {
        config_psi.psi_out = HAL_RCC_PSI_OUT_144MHZ; // 144 MHz
      }
      else
      {
        config_psi.psi_out = HAL_RCC_PSI_OUT_100MHZ; // 100 MHz
      }

      if (HAL_RCC_PSI_SetConfig(&config_psi) != HAL_OK)
      {
        return SYSTEM_CLOCK_ERROR;
      }

      if (HAL_RCC_PSIS_Enable() != HAL_OK)
      {
        return SYSTEM_CLOCK_ERROR;
      }

      hal_rcc_bus_clk_config_t config_bus;
      config_bus.hclk_prescaler  = HAL_RCC_HCLK_PRESCALER1;
      config_bus.pclk1_prescaler = HAL_RCC_PCLK_PRESCALER1;
      config_bus.pclk2_prescaler = HAL_RCC_PCLK_PRESCALER1;
      config_bus.pclk3_prescaler = HAL_RCC_PCLK_PRESCALER1;
      if (HAL_RCC_SetBusClockConfig(&config_bus) != HAL_OK)
      {
        return SYSTEM_CLOCK_ERROR;
      }

      // Safe flash latency for high speed >= 100 MHz
      HAL_FLASH_ITF_SetLatency(HAL_FLASH, HAL_FLASH_ITF_LATENCY_4);

      if (HAL_RCC_SetSYSCLKSource(HAL_RCC_SYSCLK_SRC_PSIS) != HAL_OK)
      {
        return SYSTEM_CLOCK_ERROR;
      }

      HAL_FLASH_ITF_SetProgrammingDelay(HAL_FLASH, HAL_FLASH_ITF_PROGRAM_DELAY_2);
      break;
    }

    case CLOCK_PROFILE_48MHZ:
    case CLOCK_PROFILE_24MHZ:
    case CLOCK_PROFILE_12MHZ:
    case CLOCK_PROFILE_6MHZ:
    case CLOCK_PROFILE_3MHZ:
    case CLOCK_PROFILE_750KHZ:
    {
      // 1. If we were previously on high-speed PSIS, switch to HSIDIV3 (48 MHz) first
      if (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_PSIS)
      {
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSIDIV3);
      }

      // 2. Set the AHB Prescaler
      if (profile == CLOCK_PROFILE_48MHZ)
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_1);  // 48 MHz / 1 = 48 MHz
      }
      else if (profile == CLOCK_PROFILE_24MHZ)
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_2);  // 48 MHz / 2 = 24 MHz
      }
      else if (profile == CLOCK_PROFILE_12MHZ)
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_4);  // 48 MHz / 4 = 12 MHz
      }
      else if (profile == CLOCK_PROFILE_6MHZ)
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_8);  // 48 MHz / 8 = 6 MHz
      }
      else if (profile == CLOCK_PROFILE_3MHZ)
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_16); // 48 MHz / 16 = 3 MHz
      }
      else
      {
        LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_PRESCALER_64); // 48 MHz / 64 = 750 kHz
      }

      // 3. Safe Flash Latency
      HAL_FLASH_ITF_SetLatency(HAL_FLASH, HAL_FLASH_ITF_LATENCY_1);
      break;
    }

    default:
      return SYSTEM_CLOCK_ERROR;
  }

  if (HAL_UpdateCoreClock() != HAL_OK)
  {
    return SYSTEM_CLOCK_ERROR;
  }

  return SYSTEM_OK;
}

system_status_t mx_rcc_init(void)
{
  return mx_rcc_set_clock(CLOCK_PROFILE_144MHZ);
}

void mx_rcc_deinit(void)
{
  HAL_RCC_Reset();
}

/**
  * configures and activate the clocks used by all the peripherals selected within the project
  */
system_status_t mx_rcc_peripherals_clock_config(void)
{
  /* Peripherals using PCLK1 (144 MHz):
    USART2
  */

  /* Peripherals using PSIS (144 MHz):
    ADC1
  */
  /* PSIS already enabled inside mx_rcc_init() */

  /* Peripherals using ADC_DAC_DIV (36 MHz):
    ADC1
  */
  if (HAL_RCC_ADCDAC_SetKernelClkPrescaler(HAL_RCC_ADCDAC_PRESCALER4) != HAL_OK)
  {
    return SYSTEM_CLOCK_ERROR;
  }

  return SYSTEM_OK;
}
