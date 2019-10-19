/*
 * usrts.c
 *
 * Created: 02.05.2018 23:28:24
 *  Author: medlem
 */ 
#define F_CPU 16000000UL
#define BUFFERSIZE 32
#define BUFFERSIZEMASK 0x1F

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usrts.h"
#include "screen.h"

void AT_Tick();
void XT_Tick();

//
// PK0 = AT Inn Clock
// PK1 = AT Inn Data
// PK2 = AT Out Clock
// PK3 = AT Out Data
//
// PK4 = XT Inn Clock
// PK5 = XT Inn Data
// PK6 = XT Out Clock
// PK7 = XT Out Data
//

//
// Ikke lagt til interrupt handling av input
// Ikke lagt til PS/2 handling
//

static uint16_t volatile usec = 0;

static uint8_t volatile at_txsize = 0;
static uint8_t volatile at_txtail = 0;
static uint8_t volatile at_txstep = 0;
uint8_t volatile at_txbuffer[BUFFERSIZE];

static uint8_t volatile xt_txsize = 0;
static int8_t volatile xt_txtail = 0;
static int8_t volatile xt_txstep = 0;
uint8_t volatile xt_txbuffer[BUFFERSIZE];





void init_usrts()
{
	DDRK &= ~((1<<DDK0)|(1<<DDK1)|(1<<DDK4)|(1<<DDK5));			// Inputs
	PORTK &= ~((1<<PK0)|(1<<PK1)|(1<<PK4)|(1<<PK5));
	DDRK |= (1<<DDK2)|(1<<DDK3)|(1<<DDK6)|(1<<DDK7);			// Outputs
	PORTK |= (1<<PK2)|(1<<PK3)|(1<<PK6);						// Enable other signals on the open-collector serial bus by default
	PORTK &= ~(1<<PK7);											// Data should be low on the XT kb bus

	cli();														// Setup timers
	usec = 0;
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS01);
	OCR0A = 39;						//20 us intervals
	TIMSK0 |= (1<<OCIE0A);

	PCMSK2 |= (1<<PCINT16)|(1<<PCINT20);						// Interrupt on clock change
	PCICR |= (1<<PCIE2);
	sei();

	USRT_AT_Flush();
	USRT_XT_Flush();
}

ISR(TIMER0_COMPA_vect)
{
	usec = usec+20;
	XT_Tick();
	AT_Tick();
	///////////////////////////////////////// Clear clock edge interrupt
}

