# core-v-mcu-demo

## Setup

You can run the core-v-mcu-demo with either the OpenHW Group expansion board, or with the ESP-C3-Expresslink-Devkit.

### Requirements:

#### ESP-C3-Expresslink-Devkit setup:
- Digilent Nexys A7 (artix7 100T) board
- ESP-C3-Expresslink-Devkit
- 3x USB2microUSB cables
- Digilent HS2 connector
- 3x jumper cables
- PMOD header male-male, 2 rows
- latest version of the FPGA bitfile, which can be found [here](http://downloads.openhwgroup.org/)

Note: I had to manually bend one row of pins in the PMOD header.

#### OpenHW Group expansion board setup:
- Digilent Nexys A7 (artix7 100T) board
- OpenHW Group expansion board
- Digilent HS2 connector
- PMOD header male-male, 2 rows
- latest version of the FPGA bitfile, which can be found [here](http://downloads.openhwgroup.org/)

#### CORE-V-SDK:
- download the latest version of the CORE-V-SDK from the official [download page](http://downloads.openhwgroup.org/) and install it
    - the `core-v-mcu-cli-test` [page](https://github.com/openhwgroup/core-v-mcu-cli-test/blob/main/README.md) offers a nice guide: just remember to use the `demo` project instead of the `cli-test` project

ESP setup:
- [this page](https://github.com/espressif/esp-aws-expresslink-eval) provides useful information about how to setup the ESP board
- the ESP is expected to communicate to OpenHW's AWS IoT Core: make sure your ESP has been registered by asking OpenHW's team
- the ESP must have WiFi access; the userguide mentioned above explains how to do it; the relevant commands are also in the `iot_task.c` file (`at_set_wifi()` function), but are commented out
    - to enable the configuration of the WiFi interface
        - there must be an active UART connection with the board
        - press a key when requested
        - the ESP module goes in configuration mode and the ESP BLE must be used to provide the WiFi credentials
    - once the configuration is done, do a power cycle: this is necessary to switch off the ESP module, to bring it out of configuration mode
- the same function allows to configure the correct Endpoint: ask OpenHW's team the address of the actual endpoint

Board connections
- host PC to Nexys board (J6)
- host PC to HS2 to Nexys board (JB connector, lower row)
- host PC to ESP board
- Nexys JB.2 to ESP J13.RX
- Nexys JB.3 to ESP J13.TX
- Nexys JB.5 to ESP J13.GND

Identify the USB ports on the host PC
The host PC should see 3 /dev/ttyUSB* interfaces:
1. one used for the debugger
2. one used by the MCU to send messages via CLI_print (over UART0)
3. one used by the ESP board to send debug information

Connect via minicom (or similar) to each of the /dev/ttyUSB* device to identify them. After reset:
1. prints no information
2. prints `A2 boot`
3. prints the ESP bootlog

Running the demo:
1. power up both boards (remember: SW0 must be switched towards the inside of the board, otherwise it is not possible to connect the debugger)
2. [optional] open 2 terminals to see the debug messages
3. open CORE-V-SDK and load the `cli-test` project (`iot_demo` branch)
4. compile the code
5. load the executable in the MCU
6. run

An OpenHW Group Grafana [dashboard](https://openhwgroup.grafana.net/dashboards) displays the measurements sent to the OpenHW's AWS IoT Core.
Contact the OpenHW Staff to obtain the viewer's credentials.
