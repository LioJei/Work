/*!
    \file    main.c
    \brief   USART transmit and receive interrupt

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

#include <stdio.h>
#include "gd32f10x.h"
#include "gd32f10x_eval.h"



void USART_Init(void)
{
    // GPIOʱ��ʹ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // USARTʱ��ʹ��
    rcu_periph_clock_enable(RCU_USART1);
    
    // ����TXΪ���츴��ģʽ
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    // ����RXΪ��������ģʽ
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
		gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);
    
    // ���ô��ڵĹ�������
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200U);                        // ������
    usart_word_length_set(USART1, USART_WL_8BIT);               // ֡�����ֳ�
    usart_stop_bit_set(USART1, USART_STB_1BIT);                 // ֹͣλ
    usart_parity_config(USART1, USART_PM_NONE);                 // ��żУ��λ
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);  // Ӳ��������RTS
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);  // Ӳ��������CTS
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);         // ʹ�ܽ���
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);       // ʹ�ܷ���
    usart_enable(USART1);                                       // ʹ�ܴ���    
    
    // ʹ��USART�ж�
    nvic_irq_enable(USART1_IRQn, 0, 0);
    // ʹ�ܴ��ڽ����ж�
    usart_interrupt_enable(USART1, USART_INT_RBNE);
}

/**
 @brief ����д������
 @param pData -[in] д������
 @param dataLen -[in] д�����ݳ���
 @return ��
*/
void UART_Wr(uint8_t *pData, uint32_t dataLen)
{
    uint8_t i;	
    for(i = 0; i < dataLen; i++)
    {
        usart_data_transmit(USART1, pData[i]);                  // ����һ���ֽ�����
        while(RESET == usart_flag_get(USART1, USART_FLAG_TBE)); // ��������ж�
    }
}

/**
  * @brief �ض���c�⺯��printf��USARTx
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}

/**
  * @brief �ض���c�⺯��getchar,scanf��USARTx
  * @retval None
  */
int fgetc(FILE *f)
{
    uint8_t ch = 0;
    ch = usart_data_receive(USART1);
    return ch;
}



/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure COM0 */
    //gd_com_init(COM0);
		USART_Init();
    printf("a usart transmit test example!");
    uint8_t test[3] = {'1', '2', '3'};
    UART_Wr(test, 3);
		
    while (1);
}
