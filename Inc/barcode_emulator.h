#ifndef __BARCODE_EMU_H__
#define __BARCODE_EMU_H__

#include "stm32f4xx.h"
#define BARCODE_EMU_LEN 4

void Barcode_Emu_Init();
void Barcode_Wait();
uint8_t* Barcode_Emu_GetBarcode();
uint8_t  Is_Barcode_Existing();
void HAL_UART_BarcodeEmu_TxCpltCallback(UART_HandleTypeDef* UartHandle);
void HAL_UART_BarcodeEmu_RxCpltCallback(UART_HandleTypeDef* UartHandle);
void HAL_UART_BarcodeEmu_ErrorCallback(UART_HandleTypeDef* UartHandle);

#endif //__BARCODE_EMU_H__