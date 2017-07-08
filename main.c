/*
 * main.c
 *
 *  Created on: 28 kwi 2017
 *      Author: Patryk
 */


#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<avr/pgmspace.h>
#include<math.h>

#include "my_ads1115.h"
#include "my_i2c.h"
#include "my_uart.h"


#define interrupt_time 10

// REGISTER'S ADRESSES (used to get or set variable's values via RS232)
#define c_voltagea 	'v'  	// current signal address (returns c_voltage)
#define e_voltagea 	129  	// expected signal (sets e_voltage)
#define c_filla	  	'd'	 	// current diode signal fill (returns d_fill)
#define e_filla    	130  	// sets d_fill
#define res_filla	131  	// resets to normal control, if diode signal fill has been set via RS232
#define c_resistance 'r' 	// current resistance (returns current resistance)
#define e_resistance 132 	// expected resistance
#define d_rise		'w'
#define d_reduce    'm'

// CONTROL variables (variables used to control the light brightness)
uint16_t c_voltage, e_voltage;
uint8_t d_fill;

// ADS, UART variables (variables used to control ADC)
enum _ads_status ads_status=INIT;
enum _uart_status{FREE,SET_DFILL,SET_EVOL1,SET_EVOL2};
enum _uart_status uart_status = FREE;
uint8_t ads_input,u_rec;



//Utility variables (counters, flags, etc.)

double Vol_multiply = 0.0001875, Vf,Vr,Rf,Rr=20000,Vcc=5.1;
int Resistance;

uint8_t i_counter;
uint16_t e_vol_temp;
uint8_t control_settings;
/*control_setting register informs about few things:
 * b0:	e_voltage_set:		informs if expected voltage has been set via RS232
 * b1: 	d_fill_set: 		informs if diode signal fill has been set via RS232
 * b2:  e_resistance_set:	informs if expected resistance has been set via RS232
 */

void SendAsChar(int data)
{
	if(0 == data)
	{
		uart_send('0');
		uart_send('\r');
	}
	else
	{
		int tab[10];
		int charcounter = 0;

		while(0 != data)
		{
			tab[charcounter] = data % 10;
			data = data / 10;
			charcounter ++;
		}

		char output[charcounter];
		int i=0;
		int j=0;

		for(i = charcounter - 1; i >= 0; i--)
		{
			output[j] = tab[i]+48;
			j++;
		}

		for(i=0 ;i < charcounter; i++ )
		{
			uart_send(output[i]);
		}
		uart_send('\r');
	}

}


int main()
{
	c_voltage = i_counter = d_fill = ads_input = 0;
	DDRA = 0xff;
	PORTA = 0x00;

	_delay_ms(200);
	i2c_init(200);

	_delay_ms(200);
	uart_init();

	_delay_ms(200);
	if(1 == ads_init(IN0,FS6144,SSHOT_CONV,SP475))
	{
		PORTA |= 0x02;
	}
	else PORTA &= 0x0b11111101;

	TCCR0 = (1 << CS02);
	TIMSK = (1 << TOIE0);
	sei();
	TCNT0 = interrupt_time;

		while(1)
		{

		}

}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 255 - interrupt_time;

	i_counter++;
	if(i_counter > 51) i_counter = 1;

	if(i_counter <= d_fill)
	{
		PORTA |= 0b00000001;
	}
	else
	{
		PORTA &= 0b11111110;
	}

	if(51 == i_counter)
	{

		switch (ads_status)
		{
		case INIT:
					{
						ads_status = BEG_SINGLE;
						break;
					}
		case BEG_SINGLE:
					{
						ads_begin_single(IN0);
						ads_status = PFR;
						break;
					}
		case PFR:
					{
						ads_prepare_read(CONVER_REG);
						ads_status = READ;
						break;
					}
		case READ:
					{
						c_voltage = ads_read();
						if(c_voltage & (1<<15)) c_voltage=0;

						Vf = c_voltage * Vol_multiply;
						Vr = Vcc - Vf;
						Rf = Rr * Vf / Vr;

						Resistance= (int) Rf;

						if(control_settings & (1 << 0)) // if expected voltage set
						{
							if(!(control_settings & (1 << 1) )) //if d_fill hasn't been changed via rs232
							{
								if(c_voltage < e_voltage)
								{
									if(d_fill < 51)
										d_fill++;
								}
								else
								if(c_voltage > e_voltage)
								{
									if(d_fill < 51)
										d_fill++;
								}
							}
						}

						ads_status=BEG_SINGLE;
						break;
					}
		}//switch
	}// if

}//timer0 ovf interrupt

ISR(USART_RXC_vect)
{

	u_rec=uart_receive();

	switch(uart_status)
	{
	case FREE:
			{
				switch (u_rec)
				{
				case c_voltagea:
					{
						uart_send((c_voltage >> 8));
						uart_send(c_voltage);
						uart_send('\r');
						break;
					}
				case e_voltagea:
					{
						uart_status = SET_EVOL1;
						break;
					}
				case c_filla:
					{
						SendAsChar(d_fill);
						break;
					}
				case c_resistance:
					{
						SendAsChar(Resistance);
						break;
					}
				case e_filla:
					{
						uart_status = SET_DFILL;
						break;
					}
				case res_filla:
					{
						control_settings &= 0b11111101;
						break;
					}
				case d_reduce:
					{
						if(d_fill > 0)
						{
							d_fill--;
							control_settings |= (1 << 1);
						}

						break;
					}
				case d_rise:
					{
						if(d_fill < 51)
						{
							d_fill++;
							control_settings |= (1 << 1);
						}
						break;
					}

				}	// data switch
			break;	//breaking FREE condition
			}	//FREE case

	case SET_DFILL:
			{
				if(d_fill > 51)
					d_fill = 51;
				else
					if(d_fill < 0)
						d_fill = 0;
					else
						d_fill = u_rec;

				control_settings |= (1 << 1);
				uart_status = FREE;
				break;
			}
	case SET_EVOL1:
			{
				e_vol_temp = (u_rec << 8);
				uart_status = SET_EVOL2;
				break;
			}
	case SET_EVOL2:
			{
				e_vol_temp |= u_rec;
				e_voltage = e_vol_temp;
				control_settings |= (1 << 0);
				uart_status = FREE;
				break;
			}
	}
}



