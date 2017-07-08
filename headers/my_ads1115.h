/*
 * ads1115_lib.h
 *
 *  Created on: 5 maj 2017
 *      Author: Patryk
 */

#ifndef MY_ADS1115_H_
#define MY_ADS1115_H_

#include "my_i2c.h"
#define CONFIG_REG 0b00000001
#define CONVER_REG 0b00000000

#define AGND 0b10010000
#define AVDD 0b10010010
#define ASDA 0b10010100
#define ASCL 0b10010110


#define IN0T1 	0b00000000
#define IN0T3 	0b00000001
#define IN1T3 	0b00000010
#define IN2T3 	0b00000011
#define IN0		0b00000100
#define IN1		0b00000101
#define IN2		0b00000110
#define IN3		0b00000111

#define FS6144 	0b00000000
#define FS4096 	0b00000001
#define FS2048 	0b00000010
#define FS1024 	0b00000011
#define FS512	0b00000100
#define FS256	0b00000101

#define SP8 	0b00000000
#define SP16 	0b00000001
#define SP32 	0b00000010
#define SP64 	0b00000011
#define SP128	0b00000100
#define SP250	0b00000101
#define SP475	0b00000110
#define SP860	0b00000111

#define SSHOT_CONV 1
#define CONTI_CONV 0

enum _ads_status{INIT,BEG_SINGLE,PFR,READ}; // enum used to control ads state

uint8_t ads_address = 0b10010000;	// default address of ads when addr pin connected to GND
uint16_t config_p2,config_p1;


/*
 *	Device address depends on to which input is address pin connected to.
 *	0: 		10010000 : addr pin connected to GND (AGND)
 *	1: 		10010010 : addr pin connected to VDD (AVDD)
 *	2: 		10010100 : addr pin connected to SDA (ASDA)
 *	3: 		10010110 : addr pin connected to SCL (ASCL)
 *	other:	10010000 : addr pin connected to GND
 */
void ads_set_address(uint8_t addr)
{
	switch (addr)
	{
	case 0: {ads_address = 0b10010000; break;}
	case 1: {ads_address = 0b10010010; break;}
	case 2: {ads_address = 0b10010100; break;}
	case 3: {ads_address = 0b10010110; break;}
	default:{ads_address = 0b10010000; break;}
	}
}


/*uint8_t ads_init(uint8_t input,uint8_t full/_scale,uint8_t mode, uint8_t data_rate)
 * Function requires I2c interface to be initialized at frequency below 400kHz.
 * Function initializes ads with specified configuration and checks if communication is correct.
 * It returns 1 if success and 0 in case of any fail.
 * input: specifies input port of ads, on which voltage is measured.
 * 			0: in0 to in1 (IN0T1)
 * 			1: in0 to in3 (IN1T3)
 * 			2: in1 to in3 (IN2T3)
 * 			3: in2 to in3 (IN3T3)
 * 			4: in0 to GND (IN0)
 * 			5: in1 to GND (IN1)
 * 			6: in2 to GND (IN2)
 * 			7: in3 to GND (IN3)
 * 			other: in0 to GND
 * full_scale: specifies full scale of measured voltage between -35536 to 35535
 * 			0: 		+- 6.144V	(FS6144)
 * 			1: 		+- 4.096V	(FS4096)
 * 			2: 		+- 2.048V	(FS2048)
 * 			3: 		+- 1.024V	(FS1024)
 * 			4: 		+- 0.512V	(FS512)
 * 			5-7:	+- 0.256V	(FS256)
 * 			other:	+- 6.144V
 * 	mode: specifies operation mode of the device.
 * 	Note, that if mode is set to continuous conversion, device begins to make measurements immediately.
 * 			0:		continuous conversion
 * 			1:		power down single shot mode
 * 			other:	power down single shot mode
 * 	data_rate: specifies conversion SPS (samples per second)
 * 			0:		8 SPS	(SP8)
 * 			1:		16 SPS	(SP16)
 * 			2:		32 SPS	(SP32)
 * 			3:		64 SPS	(SP64)
 * 			4:		128 SPS	(SP128)
 * 			5:		250 SPS	(SP250)
 * 			6:		475 SPS (SP475)
 * 			7:		560 SPS	(SP860)
 * 			other:	475 SPS
 */
