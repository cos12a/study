/*
*********************************************************************************************************
*                                       uC/Probe Communication
*
*                         (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        COMMUNICATION: RS-232
*
*                                          PORT FOR ST STM32
*
* Filename      : probe_rs232c.c
* Version       : V2.20
* Programmer(s) : BAN
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <probe_rs232.h>
//#include  <includes.h>
//#include  <stm32.h>
//#include  <core_cm3.h>


#include "usart.h"

/*
*********************************************************************************************************
*                                               ENABLE
*
* Note(s) : (1) See 'probe_rs232.h  ENABLE'.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_RS232_EN == DEF_ENABLED)                     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define PROBE_PORT          USART2
#define PROBE_RS232_UART_1_REMAP
#define PROBE_RS232_UART_2_REMAP
#define PROBE_RS232_UART_3_REMAP_PARTIAL
#define PROBE_RS232_UART_3_REMAP_FULL

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
extern UART_HandleTypeDef huart2;

static volatile uint8_t sendData = 0u;
static volatile uint32_t                 ErrorCode;           /*!< UART Error code                    */


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
__inline static void UART_RxISR_8BIT(void);
__inline static void  UART_Receive_IT_En(void);
__inline static void UART_EndRxTransfer(void);
__inline static void UART_TxISR_8BIT(void);
__inline static void UART_EndTransmit_IT(void);

static void Probe_UART_ErrorCallback(void);

/*
*********************************************************************************************************
*                                      LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef    PROBE_RS232_CFG_COMM_SEL

  #error  "PROBE_RS232_CFG_COMM_SEL          not #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  PROBE_RS232_UART_1]     "
  #error  "                                  [     ||  PROBE_RS232_UART_2]     "
  #error  "                                  [     ||  PROBE_RS232_UART_3]     "

#elif     (PROBE_RS232_CFG_COMM_SEL != PROBE_RS232_UART_1) && \
          (PROBE_RS232_CFG_COMM_SEL != PROBE_RS232_UART_2) && \
          (PROBE_RS232_CFG_COMM_SEL != PROBE_RS232_UART_3)

  #error  "PROBE_RS232_CFG_COMM_SEL    illegally #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  PROBE_RS232_UART_1]     "
  #error  "                                  [     ||  PROBE_RS232_UART_2]     "
  #error  "                                  [     ||  PROBE_RS232_UART_3]     "
#endif

#ifndef    PROBE_RS232_UART_1_REMAP

  #error  "PROBE_RS232_UART_1_REMAP          not #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  DEF_TRUE ]              "
  #error  "                                  [     ||  DEF_FALSE]              "
#endif

#ifndef    PROBE_RS232_UART_2_REMAP

  #error  "PROBE_RS232_UART_2_REMAP          not #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  DEF_TRUE ]              "
  #error  "                                  [     ||  DEF_FALSE]              "
#endif

#ifndef    PROBE_RS232_UART_3_REMAP_PARTIAL

  #error  "PROBE_RS232_UART_3_REMAP_PARTIAL  not #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  DEF_TRUE ]              "
  #error  "                                  [     ||  DEF_FALSE]              "
#endif

#ifndef    PROBE_RS232_UART_3_REMAP_FULL

  #error  "PROBE_RS232_UART_3_REMAP_FULL     not #define'd in 'probe_com_cfg.h'"
  #error  "                                  [MUST be  DEF_TRUE ]              "
  #error  "                                  [     ||  DEF_FALSE]              "
#endif


/*
*********************************************************************************************************
*                                       ProbeRS232_InitTarget()
*
* Description : Initialize the UART for Probe communication.
*
* Argument(s) : baud_rate       Intended baud rate of the RS-232.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_Init().
*
* Note(s)     : (1) Though the baud rate used may vary from application to application or target to
*                   target, other communication settings are constamt.  The hardware must always be
*                   configured for the following :
*
*                   (a) No parity
*                   (b) One stop bit
*                   (c) Eight data bits.
*
*               (2) Neither receive nor transmit interrupts should be enabled by this function.
*
*               (3) The following constants control the GPIO remap for the USART control lines :
*
*                        PROBE_RS232_UART_1_REMAP
*                        PROBE_RS232_UART_2_REMAP
*                        PROBE_RS232_UART_3_REMAP_PARTIAL
*                        PROBE_RS232_UART_3_REMAP_FULL
*
*                    Though the #error directives in 'LOCAL CONFIGURATION ERRORS' will require that
*                    all are defined, the value of those bearing on the USART not used will have no
*                    effect.
*
*                (4) PROBE_RS232_UART_3_REMAP_PARTIAL has precedence over PROBE_RS232_UART_3_REMAP_FULL,
*                    if both are defined to DEF_TRUE.
*********************************************************************************************************
*/

