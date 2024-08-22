/*!
    \file    gd32f10x_eval.h
    \brief   definitions for GD32F10x_EVAL's leds, keys and COM ports hardware resources

    \version 2024-01-05, V2.3.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GD32F10X_EVAL_H
#define GD32F10X_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f10x.h"
     
/* exported types */
typedef enum 
{
    LED0 = 0,
} led_typedef_enum;

typedef enum 
{
    KEY_0 = 0,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                            4U
/* LED0 */
#define LED0_PIN                        GPIO_PIN_13
#define LED0_GPIO_PORT                  GPIOC
#define LED0_GPIO_CLK                   RCU_GPIOC

#define COMn                            2U
/* com0 */
#define COM0                        		USART1
#define COM0_CLK                    		RCU_USART1
#define COM0_TX_PIN                 		GPIO_PIN_9
#define COM0_RX_PIN                 		GPIO_PIN_10
#define COM0_GPIO_PORT              		GPIOA
#define COM0_GPIO_CLK               		RCU_GPIOA

#define KEYn                            3U
/* key_0 */
#define KEY0_PIN                   			GPIO_PIN_15
#define KEY0_GPIO_PORT             			GPIOA
#define KEY0_GPIO_CLK              			RCU_GPIOA
#define KEY0_EXTI_LINE             			EXTI_15
#define KEY0_EXTI_PORT_SOURCE      			GPIO_PORT_SOURCE_GPIOA
#define KEY0_EXTI_PIN_SOURCE       			GPIO_PIN_SOURCE_15
#define KEY0_EXTI_IRQn             			EXTI10_15_IRQn  


/* function declarations */
/* configure led GPIO */
void gd_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_com_init(uint32_t com);

#ifdef cplusplus
}
#endif

#endif /* GD32F10X_EVAL_H */
