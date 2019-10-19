/*
 * Screen.c
 *
 * Created: 19.04.2018 04.22.38
 *  Author: medlem
 */ 

#include <avr/io.h>
#include "uarts.h"

void screen_write(uint8_t d)
{
	UART_Transmit(d, 3);
}

void screen_placeCursor(uint8_t x, uint8_t y)
{
	screen_write(0xFE);
	if(y&0x01)
	{
		x = x+64;
	}
	if(y&0x02)
	{
		x = x+20;
	}
	screen_write(0x80+x);
}

void screen_print(char* d)
{
	for(int i=0; d[i] != 0; i++)
	{
		screen_write(d[i]&0x7F);
	}
}


void screen_home()
{
	screen_write(0xFE);
	screen_write(0x80);
}

void screen_clear()
{
	screen_write(0xFE);
	screen_write(0x01);
	screen_home();
}

void init_screen()
{
	screen_write(0xFE);
	screen_write(0x0C);
	screen_clear();
	screen_write(0xFE);
	screen_write(0x0D);
}