void  ProbeRS232_InitTarget (CPU_INT32U baud_rate)
{

    MX_USART2_UART_Init();
    UART_Receive_IT_En();
//    ProbeRS232_Tx1(':');
//    ProbeRS232_Tx1('O');
//    ProbeRS232_Tx1('K');

}


/*
*********************************************************************************************************
*                                     ProbeRS232_RxTxISRHandler()
*
* Description : Handle Rx and Tx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) If this port is used in an RTOS, the interrupt entrance/exit procedure should be
*                   performed by the ISR that calls this handler.
*********************************************************************************************************
*/

/** @brief  Enable the specified UART interrupt.
* @param  __HANDLE__ specifies the UART Handle.
* @param  __INTERRUPT__ specifies the UART interrupt source to enable.
*          This parameter can be one of the following values:
*            @arg @ref UART_IT_RXFF  RXFIFO Full interrupt
*            @arg @ref UART_IT_TXFE  TXFIFO Empty interrupt
*            @arg @ref UART_IT_RXFT  RXFIFO threshold interrupt
*            @arg @ref UART_IT_TXFT  TXFIFO threshold interrupt
*            @arg @ref UART_IT_WUF   Wakeup from stop mode interrupt
*            @arg @ref UART_IT_CM    Character match interrupt
*            @arg @ref UART_IT_CTS   CTS change interrupt
*            @arg @ref UART_IT_LBD   LIN Break detection interrupt
*            @arg @ref UART_IT_TXE   Transmit Data Register empty interrupt
*            @arg @ref UART_IT_TXFNF TX FIFO not full interrupt
*            @arg @ref UART_IT_TC    Transmission complete interrupt
*            @arg @ref UART_IT_RXNE  Receive Data register not empty interrupt
*            @arg @ref UART_IT_RXFNE RXFIFO not empty interrupt
*            @arg @ref UART_IT_IDLE  Idle line detection interrupt
*            @arg @ref UART_IT_PE    Parity Error interrupt
*            @arg @ref UART_IT_ERR   Error interrupt (frame error, noise error, overrun error)
* @retval None
*/

/** @brief  Clear the specified UART pending flag.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __FLAG__ specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg @ref UART_CLEAR_PEF      Parity Error Clear Flag
  *            @arg @ref UART_CLEAR_FEF      Framing Error Clear Flag
  *            @arg @ref UART_CLEAR_NEF      Noise detected Clear Flag
  *            @arg @ref UART_CLEAR_OREF     Overrun Error Clear Flag
  *            @arg @ref UART_CLEAR_IDLEF    IDLE line detected Clear Flag
  *            @arg @ref UART_CLEAR_TXFECF   TXFIFO empty clear Flag
  *            @arg @ref UART_CLEAR_TCF      Transmission Complete Clear Flag
  *            @arg @ref UART_CLEAR_LBDF     LIN Break Detection Clear Flag
  *            @arg @ref UART_CLEAR_CTSF     CTS Interrupt Clear Flag
  *            @arg @ref UART_CLEAR_CMF      Character Match Clear Flag
  *            @arg @ref UART_CLEAR_WUF      Wake Up from stop mode Clear Flag
  * @retval None
*/

/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/

/**
  * @brief Handle UART interrupt request.
  * @param huart UART handle.
  * @retval None
  */
