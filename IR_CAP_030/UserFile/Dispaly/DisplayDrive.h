/*
*******************************************************************************
**
**  This device driver was created by Applilet2 for IAR Systems
**  8-Bit Single-Chip Microcontrollers
**
**  Copyright(C) AT89S51 Electronics Corporation 2000 - 2011
**  All rights reserved by AT89S51 Electronics Corporation.
**
**  This program should be used on your own responsibility.
**  NEC Electronics Corporation assumes no responsibility for any losses
**  incurred by customers or third parties arising from the use of this file.
**
**  Filename :  Display595.h
**  Abstract :  This file implements device driver for I-- module.
**  APIlib :    Display  2012/12/13
**
**  Device :    AT89S51
**
**  Compiler :  IAR Systems
**
**  Creation date:  YuZhouLuo ,2012/12/13
**
**  Email:  yzluo@wahshing.com
**
*******************************************************************************
*/
#ifndef __DISPLAY_DRIVE_H__
#define __DISPLAY_DRIVE_H__
/*
*******************************************************************************
**  Register bit define
*******************************************************************************
*/

#include "main.h"
#include "gpio.h"
/*******************************************

74HC595 IO input set
SP_data :Serial data input
SR_CLK	:Shift register clock input
ST_CLK	:Storage register clock input

********************************************/


#define STcp_L    HAL_GPIO_WritePin(STcp_GPIO_Port,STcp_Pin,GPIO_PIN_RESET)      
#define STcp_H    HAL_GPIO_WritePin(STcp_GPIO_Port,STcp_Pin,GPIO_PIN_SET) 
#define SHcp_L    HAL_GPIO_WritePin(SHcp_GPIO_Port,SHcp_Pin,GPIO_PIN_RESET) 
#define SHcp_H      HAL_GPIO_WritePin(SHcp_GPIO_Port,SHcp_Pin,GPIO_PIN_SET) 
#define DS_L      HAL_GPIO_WritePin(DS_GPIO_Port,DS_Pin,GPIO_PIN_RESET) 
#define DS_H     HAL_GPIO_WritePin(DS_GPIO_Port,DS_Pin,GPIO_PIN_SET) 


//display buffer
#define Dis1		DisBuffer[0]
#define Dis2		DisBuffer[1]
#define Dis3		DisBuffer[2]
#define Dis4		DisBuffer[3]
#define Dis5		DisBuffer[4]
#define Dis6		DisBuffer[5]
#define Dis7		DisBuffer[6]
#define Dis8		DisBuffer[7]

#define NOP()       __asm("nop")

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/

extern unsigned char DisBuffer[16];

void DisplayValue(void);
void DisplayOFF(void);

#endif




