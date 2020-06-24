/*
 * Copyright 2016-2020, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */


 /** @file
 *
 * WICED sample application for GPIO usage
 *
 * This application demonstrates how to use WICED GPIO APIs to
 * configure GPIO's as Input(with interrupt enabled) or Output pins.
 *
 * The GPIO defined using LED_GPIO_1 is configured as an output
 * pin and toggled at predefined frequency(APP_TIMEOUT_IN_SECONDS_A).
 * The GPIO defined using WICED_GPIO_BUTTON is configured as an interrupt
 * enabled input pin, upon pressing this button the blink rate
 * of LED's is toggled between 1 and 5 sec(both intervals are configurable).
 *
 * Features demonstrated
 * - GPIO WICED APIs
 *
 * Application Instructions
 *   Build and download the application as described in the WICED
 *   Studio Kit Guide
 *
 */
#ifdef CYW20706A2
#include "wiced_bt_dev.h"
#include "sparcommon.h"

#include "wiced_hal_gpio.h"
#include "wiced_hal_mia.h"
#include "wiced_gki.h"
#include "wiced_platform.h"
#include "wiced_timer.h"
#include "wiced_bt_trace.h"
#include "wiced_hal_puart.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_app_hal_common.h"

/******************************************************************************
 *                                Constants
 ******************************************************************************/

/******************************************************************************
 *                                Structures
 ******************************************************************************/

/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/
#define LED_GPIO_1                              WICED_P31

/* Delay timer for LED blinking */
#define APP_TIMEOUT_IN_SECONDS_A                  1       /* Seconds timer */
#define APP_TIMEOUT_IN_SECONDS_B                  5       /* Seconds timer */

wiced_timer_t seconds_timer;               /* wiced bt app seconds timer */
uint32_t wiced_timer_count       = 0;          /* number of seconds elapsed */

static wiced_result_t sample_gpio_app_management_cback( wiced_bt_management_evt_t event,
                                                        wiced_bt_management_evt_data_t *p_event_data );
static void gpio_set_input_interrupt( void );
static void gpio_test_led( );

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/

/*
 *  Entry point to the application. Set device configuration and start BT
 *  stack initialization.  The actual application initialization will happen
 *  when stack reports that BT device is ready.
 */
APPLICATION_START( )
{
    wiced_set_debug_uart( WICED_ROUTE_DEBUG_TO_PUART );
    wiced_hal_puart_select_uart_pads( WICED_PUART_RXD, WICED_PUART_TXD, 0, 0);

    WICED_BT_TRACE( "GPIO application start\n\r" );

    wiced_bt_stack_init( sample_gpio_app_management_cback,NULL,NULL);
}

wiced_result_t sample_gpio_app_management_cback( wiced_bt_management_evt_t event,
                                                 wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_SUCCESS;

    WICED_BT_TRACE( "sample_gpio_app_management_cback %d\n\r", event );

    switch( event )
    {
        /* Bluetooth  stack enabled */
        case BTM_ENABLED_EVT:
            /* Initializes the GPIO driver */
            wiced_bt_app_hal_init();

            /* Sample function configures LED pin as output
             * sends a square wave on it
             * if testing pin 31 then disable puart*/
            gpio_test_led( );

            /* Sample function configures GPIO as input. Enable interrupt.
             * Register a call back function to handle on interrupt*/
            gpio_set_input_interrupt( );

            break;
        default:
            break;
    }
    return result;
}

void gpio_interrrupt_handler(void *data, uint8_t port_pin)
{
    static uint32_t prev_blink_interval = APP_TIMEOUT_IN_SECONDS_A;
    uint32_t curr_blink_interval = 0;

     /* Get the status of interrupt on P# */
    if ( wiced_hal_gpio_get_pin_interrupt_status( WICED_GPIO_BUTTON ) )
    {
        /* Clear the gpio interrupt */
        wiced_hal_gpio_clear_pin_interrupt_status( WICED_GPIO_BUTTON );
    }

    /* stop the led blink timer */
    wiced_stop_timer( &seconds_timer );

    /* toggle the blink time interval */
    curr_blink_interval = (APP_TIMEOUT_IN_SECONDS_A == prev_blink_interval)?APP_TIMEOUT_IN_SECONDS_B:APP_TIMEOUT_IN_SECONDS_A;

    wiced_start_timer( &seconds_timer, curr_blink_interval );

    WICED_BT_TRACE("gpio_interrupt_handler : %d\n\r", curr_blink_interval);

    /* update the previous blink interval */
    prev_blink_interval = curr_blink_interval;
}

void gpio_set_input_interrupt( )
{
    uint16_t pin_config;

    /* Configure GPIO PIN# as input, pull up and interrupt on rising edge and output value as high
     *  (pin should be configured before registering interrupt handler )*/
    wiced_hal_gpio_configure_pin( WICED_GPIO_BUTTON, WICED_GPIO_BUTTON_SETTINGS( GPIO_EN_INT_RISING_EDGE ), WICED_GPIO_BUTTON_DEFAULT_STATE );
    wiced_hal_gpio_register_pin_for_interrupt( WICED_GPIO_BUTTON, gpio_interrrupt_handler, NULL );

    /* Get the pin configuration set above */
    pin_config = wiced_hal_gpio_get_pin_config( WICED_GPIO_BUTTON );
    WICED_BT_TRACE( "Pin config of P%d is %d\n\r", WICED_GPIO_BUTTON, pin_config );
}

/* The function invoked on timeout of app seconds timer. */
void seconds_app_timer_cb( uint32_t arg )
{
    wiced_timer_count++;
    WICED_BT_TRACE( "seconds periodic timer count: %d s\n", wiced_timer_count );

    if(wiced_timer_count & 1)
    {
        wiced_hal_gpio_set_pin_output( LED_GPIO_1, GPIO_PIN_OUTPUT_LOW);
    }
    else
    {
        wiced_hal_gpio_set_pin_output( LED_GPIO_1, GPIO_PIN_OUTPUT_HIGH);
    }
}

void gpio_test_led( )
{
    WICED_BT_TRACE( "gpio_test_led\n\r" );

    /* Configure LED PIN as input and initial outvalue as high */
    wiced_hal_gpio_configure_pin( LED_GPIO_1, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_HIGH );

    if ( wiced_init_timer( &seconds_timer, &seconds_app_timer_cb, 0, WICED_SECONDS_PERIODIC_TIMER )== WICED_SUCCESS )
    {
        if ( wiced_start_timer( &seconds_timer, APP_TIMEOUT_IN_SECONDS_A ) !=  WICED_SUCCESS )
    {
            WICED_BT_TRACE( "Seconds Timer Error\n\r" );
        }
    }
}
#endif
