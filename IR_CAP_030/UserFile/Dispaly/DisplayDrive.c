/*
*******************************************************************************
**
** This device driver was created by Applilet2 for IAR Systems
** 8-Bit Single-Chip Microcontrollers
**
** Copyright(C) NEC Electronics Corporation 2000 - 2011
** All rights reserved by NEC Electronics Corporation.
**
** This program should be used on your own responsibility.
** NEC Electronics Corporation assumes no responsibility for any losses
** incurred by customers or third parties arising from the use of this file.
**
** Filename :   ISPSend.c
** Abstract :   This file implements device driver for 595X2send module.
** APIlib :  [2012/12/13]
**
** Device : AT89C2051
**
** Compiler :   IAR Systems
**
** Creation date:   YuZhouLuo ,2012/2/13
**
** Email:    yzluo@wahshing.com
**
*******************************************************************************
*/

#include "main.h"
//#include "macrodriver.h"
#include "DisplayDrive.h"
#include "Display.h"

/*
*******************************************************************************
** Include files
*******************************************************************************
*/
//#include "user_define.h"



 /*
unsigned char const Table[24] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
0x40,0x79,0xC6,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x86,0xAF,0x9C,0xFF};//0xBF,0xFF};
 */
unsigned char const Table[24] ={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
0x88,0x83,0xC6,0xA1,0x86,0x8E, 
0x8C,/*16-P*/
0x88,/*17-A*/
0x92,/*18-S*/
0x8E,/*19-F*/
0xC7,/*20-L*/  
0xBF,/*21--*/ 
0xBF,/*22-.*/ 
0xFF  /*22-OFF*/ 
} ;
#define software  1
#define Hand       3

#define DIS_Mode  Hand

#define SEG   0
#define Buf   0

unsigned int const Seg[8]={0xfe00,0xfd00,0xfb00,0xf700,0xef00,0xdf00,0xbf00,0x7f00};


//#define DisBuffer		show
unsigned char DisBuffer[8] = {0,0,0,0,0,0,0,0};


/*
*******************************************************************************
** Global define
*******************************************************************************
*/
/* Start user code for global definition. Do not edit comment generated here */
/* End user code for global definition. Do not edit comment generated here */

//#define send_storage_output()  STcp_L;NOP();NOP();STcp_H;NOP();NOP();STcp_L

#ifndef send_storage_output
void send_storage_output(void)
{
    STcp_L;
    NOP();
    NOP();
    STcp_H;
    NOP();
    NOP();
    STcp_L;
}

#endif


void send_shift(uint16_t data)
{
  uint8_t i;
  for(i=0;i<16;i++)
  {

    if(data&0x8000)DS_H;
    else DS_L;
    data<<=1;
    SHcp_L;
    NOP();
    NOP();
    SHcp_H;
  }
}


void DisplayValue(void)
{
  static unsigned char i;
  unsigned int temp_Data;
  send_shift(0xFFFF);
  NOP();
  NOP();
  send_storage_output();
  temp_Data = ((~(0x0100<<i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
  send_shift(temp_Data);
  NOP();
  NOP();
  send_storage_output();
  i++;
  i &= 0x07;
}


void DisplayOFF(void)
{
  unsigned char i;
  for(i=0;i<8;i++)
  {
    DS_H;
    SHcp_L;
    NOP();
    NOP();
    SHcp_H;
  }
  send_storage_output();
}