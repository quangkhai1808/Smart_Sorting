#include "main.h"
#include "server_request.h"

#include "stm32f4xx_hal.h"
#include "string.h"


#define GET_RESPONSE_EXPECTED_LEN 1
/* User defined data type*/
typedef FlagStatus (*INTCallback_t)(uint8_t);
__IO INTCallback_t fp_Respnse_Parser;

/* Private function prototype-------------------------------------------------*/
void                error_handler();
FlagStatus          esp8266_init();   
FlagStatus          esp8266_reset(); 
FlagStatus          esp8266_send_cmd(const char * strCmd);
FlagStatus          esp8266_cmd_response_parser(uint8_t ch);
FlagStatus          esp8266_GETRequest_response_parser(uint8_t ch);
FlagStatus          esp8266_default_response_parser(uint8_t ch);
FlagStatus          esp8266_set_request_len_parser(uint8_t ch);

FlagStatus 					server_request_close();
FlagStatus 					server_request_connect(char* strServer, char* strPort );
FlagStatus 					server_request_send_request(char* strGETRequest);
FlagStatus 					server_request_join_accesspoint(char* strWifiName, char* strPassword);
uint8_t*   					server_get_response();
uint8_t             server_get_actuatorId_from_response(uint8_t* strResponse );

/* globally static variable*/
__IO ITStatus       g_response_full = RESET;
uint8_t             s_uart_receive;
UART_HandleTypeDef  UartHandle_ServerRequest;
uint8_t							s_join_access_point = RESET;

__IO uint8_t        s_arr_response_message[MAX_RESPONSE_MESSAGE_LEN];
__IO uint8_t        s_response_message_count = 0;

__IO uint8_t        s_arr_response_command[MAX_RESPONSE_COMMAND_LEN];
__IO uint8_t        s_response_command_count = 0;

__IO uint8_t        s_arr_response_status[MAX_RESPONSE_STATUS_LEN];
__IO uint8_t        s_response_status_count  = 0;

__IO uint8_t        s_arr_response_GETRequest[MAX_RESPONSE_GET_REQUEST_LEN];
__IO uint8_t        s_response_GETRequest_count = 0;


/* Privated function definition-------------------------------------------------*/
void Server_Join_AccessPoint()
{
	if(server_request_join_accesspoint(WIFI_DEFAULT_ACCESS_POINT,WIFI_DEFAULT_PASSWORD) == SET)
	{
		s_join_access_point = SET;
	}
	else
		s_join_access_point = RESET;
}
void Server_Request_Init()
{
	esp8266_init();
	fp_Respnse_Parser = esp8266_default_response_parser;
}

ACTUATOR_ID Server_Request(char* strBarcode)
{
	uint8_t* pResponse;
	ACTUATOR_ID id = ACTUATOR_UNDEF;
	if(s_join_access_point == RESET)
	{
		 if(server_request_join_accesspoint(WIFI_DEFAULT_ACCESS_POINT,WIFI_DEFAULT_PASSWORD) != SET)
			 return ACTUATOR_UNDEF;
		 else
			 s_join_access_point = SET;
	}
	if(s_join_access_point == SET)
	{
			if(strcmp(strBarcode, SAIGON_LABEL) == 0)
			{
				server_request_connect(SERVER_IP_DEFAULT,SERVER_PORT_DEFAULT);
				server_request_send_request("GET https://api.thingspeak.com/channels/159238/field/2/last.txt");
				server_request_close();		
				pResponse = server_get_response();
				id = (ACTUATOR_ID) server_get_actuatorId_from_response(pResponse);
			}
			else if(strcmp(strBarcode , DANANG_LABEL) == 0)
			{
				server_request_connect(SERVER_IP_DEFAULT,SERVER_PORT_DEFAULT);
				server_request_send_request("GET https://api.thingspeak.com/channels/159238/field/3/last.txt");
				server_request_close();	
				pResponse = server_get_response();
				id = (ACTUATOR_ID) server_get_actuatorId_from_response(pResponse);				
			}
			else
				id = ACTUATOR_UNDEF;
			
	}
	return id;
}
uint8_t server_get_actuatorId_from_response(uint8_t* strResponse )
{
	return atoi(strResponse);
}
/* Privated function definition-------------------------------------------------*/
uint8_t*   server_get_response()
{
	return s_arr_response_GETRequest;
}

