/*
*******************************************************************************
**
**  This device driver was created by Applilet2 for 78K0S/Kx1+
**  8-Bit Single-Chip Microcontrollers
**
**  Copyright(C) NEC Electronics Corporation 2002 - 2012
**  All rights reserved by NEC Electronics Corporation.
**
**  This program should be used on your own responsibility.
**  NEC Electronics Corporation assumes no responsibility for any losses
**  incurred by customers or third parties arising from the use of this file.
**
**  Filename :	ad.h
**  Abstract :	This file implements device driver for AD module.
**  APIlib :	Applilet2 for 78K0S/Kx1+ V2.41 [9 Sep. 2008]
**
**  Device :	uPD78F9212
**
**  Compiler :	IAR Systems icc78K
**
**  Creation date:	2012/12/15
**  
*******************************************************************************
*/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
/* Start user code for definition. Do not edit comment generated here */


//extern unsigned char show[8];

void display_change(uint16_t *value);
void display_LVI(void);


/* End user code for definition. Do not edit comment generated here */
#endif

