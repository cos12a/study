/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM14 global interrupt.
  */
void TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM14_IRQn 0 */
  __HAL_TIM_CLEAR_IT(&htim14, TIM_IT_UPDATE);
  HAL_TIM_PeriodElapsedCallback(&htim14);
  /* USER CODE END TIM14_IRQn 0 */
  //HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM14_IRQn 1 */

  /* USER CODE END TIM14_IRQn 1 */
}

/* USER CODE BEGIN 1 */

#define DEBUG_START

extern uint32_t testData;
extern uint32_t testData2;
extern uint8_t recvData[7];
#define IR_CODE_NUM 7u

static uint8_t IrCode[IR_CODE_NUM] = {0};

#define CODE1_MIN_VALUE 500u
#define CODE1_MAX_VALUE 700u
#define CODE0_MIN_VALUE 200u
#define CODE0_MAX_VALUE 350u
#define ONLY_RISING 0u
#define CODDE_NUM 56u

static uint8_t codeNum = 0; //接收计数
uint8_t revok_flag = 0;     //接收完成标志

#if defined(DEBUG_START)
//static uint8_t headok_flag = 0; //�?始标�?
#else
static uint8_t headok_flag = 1; //�?始标�?
#endif

void HAL_TIM_IC_CaptureCallback_CH2(TIM_HandleTypeDef *htim)
{
  uint32_t revcnt;

#if (ONLY_RISING == 1)

  uint32_t temp1, temp2;
  temp1 = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
  temp2 = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
  __HAL_TIM_SET_COUNTER(htim, 0);
  //    testData = temp1;
  if (temp1 > temp2)
  {
    revcnt = temp1 - temp2;
  }
#else
  revcnt = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
#endif

//  if (headok_flag) //是否接收过头�?
  {
    IrCode[6] <<= 1;
    if (IrCode[5] & 0x80)
    {
      IrCode[6] |= 0x01;
    }
    IrCode[5] <<= 1;
    if (IrCode[4] & 0x80)
    {
      IrCode[5] |= 0x01;
    }
    IrCode[4] <<= 1;
    if (IrCode[3] & 0x80)
    {
      IrCode[4] |= 0x01;
    }
    IrCode[3] <<= 1; //IrCode3左移
    if (IrCode[2] & 0x80)
    {
      IrCode[3] |= 0x01;
    }
    IrCode[2] <<= 1; //IrCode2左移
    if (IrCode[1] & 0x80)
    {
      IrCode[2] |= 0x01;
    }
    IrCode[1] <<= 1; //IrCode1左移
    if (IrCode[0] & 0x80)
    {
      IrCode[1] |= 0x01;
    }
    IrCode[0] <<= 1;                                           //IrCode0左移
    if (revcnt >= CODE1_MIN_VALUE && revcnt < CODE1_MAX_VALUE) // 1.5ms---3.125ms
    {
      IrCode[0] |= 0x01; //�?1
    }
    else if (revcnt > CODE0_MIN_VALUE && revcnt <= CODE0_MAX_VALUE) //0.25ms---1.5ms
    {
      IrCode[0] &= 0xfe; //�?0
    }
    else
    {
      memset(IrCode, 0, sizeof(IrCode));
      //IrCode0=IrCode1=IrCode2=IrCode3=0;
      //headok_flag=0;        //清头码标�?
      codeNum = 0; //清计数�??
    }
    //           revcnt=0;                 //清码值计�?
    codeNum++;
    if (codeNum >= CODDE_NUM)
    { //接收�?32�?
      codeNum = 0;
      //headok_flag=0;        //清头码标�?
      revok_flag = 1; //置接收完成标�?
      memcpy(recvData, IrCode, sizeof(IrCode));
    }
  }
//  else //判断头码
//  {
//    headok_flag = 1; //置头码标�?
//    revcnt = 0;
//  }
}
void HAL_TIM_IC_CaptureCallback_CH1(TIM_HandleTypeDef *htim)
{

  //    __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_2,0);
  __HAL_TIM_SET_COUNTER(htim, 0);
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);

  //    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
}

//溢出中断
void HAL_TIM_PeriodElapsedCallbackOver(TIM_HandleTypeDef *htim)
{
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
  memset(IrCode, 0, sizeof(IrCode));
  //IrCode0=IrCode1=IrCode2=IrCode3=0;
  //headok_flag=0;        //清头码标�?
  codeNum = 0; //清计数�??

  //    if (codeNum >= 56){
  //        codeNum = 0;
  //        }
  //    else {
  //        codeNum = 0;
  //        memset (IrCode,0,sizeof(IrCode));
  //        }
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
