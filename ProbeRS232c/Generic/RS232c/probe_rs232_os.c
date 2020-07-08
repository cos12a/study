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
*                                        Micrium uC/OS-III PORT
*
* Filename      : probe_rs232_os.c
* Version       : V2.30
* Programmer(s) : BAN
*                 FT
*********************************************************************************************************
* Note(s)       : (1) This file is the uC/OS-III layer for the uC/Probe RS-232 Communication Module.
*
*                 (2) Assumes uC/OS-II V3.00 is included in the project build.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <probe_com.h>
#include  <probe_rs232.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


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

#define  PROBE_RS232_TASK_STK_LIMIT_PCT_FULL             90u
#define  PROBE_RS232_TASK_STK_LIMIT             ((PROBE_RS232_CFG_TASK_STK_SIZE * (100u - PROBE_RS232_TASK_STK_LIMIT_PCT_FULL) / 100u))

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
//static  CPU_STK  ProbeRS232_OS_TaskStk[PROBE_RS232_CFG_TASK_STK_SIZE];  /* Probe RS-232 task stack.                         */
//static  OS_TCB   ProbeRS232_OS_TaskTCB;
//
//static  OS_SEM   ProbeRS232_OS_Sem;                                     /* Packet receive signal.                           */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"



/* Definitions for RxHandTast */
osThreadId_t RxHandTastHandle;
const osThreadAttr_t RxHandTast_attributes = {
  .name = "RxHandTast",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 1024 * 4
};



/* Definitions for ProbeRS232_OS_Sem */
osSemaphoreId_t ProbeRS232_OS_Sem;
const osSemaphoreAttr_t ProbeRS232_OS_Sem_attributes = {
  .name = "ProbeRS232_OS_Sem"
};


#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
static  void  ProbeRS232_OS_Task(void *p_arg);                          /* Probe RS-232 task.                               */
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if     (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
//#if     (OS_CFG_SEM_EN < 1)
//#error  "OS_CFG_SEM_EN          illegally #define'd in 'os_cfg.h'   "
//#error  "                       [MUST be  > 0]                      "
//#endif
#endif


/*
*********************************************************************************************************
*                                         ProbeRS232_OS_Init()
*
* Description : Create RTOS objects for RS-232 communication.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if OS initialization successful.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : ProbeRS232_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
CPU_BOOLEAN  ProbeRS232_OS_Init (void)
{
//    OS_ERR  os_err;

    /* creation of ProbeRS232_OS_Sem */
    ProbeRS232_OS_Sem = osSemaphoreNew(1, 0, &ProbeRS232_OS_Sem_attributes);

    
    /* creation of RxHandTast */
    RxHandTastHandle = osThreadNew(ProbeRS232_OS_Task, NULL, &RxHandTast_attributes);

#if 0

    OSSemCreate((OS_SEM     *)&ProbeRS232_OS_Sem,
                (CPU_CHAR   *)"Probe RS-232",
                (OS_SEM_CTR  ) 0,
                (OS_ERR     *)&os_err);

    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }


    OSTaskCreate((OS_TCB     *)&ProbeRS232_OS_TaskTCB,
                 (CPU_CHAR   *)"Probe RS232",
                 (OS_TASK_PTR ) ProbeRS232_OS_Task,
                 (void       *) 0,
                 (OS_PRIO     ) PROBE_RS232_CFG_TASK_PRIO,
                 (CPU_STK    *)&ProbeRS232_OS_TaskStk[0],
                 (CPU_STK_SIZE) PROBE_RS232_TASK_STK_LIMIT,
                 (CPU_STK_SIZE) PROBE_RS232_CFG_TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&os_err);

    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }
#endif
    return (DEF_OK);
}
#endif


/*
*********************************************************************************************************
*                                       ProbeRS232_OS_Pend()
*
* Description : Wait for a packet to be received.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
__inline
void  ProbeRS232_OS_Pend (void)
{
//    OS_ERR  os_err;
    
    
    /// Acquire a Semaphore token or timeout if no tokens are available.
    /// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
    /// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
    /// \return status code that indicates the execution status of the function.
    osStatus_t os_err = osSemaphoreAcquire (ProbeRS232_OS_Sem, portMAX_DELAY);
    (void)&os_err;

#if 0

    OSSemPend((OS_SEM  *)&ProbeRS232_OS_Sem,                    /* Wait for a packet to be received ...                 */
              (OS_TICK  ) 0u,                                   /* ... without timeout.                                 */
              (OS_OPT   ) OS_OPT_PEND_BLOCKING,
              (CPU_TS  *) 0u,
              (OS_ERR  *)&os_err);

   (void)&os_err;
#endif
}
#endif


/*
*********************************************************************************************************
*                                       ProbeRS232_OS_Post()
*
* Description : Notify a pending task that a packet has been receieved.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeRS232_RxPkt().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
__inline
void  ProbeRS232_OS_Post (void)
{
//    OS_ERR  os_err;
    /// Release a Semaphore token up to the initial maximum count.
    /// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
    /// \return status code that indicates the execution status of the function.
    osStatus_t os_err = osSemaphoreRelease (ProbeRS232_OS_Sem);
    (void)&os_err;


#if 0

    OSSemPost((OS_SEM *)&ProbeRS232_OS_Sem,                         /* A packet has been received.                          */
              (OS_OPT  )OS_OPT_POST_FIFO,
              (OS_ERR *)&os_err);

   (void)&os_err;

#endif
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         ProbeRS232_OS_Task()
*
* Description : Task which waits for packets to be received, formalates responses, and begins transmission.
*
* Argument(s) : p_arg       Argument passed to ProbeRS232_OS_Task() by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_RS232_CFG_PARSE_TASK_EN == DEF_ENABLED)
static  void  ProbeRS232_OS_Task (void *argument)
{
   (void) argument;

    ProbeRS232_Task();
}
#endif

 /* USER CODE END Header_start_task_run */
 void start_task_run(void *argument)
{
  /* USER CODE BEGIN start_task_run */       
  ProbeRS232_Init (115200);
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END start_task_run */
}

   

/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/

#endif
