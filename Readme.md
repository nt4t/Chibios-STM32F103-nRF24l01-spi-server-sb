STM32F103-example-code
==============================

This is my example code for the STM32F103 using the RTOS ChibiOS.

Based on https://github.com/wendlers/libemb/tree/master/libnrf24l01, https://github.com/wendlers/libemb/tree/master/tests/nrf-server-sb and https://github.com/omriiluz/libnrf24l01p

requirements
------------
* Chibios 2.5.0+ (or a recent development snapshot)
* arm toolchain (e.g. arm-none-eabi from summon-arm)

features
--------
* background blinker thread(not worked yet :)
* The server configures the nRF24l01 into Shockburst mode.  
* nRF24l01 send a payload to a client.

usage
-----
* edit the Makefile and point "CHIBIOS = ../../chibios" to your ChibiOS folder
* make
* connect the STM32F103 with TTL/RS232 adapter, PA9(TX) and PA10(RX) are routed to USART1 38400-8-N-1.
* connect nRF24l01 to portB 12-15 pins (http://gpio.kaltpost.de/wp-content/uploads/2012/02/stm32vl_nrf24l01.png).
* flash the STM32F103
* use your favorite terminal programm to connect to the Serial Port 38400-8n1 

