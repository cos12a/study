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
*                                              No Kernel
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

#include  <probe_term.h>
#include  <probe_term_cfg.h>


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/


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
   *p_err = PROBE_TERM_ERR_NONE;
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

}