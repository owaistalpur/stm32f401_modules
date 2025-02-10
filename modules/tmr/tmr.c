/**
 * @file
 * @author Owais Talpur (owaistalpur@hotmail.com)
 * @brief
 * @version
 * @date
 *
 * @copyright Copyright (c) 2025
 *
 **/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <tmr.h>

////////////////////////////////////////////////////////////////////////////////
// Private (Static) function declarations
////////////////////////////////////////////////////////////////////////////////
__STATIC_INLINE void tmr_2_interrupt(void);
__STATIC_INLINE void tmr_3_interrupt(void);
__STATIC_INLINE void tmr_4_interrupt(void);
////////////////////////////////////////////////////////////////////////////////
// Private (Static) variables
////////////////////////////////////////////////////////////////////////////////
static tmr_info_t tmr[TMR_NUM_INSTANCES];

static uint32_t tmrPrescLookup[TMR_BASE_NUM] = {
    84U,  /* Prescaler to achieve a base unit of 1 microsecond */
    8400U /* Prescaler to achieve a base unit of 1 millisecond */
};

static char tmrInstName[4U][7U] = {"Timer2", "Timer3", "Timer4"};
////////////////////////////////////////////////////////////////////////////////
// Public (global) variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Public (global) function definitions
////////////////////////////////////////////////////////////////////////////////
/* Default Init function */
/**
 * @brief: Creates a default tmr that works on TIMER3 with a base of 1ms
 *
 * @param[in]: tmrConfig
 * @return[out]: uint32_t
 **/
uint32_t tmr_def_init(tmr_config_t* tmrConfig) {
  /* Checking to see if the tmrConfig is already configured */
  if (tmrConfig == NULL) {
    return EXIT_FAILURE;
  }

  /* Setting the base timer unit and instances ID */
  tmrConfig->tmrInstancesId = TMR_INSTANCE3;
  tmrConfig->tmrBaseUnit = TMR_BASE_1MS;

  uint8_t tmrIdx = tmrConfig->tmrInstancesId;

  /* Configuring the main timers handler */
  (void)memset(&tmr[tmrIdx], RESET, sizeof(tmr_info_t));

  if (tmr[tmrIdx].usrConfig == NULL) {
    tmr[tmrIdx].usrConfig = tmrConfig;
  } else {
    LOG_ERR("Error: Timer default already being used\n");
  }
  return EXIT_SUCCESS;
}

/* Init function */
/**
 * @brief: Initialises a tmr module according to usr configurations
 *
 * @param[in]: tmrConfig
 * @return[out]: uint32_t
 **/
uint32_t tmr_init(tmr_config_t* tmrConfig) {
  /* Checking to see if the tmrConfig is already configured */
  if (tmrConfig == NULL) {
    return EXIT_FAILURE;
  }

  uint8_t tmrIdx = tmrConfig->tmrInstancesId;

  if (tmr[tmrIdx].usrConfig != NULL || tmr[tmrIdx].usrConfig != RESET) {
    LOG_ERR("Error: Tmr module already initialised\n");
    return EXIT_FAILURE;
  }

  /* Configuring the main timers handler */
  (void)memset(&tmr[tmrIdx], RESET, sizeof(tmr_info_t));

  /* Setting the ... */
  tmr[tmrIdx].usrConfig = tmrConfig;

  return EXIT_SUCCESS;
}

/* Open function*/
/**
 * @brief: Opens a timer instance and sets up the timer with a callback
 *         function.
 *
 * @param[in]: tmrIdx
 * @param[in]: cbFunc
 * @return[out]: uint32_t
 **/
uint32_t tmr_open(uint32_t tmrIdx, tmr_cb_func cbFunc) {
  /* Checking to see if the tmr idx is valid */
  if (tmrIdx > TMR_NUM_INSTANCES) return EXIT_FAILURE;
  tmr_info_t* tmpTmr = &tmr[tmrIdx];

  if (tmpTmr == NULL) {
    return EXIT_FAILURE;
  }

  uint32_t tmpVar = tmpTmr->usrConfig->tmrInstancesId;
  switch (tmpVar) {
    case TMR_INSTANCE2:
      tmpTmr->tmrReg = TMR_TIMER2;
      break;

    case TMR_INSTANCE3:
      tmpTmr->tmrReg = TMR_TIMER3;
      break;

    case TMR_INSTANCE4:
      tmpTmr->tmrReg = TMR_TIMER4;
      break;
  }

  tmpVar = tmpTmr->usrConfig->tmrBaseUnit;
  switch (tmpVar) {
    case TMR_BASE_1US:

      LL_TIM_SetPrescaler(tmpTmr->tmrReg, tmrPrescLookup[TMR_BASE_1US]);
      break;
    case TMR_BASE_1MS:

      LL_TIM_SetPrescaler(tmpTmr->tmrReg, tmrPrescLookup[TMR_BASE_1MS]);
      break;
    default:
      break;
  }

  if (cbFunc == NULL) {
        LOG_ERR("Error: Invalid timer callback function\n");
  } else {
    tmpTmr->cbFunc = cbFunc;
  }

  tmpTmr->isInstOpen = true;

  return EXIT_SUCCESS;
}
/********************** Close function *****************************/
/**
 * @brief
 *
 * @param[in]: tmrIdx
 * @return[out]: uint32_t
 **/
