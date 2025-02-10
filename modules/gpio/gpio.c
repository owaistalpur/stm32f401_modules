#include <gpio.h>

static io_confg_handler_t *IOconfig;

////////////////////////////////////////////////////////////////////////////////
// Private (static) function declarations
////////////////////////////////////////////////////////////////////////////////
static void gpio_clock_enable(io_port *GPIOx);

/**
 * @brief: Initialises the IO pins
 *
 * @param[in]: ioConfg
 * @return[out]: uint32_t
 **/
uint32_t io_init(io_confg_handler_t *ioConfg) {
  uint32_t ioIdx = 0U;

  // Setting the global io IOconfig structure
  IOconfig = ioConfg;

  // Initialising the inputs
  if (IOconfig->numIOInputs > 0U) {
    for (ioIdx = 0U; ioIdx < ioConfg->numIOInputs; ioIdx++) {
      // IO input handler to initialise the pins
      const io_in_handler_t *ioIn = &IOconfig->ioInputs[ioIdx];
      gpio_clock_enable(ioIn->portx);
      LL_GPIO_SetPinMode(ioIn->portx, ioIn->ioPinNo, LL_GPIO_MODE_INPUT);
      LL_GPIO_SetPinPull(ioIn->portx, ioIn->ioPinNo, ioIn->ioPupdr);
    }
  }

  if (IOconfig->ioOutputs > 0U) {
    // Initialising the outputs
    for (ioIdx = 0U; ioIdx < IOconfig->numIOOutputs; ioIdx++) {
      // IO output handler to initialise the pins
      const io_out_handler_t *ioOut = &IOconfig->ioOutputs[ioIdx];

      // Enabling the GPIO clock on the AHB bus
      gpio_clock_enable(ioOut->portx);

      LL_GPIO_SetPinMode(ioOut->portx, ioOut->ioPinNo, LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinSpeed(ioOut->portx, ioOut->ioPinNo, ioOut->ioSpeed);
      LL_GPIO_SetPinOutputType(ioOut->portx, ioOut->ioPinNo, ioOut->ioOutType);

      // Setting the initial values
      if (ioOut->ioInitVal == SET) {
        LL_GPIO_SetOutputPin(ioOut->portx, ioOut->ioPinNo);
      } else {
        LL_GPIO_ResetOutputPin(ioOut->portx, ioOut->ioPinNo);
      }
    }
  }

  // Return
  return EXIT_SUCCESS;
}

//
// @TODO: Create a 'open' handler which enables an interrupt for the specified
// input pin
//

/**
 * @brief: Get the input value of the IO pin
 *
 * @param[in]: ioInIdx. The input index of the ioInputs[] array
 * @return[out]: uint32_t
 **/
uint32_t io_get_val(uint32_t ioInIdx) {
  // Storing the Input Value
  uint8_t ioInVal = 0U;

  // Checking to see the ioInput index is valid
  if (ioInIdx > IOconfig->numIOInputs) {
    return IO_IN_SZE_ERR;
  }

  // Getting the input value
  if (IOconfig->ioInputs[ioInIdx].ioInInvert == ENABLE) {
    ioInVal = LL_GPIO_IsInputPinSet(IOconfig->ioInputs[ioInIdx].portx,
                                    IOconfig->ioInputs[ioInIdx].ioPinNo);

    // Inverting the value
    ioInVal = ioInVal ^ IOconfig->ioInputs[ioInIdx].ioInInvert;
  } else {
    ioInVal = LL_GPIO_IsInputPinSet(IOconfig->ioInputs[ioInIdx].portx,
                                    IOconfig->ioInputs[ioInIdx].ioPinNo);
  }

  // Returning the input val
  return ioInVal;
}

/**
 * @brief: Sets the value of the IO pin.
 *
 * @param[in]: ioOutIdx. The index of the output pin in the ioOutputs array
 * @param[in]: writeVal. The desired value to set
 * @return[out]: uint32_t
 **/
uint32_t io_set_val(uint32_t ioOutIdx, uint32_t writeVal) {
  // Checking to see if the ioOutput index is valid
  if (ioOutIdx > IOconfig->numIOOutputs) return IO_OUT_SZE_ERR;

  // Setting the pin HIGH
  if (writeVal) {
    LL_GPIO_SetOutputPin(IOconfig->ioOutputs[ioOutIdx].portx,
                         IOconfig->ioOutputs[ioOutIdx].ioPinNo);
  }

  // Settting the pin LOW
  else {
    LL_GPIO_ResetOutputPin(IOconfig->ioOutputs[ioOutIdx].portx,
                           IOconfig->ioOutputs[ioOutIdx].ioPinNo);
  }

  // Return
  return EXIT_SUCCESS;
}

/**
 * @brief: Toggles the value of the IO output pin
 *
 * @param[in]: ioOutIdx. The index of the output pin in the ioOutputs array
 * @return[out]: uint32_t
 **/
uint32_t io_toggle_val(uint32_t ioOutIdx) {
  if (ioOutIdx > IOconfig->numIOOutputs) return IO_OUT_SZE_ERR;

  LL_GPIO_TogglePin(IOconfig->ioOutputs[ioOutIdx].portx,
                    IOconfig->ioOutputs[ioOutIdx].ioPinNo);

  return EXIT_SUCCESS;
}

/**
 * @brief: This function returns the current value of an output pin
 *
 * @param[in]: ioOutIdx
 * @return[out]: uint32_t
 **/
uint32_t io_get_output_val(uint32_t ioOutIdx) {
  // Checks to see if the output idx is valid
  if (ioOutIdx > IOconfig->numIOOutputs) return IO_OUT_SZE_ERR;

  // Returns the current value of the output pin
  return LL_GPIO_IsOutputPinSet(IOconfig->ioOutputs[ioOutIdx].portx,
                                IOconfig->ioOutputs[ioOutIdx].ioPinNo);
}

////////////////////////////////////////////////////////////////////////////////
// Private (Static) Function Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief: This function enables the GPIO peripheral clock
 *
 * @param[in]: GPIOx
 * @return[out]: void
 **/
static void gpio_clock_enable(io_port *GPIOx) {
  // A temporay variable to determine the base address of the GPIO Register.
  uint32_t tmpGPIORegAddr = (uint32_t)GPIOx;

  // Uses the base address of the GPIO Register to enable the GPIO clock
  switch (tmpGPIORegAddr) {
    case GPIOA_BASE:

      if (LL_AHB1_GRP1_IsEnabledClock(GPIOA_BASE)) {
        break;
      } else {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        assert_param(
            LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_ALL_GPIOA));
      }
      break;

    case GPIOB_BASE:
      if (LL_AHB1_GRP1_IsEnabledClock(GPIOB_BASE)) {
        break;
      } else {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
        assert_param(
            LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_ALL_GPIOB));
      }

      break;

    case GPIOC_BASE:

      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
      assert_param(LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_GPIOC));

      break;

    case GPIOD_BASE:

      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
      assert_param(LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_GPIOD));
      break;

    case GPIOE_BASE:

      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
      assert_param(LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_GPIOE));
      break;

    case GPIOH_BASE:

      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
      assert_param(LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_GPIOH));

      break;
  }
}