ISR(PCINT2_vect)
{
	///////////////////////////////////////// Identify which clock
	///////////////////////////////////////// Measure time since last change
	///////////////////////////////////////// Read Data
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USRT_AT_Flush()
{
	cli();
	at_txtail = 0;
	at_txsize = 0;
	at_txstep = 0;
	sei();
}

int USRT_AT_Check()
{
	return 0;
}

uint8_t USRT_AT_Receive()
{
	return 0;
}

uint8_t USRT_AT_Transmit(uint8_t data)
{
	if(at_txsize < BUFFERSIZE)
	{
		cli();
		at_txbuffer[(at_txtail+at_txsize)&BUFFERSIZEMASK] = data;
		at_txsize++;
		sei();
	}
	else return 0;
	return 1;
}

////////////////////////////
//
// AT Transmit... 44 Stages:
//
// 0-3:		Tx 0
// 4-35:	Tx data
// 36-39:	Tx Parity
// 40-43:	Tx 1
// 44-93:	Clock/Data HIGH (1ms pause)
//
void USRT_AT_Setbit(uint8_t data)
{
	PORTK &= ~(((~data)&0x01)<<PK3);
	PORTK |= ((data&0x01)<<PK3);
}

void AT_Tick()
{
	if(at_txsize > 0 && (at_txstep|(PINK&(1<<PK0))|(PINK&(1<<PK1))))		// Only initiate Tx if data+clock line high before start
	{
		if(at_txstep<44)
		{
			if((at_txstep&0x03) == 0)
			{
				uint8_t d = at_txbuffer[at_txtail];
				switch(at_txstep){
					case 0:
					PORTK &= ~(1<<PK3);
					break;
					case 4:
					USRT_AT_Setbit(d);
					break;
					case 8:
					USRT_AT_Setbit(d>>1);
					break;
					case 12:
					USRT_AT_Setbit(d>>2);
					break;
					case 16:
					USRT_AT_Setbit(d>>3);
					break;
					case 20:
					USRT_AT_Setbit(d>>4);
					break;
					case 24:
					USRT_AT_Setbit(d>>5);
					break;
					case 28:
					USRT_AT_Setbit(d>>6);
					break;
					case 32:
					USRT_AT_Setbit(d>>7);
					break;
					case 36:
					USRT_AT_Setbit(d+(d>>1)+(d>>2)+(d>>3)+(d>>4)+(d>>5)+(d>>6)+(d>>7));
					break;
					case 40:
					PORTK |= (1<<PK3);
				}
			}
			else if(((at_txstep-1)&0x03) == 0)
			{
				PORTK &= ~(1<<PK2);
			}
			else if(((at_txstep-3)&0x03) == 0)
			{
				PORTK |= (1<<PK2);
			}
		}
		at_txstep++;
		if(at_txstep>93)
		{
			at_txtail = (at_txtail+1)&BUFFERSIZEMASK;
			at_txsize--;
			at_txstep = 0;
		}
	}
	///////////////////////////////////////////////////// Clock/Read/Acknowledge if Host-to-Device
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USRT_XT_Flush()
{
	cli();
	xt_txtail = 0;
	xt_txsize = 0;
	xt_txstep = 0;
	sei();
}

int USRT_XT_Check()
{
	return 0;
}

uint8_t USRT_XT_Receive()
{
	return 0;
}

uint8_t USRT_XT_Transmit(uint8_t data)
{
	if(xt_txsize < BUFFERSIZE)
	{
		cli();
		xt_txbuffer[(xt_txtail+xt_txsize)&BUFFERSIZEMASK] = data;
		xt_txsize++;
		sei();
	}
	else return 0;
	return 1;
}

////////////////////////////
//
// XT Transmit... 24 Stages:
//
// 0-1:		Tx 1
// 2-17:	Tx Data 0-7
// 18-19:	Tx 0
// 20-69:	Clock/Data HIGH (1ms pause)
//
void USRT_XT_Setbit(uint8_t data)
{
	PORTK &= ~(((~data)&0x01)<<PK7);
	PORTK |= ((data&0x01)<<PK7);
}
void XT_Tick()
{	
	if(xt_txsize > 0 && (xt_txstep|(PINK&(1<<PK4))|(PINK&(1<<PK5))))		// Only initiate Tx if data+clock line high before start
	{
		uint8_t d = xt_txbuffer[xt_txtail];
		switch(xt_txstep){
			case 0:
				PORTK |= (1<<PK7);
				break;
			case 2:
				USRT_XT_Setbit(d);
				break;
			case 4:
				USRT_XT_Setbit(d>>1);
				break;
			case 6:
				USRT_XT_Setbit(d>>2);
				break;
			case 8:
				USRT_XT_Setbit(d>>3);
				break;
			case 10:
				USRT_XT_Setbit(d>>4);
				break;
			case 12:
				USRT_XT_Setbit(d>>5);
				break;
			case 14:
				USRT_XT_Setbit(d>>6);
				break;
			case 16:
				USRT_XT_Setbit(d>>7);
				break;
			case 18:
				PORTK &= ~(1<<PK7);
		}
		if(xt_txstep&0x01 && xt_txstep<20)
		{
			PORTK &= ~(1<<PK6);
		}
		else
		{
			PORTK |= (1<<PK6);
		}
		xt_txstep++;
		if(xt_txstep>69)
		{
			xt_txtail = (xt_txtail+1)&BUFFERSIZEMASK;
			xt_txsize--;
			xt_txstep = 0;
		}
	}
}