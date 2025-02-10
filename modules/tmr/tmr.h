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

/* Standard includes */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MCU includes*/
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>

/* Module includes */
#include <log.h>

////////////////////////////////////////////////////////////////////////////////
// Common macros
////////////////////////////////////////////////////////////////////////////////

#define SET 1U
#define RESET 0U
#define ENABLE SET
#define DISBALE RESET

/* tmr module ... */
#define TMR_TIMER2 TIM2
#define TMR_TIMER3 TIM3
#define TMR_TIMER4 TIM4
#define TMR_TIMER9 TIM9

#define TMR_MAX 3U

#define __SHORT_MAX__ 65535U
#define TMR_ARR_MAX(var) (var = (var > __SHORT_MAX__) ? (__SHORT_MAX__) : (var))

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////
typedef void (*tmr_cb_func)(void);

typedef TIM_TypeDef tmrBase;

typedef enum {

  TMR_INSTANCE2,
  TMR_INSTANCE3,
  TMR_INSTANCE4,

  TMR_NUM_INSTANCES
} tmr_instances_t;

typedef enum {

  /* Microseconds */
  TMR_BASE_1US,

  /* Milliseconds */
  TMR_BASE_1MS,

  /* Number of tmr base units */
  TMR_BASE_NUM
} tmr_base_unit_t;

/* Config struct */
typedef struct {
  uint32_t tmrInstancesId;
  uint32_t tmrBaseUnit;

} tmr_config_t;

typedef struct {
  tmr_config_t* usrConfig;
  tmrBase* tmrReg;

  tmr_cb_func cbFunc;
  uint32_t tmrTime;
  bool isTmrRunning;
  bool isInstOpen;

} tmr_info_t;

////////////////////////////////////////////////////////////////////////////////
// Public (global) function declarations
////////////////////////////////////////////////////////////////////////////////

/* Core */
uint32_t tmr_def_init(tmr_config_t* tmrConfig);
uint32_t tmr_init(tmr_config_t* tmrConfig);
uint32_t tmr_open(uint32_t tmrIdx, tmr_cb_func cbFunc);
uint32_t tmr_close(uint32_t tmrIdx);
uint32_t tmr_write(uint32_t tmrIdx, uint32_t desiredMS);

/* Other */
uint32_t tmr_read(uint32_t tmrIdx);
