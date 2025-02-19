# STM32WL55JC1_end_node
Codes to implement a LoRaWAN end node starting from the example "LoRaWAN_End_Node"

This repository serves as an help to understand and develop codes adopting the STM32WL55JC1 Nucleo Board as a LoRaWAN end node.

The folder you find as End_Node setup just contains the files necessary to implement the example given in STM32 CUBE IDE ("LoRaWAN_End_Node").

After that, v01 offers a modified example where an I2C sensor is used (BMP280), and the end node sends data of its measured temperature and pressure through a library which has been written ad hoc. 

The libraries can be found at "".

In v01 the low power mode is disabled, so to avoid the i2c sensor encontering issues.

v02 offers an upgraded version, where the low power mode is managed better. Read the readme file for a better understanding.
