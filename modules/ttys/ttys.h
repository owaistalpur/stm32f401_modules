#ifndef USART_H
#define USART_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
/* Standard includes */
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>

/* MCU includes */
#include <stm32f4xx_ll_usart.h>

////////////////////////////////////////////////////////////////////////////////
// Common macros
////////////////////////////////////////////////////////////////////////////////
#define NON_BLOCKING DISABLE
#define MAX_BUFFER_SIZE 80U

//
// TTYs Mappings for the STM32F401RE
//

#define TTYS_PORT_1_BASE (USART1_BASE)
#define TTYS_PORT_2_BASE (USART2_BASE)
#define TTYS_PORT_3_BASE (USART6_BASE)

#define TTYS_PORT_1 (USART1)
#define TTYS_PORT_2 (USART2)
#define TTYS_PORT_3 (USART6)

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////
typedef USART_TypeDef ttys_port_t;

/* Error Codes */
typedef enum {

  TTYS_ERR_IDX = 0x54U,
  TTYS_ERR_RX,
  TTYS_ERR_TX,

} ttys_errors_t;

/* Ttys Instances */
typedef enum {

  TTYS_INSTANCE_1,
  TTYS_INSTANCE_2,
  TTYS_INSTANCE_3,

  TTYS_NUM_INSTANCES
} ttys_instance_id_t;

/* Ttys Handler */
typedef struct {
  ttys_port_t *ttysPortx;

  uint32_t txPutIdx;
  uint32_t txGetIdx;

  uint32_t rxPutIdx;
  uint32_t rxGetIdx;

  char txBuffer[MAX_BUFFER_SIZE];
  char rxBuffer[MAX_BUFFER_SIZE];

  bool isInstOpen;
} ttys_handler_t;

////////////////////////////////////////////////////////////////////////////////
// Module Interface
////////////////////////////////////////////////////////////////////////////////

/* Core API */
uint32_t ttys_def_init(uint32_t ttysInstIdx);
uint32_t ttys_init(uint32_t ttysInstIdx);
uint32_t ttys_start(uint32_t ttysInstIdx);
uint32_t ttys_putc(uint32_t ttysInstIdx, char data);
uint32_t ttys_read_buf(uint32_t ttysInstIdx);

/* Other API */
char ttys_getc(uint32_t ttysInstIdx);

#endif  // usart.h