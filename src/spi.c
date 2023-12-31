#include "./spi.h"

static void GPIO_Init() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODER3_0;
	GPIOE->ODR |= GPIO_ODR_OD3;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODER12_0;
    GPIOD->ODR &= ~GPIO_ODR_OD12;
    GPIOD->MODER |= GPIO_MODER_MODER13_0;
    GPIOD->ODR &= ~GPIO_ODR_OD13;
}

void SPI_Init(uint8_t SPI_PRESCALER) {
	//// GPIO
    GPIO_Init();
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	GPIOA->AFR[0] |= ((GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0) |
					  (GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_0) |
					  (GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_0));
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);

	//// SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM);

    if(SPI_PRESCALER == 2) SPI1->CR1 &= ~(SPI_CR1_BR);
	if(SPI_PRESCALER == 4) SPI1->CR1 |= SPI_CR1_BR_0;
	if(SPI_PRESCALER == 8) SPI1->CR1 |= SPI_CR1_BR_1;
	if(SPI_PRESCALER == 16) SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1;
	if(SPI_PRESCALER == 32) SPI1->CR1 |= SPI_CR1_BR_2;
	if(SPI_PRESCALER == 64) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
	if(SPI_PRESCALER == 128) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
	if(SPI_PRESCALER == 256) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;

	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
	SPI1->CR1 &= ~SPI_CR1_CPHA;
	SPI1->CR2 = 0x0000;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI_BeginTransmission() {
	while ((SPI1->SR & SPI_SR_BSY));
	while (!(SPI1->SR & SPI_SR_TXE));
	GPIOE->ODR &= ~GPIO_ODR_OD3;
}

void SPI_Write(uint8_t data) {
    while ((SPI1->SR & SPI_SR_BSY));
	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = data;
}

void SPI_WriteMulti(uint8_t* data, uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		while ((SPI1->SR & SPI_SR_BSY));
		while (!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = data[i];
	}
}

void SPI_EndTransmission() {
	while ((SPI1->SR & SPI_SR_BSY));
	while (!(SPI1->SR & SPI_SR_TXE));
	GPIOE->ODR |= GPIO_ODR_OD3;
}

uint8_t SPI_Read() {
	while ((SPI1->SR & SPI_SR_BSY));
	while (!(SPI1->SR & SPI_SR_RXNE));
	return (SPI1->DR);
}


