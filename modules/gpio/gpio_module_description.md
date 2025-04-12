# STM32F401RE GPIO Peripheral Driver Library

## Overview
This header file (`gpio.h`) defines a hardware abstraction layer for the GPIO peripheral on the STM32F401RE microcontroller. It provides a simplified interface for configuring and controlling GPIO pins while abstracting the low-level STM32 LL (Low-Level) driver details.

## Features
- Abstracted GPIO initialization for input and output pins
- Error code definitions for debugging and error handling
- Simplified pin manipulation functions (set, toggle, read)
- Support for all GPIO ports (A through E, and H) available on the STM32F401RE
- Configuration options for:
  - Pull-up/pull-down resistors
  - Output types (push-pull/open-drain)
  - Speed settings
  - Input signal inversion

## API Functions
- `io_init()`: Initializes GPIO pins based on configuration structure
- `io_set_val()`: Sets the state of an output pin
- `io_toggle_val()`: Toggles the state of an output pin
- `io_get_val()`: Reads the state of an input pin
- `io_get_output_val()`: Reads the current state of an output pin

## Data Structures
- `io_in_handler_t`: Configuration structure for input pins
- `io_out_handler_t`: Configuration structure for output pins
- `io_confg_handler_t`: Main configuration structure containing arrays of input and output configurations

## Usage Example
This library allows for easy configuration of GPIO pins through structured initialization, making your application code more readable and maintainable by separating hardware-specific details from application logic.