void ProbeRS232_RxTxISRHandler( void )
{
  uint32_t isrflags   = READ_REG(PROBE_PORT->ISR);
  uint32_t cr1its     = READ_REG(PROBE_PORT->CR1);
  uint32_t cr3its     = READ_REG(PROBE_PORT->CR3);

  uint32_t errorflags;
  uint32_t errorcode;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == 0U)
  {
    /* UART in mode Receiver ---------------------------------------------------*/
    if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
            || ((cr3its & USART_CR3_RXFTIE) != 0U)))
    {
      UART_RxISR_8BIT();
      return;
    }
  }

  /* If some errors occur */
  if ((errorflags != 0U)
      && ((((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)
           || ((cr1its & (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE)) != 0U))))
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
      LL_USART_ClearFlag_PE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
      LL_USART_ClearFlag_FE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
      LL_USART_ClearFlag_NE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if (((isrflags & USART_ISR_ORE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
            ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
      LL_USART_ClearFlag_ORE(PROBE_PORT);


      ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if (ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver ---------------------------------------------------*/
      if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
          && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
              || ((cr3its & USART_CR3_RXFTIE) != 0U)))
      {
          UART_RxISR_8BIT();
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      errorcode = ErrorCode;
      if ((HAL_IS_BIT_SET(PROBE_PORT->CR3, USART_CR3_DMAR)) ||
          ((errorcode & HAL_UART_ERROR_ORE) != 0U))
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        UART_EndRxTransfer();
        /* Call user error callback */
        /*Call legacy weak error callback*/
        Probe_UART_ErrorCallback();

      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
           
        /*Call legacy weak error callback*/
        Probe_UART_ErrorCallback();
        
        ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;

  } /* End if some error occurs */

//  /* UART wakeup from Stop mode interrupt occurred ---------------------------*/
//  if (((isrflags & USART_ISR_WUF) != 0U) && ((cr3its & USART_CR3_WUFIE) != 0U))
//  {
//    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);
//
//    /* UART Rx state is not reset as a reception process might be ongoing.
//       If UART handle state fields need to be reset to READY, this could be done in Wakeup callback */
//
//    /* Call legacy weak Wakeup Callback */
//    HAL_UARTEx_WakeupCallback(huart);
//    return;
//  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_ISR_TXE_TXFNF) != 0U)
      && (((cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U)
          || ((cr3its & USART_CR3_TXFTIE) != 0U)))
  {
        UART_TxISR_8BIT();
    return;
  }

  /* UART in mode Transmitter (transmission end) -----------------------------*/
  if (((isrflags & USART_ISR_TC) != 0U) && ((cr1its & USART_CR1_TCIE) != 0U))
  {
    UART_EndTransmit_IT();
    return;
  }

}

#if 0

void  ProbeRS232_RxTxISRHandler (void)
{
    USART_TypeDef  *usart;
    CPU_INT08U      rx_data;

//
//#if   (PROBE_RS232_CFG_COMM_SEL == PROBE_RS232_UART_1)
//    usart = USART1;
//#elif (PROBE_RS232_CFG_COMM_SEL == PROBE_RS232_UART_2)
//    usart = USART2;
//#elif (PROBE_RS232_CFG_COMM_SEL == PROBE_RS232_UART_3)
//    usart = USART3;
//#else
//    return;
//#endif
    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
           rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register.        */
           ProbeRS232_RxHandler(rx_data);
           USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the USART1 receive interrupt.                  */
       }

    if (LL_USART_IsActiveFlag_TXE (ProbeRS232_PORT)!= RESET){
        rx_data = LL_USART_ReceiveData8(ProbeRS232_PORT) & 0xFF;              /* Read one byte from the receive data register.        */
        ProbeRS232_TxHandler(rx_data);
        LL_USART_ClearFlag_TC(ProbeRS232_PORT);
        LL_USART_ClearFlag_TXFE(ProbeRS232_PORT);
    }

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register.        */
        ProbeRS232_RxHandler(rx_data);
        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the USART1 receive interrupt.                  */
    }

    if (USART_GetITStatus(usart, USART_IT_TXE) != RESET) {
        ProbeRS232_TxHandler();
        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the USART1 transmit interrupt.                 */
    }

}

#endif



/*
*********************************************************************************************************
*                                      ProbeRS232_RxISRHandler()
*
* Description : Handle Rx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) This function is empty because receive interrupts are handled by ProbeRS232_RxTxISRHandler().
*********************************************************************************************************
*/

void  ProbeRS232_RxISRHandler (void)
{
}


/*
*********************************************************************************************************
*                                        ProbeRS232_RxIntDis()
*
* Description : Disable Rx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The application should call this function to stop communication.
*********************************************************************************************************
*/

void  ProbeRS232_RxIntDis (void)
{
    /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(PROBE_PORT->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
    CLEAR_BIT(PROBE_PORT->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));
}


/*
*********************************************************************************************************
*                                        ProbeRS232_RxIntEn()
*
* Description : Enable Rx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The application should call this function to start communication.
*********************************************************************************************************
*/

void  ProbeRS232_RxIntEn (void)
{
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(PROBE_PORT->CR3, USART_CR3_EIE);
    /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
    SET_BIT(PROBE_PORT->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);

}


/*
*********************************************************************************************************
*                                      ProbeRS232_TxISRHandler()
*
* Description : Handle Tx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : This function is empty because transmit interrupts are handled by ProbeRS232_RxTxISRHandler().
*********************************************************************************************************
*/

void  ProbeRS232_TxISRHandler (void)
{
}


