#include "../system/include/cmsis/stm32f4xx.h"

#include <string.h>
#include <stdint.h>

void MyDelay(uint32_t time) {
	for (uint32_t t = 0; t < time; t++);
}

void GPIO_Init() {
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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	GPIOA->AFR[0] |= ((GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0) |
					  (GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_0) |
					  (GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_0));
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);

	//// SPI
	// 0. Включаем тактирование
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// 1. Настраиваем на Master Mode
	SPI1->CR1 |= SPI_CR1_MSTR;

	// 2. Отключаем
	// однонаправленный режим работы по одной линии данных
//	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
	// выход запрещен (режим "только прием"), теперь работает одновременные передача и прием
//	SPI1->CR1 &= ~SPI_CR1_RXONLY;

	// 3. Настраиваем прием на 8 бит
//	SPI1->CR1 &= ~SPI_CR1_DFF;

	// 4. Настройка управления slave устройством
	SPI1->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM);

	// 5. Управление скоростью передачи 000: fPCLK/2
	//SPI1->CR1 &= ~SPI_CR1_BR;
    if(SPI_PRESCALER == 2) SPI1->CR1 &= ~(SPI_CR1_BR);
	if(SPI_PRESCALER == 4) SPI1->CR1 |= SPI_CR1_BR_0;
	if(SPI_PRESCALER == 8) SPI1->CR1 |= SPI_CR1_BR_1;
	if(SPI_PRESCALER == 16) SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1;
	if(SPI_PRESCALER == 32) SPI1->CR1 |= SPI_CR1_BR_2;
	if(SPI_PRESCALER == 64) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
	if(SPI_PRESCALER == 128) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
	if(SPI_PRESCALER == 256) SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
	// 6. Установка передачи в первый режим MSB
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

	// 7. Настройка полярности тактов(CPOL) и фазы тактов(CPHA)
	SPI1->CR1 &= ~SPI_CR1_CPHA;
//	SPI1->CR1 &= ~SPI_CR1_CPOL;

	// 8. Включить CRC
//	SPI1->CR1 |= SPI_CR1_CRCEN;

	// 9. Выбор формата Motorola, какой то внутренний, пока не понятно что это
	SPI1->CR2 = 0x0000;

	// 10. Включаем SPI
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

//===========================================================================================================
#define SD_CMD0			0		//Software Reset
#define SD_CMD1			1		//Initiate Initialization Process
#define SD_CMD8			8		//For Only SDC V2. Check Voltage Range
#define SD_CMD9			9       //Read CSD Register
#define SD_CMD10		10      //Read CID Register
#define SD_CMD12		12      //Stop To Read Data
#define SD_CMD13		13		//IDK Why But Important
#define SD_CMD16		16      //Change R/W Block Size
#define SD_CMD17		17      //Read Block
#define SD_CMD18		18      //Read Multiple Blocks
#define SD_CMD23		23      //For Only MMC. Define Number Of Blocks To Transfer With Next Multi-Block R/W Command
#define SD_CMD24		24      //Write A Block
#define SD_CMD25		25      //Write Multiple Blocks
#define SD_CMD55		55      //Leading Command Of ACMD<n> Command
#define SD_CMD58		58      //Read OCR
#define SD_ACMD23		23      //For Only SDC. Define Number Of Blocks To Pre-Erase With Next Multi-Block Write Command
#define SD_ACMD41		41      //For Only SDC Initiate Initialization Process

#define SD_CMD0_ARG		0x00000000
#define SD_CMD8_ARG		0x000001AA
#define SD_CMD55_ARG	0x00000000
#define SD_ACMD41_ARG	0x40000000
#define SD_CMD58_ARG	0x00000000
#define SD_CMD13_ARG	0x00000000

#define SD_CMD0_CRC 	0x95
#define SD_CMD8_CRC		0x87
#define SD_CMD55_CRC	0xFF
#define SD_CMD58_CRC	0xFD
#define SD_ACMD41_CRC	0xFF
#define SD_CMD13_CRC	0xFF
#define SD_CMD16_CRC 	0xFF
#define SD_CMD17_CRC 	0xFF
#define SD_CMD24_CRC 	0xFF
#define SD_CMD25_CRC 	0xFF

#define SD_BLOCKSIZE	0x00000200	//512 (decimal)

static void SD_Command(uint8_t _cmd, uint32_t _arg, uint8_t _crc) {
	SPI_Write((uint8_t)(_cmd | 0x40));
	SPI_Write((uint8_t)(_arg >> 24));
	SPI_Write((uint8_t)(_arg >> 16));
	SPI_Write((uint8_t)(_arg >> 8));
	SPI_Write((uint8_t)_arg);
	SPI_Write(_crc);
}

static uint8_t SD_readRes1() {
	uint8_t counter = 0;
	uint8_t resp = 0;

	while ((resp = SPI_Read()) != 0x01) {
		SPI_Write(0xFF);
		counter++;
		if (counter == 8) 
            break;
	}
	SPI_Write(0xFF);  // Dummy Byte
	return resp;
}

static uint8_t SD_readTillSDOK() {
	uint8_t counter = 0;
	uint8_t resp = 1;

	while (resp != 0x00) {
		SPI_Write(0xFF);
		resp = SPI_Read();
		counter++;
		if (counter == 8)
            break;
	}
	SPI_Write(0xFF);  // Dummy Byte
	return resp;
}

