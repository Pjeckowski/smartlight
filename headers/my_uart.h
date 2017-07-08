/*
 * my_uart.h
 *
 *  Created on: 9 maj 2017
 *      Author: Patryk
 */

#ifndef MY_UART_H_
#define MY_UART_H_



#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<avr/pgmspace.h>
#define speed 8

void uart_init()
{
	// 8 data, 1 stop, no parity
	UCSRB |= (1<<TXEN) | (1<< RXEN) | (1 << RXCIE);
	UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);

	UBRRH= speed >> 8;
	UBRRL=(uint8_t) speed;

}

void uart_send(uint8_t data)
{
    while (!( UCSRA & (1 << UDRE))); // wait while register is free
    UDR = data;                    // load data into the register
}

uint8_t uart_receive()
{
	while(!((UCSRA) & (1 << RXC)));     // wait while data is being received
	return UDR;                     // return data
}


#endif /* MY_UART_H_ */
