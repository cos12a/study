/*
*********************************************************************************************************
*                                             uC/Probe
*                                        Live Watch Window
*
*                         (c) Copyright 2004-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/Probe is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                            UC/PROBE TERMINAL WINDOW OPERATING SYSTEM LAYER
*                                         Micrium uC/OS-III
*
* File          : probe_term_os.c
* Version       : V1.00.00
* Programmer(s) : JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    MICRIUM_SOURCE
//#include  <os.h>
#include  <probe_term.h>
#include  <probe_term_cfg.h>


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"



/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_CMD_EN > 0
#ifndef  PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE
#error  "PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE      not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                            [MUST be  > 0]                    "
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_RX_TASK_PRIO
#error  "PROBE_TERM_OS_CFG_CMD_RX_TASK_PRIO          not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                            [MUST be  > 0]                    "
#endif
#ifndef  PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE
#error  "PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE      not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                            [MUST be  > 0]                    "
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO
#error  "PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO          not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                            [MUST be  > 0]                    "
#endif
#endif

#if PROBE_TERM_CFG_TRC_EN > 0
#ifndef  PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE
#error  "PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE      not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                   [MUST be  > 0]                    "
#endif

#ifndef  PROBE_TERM_OS_CFG_TRC_TASK_PRIO
#error  "PROBE_TERM_OS_CFG_TRC_TASK_PRIO          not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                   [MUST be  > 0]                    "
#endif
#endif

#if (PROBE_TERM_CFG_CMD_EN > 0 || PROBE_TERM_CFG_TRC_EN > 0)
#ifndef  PROBE_TERM_OS_CFG_TASK_DLY_MSEC
#error  "PROBE_TERM_OS_CFG_TASK_DLY_MSEC          not #define'd in 'app_cfg.h or probe_term_cfg.h'"
#error  "                                   [MUST be  > 0 and < 1000]         "
#endif
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


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

#if PROBE_TERM_CFG_CMD_EN > 0
//static  OS_TCB     ProbeTermOS_CmdRxTaskTCB;
//static  CPU_STK    ProbeTermOS_CmdRxTaskStk[PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE];

/* The handle of the task that receives the notifications. */
static TaskHandle_t ProbeTermOS_CmdRxTaskTCB = NULL;



//static  OS_TCB     ProbeTermOS_CmdTxTaskTCB;
//static  CPU_STK    ProbeTermOS_CmdTxTaskStk[PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE];

/* The handle of the task that receives the notifications. */
static TaskHandle_t ProbeTermOS_CmdTxTaskTCB = NULL;


#endif

#if PROBE_TERM_CFG_TRC_EN > 0
//static  OS_TCB     ProbeTermOS_TrcTaskTCB;
//static  CPU_STK    ProbeTermOS_TrcTaskStk[PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE];

/* The handle of the task that receives the notifications. */
static TaskHandle_t ProbeTermOS_TrcTaskTCB = NULL;



#endif


/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_CMD_EN > 0
static  void  ProbeTermOS_CmdRxTask (void  *p_arg);
static  void  ProbeTermOS_CmdTxTask (void  *p_arg);
#endif

#if PROBE_TERM_CFG_TRC_EN > 0
static  void  ProbeTermOS_TrcTask (void  *p_arg);
#endif


/* Used to count the notifications sent to the task from a software timer and
the number of notifications received by the task from the software timer.  The
two should stay synchronised. */

static uint32_t CmdMsgWaitReceived = 0UL, TrcMsgWaitReceived = 0UL;



