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
#include "tmr.h"

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
	if(tmrConfig != NULL) return TMR_CONFG_ALREADY_CONFIGED;
	
	/* Setting the base timer unit and instances ID */
	tmrConfig->tmrInstancesId = TMR_INSTANCE3;
	tmrConfig->tmrBaseUnit = TMR_BASE_1MS;

	uint8_t tmrIdx = tmrConfig->tmrInstancesId;

	/* Configuring the main timers config handler */
	(void)memset(&tmr[tmrIdx], RESET, sizeof(tmr_info_t));
	
	/* Checking to see if the TMR3 instance is already configured */ 
	if (tmr[tmrIdx].usrConfig == NULL) {
		tmr[tmrIdx].usrConfig = tmrConfig;
	}  
	else {
		return TMR_INST_ALREADY_CONFIGED;
	}
	return TMR_RETURN_SUCCESS; 
}

/* Init function */
/**
 * @brief: Initialises a tmr module according to usr configurations
 *
 * @param[in]: tmrConfig
 * @return[out]: uint32_t
 **/
uint32_t tmr_init(tmr_config_t* tmrConfig) {

	/* Checking to see if the configuration struct is valid */
	if (tmrConfig == NULL) {
		return TMR_CONFIG_NULL;
	}

	uint8_t tmrIdx = tmrConfig->tmrInstancesId;

	/* Configuring the main timers config handler */
	(void)memset(&tmr[tmrIdx], RESET, sizeof(tmr_info_t));
	
	/* Checking to see if the TMR3 instance is already configured */ 
	if (tmr[tmrIdx].usrConfig == NULL) {
		tmr[tmrIdx].usrConfig = tmrConfig;
	}  
	else {
		return TMR_INST_ALREADY_CONFIGED;
	}

	return TMR_RETURN_SUCCESS;
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
uint32_t tmr_open(uint32_t tmrIdx, tmr_cb_func cbFunc, uint32_t time) {
	/* Checking to see if the tmr idx is valid */
	if (tmrIdx > TMR_NUM_INSTANCES) return TMR_INVALID_IDX;

	/* Creating a temporary tmr instances, so its easier access the instance */
	tmr_info_t* tmpTmr = &tmr[tmrIdx];
	if (tmpTmr == NULL) {
		return TMR_CONFIG_NULL;
	}

	uint32_t tmpVar;
	// // Getting the tmr
	// tmpVar = tmpTmr->usrConfig->tmrInstancesId;

	// Setting the appropriate STM32 TIMER Registers
	switch (tmrIdx) {
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

	// Initialising the tmr prescaler to achieve the required baseunit (us or ms)
	tmpVar = tmpTmr->usrConfig->tmrBaseUnit;
	uint32_t tmpTime = 0;
	switch (tmpVar) {
		case TMR_BASE_1US:

			// Setting the prescaler
			LL_TIM_SetPrescaler(tmpTmr->tmrReg, tmrPrescLookup[TMR_BASE_1US]);

			// configuring the autoreload register to get the desired time
			// interval
			tmpTime = time;
			TMR_ARR_MAX(tmpTime);
			tmpTmr->tmrTime = tmpTime;
			LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpTime);

			break;
		case TMR_BASE_1MS:

			// Setting the prescaler
			LL_TIM_SetPrescaler(tmpTmr->tmrReg, tmrPrescLookup[TMR_BASE_1MS]);

			// configuring the autoreload register to get the desired time
			// interval
			tmpTime = 10 * time;
			TMR_ARR_MAX(tmpTime);
			tmpTmr->tmrTime = tmpTime;
			LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpTime);

			break;

		default:
			return TMR_INVALID_BASEUNIT; 
	}

	// Setting up the IRQ for the tmr module
	IRQn_Type tmrIrq = 0U;

	/* Selecting the correct tmr IRQ based on tmrIdx */
	switch (tmrIdx) {
		case TMR_INSTANCE2:
			tmrIrq = TIM2_IRQn;
			break;
		case TMR_INSTANCE3:
			tmrIrq = TIM3_IRQn;
			break;
		case TMR_INSTANCE4:
			tmrIrq = TIM4_IRQn;
			break;
		default:
			return TMR_INVALID_IDX;

	}
	/* Setting the Update interrupt for the tmr */
	LL_TIM_EnableIT_UPDATE(tmpTmr->tmrReg);

	/* Enabling the interrupt and setting priority */
	NVIC_SetPriority(tmrIrq,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U,
				0U));  // Set priority (0 = highest)
	NVIC_EnableIRQ(tmrIrq);                     // Enable TIM3 interrupt in NVIC

	// Setting the callback function
	if (cbFunc == NULL) {
		return TMR_INVALID_CBFUNC;
	} else {
		tmpTmr->cbFunc = cbFunc;
	}
	
	tmpTmr->isInstOpen = true;

	/* Enabling the tmr counter */
	LL_TIM_EnableCounter(tmpTmr->tmrReg);

	/* Setting the tmr running */
	tmpTmr->isTmrRunning = true;

	return TMR_RETURN_SUCCESS;
}

/* Write function */
/**
 * @brief
 *
 * @param[in]: tmrIdx
 * @param[in]: desiredMS
 * @return[out]: uint32_t
 **/
uint32_t tmr_write(uint32_t tmrIdx, uint32_t time) {
	if (tmrIdx > TMR_NUM_INSTANCES) {
		return EXIT_FAILURE;
	}

	tmr_info_t* tmpTmr = &tmr[tmrIdx];
	if (!tmpTmr->isInstOpen) {
		return EXIT_FAILURE;
	}
	if(tmpTmr->isTmrRunning){
		__disable_irq();
		LL_TIM_DisableIT_UPDATE(tmpTmr->tmrReg);
		tmpTmr->isTmrRunning = false;
		__enable_irq();
	}
	
	uint32_t tmpVar = tmpTmr->usrConfig->tmrBaseUnit;
	uint32_t tmpTime;

	switch (tmpVar) {
		case TMR_BASE_1US:
			/* Calculating the autoreload register value for the desired time */
			tmpTime = time;

			TMR_ARR_MAX(tmpTime);

			/* Updating the tmr update to ... */
			tmpTmr->tmrTime = time;
			/* Setting the autoreload register */
			LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpTime);
			break;

		case TMR_BASE_1MS:
			/* Calculating the autoreload register value for the desired time */
			tmpTime = 10U * time; 

			TMR_ARR_MAX(tmpTime);

			/* Updating the tmr update to ... */
			tmpTmr->tmrTime = time;

			/* Setting the autoreload register */
			LL_TIM_SetAutoReload(tmpTmr->tmrReg, tmpTime);
			break;
	}

	/* Setting the Update interrupt for the tmr */
	LL_TIM_EnableIT_UPDATE(tmpTmr->tmrReg);

	/* Enabling the tmr counter */
	LL_TIM_EnableCounter(tmpTmr->tmrReg);

	/* Setting the tmr running */
	tmpTmr->isTmrRunning = true;

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
		return EXIT_FAILURE;
	}

	printf("\n\rTmr\tOpen\tTime\n\r");
	printf("===\t====\t====\n\n\r");
	printf("%s\t%ld\t%ld\n\r", tmrInstName[tmrIdx], (uint32_t)tmpTmr->isInstOpen,
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
