/*
 * uarts.c
 *
 * Created: 18.04.2018 22:14:25
 *  Author: medlem
 */ 
#define F_CPU 16000000UL

#define BAUD9600 F_CPU/(16UL*9600UL) - 1UL
#define BAUD2400 F_CPU/(16UL*2400UL) - 1UL
#define BAUD600 F_CPU/(16UL*600UL) - 1UL

#define BUFFERSIZE 32
#define BUFFERSIZEMASK 0x1F

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uarts.h"

//
// TX0 = RS422 Out
// RX1 = RS232 Inn
// TX1 = RS232 Out
// RX2 = RS422 Inn
// TX3 = LCD Out
//

void init_uarts()
{
	cli();
	DDRJ = 0xFF;
	
	// Set BAUD rate
	UBRR0 = BAUD600;	// Tandberg Tx
	UBRR1 = BAUD9600;	// RS-232
	UBRR2 = BAUD2400;	// Tandberg Rx
	UBRR3 = BAUD9600;	// LCD
	
	// Power on
	PRR0 &= ~(1<<PRUSART0);
	PRR1 &= ~((1<<PRUSART1)|(1<<PRUSART2)|(1<<PRUSART3));
	
	// Enable stuff
	UCSR0B |= (1<<TXEN0);
	UCSR1B |= (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	UCSR2B |= (1<<RXEN2)|(1<<RXCIE2);
	UCSR3B |= (1<<TXEN3);

	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	UCSR1C = (1<<USBS1)|(1<<UCSZ10)|(1<<UCSZ11);
	UCSR2C = (1<<USBS2)|(1<<UCSZ20)|(1<<UCSZ21);
	UCSR3C = (1<<USBS3)|(1<<UCSZ30)|(1<<UCSZ31);

	sei();
}

static uint8_t rx0size = 0;
static uint8_t rx0tail = 0;
static uint8_t rx0[BUFFERSIZE];
static uint8_t rx1size = 0;
static uint8_t rx1tail = 0;
static uint8_t rx1[BUFFERSIZE];
static uint8_t rx2size = 0;
static uint8_t rx2tail = 0;
static uint8_t rx2[BUFFERSIZE];
static uint8_t rx3size = 0;
static uint8_t rx3tail = 0;
static uint8_t rx3[BUFFERSIZE];



int UART_Check0()
{
	return (rx0size > 0);
}

int UART_Check1()
{
	return (rx1size > 0);
}

int UART_Check2()
{
	return (rx2size > 0);
}

int UART_Check3()
{
	return (rx3size > 0);
}

int UART_Check(uint8_t channel)
{
	if(channel == 0){return UART_Check0();}
	else if(channel == 1){return UART_Check1();}
	else if(channel == 2){return UART_Check2();}
	else if(channel == 3){return UART_Check3();}
	return (0 != 0);
}

uint8_t UART_Receive0()
{
	uint8_t data = 0;
	while(rx0size == 0){}
	cli();
	data = rx0[rx0tail++];
	rx0tail = rx0tail%BUFFERSIZE;
	rx0size--;
	sei();
	return data;
}

uint8_t UART_Receive1()
{
	uint8_t data = 0;
	while(rx1size == 0){}
	cli();
	data = rx1[rx1tail++];
	rx1tail = rx1tail%BUFFERSIZE;
	rx1size--;
	sei();
	return data;
}

uint8_t UART_Receive2()
{
	uint8_t data = 0;
	while(rx2size == 0){}
	cli();
	data = rx2[rx2tail++];
	rx2tail = rx2tail%BUFFERSIZE;
	rx2size--;
	sei();
	return data;
}

uint8_t UART_Receive3()
{
	uint8_t data = 0;
	while(rx3size == 0){}
	cli();
	data = rx3[rx3tail++];
	rx3tail = rx3tail%BUFFERSIZE;
	rx3size--;
	sei();
	return data;
}

uint8_t UART_Receive(uint8_t channel)
{
	if(channel == 0){return UART_Receive0();}
	else if(channel == 1){return UART_Receive1();}
	else if(channel == 2){return UART_Receive2();}
	else if(channel == 3){return UART_Receive3();}
	return 0;
}

void UART_Transmit0(uint8_t data)
{
	while(!(UCSR0A & (1<<UDRE0))){};
	UDR0 = data;
}

void UART_Transmit1(uint8_t data)
{
	while(!(UCSR1A & (1<<UDRE1))){};
	UDR1 = data;
}

void UART_Transmit2(uint8_t data)
{
	while(!(UCSR2A & (1<<UDRE2))){};
	UDR2 = data;
}

void UART_Transmit3(uint8_t data)
{
	while(!(UCSR3A & (1<<UDRE3))){};
	UDR3 = data;
}

void UART_Transmit(uint8_t data, uint8_t channel)
{
	if(channel == 0){UART_Transmit0(data);}
	else if(channel == 1){UART_Transmit1(data);}
	else if(channel == 2){UART_Transmit2(data);}
	else if(channel == 3){UART_Transmit3(data);}
}

ISR(USART0_RX_vect)
{
	if((UCSR0A&(1<<RXC0)) && rx0size<BUFFERSIZE)
	{
		cli();
		rx0[(rx0tail+rx0size)&BUFFERSIZEMASK] = UDR0;
		rx0size++;
	}
	sei();
}

ISR(USART1_RX_vect)
{
	if((UCSR1A&(1<<RXC1)) && rx1size<BUFFERSIZE)
	{
		cli();
		rx1[(rx1tail+rx1size)&BUFFERSIZEMASK] = UDR1;
		rx1size++;
	}
	sei();
}

ISR(USART2_RX_vect)
{
	if((UCSR2A&(1<<RXC2)) && rx2size<BUFFERSIZE)
	{
		cli();
		rx2[(rx2tail+rx2size)&BUFFERSIZEMASK] = UDR2;
		rx2size++;
	}
	sei();
}

ISR(USART3_RX_vect)
{
	if((UCSR3A&(1<<RXC3)) && rx3size<BUFFERSIZE)
	{
		cli();
		rx3[(rx3tail+rx3size)&BUFFERSIZEMASK] = UDR3;
		rx3size++;
	}
	sei();
}