FlagStatus server_request_join_accesspoint(char* strWifiName, char* strPassword)
{
	char strCmd[50];
	FlagStatus result = SET;
	
	//Assign Uart interupt callback to parser response
	fp_Respnse_Parser = esp8266_cmd_response_parser;
	
	//Send command AT to check
	strcpy(strCmd, "AT\x0D\x0A");
	if(esp8266_send_cmd(strCmd)          ==  RESET) return RESET;
	
	//Send command AT+CWMODE=1 to set device as client
	strcpy(strCmd, "AT+CWMODE=1\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;
	
	//Send AT+CWJAP="Wifi","pass" to join acces point
	strcpy(strCmd, "AT+CWJAP=");
	strcat(strCmd,"\"");
	strcat(strCmd,strWifiName);
	strcat(strCmd,"\"");
	strcat(strCmd,",");
	strcat(strCmd,"\"");
	strcat(strCmd,strPassword);
	strcat(strCmd,"\"");
	strcat(strCmd,"\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;
	
	//Setup multiple TCP connection
	strcpy(strCmd, "AT+CIPMUX=1\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;
	return SET;
}
FlagStatus server_request_connect(char* strServer, char* strPort )
{
	char strCmd[50];
	
	//Assign Uart interupt callback to parser response
	fp_Respnse_Parser = esp8266_cmd_response_parser;
	
	//Establish connection
	strcpy(strCmd, "AT+CIPSTART=1,");
	strcat(strCmd, "\"TCP\"");
	strcat(strCmd, ",");
	strcat(strCmd, "\"");
	strcat(strCmd, strServer);
	strcat(strCmd, "\"");
	strcat(strCmd, ",");
	strcat(strCmd, strPort);
	strcat(strCmd, "\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;

	return SET;
}
FlagStatus server_request_close()
{
	char strCmd[50];
	
	//Assign Uart interupt callback to parser response
	fp_Respnse_Parser = esp8266_cmd_response_parser;
	
	//Close connection
	strcpy(strCmd, "AT+CIPCLOSE=5\x0D\x0A");

	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;

	return SET;
}
FlagStatus server_request_send_request(char* strGETRequest)
{
	//AT+CIPSEND=1,63
	char strRequestLen[4];
	char strCmd[50];
	
	//Assign Uart interupt callback to parser response
	fp_Respnse_Parser = esp8266_set_request_len_parser;
	sprintf(strRequestLen , "%d", strlen(strGETRequest) + 2);
	
	strcpy(strCmd, "AT+CIPSEND=1,");
	strcat(strCmd, strRequestLen); //len
	strcat(strCmd, "\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;

	fp_Respnse_Parser = esp8266_GETRequest_response_parser;
	//Send GET request
	strcpy(strCmd, strGETRequest);
	strcat(strCmd, "\x0D\x0A");
	if(esp8266_send_cmd(strCmd) ==  RESET) return RESET;

	return SET;
}


/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of IT Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_ESP8266_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete*/

  /* Turn LED6 on: Transfer in transmission process is correct */
  //BSP_LED_On(LED6);
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */

//<cr><lf>OK<cr><lf>.
#define CR_CHAR 0x0D
#define LF_CHAR 0x0A


void HAL_UART_ESP8266_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	
	if(fp_Respnse_Parser(s_uart_receive) == SET)
	{
		g_response_full = SET;
	}
	else
	{
		g_response_full = RESET;
	}
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
 void HAL_UART_ESP8266_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
  //BSP_LED_On(LED3); 
}
/* Private function definition-------------------------------------------------*/
FlagStatus esp8266_reset()
{
	//Assign Uart interupt callback to parser response
	fp_Respnse_Parser = esp8266_cmd_response_parser;
	
	//Send command AT to check
	if(esp8266_send_cmd("AT\x0D\x0A") == RESET) return RESET;
	if(esp8266_send_cmd("AT+RST\x0D\x0A") == RESET) return RESET;
	
	return SET;
}
FlagStatus esp8266_init()
{
		
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART1 configured as follow:
	- Word Length = 8 Bits
	- Stop Bit = One Stop bit
	- Parity = None
	- BaudRate = 9600 baud
	- Hardware flow control disabled (RTS and CTS signals) 
	*/
	
	UartHandle_ServerRequest.Instance          = USARTx;

	UartHandle_ServerRequest.Init.BaudRate     = 115200;
	UartHandle_ServerRequest.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle_ServerRequest.Init.StopBits     = UART_STOPBITS_1;
	UartHandle_ServerRequest.Init.Parity       = UART_PARITY_NONE;
	UartHandle_ServerRequest.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle_ServerRequest.Init.Mode         = UART_MODE_TX_RX;
	UartHandle_ServerRequest.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&UartHandle_ServerRequest) != HAL_OK)
	{
		error_handler();
  }

}

//command  : command<lf><cr>
//response : <cr><lf>response<cr><lf>. 
FlagStatus esp8266_send_cmd(const char * strCmd)
{
	//Transmit command
	if(HAL_UART_Transmit(&UartHandle_ServerRequest, (uint8_t*)strCmd, strlen(strCmd) , 100)!= HAL_OK)
	{
		error_handler();
	}

	//Wait for response	g_response_full = RESET; 
	do {
		if(HAL_UART_Receive_IT(&UartHandle_ServerRequest, &s_uart_receive, 1) != HAL_OK)
		{
			error_handler();
		}
		while(UartHandle_ServerRequest.RxState == HAL_UART_STATE_BUSY_RX);
	}while(g_response_full == RESET);		
	
	return SET;
}

void error_handler()
{
	
}

//command<cr><cr><lf> response<cr><lf> <cr><lf>status<cr><lf>

typedef enum{
	RECEIVE_UNDEF_STA,
	RECEIVE_COMMAND_STA,
	RECEIVE_MESSAGE_STA,
	RECEIVE_STATUS_STA,

}RESPONSE_RECEPTION_STATE;

uint8_t s_char_expected;
uint8_t s_start_tracking = 0;
static RESPONSE_RECEPTION_STATE s_parser_state = RECEIVE_COMMAND_STA;

void esp8266_response_unexpected()
{
	s_parser_state   = RECEIVE_UNDEF_STA;
	s_start_tracking = RESET;
	s_char_expected  = 0;
	s_response_message_count = 0;
	s_response_command_count = 0;
	s_response_status_count  = 0;
}
void esp8266_cmd_response_reset()
{
	s_parser_state   = RECEIVE_COMMAND_STA;
	s_start_tracking = RESET;
	s_char_expected  = 0;
	s_response_message_count = 0;
	s_response_command_count = 0;
	s_response_status_count  = 0;
}
FlagStatus esp8266_default_response_parser(uint8_t ch)
{
	return RESET;
}
FlagStatus s_expected_character = RESET;
//response example : +IPD 1,1,1:1
FlagStatus esp8266_GETRequest_response_parser(uint8_t ch)
{
	//Simply , store entire response to buffer;
	
	
//	if(s_start_tracking == SET)
//	{
//		if(ch == '}')
//		{
//			s_start_tracking = RESET;
//			return SET;
//		}
//		else s_arr_response_GETRequest[s_response_GETRequest_count++] = ch;
//	}
//	else
//	{
//		if(ch == '{')
//			s_start_tracking = SET;
//	}
		if(ch == '+')
			s_start_tracking = SET;
		
		else if(s_start_tracking == SET)
		{
			if(ch == ':')
			{
				s_expected_character = SET;
				s_response_GETRequest_count = 0;
			}
			else if (s_expected_character == SET)
			{
				s_arr_response_GETRequest[s_response_GETRequest_count++] = ch;
				
				if(s_response_GETRequest_count == GET_RESPONSE_EXPECTED_LEN)
				{
					s_expected_character = RESET;
					s_start_tracking = RESET;
					return SET;					
				}

			}
		}
	
	return RESET;
	
}
FlagStatus esp8266_set_request_len_parser(uint8_t ch)
{
	if(ch == '>')
		return SET;
	else 
		return RESET;
}
FlagStatus esp8266_cmd_response_parser(uint8_t ch)
{
	uint8_t result = RESET;

	switch(s_parser_state)
	{
	//This state format : COMMAND<cr><cr><lf>
	case RECEIVE_COMMAND_STA:
		//At the beginning
		if( s_start_tracking == RESET)
		{
			if(ch == CR_CHAR)
			{
				s_start_tracking = SET;
				s_char_expected  = CR_CHAR;
			}
			else
			{
				s_arr_response_command[s_response_command_count++] = ch;
				//Save to buffer of command reception
			}
		}
		//Tracking is started
		else
		{
			//Next character after tracking start must be CR
			if(ch == s_char_expected )
			{
				if(ch == CR_CHAR) s_char_expected = LF_CHAR;
				else if(ch == LF_CHAR)
				{
					//switch to next state
					s_parser_state   = RECEIVE_MESSAGE_STA;
					s_start_tracking = RESET;

				}
			}
			else
			{
				//Unexpected character
				esp8266_response_unexpected();

			}
		}
		break;

	//This state format : RESPONSE<cr><lf>
	case RECEIVE_MESSAGE_STA:
		if(s_start_tracking == RESET)
		{
			if(ch == CR_CHAR)
			{
				//Start tracking
				s_start_tracking = SET;
				s_char_expected = LF_CHAR;
			}
			else
				//Save to reponse message buffer
				s_arr_response_message[s_response_message_count++] = ch;
		}
		//Tracking is started
		else
		{
			if(ch == s_char_expected)
			{
				//switch to next state
				s_parser_state   = RECEIVE_STATUS_STA;
				s_start_tracking = RESET;
				s_char_expected  = CR_CHAR;
			}

			else
			{
				//Unexpected character
				esp8266_response_unexpected();
			}
		}
		break;

	//This state format : <cr><lf>STATUS<cr><lf>
	case RECEIVE_STATUS_STA:
		if(s_start_tracking == RESET)
		{
			if(ch == s_char_expected)
			{
				s_char_expected = LF_CHAR;
				s_start_tracking = SET;
			}
			else
			{
				s_arr_response_status[s_response_status_count++] = ch;
			}
		}
		else
		{
			if(ch == s_char_expected)
			{
				if(s_response_status_count == 0)
				{
					s_start_tracking = RESET;
					s_char_expected = CR_CHAR;
				}
				else
					//End state machine
					{
						result = SET;
						esp8266_cmd_response_reset();
					}
			}
		}
		break;
	}
	return result ;
}