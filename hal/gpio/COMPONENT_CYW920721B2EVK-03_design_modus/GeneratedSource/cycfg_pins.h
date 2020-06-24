/*******************************************************************************
* File Name: cycfg_pins.h
*
* Description:
* Pin configuration
* This file was automatically generated and should not be modified.
* Tools Package 2.1.0.1266
* 20721B2
* personalities 1.0.0.31
* udd 1.2.0.370
*
********************************************************************************
* Copyright 2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#if !defined(CYCFG_PINS_H)
#define CYCFG_PINS_H

#include "cycfg_notices.h"
#include "wiced_platform.h"
#include "cycfg_routing.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define amplifiers_0_ENABLED 1U
#define bluetooth_0_ENABLED 1U
#define ioss_0_ENABLED 1U
#define WICED_GET_PIN_FOR_LED(idx) (*(platform_led[(idx)].gpio))
#define WICED_GET_PIN_FOR_BUTTON(idx) (*(platform_button[(idx)].gpio))
#define WICED_GET_PIN_FOR_IO(idx) (*(platform_gpio[(idx)].gpio))
#define BUTTON_MFB_ENABLED 1U
#define BUTTON_MFB WICED_P00
#define ioss_0_pin_10_ENABLED 1U
#define ioss_0_pin_10 WICED_P10
#define SPI_MISO_ENABLED 1U
#define SPI_MISO WICED_P16
#define SPI_SCK_ENABLED 1U
#define SPI_SCK WICED_P17
#define SPI_CS_ENABLED 1U
#define SPI_CS WICED_P25
#define LED1_ENABLED 1U
#define LED1 WICED_P28
#define SPI_MOSI_ENABLED 1U
#define SPI_MOSI WICED_P29
#define BUTTON_VOLUME_DOWN_ENABLED 1U
#define BUTTON_VOLUME_DOWN WICED_P02
#define PUART_TX_ENABLED 1U
#define PUART_TX WICED_P33
#define PUART_RX_ENABLED 1U
#define PUART_RX WICED_P34
#define BUTTON_CUSTOM_ENABLED 1U
#define BUTTON_CUSTOM WICED_P04
#define BUTTON_VOLUME_UP_ENABLED 1U
#define BUTTON_VOLUME_UP WICED_P06
#define spi_1_ENABLED 1U
#define uart_1_ENABLED 1U

extern const wiced_platform_gpio_t platform_gpio_pins[];
extern const size_t platform_gpio_pin_count;
extern const wiced_platform_led_config_t platform_led[];
extern const size_t led_count;
extern const wiced_platform_button_config_t platform_button[];
extern const size_t button_count;
extern const wiced_platform_gpio_config_t platform_gpio[];
extern const size_t gpio_count;


#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PINS_H */