uint32_t tmr_close(uint32_t tmrIdx) {
  if (tmrIdx > TMR_NUM_INSTANCES || tmrIdx < 0U) {
    return EXIT_FAILURE;
  }

  /* Disabling Interrutps */
  __disable_irq();

  tmr_info_t* tmpTmr = &tmr[tmrIdx];
  if (tmpTmr == NULL) return EXIT_FAILURE;

  if (!tmpTmr->isInstOpen) {
    LOG_ERR("Error: Tmr instances is not open and so cannot be closed\n");
    return EXIT_FAILURE;
  }

  /* Disabling the counter */
  LL_TIM_DisableCounter(tmpTmr->tmrReg);

  /* Turning off the interrupt */
  LL_TIM_DisableIT_UPDATE(tmpTmr->tmrReg);

  tmpTmr->cbFunc = NULL;
  tmpTmr->isInstOpen = false;
  tmpTmr->isTmrRunning = false;

  __enable_irq();

  return EXIT_SUCCESS;
}

/* Write function */
/**
 * @brief
 *
 * @param[in]: tmrIdx
 * @param[in]: desiredMS
 * @return[out]: uint32_t
 **/
uint32_t tmr_write(uint32_t tmrIdx, uint32_t desiredMS) {
  if (tmrIdx > TMR_NUM_INSTANCES) {
    return EXIT_FAILURE;
  }

  IRQn_Type tmrIrq = 0U;
  tmr_info_t* tmpTmr = &tmr[tmrIdx];

  if (!tmpTmr->isInstOpen) {
    LOG_ERR("Error: Tmr is not open\n");
    return EXIT_FAILURE;
  }

  /* Selecting the correct tmr IRQ based on the tmr register base addres */
  switch ((uint32_t)tmpTmr->tmrReg) {
    case TIM2_BASE:
      tmrIrq = TIM2_IRQn;
      break;
    case TIM3_BASE:
      tmrIrq = TIM3_IRQn;
      break;
    case TIM4_BASE:
      tmrIrq = TIM4_IRQn;
      break;

    default:
      LOG_ERR("Error: Tmr IRQn failed to initialise\n");
      return EXIT_FAILURE;
  }

  uint32_t tmpVar = tmpTmr->usrConfig->tmrBaseUnit;

  uint32_t tmpMs;
  switch (tmpVar) {
    case TMR_BASE_1US:
      /* Calculating the autoreload register value for the desired time */
      tmpMs = desiredMS;

      TMR_ARR_MAX(tmpMs);

      /* Updating the tmr update to ... */
      tmpTmr->tmrTime = desiredMS;

      /* Setting the autoreload register */
      LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpMs);
      break;

    case TMR_BASE_1MS:
      /* Calculating the autoreload register value for the desired time */
      tmpMs = 10U * desiredMS;

      TMR_ARR_MAX(tmpMs);

      /* Updating the tmr update to ... */
      tmpTmr->tmrTime = desiredMS;

      /* Setting the autoreload register */
      LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpMs);
      break;
  }

  /* Setting the Update interrupt for the tmr */
  LL_TIM_EnableIT_UPDATE(tmpTmr->tmrReg);

  /* Enabling the interrupt and setting priority */
  NVIC_SetPriority(tmrIrq,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U,
                                       0U));  // Set priority (0 = highest)
  NVIC_EnableIRQ(tmrIrq);                     // Enable TIM3 interrupt in NVIC

  /* Enabling the tmr counter */
  LL_TIM_EnableCounter(tmpTmr->tmrReg);

  /* Setting the tmr running */
  tmpTmr->isTmrRunning = true;

  return EXIT_SUCCESS;
}

/* Read Function */
/**
 * @brief: This function reads the current status of the specified tmr
 *
 * @param[in]: tmrIdx
 * @return[out]: uint32_t
 **/
uint32_t tmr_read(uint32_t tmrIdx) {
  if (tmrIdx > TMR_NUM_INSTANCES) {
    return EXIT_FAILURE;
  }

  tmr_info_t* tmpTmr = &tmr[tmrIdx];

  if (tmpTmr == NULL || !tmpTmr->isInstOpen) {
    LOG_ERR("Error: Tmr is not open");
  }

  printf("\nTmr\tOpen\tTime\n");
  printf("===\t====\t====\n\n");
  printf("%s\t%ld\t%ld\n", tmrInstName[tmrIdx], (uint32_t)tmpTmr->isInstOpen,
         tmpTmr->tmrTime);

  return EXIT_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////
// Private (static) function definitions
////////////////////////////////////////////////////////////////////////////////
/* Tmr interrupt handlers */
__STATIC_INLINE void tmr_2_interrupt(void) { tmr[TMR_INSTANCE2].cbFunc(); }
__STATIC_INLINE void tmr_3_interrupt(void) { tmr[TMR_INSTANCE3].cbFunc(); }
__STATIC_INLINE void tmr_4_interrupt(void) { tmr[TMR_INSTANCE4].cbFunc(); }

/* TIMER 2 IRQ */
void TIM2_IRQHandler(void) {
  if (LL_TIM_IsActiveFlag_UPDATE(tmr[TMR_INSTANCE2].tmrReg)) {
    LL_TIM_ClearFlag_UPDATE(tmr[TMR_INSTANCE2].tmrReg);
  }

  __disable_irq();
  tmr_2_interrupt();
  __enable_irq();
}

/* TIMER 3 IRQ */
void TIM3_IRQHandler(void) {
  if (LL_TIM_IsActiveFlag_UPDATE(tmr[TMR_INSTANCE3].tmrReg)) {
    LL_TIM_ClearFlag_UPDATE(tmr[TMR_INSTANCE3].tmrReg);
  }

  __disable_irq();
  tmr_3_interrupt();
  __enable_irq();
}

/* TIMER 4 IRQ */
void TIM4_IRQHandler(void) {
  if (LL_TIM_IsActiveFlag_UPDATE(tmr[TMR_INSTANCE4].tmrReg)) {
    LL_TIM_ClearFlag_UPDATE(tmr[TMR_INSTANCE4].tmrReg);
  }

  __disable_irq();
  tmr_4_interrupt();
  __enable_irq();
}
