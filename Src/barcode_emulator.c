#include "main.h"
#include "barcode_emulator.h"


/*Private function prototype*/
void error_barcode_emu_handler();

UART_HandleTypeDef  UartHandle_BarcodeEmu;
uint8_t s_arr_barcode[BARCODE_EMU_LEN + 1] = {0};
uint8_t s_barcode_existing = 0;

void Barcode_Emu_Init()
{
	UartHandle_BarcodeEmu.Instance          = USARTy;

	UartHandle_BarcodeEmu.Init.BaudRate     = 115200;
	UartHandle_BarcodeEmu.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle_BarcodeEmu.Init.StopBits     = UART_STOPBITS_1;
	UartHandle_BarcodeEmu.Init.Parity       = UART_PARITY_NONE;
	UartHandle_BarcodeEmu.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle_BarcodeEmu.Init.Mode         = UART_MODE_TX_RX;
	UartHandle_BarcodeEmu.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&UartHandle_BarcodeEmu) != HAL_OK)
	{
		error_barcode_emu_handler();
  }
	
	if(HAL_UART_Receive_IT(&UartHandle_BarcodeEmu, s_arr_barcode , BARCODE_EMU_LEN) != HAL_OK)
	{
			error_barcode_emu_handler();
	}
}
void Barcode_Wait()
{
	if(HAL_UART_Receive_IT(&UartHandle_BarcodeEmu, s_arr_barcode , BARCODE_EMU_LEN) != HAL_OK)
	{
			error_barcode_emu_handler();
	}
	while(UartHandle_BarcodeEmu.RxState == HAL_UART_STATE_BUSY_RX);
}
uint8_t  Is_Barcode_Existing()
{
	return (s_barcode_existing > 0 );
}
uint8_t* Barcode_Emu_GetBarcode()
{
	s_barcode_existing = 0;
	return s_arr_barcode;
}

void HAL_UART_BarcodeEmu_TxCpltCallback(UART_HandleTypeDef* UartHandle)
{
	
}
void HAL_UART_BarcodeEmu_RxCpltCallback(UART_HandleTypeDef* UartHandle)
{
	s_barcode_existing = 1;
	//if(HAL_UART_Receive_IT(&UartHandle_BarcodeEmu, s_arr_barcode , BARCODE_EMU_LEN) != HAL_OK)
	//{
	//		error_barcode_emu_handler();
	//}
}
void HAL_UART_BarcodeEmu_ErrorCallback(UART_HandleTypeDef* UartHandle)
{
	
}

/*Private function definition*/
void error_barcode_emu_handler()
{
	
}