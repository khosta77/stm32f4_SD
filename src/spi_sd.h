#ifndef STM32F4_SD_SPI_SD_H_
#define STM32F4_SD_SPI_SD_H_

#include <string.h>
#include <stdint.h>

#include "./spi.h"

#define SD_CMD0			0	// Сброс программного обеспечения
#define SD_CMD1			1	// Инициировать процесс инициализации
#define SD_CMD8			8	// Только для SDC версии 2. Диапазон контрольного напряжения
#define SD_CMD9			9   // Считывание регистра CSD
#define SD_CMD10		10  // Считывание регистра CID
#define SD_CMD12		12  // Остановитесь, чтобы Прочитать Данные
#define SD_CMD13		13	// Непонятно почему, но важно
#define SD_CMD16		16  // Изменить размер блока R/W
#define SD_CMD17		17  // Блок чтения
#define SD_CMD18		18  // Считывание нескольких блоков
#define SD_CMD23		23  // Только для MMC. Определите количества блоков передачи с помощью  многоблочной Команды R/W
#define SD_CMD24		24  // Напишите Блок
#define SD_CMD25		25  // Напишите несколько блоков
#define SD_CMD55		55  // Ведущая команда Команды CMD<n>
#define SD_CMD58		58  // Чтение с распознаванием текста
#define SD_ACMD23		23  // Только для SDC. Определите количество блоков для предварительного стирания
#define SD_ACMD41		41  // Только для SDC Инициируйте процесс инициализацииs

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

#define SD_BLOCKSIZE	0x00000200	// 512

uint8_t SD_Init();
void SD_ReadBlock(uint32_t _block, uint8_t* _buffer);
void SD_WriteBlock(uint32_t _block, uint8_t _buffer[]);

#endif  // STM32F4_SD_SPI_SD_H_
