#include "main.h"
#include "stm32f4xx_hal.h"
#include "sensor.h"
typedef enum {SENSOR_ON , SENSOR_OFF} SENSOR_STATUS_t;
__IO SENSOR_STATUS_t s_sensor_status;

void Sensor_Init()
{
	 __GPIOA_CLK_ENABLE();
	//PA6 External Interrupt
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Configure Button pin as input with External interrupt */
	GPIO_InitStruct.Pin  = SENSOR_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Enable and set Button EXTI Interrupt to the highest priority */
	HAL_NVIC_SetPriority(SENSOR_IRQ, 0x01, 0x00);
	HAL_NVIC_EnableIRQ(SENSOR_IRQ);
}
void Sensor_Wait()
{
		s_sensor_status = SENSOR_OFF;
		while(s_sensor_status == SENSOR_OFF);
		s_sensor_status = SENSOR_OFF;
	
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == SENSOR_PIN)
	{
		s_sensor_status = SENSOR_ON;
	}
	
}
