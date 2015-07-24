/**
  ******************************************************************************
  * @file    IO_Toggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 
#include <stdint.h>
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "led.h"
#include "spi.h"
#include "i2c.h"

#include "usart.h"




/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/




/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
static __IO uint32_t TimingDelay;
/* Private functions ---------------------------------------------------------*/


void mySPI_Init(void){
 
SPI_InitTypeDef SPI_InitTypeDefStruct; 
GPIO_InitTypeDef GPIO_InitTypeDefStruct;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
 
SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low;
SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_2Edge;
SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
SPI_InitTypeDefStruct.SPI_CRCPolynomial = 7;
 
SPI_Init(SPI1, &SPI_InitTypeDefStruct);
 
 
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE , ENABLE);
 

 
GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_6;
GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);
 
GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_3;
GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
GPIO_Init(GPIOE, &GPIO_InitTypeDefStruct);
 

 
GPIO_SetBits(GPIOE, GPIO_Pin_3);
 
 
SPI_Cmd(SPI1, ENABLE);
 
}

void SPI_send(uint8_t address, uint8_t data)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
 
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPI1, address);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  SPI_I2S_ReceiveData(SPI1);
 
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPI1, data);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  SPI_I2S_ReceiveData(SPI1);
 
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
}



uint8_t SPI_read(uint8_t address)
{
  uint8_t data;
  GPIO_ResetBits(GPIOE, GPIO_Pin_3); 
  address = 0x80 | address;                         // 0b10 - reading and clearing status
  //Delay(1000);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, address);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); 
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  SPI_I2S_ReceiveData(SPI1);

  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, 0x00);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
  data = SPI_I2S_ReceiveData(SPI1);
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
 
  return data;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

uint8_t in[6];
void Systick_Init(void)
{
  if (SysTick_Config(SystemCoreClock / 1000))
    {
      /* Capture error */
      while (1);
    }
  NVIC_SetPriority(SysTick_IRQn,0x0f);
}


int main(void)
{
    uint8_t tmp;

    InitFifo();
    USART_Config();
    LED_Init();
//    I2C_Configuration();
    Systick_Init();
    mySPI_Init();
    //spi();
    //Delay(1000);
    printf("init SPI test \n\r");
  SPI_send(0x23, 0xc9);                         // resetting the accelerometer internal circuit
  SPI_send(0x20, 0x67);                         // 100Hz data update rate, block data update disable, x/y/z enabled 
  SPI_send(0x24, 0x20);                         // Anti aliasing filter bandwidth 800Hz, 16G (very sensitive), no self-test, 4-wire interface
  SPI_send(0x10, 0x00);                         // Output(X) = Measurement(X) - OFFSET(X) * 32;
  SPI_send(0x11, 0x00);                         // Output(Y) = Measurement(Y) - OFFSET(Y) * 32;
  SPI_send(0x12, 0x00);                         // Output(Z) = Measurement(Z) - OFFSET(Z) * 32;
  while (1)
  {

      //tmp = SPI_read(0x28);
      //printf("x : %x \n\r",tmp);
        tmp = SPI_read(0x29);
      printf("x : %x \n\r",tmp);
      Delay(1000);
  }
}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

void SysTick_Handler(void)
{
  TimingDelay_Decrement();
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

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
