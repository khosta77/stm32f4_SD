#ifndef STM32F4_SD_SPI_H_
#define STM32F4_SD_SPI_H_

#include "../system/include/cmsis/stm32f4xx.h"

void SPI_Init(uint8_t SPI_PRESCALER);
void SPI_BeginTransmission();
void SPI_Write(uint8_t data);
void SPI_WriteMulti(uint8_t* data, uint32_t size);
void SPI_EndTransmission();
uint8_t SPI_Read();

#endif  // STM32F4_SD_SPI_H_
