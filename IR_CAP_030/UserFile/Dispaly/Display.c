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
**  Filename :	ad_user.c
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

/*
*******************************************************************************
**  Include files
*******************************************************************************
*/

/* Start user code for include definition. Do not edit comment generated here */
#include "DisplayDrive.h"
#include "Display.h"
/* End user code for include definition. Do not edit comment generated here */
//#include "includes.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
/* Start user code for global definition. Do not edit comment generated here */
/* End user code for global definition. Do not edit comment generated here */


/* Start adding user code. Do not edit comment generated here */


/*
**-----------------------------------------------------------------------------
**
**	Abstract:
**		This function read the A/D converter value.
**
**	Parameters:
**		None
**
**	Returns:
**		None return ADValue>>2;
**
**-----------------------------------------------------------------------------
*/
void display_change(uint16_t *value)
{	
	Dis4 = 	*value/10000;
	Dis5 =  *value/1000%10;
	Dis6 =  *value/100%10;
	Dis7 =  *value/10%10;
	Dis8 =  *value%10;
}


/*
**-----------------------------------------------------------------------------
**
**	Abstract:
**		This function read the A/D converter value.
**
**	Parameters:
**		None
**
**	Returns:
**		None return ADValue>>2;
**
**-----------------------------------------------------------------------------
*/
void display_LVI(void)
{	
	
	Dis1 = 0;
	Dis2 = 0;
	Dis3 = 0;
	Dis4 = 0;		
	Dis5 = 0;
	Dis6 = 0;
	Dis7 = 0;
	Dis8 = 0;
}



/* End user code adding. Do not edit comment generated here */