/*
*********************************************************************************************************
*                                          ProbeRS232_Tx1()
*
* Description : Transmit one byte.
*
* Argument(s) : c           The byte to transmit.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_TxHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/
__inline
void  ProbeRS232_Tx1 (CPU_INT08U c)
{

    sendData = c;
    /* Enable the Transmit Data Register Empty interrupt */
    SET_BIT(PROBE_PORT->CR1, USART_CR1_TXEIE_TXFNFIE);

}


/*
*********************************************************************************************************
*                                        ProbeRS232_TxIntDis()
*
* Description : Disable Tx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_TxHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/
__inline
void  ProbeRS232_TxIntDis (void)
{
    /* Disable the UART Transmit Data Register Empty Interrupt */
    CLEAR_BIT(PROBE_PORT->CR1, USART_CR1_TXEIE_TXFNFIE);

    /* Disable the UART Transmit Complete Interrupt */
    CLEAR_BIT(PROBE_PORT->CR1, USART_CR1_TCIE);

}

/*
*********************************************************************************************************
*                                        ProbeRS232_TxIntEn()
*
* Description : Enable Tx interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_TxStart().
*
* Note(s)     : none.
*********************************************************************************************************
*/
// 在这里将数据传输出去
__inline
void  ProbeRS232_TxIntEn (void)
{
    /* Enable the Transmit Data Register Empty interrupt */
//    SET_BIT(PROBE_PORT->CR1, USART_CR1_TXEIE_TXFNFIE);

}

/** @brief  Enable the specified UART interrupt.
* @param  __HANDLE__ specifies the UART Handle.
* @param  __INTERRUPT__ specifies the UART interrupt source to enable.
*          This parameter can be one of the following values:
*            @arg @ref UART_IT_RXFF  RXFIFO Full interrupt
*            @arg @ref UART_IT_TXFE  TXFIFO Empty interrupt
*            @arg @ref UART_IT_RXFT  RXFIFO threshold interrupt
*            @arg @ref UART_IT_TXFT  TXFIFO threshold interrupt
*            @arg @ref UART_IT_WUF   Wakeup from stop mode interrupt
*            @arg @ref UART_IT_CM    Character match interrupt
*            @arg @ref UART_IT_CTS   CTS change interrupt
*            @arg @ref UART_IT_LBD   LIN Break detection interrupt
*            @arg @ref UART_IT_TXE   Transmit Data Register empty interrupt
*            @arg @ref UART_IT_TXFNF TX FIFO not full interrupt
*            @arg @ref UART_IT_TC    Transmission complete interrupt
*            @arg @ref UART_IT_RXNE  Receive Data register not empty interrupt
*            @arg @ref UART_IT_RXFNE RXFIFO not empty interrupt
*            @arg @ref UART_IT_IDLE  Idle line detection interrupt
*            @arg @ref UART_IT_PE    Parity Error interrupt
*            @arg @ref UART_IT_ERR   Error interrupt (frame error, noise error, overrun error)
* @retval None
*/

/** @brief  Clear the specified UART pending flag.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __FLAG__ specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg @ref UART_CLEAR_PEF      Parity Error Clear Flag
  *            @arg @ref UART_CLEAR_FEF      Framing Error Clear Flag
  *            @arg @ref UART_CLEAR_NEF      Noise detected Clear Flag
  *            @arg @ref UART_CLEAR_OREF     Overrun Error Clear Flag
  *            @arg @ref UART_CLEAR_IDLEF    IDLE line detected Clear Flag
  *            @arg @ref UART_CLEAR_TXFECF   TXFIFO empty clear Flag
  *            @arg @ref UART_CLEAR_TCF      Transmission Complete Clear Flag
  *            @arg @ref UART_CLEAR_LBDF     LIN Break Detection Clear Flag
  *            @arg @ref UART_CLEAR_CTSF     CTS Interrupt Clear Flag
  *            @arg @ref UART_CLEAR_CMF      Character Match Clear Flag
  *            @arg @ref UART_CLEAR_WUF      Wake Up from stop mode Clear Flag
  * @retval None
*/

/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/
#if 0
/**
  * @brief Handle UART interrupt request.
  * @param huart UART handle.
  * @retval None
  */