/*
*********************************************************************************************************
*                                         ProbeTerm_OS_Init()
*
* Description : Initialize the OS layer for the terminal window.
*
* Argument(s) : p_err       Pointer to variable that will receive the return error code from this function :
*
*                               PROBE_TERM_ERR_NONE           OS initialization successful.
*                               PROBE_TERM_ERR_OS_INIT_FAIL   OS objects NOT successfully initialized.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeTermInit().
*
* Note(s)     : none.
*********************************************************************************************************
*/
#if (PROBE_TERM_CFG_CMD_EN > 0 || PROBE_TERM_CFG_TRC_EN > 0)
void  ProbeTermOS_Init (PROBE_TERM_ERR  *p_err)
{
//    OS_ERR  err_os;

	BaseType_t err_os;

#if PROBE_TERM_CFG_CMD_EN > 0

	err_os = xTaskCreate(	 ProbeTermOS_CmdRxTask, /* Function that implements the task. */
            				 "uC/Probe-Term Cmd-Line Rx Task", /* Text name for the task - for debugging only - not used by the kernel. */
            				 PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE, /* Task's stack size in words, not bytes!. */
            				 NULL, /* Task parameter, not used in this case. */
            				 (osPriority_t) osPriorityHigh1, /* Task priority, 0 is the lowest. */
            				 &ProbeTermOS_CmdRxTaskTCB ); /* Used to pass a handle to the task out is needed, otherwise set to NULL. */


    if (err_os !=  pdPASS) {
       *p_err = PROBE_TERM_ERR_OS_INIT_FAIL;
        return;
    }

	err_os = xTaskCreate(	 ProbeTermOS_CmdTxTask, /* Function that implements the task. */
            				 "uC/Probe-Term Cmd-Line Tx Task", /* Text name for the task - for debugging only - not used by the kernel. */
            				 PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE, /* Task's stack size in words, not bytes!. */
            				 NULL, /* Task parameter, not used in this case. */
            				 (osPriority_t) osPriorityLow6, /* Task priority, 0 is the lowest. */
            				 &ProbeTermOS_CmdTxTaskTCB ); /* Used to pass a handle to the task out is needed, otherwise set to NULL. */

//
//    OSTaskCreate(         &ProbeTermOS_CmdTxTaskTCB,
//                          "uC/Probe-Term Cmd-Line Tx Task",
//                           ProbeTermOS_CmdTxTask,
//                  (void *) 0,
//                           PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO,
//                          &ProbeTermOS_CmdTxTaskStk[0],
//                           PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE / 10u,
//                           PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE,
//                           0u,
//                           0u,
//                  (void *) 0,
//                           OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
//                          &err_os);

    if (err_os !=  pdPASS) {
       *p_err = PROBE_TERM_ERR_OS_INIT_FAIL;
        return;
    }
#endif

#if PROBE_TERM_CFG_TRC_EN > 0
	err_os = xTaskCreate(	 ProbeTermOS_TrcTask, /* Function that implements the task. */
            				 "uC/Probe-Term Trace Task", /* Text name for the task - for debugging only - not used by the kernel. */
            				 PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE, /* Task's stack size in words, not bytes!. */
            				 NULL, /* Task parameter, not used in this case. */
            				 (osPriority_t) osPriorityHigh3, /* Task priority, 0 is the lowest. */
            				 &ProbeTermOS_TrcTaskTCB ); /* Used to pass a handle to the task out is needed, otherwise set to NULL. */

//    OSTaskCreate(         &ProbeTermOS_TrcTaskTCB,
//                          "uC/Probe-Term Trace Task",
//                           ProbeTermOS_TrcTask,
//                  (void *) 0,
//                           PROBE_TERM_OS_CFG_TRC_TASK_PRIO,
//                          &ProbeTermOS_TrcTaskStk[0],
//                           PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE / 10u,
//                           PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE,
//                           0u,
//                           0u,
//                  (void *) 0,
//                           OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
//                          &err_os);

    if (err_os !=  pdPASS) {
       *p_err = PROBE_TERM_ERR_OS_INIT_FAIL;
        return;
    }
#endif

   *p_err = PROBE_TERM_ERR_NONE;
}
#endif

/*
*********************************************************************************************************
*                                         ProbeTermOS_CmdRxTask()
*
* Description : OS-dependent shell task to process strings for the command line interface responses.
*
* Argument(s) : p_arg       Pointer to task initialization argument (required by uC/OS-III).
*
* Return(s)   : none.
*
* Created by  : PROBE_TERM_OS_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_CMD_EN > 0
static  void  ProbeTermOS_CmdRxTask (void *p_arg)
{
//    OS_ERR  err;
	TickType_t xLastWakeTime;
	

//	const TickType_t xFrequency = PROBE_TERM_OS_CFG_TASK_DLY_MSEC; 
	const TickType_t xTicksToWait = pdMS_TO_TICKS( PROBE_TERM_OS_CFG_TASK_DLY_MSEC );
    p_arg = p_arg;


	/* 获取当前的系统时间 */ 
	xLastWakeTime = xTaskGetTickCount(); 



    while (DEF_ON) {
//        OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
//                      OS_OPT_TIME_HMSM_STRICT,
//                     &err);

		vTaskDelayUntil(&xLastWakeTime, xTicksToWait); 

        ProbeTermCmdRxTaskHandler();
    }
}
#endif


