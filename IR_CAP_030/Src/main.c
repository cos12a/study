/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "DisplayDrive.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t recvData[7] = {0};
extern uint8_t revok_flag;
extern unsigned char DisBuffer[16];
static uint8_t update_flag = 1;
static uint8_t disOFF_flag = 0;
static uint8_t rev3_flag = 0;
static uint8_t rev2_flag = 0;
static uint8_t fallCnt = 0;  //失败前计数
static uint8_t capID[3] = {0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim14);

  //  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);

    Dis5 = 23;
    Dis6 = 23;
    Dis7 = 23;
    Dis8 = 23;
    DisBuffer[14]=  23;
    DisBuffer[15]=  23;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#if 0
    for(;;){
    for (int i = 0; i <16; i++)
    {
      Dis1 = i;
      Dis2 = i+1;
      Dis3 = i+2;
      Dis4 = i+3;
      Dis5 = i+4;
      Dis6 = i+5;
      Dis7 = i+6;
      Dis8 = i+7;
      
      uint32_t j = 0;
      for (j=0; j < 0xffff; j++)
      {
        DisplayValue();
      }
    } 
    
    }
#endif
    if (revok_flag)
    {
      revok_flag = 0;
      disOFF_flag = 0;
      //        printf("Test: [%X], [%X], [%X], [%X], [%X], [%X], [%X].\r\n",recvData[0],\
//                        recvData[1],recvData[2],recvData[3],recvData[4],\
//                        recvData[5],recvData[6]  );
      if (update_flag)
      {
        fallCnt++;
        Dis2 = recvData[2] % 10;
        Dis1 = (recvData[2] / 10) % 10;
        Dis4 = recvData[1] % 10;
        Dis3 = (recvData[1] / 10) % 10;
        
        DisBuffer[8]=   recvData[3]&0x0F;
        DisBuffer[9]=   recvData[3]>>4;
        DisBuffer[10]=  recvData[4]&0x0F;;
        DisBuffer[11]=  recvData[4]>>4;
        DisBuffer[12]=  recvData[5]&0x0F;;
        DisBuffer[13]=  recvData[5]>>4;
        

 //       if (((recvData[1] == 2) && (recvData[2] == 3)) ||((recvData[1] == 3) && (recvData[2] == 2)))
 //       if (1==1)
        if (recvData[2] == 3)
        {
          Dis7 = 3;
          rev3_flag = 1;
        }
        if (recvData[2] == 2)
        {
          Dis8 = 2;
          rev2_flag  = 1;

        }
        if( fallCnt > 1){
          if (( capID[0]!= recvData[3])||(capID[1] != recvData[4])||(capID[2] != recvData[5]))
          {
            fallCnt = 0;
            update_flag = 0;
            Dis1 = 19;
            Dis2 = 17;
            Dis3 = 20;
            Dis4 = 20;
            Dis5 = 19;
            Dis6 = 19;
            Dis7 = 19;
            Dis8 = 19;
            DisBuffer[8]=   23;
            DisBuffer[9]=   23;
            DisBuffer[10]=  23;
            DisBuffer[11]=  23;
            DisBuffer[12]=  23;
            DisBuffer[13]=  23;
            goto Next_end;
          }
        }else {
            capID[0]= recvData[3];
            capID[1] = recvData[4];
            capID[2] = recvData[5];
        }
          if ((rev2_flag) && (rev3_flag))
          {
            //            printf("<><><><<><<><><>><><<><><><><><<>><><");
            //            printf("\r\nTEST OK!\r\n");
            // HAL_Delay(2000);
            fallCnt = 0;
            update_flag = 0;
            Dis1 = 16;
            Dis2 = 17;
            Dis3 = 18;
            Dis4 = 18;
          }else if(fallCnt > 7){
            fallCnt = 0;
            update_flag = 0;
            Dis1 = 19;
            Dis2 = 17;
            Dis3 = 20;
            Dis4 = 20;
            Dis5 = 21;
            Dis6 = 21;
          }  
      }
    }
Next_end:
    if (disOFF_flag)
    {
//      DisplayOFF();
      Dis7 = 23;
      Dis8 = 23;
        Dis5 = 23;
        Dis6 = 23;
       Dis1 = 0;
      Dis2 = 0;
       Dis3 = 0;
      Dis4 = 0;
      revok_flag = 0;
      update_flag = 1;
      rev2_flag = 0;
      rev3_flag = 0;
    }
    DisplayValue();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t cnt = 0;

  if (update_flag == 0)
  {

    cnt++;
    if (cnt > 30)
    {
      disOFF_flag = 1;
      cnt = 0;
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
