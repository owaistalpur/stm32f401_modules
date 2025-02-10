/**
 * @file gpio.h
 * @author Owais Talpur (owaistalpur@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 **/

#ifndef GPIO_H
#define GPIO_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
/* Standard includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* MCU includes */
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_rcc.h>

////////////////////////////////////////////////////////////////////////////////
// Common Macros
////////////////////////////////////////////////////////////////////////////////
#define SET 1U
#define RESET 0U
#define ENABLE SET
#define DISABLE RESET

#define __vo volatile

////////////////////////////////////////////////////////////////////////////////
// IO Error Code Macros
////////////////////////////////////////////////////////////////////////////////

typedef enum {

  IO_IN_SZE_ERR = 100U,
  IO_IN_CONFG_FAIL,
  IO_IN_GET_FAIL

} io_in_fail_t;

typedef enum {

  IO_OUT_SZE_ERR = 125U,
  IO_OUT_CONFG_FAIL,
  IO_OUT_SET_FAIL,

} io_out_fail_t;

//
//  GPIO Mappings for the STM32F401RE
//

// Ports
#define IO_PORT_A (GPIOA)
#define IO_PORT_B (GPIOB)
#define IO_PORT_C (GPIOC)
#define IO_PORT_D (GPIOD)
#define IO_PORT_E (GPIOE)
#define IO_PORT_H (GPIOH)

// Pins
#define IO_PIN_00 (LL_GPIO_PIN_0)
#define IO_PIN_01 (LL_GPIO_PIN_1)
#define IO_PIN_02 (LL_GPIO_PIN_2)
#define IO_PIN_03 (LL_GPIO_PIN_3)
#define IO_PIN_04 (LL_GPIO_PIN_4)
#define IO_PIN_05 (LL_GPIO_PIN_5)
#define IO_PIN_06 (LL_GPIO_PIN_6)
#define IO_PIN_07 (LL_GPIO_PIN_7)
#define IO_PIN_08 (LL_GPIO_PIN_8)
#define IO_PIN_09 (LL_GPIO_PIN_9)
#define IO_PIN_10 (LL_GPIO_PIN_10)
#define IO_PIN_11 (LL_GPIO_PIN_11)
#define IO_PIN_12 (LL_GPIO_PIN_12)
#define IO_PIN_13 (LL_GPIO_PIN_13)
#define IO_PIN_14 (LL_GPIO_PIN_14)
#define IO_PIN_15 (LL_GPIO_PIN_15)

// Pin configurations
#define IO_PULL_NO (LL_GPIO_PULL_NO)
#define IO_PULL_UP (LL_GPIO_PULL_UP)
#define IO_PULL_DWN (LL_GPIO_PULL_DOWN)

#define IO_SPDR_FREQ_LOW (LL_GPIO_SPEED_FREQ_LOW)
#define IO_SPDR_FREQ_MED (LL_GPIO_SPEED_FREQ_MEDIUM)
#define IO_SPDR_FREQ_HIGH (LL_GPIO_SPEED_FREQ_HIGH)
#define IO_SPDR_FREQ_VHIGH (LL_GPIO_SPEED_FREQ_VERY_HIGH)

#define IO_OUPT_PUSHPULL (LL_GPIO_OUTPUT_PUSHPULL)
#define IO_OUPT_OPNDRAIN (LL_GPIO_OUTPUT_OPENDRAIN)

#define IO_INVERT_ENABLE ENABLE
#define IO_INVERT_DISABLE DISABLE

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////
typedef GPIO_TypeDef io_port;

typedef struct {
  io_port *const portx;
  const uint32_t ioPinNo;
  const uint32_t ioPupdr;
  const uint8_t ioInInvert;

} io_in_handler_t;

typedef struct {
  io_port *const portx;
  const uint32_t ioPinNo;
  const uint32_t ioSpeed;
  const uint32_t ioOutType;
  const uint32_t ioInitVal;

} io_out_handler_t;

typedef struct {
  const uint32_t numIOInputs;
  const io_in_handler_t *ioInputs;

  const uint32_t numIOOutputs;
  const io_out_handler_t *ioOutputs;

} io_confg_handler_t;

////////////////////////////////////////////////////////////////////////////////
// Module Interface
////////////////////////////////////////////////////////////////////////////////

// Initialise/Open interface
uint32_t io_init(io_confg_handler_t *ioConfg);

// Write interfaces
uint32_t io_set_val(uint32_t ioOutIdx, uint32_t writeVal);
uint32_t io_toggle_val(uint32_t ioOutIdx);

// Read interfaces
uint32_t io_get_val(uint32_t ioInIdx);
uint32_t io_get_output_val(uint32_t ioOutIdx);

#endif  // gpio.h