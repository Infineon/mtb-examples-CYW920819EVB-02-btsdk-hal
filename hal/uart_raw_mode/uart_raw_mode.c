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
* UART Raw Mode sample application
*
* The HCI UART in the CYW20719B2/20721B2/20819A1/20821A1 devices supports
* two modes: HCI Mode and Raw Data Mode.
*
* This application provides a working example for the HCI UART interface
* configured for the Raw Data Mode
*
* Features demonstrated: UART in Raw Data Mode
*
* To use the app, work through the following steps.
* 1. Plug the eval board into your computer
* 2. Open a terminal such as "Tera Term" at 115.2Kbps to receive messages
*    from the puart interface (usually the 2nd COM port listed in device manager)
* 3. Build and download the application to the eval board
* 4. Observe the puart terminal to see the WICED_BT_TRACE messages
* 5. Open a 2nd terminal at 115.2Kbps to receive and send messages over the
*    hci uart interface
* 6. Type anything into the 2nd terminal and observe it received on the 1st
*    terminal and echoed back on the 1st terminal
*/

#include "wiced_bt_cfg.h"
#include "sparcommon.h"
#include "wiced_bt_dev.h"
#include "wiced_platform.h"
#include "wiced_bt_trace.h"
#include "wiced_hal_puart.h"
#include "wiced_bt_stack.h"
#include "wiced_transport.h"

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define RAW_UART_BUFFER_SIZE        2000 //Raw UART Driver buffer size
#define TX_BUFFER_SIZE              264  //Buffer to send data to host
#define TX_BUFFER_COUNT             2    //Number of buffers to send data to host

/******************************************************************************
 *                            Variables Definitions
 ******************************************************************************/
extern const wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[WICED_BT_CFG_NUM_BUF_POOLS];

wiced_transport_buffer_pool_t *uart_raw_data_pool = NULL; // Pool to send raw bytes to UART

static void transport_status( wiced_transport_type_t type );
static uint32_t data_rx_cback( uint8_t* ptr, uint32_t length );

/* transport configuration */
const wiced_transport_cfg_t  transport_cfg =
{
    .type = WICED_TRANSPORT_UART,
    .cfg =
    {
        .uart_cfg =
        {
            .mode = WICED_TRANSPORT_UART_RAW_MODE,
            .baud_rate =  115200,
        },
    },
    .rx_buff_pool_cfg =
    {
            .buffer_size = RAW_UART_BUFFER_SIZE,
            .buffer_count = 1
    },
    .p_status_handler = transport_status,
	.p_data_handler = data_rx_cback,
    .p_tx_complete_cback = NULL
};

/*******************************************************************
 * Function Prototypes
 ******************************************************************/
extern wiced_result_t wiced_transport_send_raw_buffer( uint8_t* p_buf, uint16_t length );

static void transport_status( wiced_transport_type_t type );
static uint32_t data_rx_cback( uint8_t* ptr, uint32_t length );
static wiced_bt_dev_status_t  app_bt_management_callback    ( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data );

/******************************************************
 *               Function Definitions
 ******************************************************/

/*
 *  Application Start, ie, entry point to the application.
 */
APPLICATION_START()
{
	wiced_transport_init( &transport_cfg );

#if ((defined WICED_BT_TRACE_ENABLE) || (defined HCI_TRACE_OVER_TRANSPORT))
    /* Select Debug UART setting to see debug traces on the appropriate port */
    wiced_set_debug_uart(  WICED_ROUTE_DEBUG_TO_PUART );
#endif

    WICED_BT_TRACE("**** App Start **** \n\r");

    /* Initialize Stack and Register Management Callback */
    wiced_bt_stack_init(app_bt_management_callback, &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools);

    uart_raw_data_pool = wiced_transport_create_buffer_pool( TX_BUFFER_SIZE, TX_BUFFER_COUNT );
    WICED_BT_TRACE( "Created transport pool at 0x%x\n", uart_raw_data_pool );
}

/*
 *  Management callback receives various notifications from the stack
 */
 wiced_result_t app_bt_management_callback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_result_t status = WICED_BT_SUCCESS;

    switch (event)
    {
    case BTM_ENABLED_EVT:
        /* Bluetooth Controller and Host Stack Enabled */
        WICED_BT_TRACE("Bluetooth Enabled (%s)\n",
                ((WICED_BT_SUCCESS == p_event_data->enabled.status) ? "success" : "failure"));
        break;

    default:
        WICED_BT_TRACE("Unhandled Bluetooth Management Event: 0x%x (%d)\n", event, event);
        break;
    }

    return status;
}

/*
 *  UART data received callback. Read the data from uart and loop back
 *  the received data to UART
 */
uint32_t data_rx_cback( uint8_t* p_rx_data, uint32_t data_len )
{
	wiced_result_t result;
	uint8_t* p_data;

    WICED_BT_TRACE( "Data received %d\n", data_len );

    /* If valid data */
    if ( p_rx_data && data_len )
    {
        /* Allocating a buffer to send the received string data */
		p_data = (uint8_t*)wiced_transport_allocate_buffer( uart_raw_data_pool );

        if ( p_data )
        {
			memset( p_data, 0, TX_BUFFER_SIZE );
			memcpy( p_data, p_rx_data, data_len );

			WICED_BT_TRACE("Received \"%s\"\n", p_data);

			WICED_BT_TRACE("Attempting to loopback data...\n");

			/* loop back the received data to UART */
			result = wiced_transport_send_raw_buffer(p_data, data_len);
			WICED_BT_TRACE("wiced_transport_send_raw_buffer result %d\n", result );

			if(result == WICED_SUCCESS)
				wiced_transport_free_buffer(p_data);
        }
        else
        {
			WICED_BT_TRACE("Failed to allocate transport buffer!\n");
        }

		/* Return length of the data that is processed by the application so
		 * that the uart can update the data pointer */
		return data_len;
    }
    return 0;
}

static void transport_status( wiced_transport_type_t type )
{
	wiced_result_t result;
	uint8_t* p_data;
    /* Buffer that holds the startup string */
    char  buf[] = { "Hello World!\r\nType something! Keystrokes are echoed to the terminal ...\n" };

    /* Allocating a buffer to send the startup string data */
	p_data = (uint8_t*)wiced_transport_allocate_buffer( uart_raw_data_pool );

    if ( p_data )
    {
		memcpy( p_data, buf, sizeof(buf) );

		/* loop back the received data to UART */
		result = wiced_transport_send_raw_buffer(p_data, sizeof(buf));
		WICED_BT_TRACE("wiced_transport_send_raw_buffer result %d size %d\n", result, sizeof(buf) );

		if(result == WICED_SUCCESS)
			wiced_transport_free_buffer(p_data);
    }
    else
    {
		WICED_BT_TRACE("Failed to allocate transport buffer!\n");
    }
}
