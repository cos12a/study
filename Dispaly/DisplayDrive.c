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

const unsigned char  Table[30] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
    0x83,0xfe,0xa1,0x86,0x8e,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x8c,0xaf,0x92,0xce,0xFF};

0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10

*/

unsigned char const Table[24] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
0x40,0x79,0xC6,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x86,0xAF,0x9C,0xFF};//0xBF,0xFF};

//const unsigned char Bit_Tab[6] = {0x00,22,0x00,0x00,0x00,0x00};  //NPN

//12_456


#define software  1
#define Hand       3

#define DIS_Mode  Hand

#define SEG   0
#define Buf   0

unsigned int const Seg[8]={0xfe00,0xfd00,0xfb00,0xf700,0xef00,0xdf00,0xbf00,0x7f00};

//#if   (Buf==1)
//unsigned int const Seg[8]={0xfeff,0xfdff,0xfbff,0xf7ff,0xefff,0xdfff,0xbfff,0x7fff};
//#else
//unsigned int const Seg[8]={0xfe00,0xfd00,0xfb00,0xf700,0xef00,0xdf00,0xbf00,0x7f00};
//#endif



//#define DisBuffer		show
unsigned char DisBuffer[8] = {0,0,0,0,0,0,22,12};


/*
*******************************************************************************
** Global define
*******************************************************************************
*/
/* Start user code for global definition. Do not edit comment generated here */
/* End user code for global definition. Do not edit comment generated here */

#define send_storage_output()  STcp=0;NOP();NOP();STcp=1;NOP();NOP();STcp=0

#ifndef send_storage_output
void send_storage_output(void)
{
    STcp=0;
    NOP();
    NOP();
    STcp=1;
    NOP();
    NOP();
    STcp=0;
}

#endif


void send_shift(uint data)
{
  uchar i;
  for(i=0;i<16;i++)
  {

    if(data&0x8000)DS=1;
    else DS=0;
    data<<=1;
    SHcp=0;
    NOP();
    NOP();
    SHcp=1;
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


//  temp_Data = ((~(0x100<<i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
#if 0
	temp_Data = ((~(0x8000>>i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
  temp_Data = ((~(0x100<<i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
#else
  temp_Data = ((~(0x0100<<i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
//	if(i==3)temp_Data &=0xff7F;
#endif
//  temp_Data = Seg[i]|(unsigned int)Table[DisBuffer[i]];
   // (0x8000>>i)|(unsigned int)Table[DisBuffer[i]];
#if (DIS_Mode == Hand)
#elif (DIS_Mode == software)
 // temp_Data = (0x100<<i)|(unsigned int)Table[DisBuffer[i]];
#endif

//  temp_Data = ((~(0x100<<i))&0xff00)|(unsigned int)Table[DisBuffer[i]];
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
    DS=1;
    SHcp=0;
    NOP();
    NOP();
    SHcp=1;
  }
  send_storage_output();
}