#include "./spi_sd.h"

static void MyDelay(uint32_t time) {
	for (uint32_t t = 0; t < time; t++);
}

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
	SPI_Write(0xFF);  // Фиктивный байт
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
	SPI_Write(0xFF);  // Фиктивный байт
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
	SPI_Write(0xFF);  // Фиктивный байт
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
    SPI_Init(64);
	SPI_EndTransmission();  // Выставить CS

	MyDelay(12);  // Wait 2ms

	for (uint8_t i = 0; i < 10; i++) {  // Установить 80 BITS
		SPI_Write(0xFF);  // MOSI высокое значение
	}
	SPI_BeginTransmission();
	SPI_Write(0xFF);  // Фиктивный байт
	SPI_Write(0xFF);  // Фиктивный байт

	SD_Command(SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);
	uint8_t _res1 = SD_readRes1();

	if (_res1 == 0x01) {
		SD_Command(SD_CMD8, SD_CMD8_ARG, SD_CMD8_CRC);
		SD_readRes3();

		uint8_t resp = SD_waitTillIdleClear();

		if (resp == 0x00) {
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
			SPI_Init(2);  // Установите Предварительный Калибратор В Соответствии С Потребностями
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

void SD_ReadBlock(uint32_t _block, uint8_t* _buffer) {
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


