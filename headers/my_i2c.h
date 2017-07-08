/*
 * I2c_lib.h
 *
 *  Created on: 4 maj 2017
 *      Author: Lapek
 */

#ifndef MY_I2C_H_
#define MY_I2C_H_

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<avr/pgmspace.h>

#define W 0
#define R 1

/*sets the frequency of scl clock
 according to pattern scl=uCf/(16+2*TWBR*4^TWPS)
 10 or other  	10 kHz
 20 			20 kHz
 50				50 kHz
 200			200 kHz
 */
uint8_t i2c_init(int freq)
{
	switch (freq)
	{
	case 10:
	{
		TWBR=98;
		TWSR|=(1<<TWPS0);//4^1
		return 1;
	}
	case 20:
	{
		TWBR=48;
		TWSR|=(1<<TWPS0);//4^1
		return 1;
	}
	case 50:
	{
		TWBR=18;
		TWSR|=(1<<TWPS0);//4^1
		return 1;
	}
	case 200:
	{
		TWBR=3;
		TWSR|=(1<<TWPS0);//4^1
		return 1;
	}
	default:
	{
		TWBR=98;
		TWSR|=(1<<TWPS0);//4^1
		return 1;
	}
	}
}

/*sends a start signal
returns 1 if success*/

uint8_t i2c_start()
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT))); // awaits untill the buss is free and start signal is send
	if ((TWSR & 0xF8) != 0x08)
			return 0;
	return 1;
}

/* Sends i2c slave address to the buss.
 * Slave with appropriate address should response with ack signal,
 * which means its waiting for future communication,  so method returns 1.
 * Note, that slave addresses are usually 7bit long, but full address is size of byte.
 * The LSB of sent address indicates if you intend to write (0) or read (1) from the device.
 */
uint8_t i2c_send_addr(uint8_t addr,uint8_t wr)
{
	if(wr == R)
		addr |= 0b00000001;
	TWDR = addr; // loading addr to data register
	TWCR = (1<<TWINT) | (1<<TWEN); //clearing send flag
	while(!(TWCR & (1<<TWINT))); //waiting for data to be sent
	if ((TWSR & 0xF8) != 0x20) // if not data transmitted and ack received
		return 0;
	return 1;
}

uint8_t i2c_send_data(uint8_t data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28) // if not data trasmitted and ack received
		return 0;
	return 1;
}

uint8_t i2c_receive()
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));

	return TWDR;
}

void i2c_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

#endif /* MY_I2C_H_ */
