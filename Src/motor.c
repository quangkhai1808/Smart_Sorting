#include "main.h"
#include "motor.h"

/*Prototype*/
void motor_issue_pulse(uint32_t numPulse);
void motor_run_forward();
void motor_run_backward();
void delay_motor();
void motor_run_to_push(uint32_t numPulse);
void motor_run_to_pull(uint32_t numPulse);

extern void delay_ticks(uint32_t t);

/*---------------------PUBLIC functions--------------------*/
void Motor_Init()
{
	//Init GPIO for CLK pin, DIR pin
	__MOTOR_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin  = MOTOR_CLK_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	__MOTOR_DIR_ENABLE();
	GPIO_InitStruct.Pin  = MOTOR_DIR_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	MOTOR_CLK_PIN_LOW();
	MOTOR_DIR_PIN_LOW();

}
void Motor_Run()
{
		motor_run_to_push(MOTOR_DEFAULT_DISTANCE);
		delay_ticks(500);
		
		motor_run_to_pull(MOTOR_DEFAULT_DISTANCE);
		delay_ticks(500);
}
void motor_run_to_push(uint32_t numPulse)
{
	motor_run_forward();
	motor_issue_pulse(numPulse);

}
void motor_run_to_pull(uint32_t numPulse)
{
	motor_run_backward();
	motor_issue_pulse(numPulse);
}

/*-----
------------Private function definition--------------*/
void delay_motor()
{
	int i =0;
	for(i =0 ; i< MOTOR_DEFAULT_SPEED; i++);
}
void motor_issue_pulse(uint32_t numPulse)
{
	
	while(numPulse--)
	{
		MOTOR_CLK_PIN_HIGH();
		delay_motor();
		MOTOR_CLK_PIN_LOW();
		delay_motor();
		
	}
}

void motor_run_forward()
{
	MOTOR_DIR_PIN_LOW();
	delay_ticks(1);
}
void motor_run_backward()
{
	MOTOR_DIR_PIN_HIGH();
	delay_ticks(1);
}