static uint8_t SD_readTillReadOK() {
	uint8_t counter = 0;
	uint8_t resp = 0;

	while (resp != 0xFE) {
		SPI_Write(0xFF);
		resp = SPI_Read();
		counter++;
		if (counter == 149)
            break;
	}
	SPI_Write(0xFF);  // Dummy Byte
	return resp;
}

static void SD_readRes3() {
	SD_readRes1();
	SPI_Write(0xFF);
	SPI_Write(0xFF);
	SPI_Write(0xFF);
	SPI_Write(0xFF);
	SPI_Write(0xFF);
}

static uint8_t SD_waitTillIdleClear() {
	uint8_t resp = 1;
	uint8_t counter = 0;

	while (resp != 0x00) {
		SD_Command(SD_CMD55, SD_CMD55_ARG, SD_CMD55_CRC);
		SD_readRes1();
		SD_Command(SD_ACMD41, SD_ACMD41_ARG, SD_ACMD41_CRC);

		if ((resp = SD_readTillSDOK()) == 0x00) {
			return 1;
		} else {
			counter++;
			if (counter == 149) 
                break;
		}
	}
	return 0;
}

uint8_t SD_Init() {
	//SPI_Init(sd_handler->SPI, remap_func, CS_GPIO, CS, SPI_MASTER_MODE, SPI_MODE_0, SPI_CLK_DIV_BY_64);		//SD CLK Set To 250KHz

    GPIO_Init();
    SPI_Init(64);
	SPI_EndTransmission();  // Pull CS HIGH

	MyDelay(12);  // Wait 2ms

	for (uint8_t i = 0; i < 10; i++) {  // Send 80 BITS
		SPI_Write(0xFF);  // MOSI HIGH
	}
	SPI_BeginTransmission();
	SPI_Write(0xFF);  // Dummy Byte
	SPI_Write(0xFF);  // Dummy Byte

	SD_Command(SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);
	uint8_t _res1 = SD_readRes1();

	if (_res1 == 0x01) {
		SD_Command(SD_CMD8, SD_CMD8_ARG, SD_CMD8_CRC);
		SD_readRes3();

		uint8_t resp = SD_waitTillIdleClear();

		if (resp == 0x00) {
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
			SPI_Init(2);  // Set The Pre-scaler According To Needs
		}
		return resp;
	}
	else return 0;
}

static uint8_t SD_waitTillReadOK(uint32_t _block) {
	uint8_t resp = 1;
	uint8_t counter = 0;

	while (resp != 0xFE) {
		SD_Command(SD_CMD17, _block, SD_CMD17_CRC);
		SD_readTillSDOK();

		if ((resp = SD_readTillReadOK()) == 0xFE) {
			return 1;
		} else {
			counter++;
			if (counter == 149)
                break;
		}
	}
	return 0;
}

void SD_ReadBlock(uint32_t _block, uint8_t* _buffer)
{
	SPI_EndTransmission();
	SPI_Write(0xFF);
	SPI_BeginTransmission();
	SPI_Write(0xFF);

	SD_waitTillReadOK(_block);

	*_buffer++ = SPI_Read();

	for(uint16_t i = 0; i < 512; i++) {
		SPI_Write(0xFF);
		*_buffer++ = SPI_Read();
	}
	SPI_Write(0xFF);
	SPI_Write(0xFF);

	SPI_Write(0xFF);
	SPI_EndTransmission();
	SPI_Write(0xFF);
}

static uint8_t SD_readWriteFinish() {
	uint8_t counter = 0;
	uint8_t resp = 0;

	while ((resp = SPI_Read()) != 0xE5) {
		SPI_Write(0xFF);
		counter++;
		if (counter == 8)
            break;
	}
	SPI_Write(0xFF);
	return resp;
}

static void SD_readTillDataWriteComplete() {
	uint8_t resp = 0;

	SPI_BeginTransmission();

	while ((resp = SPI_Read()) == 0x00) {
		SPI_Write(0xFF);
	}
	SPI_EndTransmission();
	SPI_Write(0xFF);
	SPI_Write(0xFF);
}

void SD_WriteBlock(uint32_t _block, uint8_t _buffer[]) {
	SPI_EndTransmission();
	SPI_Write(0xFF);
	SPI_BeginTransmission();
	SPI_Write(0xFF);

	SD_Command(SD_CMD24, _block, SD_CMD24_CRC);
	SD_readTillSDOK();
	SPI_Write(0xFE);

	for (uint16_t i = 0; i < 512; i++) {
		SPI_Write(_buffer[i]);
	}
	SD_readWriteFinish();
	SD_readTillDataWriteComplete();
}
//===========================================================================================================

int main(void) {
    char write_buffer[512];
	char read_buffer[512];

	memset(write_buffer, 'A',  sizeof(write_buffer));
	memset(read_buffer,  0x00, sizeof(read_buffer));

 	if(SD_Init() == 0) {
        while(1) {
            GPIOD->ODR |= GPIO_ODR_OD12;
        }
    }

 	//SD_WriteBlock(50, (uint8_t*)write_buffer);
 	SD_ReadBlock(50, (uint8_t*)read_buffer);

    if (read_buffer[0] == 'A')
        GPIOD->ODR |= GPIO_ODR_OD12;

	while(1) {
        GPIOD->ODR |= GPIO_ODR_OD13;
	}
}