void ProbeRS232_RxTxISRHandler( void )
{
  uint32_t isrflags   = READ_REG(PROBE_PORT->ISR);
  uint32_t cr1its     = READ_REG(PROBE_PORT->CR1);
  uint32_t cr3its     = READ_REG(PROBE_PORT->CR3);

  uint32_t errorflags;
  uint32_t errorcode;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == 0U)
  {
    /* UART in mode Receiver ---------------------------------------------------*/
    if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
            || ((cr3its & USART_CR3_RXFTIE) != 0U)))
    {
      UART_RxISR_8BIT(huart);
      return;
    }
  }

  /* If some errors occur */
  if ((errorflags != 0U)
      && ((((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)
           || ((cr1its & (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE)) != 0U))))
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
      LL_USART_ClearFlag_PE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
      LL_USART_ClearFlag_FE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
      LL_USART_ClearFlag_NE(PROBE_PORT);

      ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if (((isrflags & USART_ISR_ORE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
            ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)))
    {
//      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
      LL_USART_ClearFlag_ORE(PROBE_PORT);


      ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if (ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver ---------------------------------------------------*/
      if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
          && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
              || ((cr3its & USART_CR3_RXFTIE) != 0U)))
      {
          UART_RxISR_8BIT();
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      errorcode = ErrorCode;
      if ((HAL_IS_BIT_SET(PROBE_PORT->CR3, USART_CR3_DMAR)) ||
          ((errorcode & HAL_UART_ERROR_ORE) != 0U))
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        UART_EndRxTransfer();
        /* Call user error callback */
        /*Call legacy weak error callback*/
        Probe_UART_ErrorCallback();

      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
           
        /*Call legacy weak error callback*/
        Probe_UART_ErrorCallback();
        
        ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;

  } /* End if some error occurs */

//  /* UART wakeup from Stop mode interrupt occurred ---------------------------*/
//  if (((isrflags & USART_ISR_WUF) != 0U) && ((cr3its & USART_CR3_WUFIE) != 0U))
//  {
//    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);
//
//    /* UART Rx state is not reset as a reception process might be ongoing.
//       If UART handle state fields need to be reset to READY, this could be done in Wakeup callback */
//
//    /* Call legacy weak Wakeup Callback */
//    HAL_UARTEx_WakeupCallback(huart);
//    return;
//  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_ISR_TXE_TXFNF) != 0U)
      && (((cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U)
          || ((cr3its & USART_CR3_TXFTIE) != 0U)))
  {
        UART_TxISR_8BIT();
    return;
  }

  /* UART in mode Transmitter (transmission end) -----------------------------*/
  if (((isrflags & USART_ISR_TC) != 0U) && ((cr1its & USART_CR1_TCIE) != 0U))
  {
    UART_EndTransmit_IT(huart);
    return;
  }

}

#endif


/**
  * @brief RX interrrupt handler for 7 or 8 bits data word length .
  * @param huart UART handle.
  * @retval None
  */
  
__inline
static void UART_RxISR_8BIT(void)
{
    uint8_t rx_data;
    rx_data = LL_USART_ReceiveData8(PROBE_PORT);             /* Read one byte from the receive data register.        */
    ProbeRS232_RxHandler(rx_data);
//    ProbeRS232_Tx1(rx_data);                              // 测试收到的数据
}



/**
  * @brief Receive an amount of data in interrupt mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         address of user data buffer for storing data to be received, should be aligned on a half word frontier (16 bits)
  *         (as received data will be handled using u16 pointer cast). Depending on compilation chain,
  *         use of specific alignment compilation directives or pragmas might be required to ensure proper alignment for pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */

__inline
static void  UART_Receive_IT_En(void)
{
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(PROBE_PORT->CR3, USART_CR3_EIE);
    /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
    SET_BIT(PROBE_PORT->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
}




/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
__inline
static void UART_EndRxTransfer(void)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(PROBE_PORT->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
  CLEAR_BIT(PROBE_PORT->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

}

/**
  * @brief  Wrap up transmission in non-blocking mode.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
  
__inline
static void UART_EndTransmit_IT(void)
{
  /* Disable the UART Transmit Complete Interrupt */
  CLEAR_BIT(PROBE_PORT->CR1, USART_CR1_TCIE);
  /*Call legacy weak Tx complete callback*/
//  HAL_UART_TxCpltCallback(huart);
    ProbeRS232_TxHandler();
  
}


/**
  * @brief TX interrrupt handler for 7 or 8 bits data word length .
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Transmit_IT().
  * @param huart UART handle.
  * @retval None
  */
 __inline
static void UART_TxISR_8BIT(void)
{

    /* Disable the UART Transmit Data Register Empty Interrupt */
    CLEAR_BIT(PROBE_PORT->CR1, USART_CR1_TXEIE_TXFNFIE);

    /* Enable the UART Transmit Complete Interrupt */
    SET_BIT(PROBE_PORT->CR1, USART_CR1_TCIE);
//    Write in Transmitter Data Register (Transmit Data value, 8 bits)
    LL_USART_TransmitData8(PROBE_PORT, sendData);

}



static 
void Probe_UART_ErrorCallback(void)
{
    ProbeRS232_Tx1(ErrorCode+32);
}

#endif