/*
*********************************************************************************************************
*                                         ProbeTermOS_CmdTxTask()
*
* Description : OS-dependent shell task to process strings for the command line interface responses.
*
* Argument(s) : p_arg       Pointer to task initialization argument (required by uC/OS-III).
*
* Return(s)   : none.
*
* Created by  : PROBE_TERM_OS_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_CMD_EN > 0
static  void  ProbeTermOS_CmdTxTask (void *p_arg)
{
//    OS_ERR  err;
    
	TickType_t xLastWakeTime;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( PROBE_TERM_OS_CFG_TASK_DLY_MSEC );

    p_arg = p_arg;
    
	/* 获取当前的系统时间 */ 
	xLastWakeTime = xTaskGetTickCount(); 

    while (DEF_ON) {
//        OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
//                      OS_OPT_TIME_HMSM_STRICT,
//                     &err);

		vTaskDelayUntil(&xLastWakeTime, xTicksToWait); 

        ProbeTermCmdTxTaskHandler();
        vTaskDelay(500);
    }
}
#endif


/*
*********************************************************************************************************
*                                         ProbeTermOS_TrcTask()
*
* Description : OS-dependent shell task to process strings for tracing purposes.
*
* Argument(s) : p_arg       Pointer to task initialization argument (required by uC/OS-III).
*
* Return(s)   : none.
*
* Created by  : PROBE_TERM_OS_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_TRC_EN > 0
static  void  ProbeTermOS_TrcTask (void *p_arg)
{
//    OS_ERR  err;
    
	TickType_t xLastWakeTime;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( PROBE_TERM_OS_CFG_TASK_DLY_MSEC );

    p_arg = p_arg;
    
	/* 获取当前的系统时间 */ 
	xLastWakeTime = xTaskGetTickCount(); 

    while (DEF_ON) {
//        OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
//                      OS_OPT_TIME_HMSM_STRICT,
//                     &err);

		vTaskDelayUntil(&xLastWakeTime, xTicksToWait); 
        ProbeTermTrcTaskHandler();
    }
}
#endif


/*
*********************************************************************************************************
*                                       ProbeTermOS_Q_CmdMsgRdy()
*
* Description : Signals that a command line response message is ready for processing.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeTermPrintCmd().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_CMD_EN > 0
void  ProbeTermOS_Q_CmdMsgRdy (void)
{
//    OS_ERR  err;

	xTaskNotifyGive (ProbeTermOS_CmdTxTaskTCB);

//    (void)OSTaskSemPost(&ProbeTermOS_CmdTxTaskTCB,
//                         OS_OPT_POST_NONE,
//                        &err);
}
#endif


/*
*********************************************************************************************************
*                                       ProbeTermOS_Q_TrcMsgRdy()
*
* Description : Signals that a trace message is ready for processing.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeTermPrintTrc().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if PROBE_TERM_CFG_TRC_EN > 0
void  ProbeTermOS_Q_TrcMsgRdy (void)
{
//    OS_ERR  err;

	xTaskNotifyGive (ProbeTermOS_TrcTaskTCB);

//    (void)OSTaskSemPost(&ProbeTermOS_TrcTaskTCB,
//                         OS_OPT_POST_NONE,
//                        &err);
}
#endif


/*
*********************************************************************************************************
*                                     ProbeTermOS_Q_CmdMsgWait()
*
* Description : Waits until a command line response message is available.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeTermCmdTaskHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeTermOS_Q_CmdMsgWait (void)
{
//    OS_ERR  err;
	CmdMsgWaitReceived += ulTaskNotifyTake( pdTRUE, portMAX_DELAY );


//    (void)OSTaskSemPend(           0,
//                                   OS_OPT_PEND_BLOCKING,
//                        (CPU_TS *) 0,
//                                  &err);
}


/*
*********************************************************************************************************
*                                     ProbeTermOS_Q_TrcMsgWait()
*
* Description : Waits until a trace message is available.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeTermTrcTaskHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeTermOS_Q_TrcMsgWait (void)
{
//    OS_ERR  err;
	TrcMsgWaitReceived += ulTaskNotifyTake( pdTRUE, portMAX_DELAY );


//    (void)OSTaskSemPend(           0,
//                                   OS_OPT_PEND_BLOCKING,
//                        (CPU_TS *) 0,
//                                  &err);
}
