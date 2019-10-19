/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
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
/* Read motion sensor acceleration data and display to the PUART */

#include "wiced.h"
#include "wiced_platform.h"
#include "sparcommon.h"
#include "wiced_bt_stack.h"
#include "wiced_rtos.h"
#include "wiced_hal_i2c.h"
#include "wiced_bt_trace.h"

/*****************************    Constants   *****************************/
/* Useful macros for thread priorities */
#define PRIORITY_HIGH               (3)
#define PRIORITY_MEDIUM             (5)
#define PRIORITY_LOW                (7)

/* Sensible stack size for most threads */
#define THREAD_STACK_MIN_SIZE       (500)

/* Thread will delay so that sensor values are read every 1s */
#define THREAD_DELAY_IN_MS          (1000)

/* Motion sensor registers and settings */
#define ACCEL_ADDRESS     (0x6A) /* This is 0xD4 shifted right by 1 */
#define ACCEL_CONFIG_REG  (0x20)
#define ACCEL_CONFIG_VAL  (0x40)

/*****************************    Variables   *****************************/
wiced_thread_t * sensor_thread_handle;

/* Structure to hold sensor data read from I2C */
struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;
} __attribute__((packed)) accelData;

/*****************************    Function Prototypes   *******************/
static wiced_result_t
i2c_master_management_callback(wiced_bt_management_evt_t event,
                               wiced_bt_management_evt_data_t *p_event_data);
void sensor_thread(uint32_t arg);

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

/*
 Function name:
 application_start

 Function Description:
 @brief    Starting point of your application. Entry point to the application.
           Set device configuration and start BT stack initialization.
           The actual application initialization will happen when stack reports
           that BT device is ready.

 @param void

 @return void
 */
void application_start(void)
{
    wiced_result_t result = WICED_BT_SUCCESS;

    /* WICED_BT_TRACE_ENABLE*/
    wiced_set_debug_uart(WICED_ROUTE_DEBUG_TO_PUART);
    WICED_BT_TRACE("************Starting I2C Master Application**********\n\r");
    /* Register BT stack callback */
    result = wiced_bt_stack_init(i2c_master_management_callback, NULL, NULL);
    if(WICED_BT_SUCCESS != result)
    {
        WICED_BT_TRACE("Stack Initialization Failed!!\n\r");
    }
}

/*
 Function Name:
 i2c_master_management_callback

 Function Description:
 @brief  Callback function that will be invoked by application_start()

 @param  event           Bluetooth management event type
 @param  p_event_data    Pointer to the the bluetooth management event data

 @return        status of the callback function
 */
wiced_result_t
i2c_master_management_callback(wiced_bt_management_evt_t event,
                               wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_SUCCESS;

    switch(event)
    {
        /* BlueTooth stack enabled */
        case BTM_ENABLED_EVT:
            /* Start a thread to read motion sensor values
             * and Get memory for the thread handle */
            sensor_thread_handle = wiced_rtos_create_thread();
            if(NULL != sensor_thread_handle)
            {
                wiced_rtos_init_thread(
                        sensor_thread_handle,           /* Thread handle */
                        PRIORITY_MEDIUM,                /* Priority */
                        "MotionSensor",                 /* Name */
                        sensor_thread,                  /* Function */
                        THREAD_STACK_MIN_SIZE,          /* Stack */
                        NULL );                         /* Function argument */
            }
            else
            {
                WICED_BT_TRACE("failed to create thread!!\n\r");
                result = WICED_ERROR;
            }
            break;

        default:
            break;
    }
    return result;
}

/*
 Function Name:
 sensor_thread

 Function Description:
 @brief  sensor thread responsible for reading data from motion sensor

 @param  arg           unused

 @return  none
 */
void sensor_thread(uint32_t arg)
{
	/* Address of first Data register, Total of six registers */
	uint8_t accelDataReg = 0x28;

    /*Initialize I2C and set speed to 400kHz */
    wiced_hal_i2c_init();
    wiced_hal_i2c_set_speed(I2CM_SPEED_400KHZ);

     /* Write to the configuration register. 2 bytes are sent,first the
      * register location and then the register value */
     uint8_t data[] = {ACCEL_CONFIG_REG, ACCEL_CONFIG_VAL};
     wiced_hal_i2c_write(data, sizeof(data), ACCEL_ADDRESS);

     while(1)
     {
         /* Read the sensor data */
         /* We need to send a write to the data register followed by a read of
          * 6 bytes into the accelData structure */
         wiced_hal_i2c_combined_read((uint8_t *)&accelData,
                                     sizeof(accelData),
                                     (uint8_t *)&accelDataReg,
                                     sizeof(accelDataReg),
                                     ACCEL_ADDRESS);

         WICED_BT_TRACE("Ax=%6d       Ay=%6d       Az=%6d\n\r",
                        accelData.ax,accelData.ay,accelData.az);

         /* Send the thread to sleep for a period of time */
         wiced_rtos_delay_milliseconds(THREAD_DELAY_IN_MS,
                                       ALLOW_THREAD_TO_SLEEP);
    }
}
