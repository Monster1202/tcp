#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


// #define noInterrupts() portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;taskENTER_CRITICAL(&mux)
// #define interrupts() taskEXIT_CRITICAL(&mux)

// #define DEVICE_DISCONNECTED_C -127
// #define DEVICE_DISCONNECTED_F -196.6
// #define DEVICE_DISCONNECTED_RAW -7040
// #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
// #define pgm_read_byte(addr)   (*(const unsigned char *)(addr))

// typedef uint8_t DeviceAddress[8];
// typedef uint8_t ScratchPad[9];

// // Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// // Tiny 2x16 entry CRC table created by Arjen Lentz
// // See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
// static const uint8_t dscrc2x16_table[] = {
// 	0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
// 	0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
// 	0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
// 	0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
// };
#ifdef __cplusplus
extern "C" {
#endif


// //test1
void ds18b20_read(void* arg);
#define DS18B20_DQSet()		gpio_set_level(GPIO_IO_DS18B20, 1)		//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define DS18B20_DQReset()	gpio_set_level(GPIO_IO_DS18B20, 0)		//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define DS18B20_DQModeOutput()	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_OUTPUT)	//	Port_SetMode(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP)
#define DS18B20_DQModeInput()	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_INPUT)
#define DS18B20_DIORead()		gpio_get_level(GPIO_IO_DS18B20)	//	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)
double DS18B20_Start(void);
uint8_t DS18B20_ReadByte(void);


// void ds18b20_init(int GPIO);

// #define ds18b20_send ds18b20_write
// #define ds18b20_send_byte ds18b20_write_byte
// #define ds18b20_RST_PULSE ds18b20_reset

// void ds18b20_write(char bit);
// unsigned char ds18b20_read(void);
// void ds18b20_write_byte(char data);
// unsigned char ds18b20_read_byte(void);
// unsigned char ds18b20_reset(void);

// bool ds18b20_setResolution(const DeviceAddress tempSensorAddresses[], int numAddresses, uint8_t newResolution);
// bool ds18b20_isConnected(const DeviceAddress *deviceAddress, uint8_t *scratchPad);
// void ds18b20_writeScratchPad(const DeviceAddress *deviceAddress, const uint8_t *scratchPad);
// bool ds18b20_readScratchPad(const DeviceAddress *deviceAddress, uint8_t *scratchPad);
// void ds18b20_select(const DeviceAddress *address);
// uint8_t ds18b20_crc8(const uint8_t *addr, uint8_t len);
// bool ds18b20_isAllZeros(const uint8_t * const scratchPad);
// bool isConversionComplete();
// uint16_t millisToWaitForConversion();

// void ds18b20_requestTemperatures();
// float ds18b20_getTempF(const DeviceAddress *deviceAddress);
// float ds18b20_getTempC(const DeviceAddress *deviceAddress);
// int16_t calculateTemperature(const DeviceAddress *deviceAddress, uint8_t* scratchPad);
// float ds18b20_get_temp(void);

// void reset_search();
// bool search(uint8_t *newAddr, bool search_mode);

//test2
// double ReadTemperature(void);
// void ds18b20_read(void* arg);
//#define GPIO_IO_DS18B20    4
// #define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)



#ifdef __cplusplus
}
#endif