uint8_t ads_init(uint8_t input,uint8_t full_scale,uint8_t mode, uint8_t data_rate)
{
	config_p2=0;
	switch (input)
	{
	case 0:{ config_p2 |= 0b00000000; break;}
	case 1:{ config_p2 |= 0b00010000; break;}
	case 2:{ config_p2 |= 0b00100000; break;}
	case 3:{ config_p2 |= 0b00110000; break;}
	case 4:{ config_p2 |= 0b01000000; break;}
	case 5:{ config_p2 |= 0b01010000; break;}
	case 6:{ config_p2 |= 0b01100000; break;}
	case 7:{ config_p2 |= 0b01110000; break;}
	default :{ config_p2 |= 0b01000000;}
	}

	switch (full_scale)
	{
	case 0:{ config_p2 |= 0b00000000; break;}
	case 1:{ config_p2 |= 0b00000010; break;}
	case 2:{ config_p2 |= 0b00000100; break;}
	case 3:{ config_p2 |= 0b00000110; break;}
	case 4:{ config_p2 |= 0b00001000; break;}
	case 5:{ config_p2 |= 0b00001010; break;}
	case 6:{ config_p2 |= 0b00001100; break;}
	case 7:{ config_p2 |= 0b00001110; break;}
	default :{ config_p2 |= 0b00000000;}
	}

	switch(mode)
	{
	case 0:{ config_p2 |= 0b00000000; break;}
	case 1:{ config_p2 |= 0b00000001; break;}
	default:{ config_p2 |= 0b00000001; break;}
	}

	config_p1=0;

	switch(data_rate)
	{
	case 0:{ config_p1 |= 0b00000000; break;}
	case 1:{ config_p1 |= 0b00100000; break;}
	case 2:{ config_p1 |= 0b01000000; break;}
	case 3:{ config_p1 |= 0b01100000; break;}
	case 4:{ config_p1 |= 0b10000000; break;}
	case 5:{ config_p1 |= 0b10100000; break;}
	case 6:{ config_p1 |= 0b11000000; break;}
	case 7:{ config_p1 |= 0b11100000; break;}
	default :{ config_p1 |= 0b11000000;}
	}


	if(1 != i2c_start()) return 0;
	i2c_send_addr(ads_address,W);
	if(i2c_send_data(CONFIG_REG) != 1) return 0;
	if(i2c_send_data(config_p2) != 1) return 0;
	if(i2c_send_data(config_p1) !=1) return 0;
	i2c_stop();

	_delay_ms(10);

	if(1 != i2c_start()) return 0;
	i2c_send_addr(ads_address,W);
	if(1 != i2c_send_data(CONFIG_REG)) return 0;
	i2c_stop();

	_delay_ms(10);


	if(1 != i2c_start()) return 0;
	i2c_send_addr(ads_address,R);
	if(0 != mode) // if not continuous conversion
	{
	if(i2c_receive() != (config_p2|=0b10000000)) return 0;
	}
	else
		if(i2c_receive() != config_p2) return 0;
	if(i2c_receive() != config_p1) return 0;
	i2c_stop();

	return 1;
}

/*
 * Function changes the input of the device on which measurement is done.
 * Function is dedicated to use, when ads is in continuous conversion mode, because in other case
 * it is better to use one of the ads_begin_single function overloads.
 * input: specifies input port of ads, on which voltage is measured.
 * 			0: in0 to in1 (IN0T1)
 * 			1: in0 to in3 (IN0T3)
 * 			2: in1 to in3 (IN1T3)
 * 			3: in2 to in3 (IN2T3)
 * 			4: in0 to GND (IN0)
 * 			5: in1 to GND (IN1)
 * 			6: in2 to GND (IN2)
 * 			7: in3 to GND (IN3)
 * 			other: in0 to GND
 */
void ads_set_input(uint8_t input)
{
	switch (input)
	{
	case 0:{ config_p2 &= 0b10001111; break;}
	case 1:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00010000;
			break;
			}
	case 2:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00100000;
			break;
			}
	case 3:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00110000;
			break;
			}
	case 4:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01000000;
			break;
			}
	case 5:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01010000;
			break;
			}
	case 6:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01100000;
			break;
			}
	case 7:{ config_p2 |= 0b01110000; break;}
	}

	i2c_start();
	i2c_send_addr(ads_address,W);
	i2c_send_data(CONFIG_REG);
	i2c_send_data(config_p2);
	i2c_send_data(config_p1);
	i2c_stop();

}

/*
 * Function begins a single measurement on ADS using configuration specified during ADS initialization.
 * Note, that usage of this function requires to use ads_init function previously.
 */
void ads_begin_single()
{
	i2c_start();
	i2c_send_addr(ads_address,W);
	i2c_send_data(CONFIG_REG);
	i2c_send_data((config_p2 | 0b10000000));
	i2c_send_data(config_p1);
	i2c_stop();
}

/*
 * Function begins a single measurement on ADS on specified input and previously set parameters.
 * input: specifies input port of ads, on which voltage is measured.
 * 			0: in0 to in1
 * 			1: in0 to in3
 * 			2: in1 to in3
 * 			3: in2 to in3
 * 			4: in0 to GND
 * 			5: in1 to GND
 * 			6: in2 to GND
 * 			7: in3 to GND
 */
void ads_begin_single_i(uint8_t input)
{
	switch (input)
	{
	case 0:{ config_p2 &= 0b10001111; break;}
	case 1:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00010000;
			break;
			}
	case 2:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00100000;
			break;
			}
	case 3:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b00110000;
			break;
			}
	case 4:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01000000;
			break;
			}
	case 5:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01010000;
			break;
			}
	case 6:{
			config_p2 &= 0b10001111;
			config_p2 |= 0b01100000;
			break;
			}
	case 7:{ config_p2 |= 0b01110000; break;}
	}

	i2c_start();
	i2c_send_addr(ads_address,W);
	i2c_send_data(CONFIG_REG);
	i2c_send_data((config_p2 | 0b10000000));
	i2c_send_data(config_p1);
	i2c_stop();
}

/*
 * Function points an ADS register, that might be read later.
 * It allows to point on Config register (1) or Conversion register(0).
 */
void ads_prepare_read(uint8_t reg)
{
	uint8_t _reg;
	switch (reg)
	{
	case CONFIG_REG:{_reg=reg; break;}
	case CONVER_REG:{_reg=reg; break;}
	default: {_reg=CONVER_REG;};
	}

	i2c_start();
	i2c_send_addr(ads_address,W);
	i2c_send_data(_reg);
	i2c_stop();
}

/*
 * Function returns value of the last pointed ADS register
 */
uint16_t ads_read()
{
	i2c_start();
	i2c_send_addr(ads_address,R);
	uint8_t temp2 = i2c_receive();
	uint8_t temp1 = i2c_receive();
	i2c_stop();
	uint16_t temp = (temp1 | (temp2<<8));

	return temp;
}

#endif /* MY_ADS1115_H_ */
