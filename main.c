/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/main.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   This example describes how to configure and use GPIOs through 
  *          the STM32F4xx HAL API.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t static uAktuelleGeschwindigkeit_zehntelkmh = 0;
uint8_t static uAenderung = 0;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void EXTILine0_Config(void);
uint32_t geschwindigkeit(uint32_t);
static void GeschwindigkeitAufLCD(uint32_t);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
 /* This sample code shows how to use STM32F4xx GPIO HAL API to toggle PG13 
     IOs (connected to LED3 on STM32F429i-Discovery board) 
    in an infinite loop.
    To proceed, 3 steps are required: */

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  /* -1- Initialize LEDs mounted on Discovery board */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  
  /* -2- Configure the system clock */
  SystemClock_Config();
  
  /* -3- Configure EXTI Line0 (connected to PA0 pin) in interrupt mode */
  EXTILine0_Config();
  
  /*##-1- Initialize the LCD #################################################*/
  /* Initialize the LCD */
  BSP_LCD_Init();

  /* Initialise the LCD Layers */
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER);

  /* Gleich bleibende Einstellenungen am LCD-Display */
  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Clear the LCD */ 
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);  

  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"GOLF CART", CENTER_MODE);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t*)"v in km/h", CENTER_MODE);  
  
  /* Infinite loop */
  while (1)
  {
      if (uAenderung) {
        GeschwindigkeitAufLCD(uAktuelleGeschwindigkeit_zehntelkmh);  
        uAenderung = 0;
      }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_PWREx_ActivateOverDrive();
 
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTILine0_Config(void)
{
    
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  __GPIOA_CLK_ENABLE();
  
  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint32_t static uDauer_ms = 0;
  uint32_t static uLetzterTick_ms = 0;
  uint32_t uAktuellerTick_ms;                   
  uint32_t static uLetzteGeschwindigkeit_zehntelkmh = 0;
    
  if(GPIO_Pin == KEY_BUTTON_PIN)
  {
    /* Toggle LED3 */
    BSP_LED_Toggle(LED3);
    
    /* Dauer seit letztem Radimpuls */
    uAktuellerTick_ms = HAL_GetTick();                   
    if ((0 != uLetzterTick_ms) && (uAktuellerTick_ms >= uLetzterTick_ms)) 
        uDauer_ms = uAktuellerTick_ms - uLetzterTick_ms;
    else
        uDauer_ms = uAktuellerTick_ms + (UINT32_MAX - uLetzterTick_ms);
    
    /* Geschwindigkeit in km/h berechnen lassen */
    uAktuelleGeschwindigkeit_zehntelkmh = geschwindigkeit(uDauer_ms);
    uAenderung = (uLetzteGeschwindigkeit_zehntelkmh != uAktuelleGeschwindigkeit_zehntelkmh);
    
    /* Geschwindigkeit auf dem Display ausgeben */
    
    /* Aufräumen */
    uLetzterTick_ms = uAktuellerTick_ms;
    uLetzteGeschwindigkeit_zehntelkmh = uAktuelleGeschwindigkeit_zehntelkmh;
  }
}

/**
  * @brief Geschwindigkeit berechnen aus Dauer seit dem letzten Radpuls
  * @param dauer_msec: Dauer seit dem letzten Radpuls in Millisekunden
  * @retval geschwindigkeit_kmh: Geschwindigkeit in km/h
  */
uint32_t geschwindigkeit(uint32_t dauer_ms)
{
  uint32_t const umfang_mm = 2 * 3.14159 * 300; // diesen Wert anpassen je nach Rad
  uint32_t static geschwindigkeit_mm_pro_ms;
  uint32_t static geschwindigkeit_zehntelkm_pro_h;
  {
    /* Funktionswert */
    geschwindigkeit_mm_pro_ms = umfang_mm / dauer_ms;
    geschwindigkeit_zehntelkm_pro_h = geschwindigkeit_mm_pro_ms * 3600 / 100;
    return geschwindigkeit_zehntelkm_pro_h;
  }
}

static void GeschwindigkeitAufLCD(uint32_t geschwindigkeit_zehntelkmh)
{
  uint8_t desc[50];
  
  /* Draw Bitmap */
  // BSP_LCD_DrawBitmap((BSP_LCD_GetXSize() - 80)/2, 65, (uint8_t *)stlogo);
  
  BSP_LCD_SetFont(&Font8);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 20, (uint8_t*)"Copyright (c) vollmermen 2015", CENTER_MODE);
  
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/2 + 15, BSP_LCD_GetXSize(), 60);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE); 
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 + 30, (uint8_t*)"Blauen Taster druecken fuer Puls.", CENTER_MODE);
  BSP_LCD_SetFont(&Font16);
  sprintf((char *)desc, "%4.1f", (float) (geschwindigkeit_zehntelkmh/10));
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 + 45, (uint8_t *)desc, CENTER_MODE);   
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED4 (RED) on */
  BSP_LED_On(LED4);
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
