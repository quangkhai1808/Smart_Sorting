
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f411e_discovery.h"
#include "stdbool.h"

/*********This USART2 is used to communicate to Wifi module ESP8266************/
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF7_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

/**************This USART1 is used to emulate barcode reader*****************/
#define USARTy                           USART1
#define USARTy_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USARTy_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTy_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTy_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTy_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTy_TX_PIN                    GPIO_PIN_9
#define USARTy_TX_GPIO_PORT              GPIOA  
#define USARTy_TX_AF                     GPIO_AF7_USART1
#define USARTy_RX_PIN                    GPIO_PIN_10
#define USARTy_RX_GPIO_PORT              GPIOA 
#define USARTy_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
#define USARTy_IRQn                      USART1_IRQn
#define USARTy_IRQHandler                USART1_IRQHandler

/**************This External Interrupt is used for Sensor*****************/
//Sensor signal is read at PA.6 pin
#define SENSOR_PORT											GPIOA
#define SENSOR_PIN											GPIO_PIN_1
#define SENSOR_IRQ											EXTI1_IRQn
#define SENSOR_IRQHandler								EXTI1_IRQHandler

/*********************This is motor declaration***************************/
#define __MOTOR_CLK_ENABLE()						__GPIOE_CLK_ENABLE()
#define MOTOR_CLK_PIN										GPIO_PIN_3
#define MOTOR_CLK_PORT									GPIOE
#define __MOTOR_DIR_ENABLE()						__GPIOB_CLK_ENABLE()
#define MOTOR_DIR_PIN										GPIO_PIN_15
#define MOTOR_DIR_PORT									GPIOD
#define MOTOR_DEFAULT_DISTANCE					2500
#define MOTOR_DEFAULT_SPEED             22000

/********************This is Wifi Access Point declaration***************/
#define WIFI_DEFAULT_ACCESS_POINT				"Phuochang"
#define WIFI_DEFAULT_PASSWORD						"08041506"
#define SERVER_IP_DEFAULT								"184.106.153.149"
#define SERVER_PORT_DEFAULT             "80" 


/********************This is label declaration***********************/
#define HANOI_LABEL					            "HN\x0D\x0A"
#define SAIGON_LABEL										"SG\xD\x0A"
#define DANANG_LABEL										"DN\xD\x0A"

typedef enum {
	ACTUATOR_UNDEF = 0,
	ACTUATOR_1,
	ACTUATOR_2,
	
	ACTUATOR_MAX,
}ACTUATOR_ID;


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
