/*
 * dp.h
 *
 * Created: 2024/5/12 15:41:34
 *  Author: crk
 */ 


#ifndef DP_H_
#define DP_H_

#include <avr/io.h>
#define F_CPU 1000000UL
#include <avr/interrupt.h>
#include <util/delay.h>
void dp_show(unsigned char grade)
{
	unsigned char seg7_hex[16]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e};
	DDRB=(0xff);
	DDRC=(0x01);
	PORTC=(0X00);
	PORTC=0x00; //Ω˚÷πœ‘ æ
	PORTB=seg7_hex[grade];
}


#endif /* DP_H_ */