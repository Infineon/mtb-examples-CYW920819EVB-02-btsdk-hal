# Overview

## ADC sample application for ModusToolbox BT-SDK

This application demonstrates how to configure and use ADC in WICED Evaluation boards to measure DC voltage on various DC input channels. Please see the makefile for the supported kits.

Once in every 5 seconds, voltage is measured on 4 channels.

1. VDD_CORE, ADC_BGREF, ADC_INPUT_VDDIO.
2. The selected GPIO pin (ADC_INPUT_P0).

### How to validate

 1. When the selected GPIO pin is connected to ground, the output on the terminal emulator will show 0 to ~2mV.
 2. When the selected GPIO pin is connected to 3.3V, the output on the terminal emulator will show approximately 3300mV +/- (3% of 3300mV).
 3. When the selected GPIO pin is left unconnected, the output will capture environmental noise resulting in some lower voltage levels in the terminal emulator.

Please refer to the datasheet for more electrical specifications of ADC like Full scale voltage, Bandgap reference., etc.

### Instructions

To view the working of the app, please follow these steps.
 1. Plug any one of the supported Evaluation boards into your PC.
 2. Build and download the application (See the respective Kit User Guide).
 3. Use Terminal emulation tools like Teraterm to view the log/trace messages.
 4. The user can notice the raw sample values and voltage values in the terminal
    emulation tool. The GPIO pin is connected to user button on the Evaluation
    kit to check the value during button press and release.
