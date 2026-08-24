/**
  ******************************************************************************
  * @file           : mx_rcc.h
  * @brief          : Exporting functions for mx_rcc.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX_RCC_H
#define MX_RCC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"
#include "mx_def.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  CLOCK_PROFILE_160MHZ = 0, /*!< 160 MHz - Maximum performance (PSI Synthesizer) */
  CLOCK_PROFILE_144MHZ,     /*!< 144 MHz - High performance (PSI Synthesizer)    */
  CLOCK_PROFILE_100MHZ,     /*!< 100 MHz - Balanced high performance (PSI)       */
  CLOCK_PROFILE_48MHZ,      /*!<  48 MHz - Standard default clock (HSIDIV3)       */
  CLOCK_PROFILE_24MHZ,      /*!<  24 MHz - Medium power (HSI / 2)                 */
  CLOCK_PROFILE_12MHZ,      /*!<  12 MHz - Low power (HSI / 4)                    */
  CLOCK_PROFILE_6MHZ,       /*!<   6 MHz - Very low power (HSI / 8)               */
  CLOCK_PROFILE_3MHZ,       /*!<   3 MHz - Ultra low power (HSI / 16)             */
  CLOCK_PROFILE_750KHZ,     /*!< 750 kHz - Deep sub-MHz low power (HSI / 64)      */
} clock_profile_t;

/* Exported constants --------------------------------------------------------*/

/** Primary aliases for RCC_OSC_IN pin */
#define OSC_IN_PORT                           HAL_GPIOH
#define OSC_IN_PIN                            HAL_GPIO_PIN_0

/** Primary aliases for RCC_OSC_OUT pin */
#define OSC_OUT_PORT                          HAL_GPIOH
#define OSC_OUT_PIN                           HAL_GPIO_PIN_1
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/******************************************************************************/
/* Exported functions for RCC in HAL layer */
/******************************************************************************/
system_status_t mx_rcc_init(void);
system_status_t mx_rcc_set_clock(clock_profile_t profile);
void mx_rcc_deinit(void);

system_status_t mx_rcc_peripherals_clock_config(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX_RCC_H */
