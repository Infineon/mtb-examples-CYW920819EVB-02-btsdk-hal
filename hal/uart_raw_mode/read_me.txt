-------------------------------------------------------------------------------
UART Raw Mode sample application
-------------------------------------------------------------------------------

The HCI UART in the CYW20719B2/20721B2/20819A1/20821A1 devices supports
two modes: HCI Mode and Raw Data Mode.

This application provides a working example for the HCI UART interface
configured for the Raw Data Mode

Features demonstrated: UART in Raw Data Mode

To use the app, work through the following steps.
1. Plug the eval board into your computer
2. Open a terminal such as "Tera Term" at 115.2Kbps to receive messages
   from the puart interface (usually the 2nd COM port listed in device manager)
3. Build and download the application to the eval board
4. Observe the puart terminal to see the WICED_BT_TRACE messages
5. Open a 2nd terminal at 115.2Kbps to receive and send messages over the
   hci uart interface
6. Type anything into the 2nd terminal and observe it received on the 1st
   terminal and echoed back on the 1st terminal
-------------------------------------------------------------------------------
