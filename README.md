# STM32F401 Modules
This repo consists of a set of modules that i have developed using inspiration from Gene Schroader's "Bare-Metal Embedded" series on Youtube.

The modules use the peripherals avaliable on the STM32F401RE chip. The functionationilty of each module is:
- The `tmr` module provides an abstraction for configuring TIM2, TIM3, and TIM4 on the STM32F401RE. It allows users to set up periodic interrupts at a specified interval (e.g., every 100ms) and executes a user-defined callback function upon each trigger event.
- The `gpio` module provides a clean hardware abstraction layer over ST's LL drivers. It features simplified interfaces for GPIO configuration and control with comprehensive error handling. The library supports all available ports (A-E, H) with configurations for pull resistors, output types, and speed settings.
- The `ttys` module implements a TTY-style serial communication interface with buffered I/O. The library provides buffered transmit and receive capabilities for USART1, USART2, and USART6 peripherals. It features circular buffer management with separate read/write indexes for TX and RX operations, supporting non-blocking communication
