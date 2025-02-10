
#include <ttys.h>

////////////////////////////////////////////////////////////////////////////////
// Private (Static) Variables
////////////////////////////////////////////////////////////////////////////////
static ttys_handler_t ttysInstances[TTYS_NUM_INSTANCES];
static void ttys_interrupt(uint32_t ttysInstId, IRQn_Type irqType);

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief: This function initialises the ttys instance by setting the rx and tx
 * buffers to 0 and setting the index values to 0.
 *
 * @param[in]: ttysInstIdx
 * @param[in]: ttysInst
 * @return[out]: uint32_t
 **/

uint32_t ttys_def_init(uint32_t ttysInstIdx) {
  // Checks to see if the index is valid
  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  // Creates a temporary ttys handler to modify the ttys instances
  ttys_handler_t* ttysTmp = &ttysInstances[ttysInstIdx];

  // Initialises it to zero
  (void)memset(ttysTmp, RESET, sizeof(ttys_handler_t));
  ttysTmp->isInstOpen = true;

  return EXIT_SUCCESS;
}

uint32_t ttys_init(uint32_t ttysInstIdx) {
  ttys_handler_t* ttysTmp = NULL;

  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  ttysTmp = &ttysInstances[ttysInstIdx];

  switch (ttysInstIdx) {
    case TTYS_INSTANCE_1:

      ttysTmp->ttysPortx = TTYS_PORT_1;

      break;

    case TTYS_INSTANCE_2:

      ttysTmp->ttysPortx = TTYS_PORT_2;
      break;

    case TTYS_INSTANCE_3:

      ttysTmp->ttysPortx = TTYS_PORT_3;
      break;

    default:
      return TTYS_ERR_IDX;
  }
  ttysTmp->txGetIdx = 0U;
  ttysTmp->txPutIdx = 0U;
  ttysTmp->rxGetIdx = 0U;
  ttysTmp->rxPutIdx = 0U;

  (void)memset(ttysTmp->rxBuffer, 0U, MAX_BUFFER_SIZE);
  (void)memset(ttysTmp->txBuffer, 0U, MAX_BUFFER_SIZE);

  return EXIT_SUCCESS;
}

/**
 * @brief: This function start the ttys instance by 'opening' it.
 *
 * @param[in]: ttysInstId
 * @return[out]: uint32_t
 **/
uint32_t ttys_start(uint32_t ttysInstIdx) {
  ttys_handler_t* ttysTmp;
  IRQn_Type irqType = 0U;

  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  ttysTmp = &ttysInstances[ttysInstIdx];

  // Enabling the TX and RX interrupts for the ttys instances
  LL_USART_EnableIT_RXNE(ttysTmp->ttysPortx);

  // Setting the NVIC IRQ type
  switch (ttysInstIdx) {
    case TTYS_INSTANCE_1:

      irqType = USART1_IRQn;
      break;

    case TTYS_INSTANCE_2:

      irqType = USART2_IRQn;
      break;

    case TTYS_INSTANCE_3:
      irqType = USART6_IRQn;
      break;
  }

  NVIC_SetPriority(irqType,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U, 0U));
  NVIC_EnableIRQ(irqType);

  return EXIT_SUCCESS;
}

/**
 * @brief: This function closes the ttys instance by disbaling and reseting the
 *buffers and indexes to zero.
 *
 * @param[in]: ttysInstId
 * @return[out]: uint32_t
 **/
uint32_t ttys_close(uint32_t ttysInstIdx) {
  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  ttys_handler_t* tmpTtys;

  tmpTtys = &ttysInstances[ttysInstIdx];

  // Clearing the Tx and Rx buffers
  (void)memset(tmpTtys->rxBuffer, 0U, MAX_BUFFER_SIZE);
  (void)memset(tmpTtys->txBuffer, 0U, MAX_BUFFER_SIZE);

  // Closing the ttys instances
  tmpTtys->isInstOpen = false;

  return EXIT_SUCCESS;
}

/**
 * @brief
 *
 * @param[in]: ttysInstId
 * @return[out]: uint32_t
 **/
