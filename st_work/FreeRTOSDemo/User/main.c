#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "gd32f10x_eval.h"
#include "FreeRTOS.h"
#include "task.h"

void Task_Led1(void * pvParameters)
{
	while(1)
	{
		gd_led_off(LED0);	vTaskDelay(500);
		gd_led_on(LED0);	vTaskDelay(500);
	}
}
 
 
 
void Task_Led2(void * pvParameters)
{
	while(1)
	{
		gd_led_off(LED0);	vTaskDelay(500);
		gd_led_on(LED0);	vTaskDelay(500);
	}	
}
 
 
 
int main(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);	
  gd_led_init(LED0);
 
	xTaskCreate(Task_Led1,"TaskLed1",configMINIMAL_STACK_SIZE,NULL,2,NULL);	
	xTaskCreate(Task_Led2,"TaskLed2",configMINIMAL_STACK_SIZE,NULL,2,NULL);
	vTaskStartScheduler();
    
    while(1){}
}
