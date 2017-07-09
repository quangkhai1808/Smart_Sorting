/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */

#ifndef __SERVER_REQUEST_H__
#define __SERVER_REQUEST_H__

#include "stm32f4xx.h"
#include "main.h"

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

#define MAX_RESPONSE_COMMAND_LEN 20
#define MAX_RESPONSE_MESSAGE_LEN 100
#define MAX_RESPONSE_STATUS_LEN  20 
#define MAX_RESPONSE_GET_REQUEST_LEN 100

extern void delay_ticks(uint32_t t);
void Server_Request_Init();
void Server_Join_AccessPoint();
ACTUATOR_ID Server_Request(char* strBarcode);


void HAL_UART_ESP8266_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_ESP8266_RxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_ESP8266_ErrorCallback(UART_HandleTypeDef *UartHandle);
#endif