char ttys_getc(uint32_t ttysInstIdx) {
  char dataRec = 0U;

  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  ttys_handler_t* ttysTmp = &ttysInstances[ttysInstIdx];

  if (!ttysTmp->isInstOpen) {
    return EXIT_FAILURE;
  }

  uint32_t putIdx = ttysTmp->rxPutIdx;

  if (putIdx + 1 > MAX_BUFFER_SIZE - 1) putIdx = 0U;

  uint32_t getIdx = ttysTmp->rxGetIdx + 1;
  if (getIdx > MAX_BUFFER_SIZE) getIdx = 0U;

  dataRec = ttysTmp->rxBuffer[getIdx];

  if (dataRec != 0U) {
    ttysTmp->rxGetIdx = getIdx;
  }

  return dataRec;
}

/**
 * @brief
 *
 * @param[in]: ttysInstId
 * @return[out]: uint32_t
 **/
uint32_t ttys_read_buf(uint32_t ttysInstIdx) {
  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;

  ttys_handler_t* ttysTmp = &ttysInstances[ttysInstIdx];
  char data;

  if (!ttysTmp->isInstOpen) return EXIT_FAILURE;

  uint32_t getIdx = ttysTmp->rxGetIdx + 1;
  if (getIdx > MAX_BUFFER_SIZE) getIdx = 0U;

  data = ttysTmp->rxBuffer[getIdx];

  if (data != 0U) {
    ttysTmp->rxGetIdx = getIdx;
  }

  return data;
}

/**
 * @brief
 *
 * @param[in]: ttysInstId
 * @param[in]: data
 * @return[out]: uint32_t
 **/
uint32_t ttys_putc(uint32_t ttysInstIdx, char data) {
  if (ttysInstIdx > TTYS_NUM_INSTANCES || ttysInstIdx < 0U) return TTYS_ERR_IDX;
  ttys_handler_t* ttysTmp = &ttysInstances[ttysInstIdx];

  uint16_t txPutIdx = ttysTmp->txPutIdx;
  uint16_t txGetIdx = ttysTmp->txGetIdx;

  txPutIdx = txPutIdx + 1;

  if ((txPutIdx) > MAX_BUFFER_SIZE - 1) txPutIdx = 0U;
  if (txGetIdx + 1 > MAX_BUFFER_SIZE) txGetIdx = 0U;
  while (!LL_USART_IsActiveFlag_TXE(ttysTmp->ttysPortx));

  ttysTmp->txBuffer[ttysTmp->txPutIdx] = data;
  LL_USART_TransmitData8(ttysTmp->ttysPortx, ttysTmp->txBuffer[txGetIdx++]);

  while (!LL_USART_IsActiveFlag_TC(ttysTmp->ttysPortx));

  ttysTmp->txPutIdx = txPutIdx;
  ttysTmp->txGetIdx = txGetIdx;

  return EXIT_SUCCESS;
}

void USART2_IRQHandler(void) { ttys_interrupt(TTYS_INSTANCE_2, USART2_IRQn); }
//

static void ttys_interrupt(uint32_t ttysInstIdx, IRQn_Type irqType) {
  ttys_handler_t* ttysTmp;
  uint8_t usartSr = 0;

  ttysTmp = &ttysInstances[ttysInstIdx];

  if (ttysTmp->ttysPortx == NULL) {
    NVIC_DisableIRQ(irqType);
    return;
  }

  usartSr = ttysTmp->ttysPortx->SR;

  // Check to see if data is received
  if (usartSr & LL_USART_SR_RXNE) {
    uint32_t putIdx = ttysTmp->rxPutIdx + 1;

    if (putIdx > MAX_BUFFER_SIZE - 1) putIdx = 0U;

    // char dataRec = ttysTmp->ttysPortx->DR;
    char dataRec = 0U;
    dataRec = LL_USART_ReceiveData8(TTYS_PORT_2);

    ttysTmp->rxBuffer[putIdx] = dataRec;
    ttysTmp->rxPutIdx = putIdx;
  }
}

/**
 * @brief: Write function
 *
 **/
__attribute__((weak)) int _write(int file, char* ptr, int len) {
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    ttys_putc(TTYS_INSTANCE_2, *ptr++);
  }
  return len;
}
