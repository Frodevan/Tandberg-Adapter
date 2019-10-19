/*
 * kode.c
 *
 * Created: 18.04.2018 22:04:23
 * Author : medlem
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "uarts.h"
#include "usrts.h"
#include "screen.h"

uint8_t tandberg2at[256] = {};

						//	  0        1        2        3        4        5        6        7        8        9        A        B        C        D        E        F
uint8_t tandberg2xt[256] = {255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     , 14     , 15     , 28     ,255     ,255     , 28     ,255     ,255     , // 0
							255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,  1     ,255     ,255     ,255     ,255     , // 1
							 57     ,  2|0x80, 40|0x80,  4|0x80,  5|0x80,  6|0x80,  8|0x80, 40     , 10|0x80, 11|0x80,  9|0x80, 13|0x80, 51     , 12     , 52     , 53     , // 2
							 11     ,  2     ,  3     ,  4     ,  5     ,  6     ,  7     ,  8     ,  9     , 10     , 39|0x80, 39     , 51|0x80, 13     , 52|0x80, 53|0x80, // 3
							  3|0x80, 30|0x80, 48|0x80, 46|0x80, 32|0x80, 18|0x80, 33|0x80, 34|0x80, 35|0x80, 23|0x80, 36|0x80, 37|0x80, 38|0x80, 50|0x80, 49|0x80, 24|0x80, // 4
							 25|0x80, 16|0x80, 19|0x80, 31|0x80, 20|0x80, 22|0x80, 47|0x80, 17|0x80, 45|0x80, 21|0x80, 44|0x80, 26     , 43     , 27     ,  7|0x80, 12|0x80, // 5
							255     , 30     , 48     , 46     , 32     , 18     , 33     , 34     , 35     , 23     , 36     , 37     , 38     , 50     , 49     , 24     , // 6
							 25     , 16     , 19     , 31     , 20     , 22     , 47     , 17     , 45     , 21     , 44     , 26|0x80, 43|0x80, 27|0x80, 41|0x80, 14     , // 7
	
							255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     , // 8
							255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     , 79     , 28     ,255     ,255     ,255     ,255     ,255     , // 9
							 59     , 60     , 61     , 62     , 63     , 64     , 65     , 66     , 67     , 68     ,255     ,255     ,255     ,255     ,255     ,255     , // A
							 82     , 72     , 80     , 77     , 75     ,255     ,255     ,255     , 71     ,255     ,255     ,255     , 82     , 83     ,255     ,255     , // B
							 83     ,255     ,255     , 73     , 81     ,255     ,255     ,255     ,255     ,255     , 71     ,255     ,255     , 28     , 58     , 69     , // C
							255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,255     ,  1     ,255     ,255     ,255     ,255     , // D
							 82|0x80, 79|0x80, 80|0x80, 81|0x80, 75|0x80, 76|0x80, 77|0x80, 71|0x80, 72|0x80, 73|0x80, 74     , 83|0x80, 57     , 28     ,255     ,255     , // E
							 14     , 14     ,255     ,255     ,255     ,255     ,255     ,255     , 70     ,255     , 28     , 14     , 55|0x80,255     ,255     ,255     };// F
							 
								   //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
uint8_t tandberg2asciilookup[128] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,	// 8
									   0,  0,  0,  0,  0,  0,  0,  0,  0, 27, 13, 20,  0,  0,  0,  0,	// 9
									   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,	// A
									  62,  3,  5,  9,  1,  0,  0,  0,  7,  0,  0,  0, 54, 50,  0,  0,	// B
									  58,  0,  0, 42, 44,  0,  0,  0,  0,  0, 29,  0,  0, 11, 46, 48,	// C
									   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 18,  0,  0,  0,  0,	// D
									  66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 15,  0,  0,	// E
									  23, 25,  0,  0, 92,  0,  0,  0,  0, 92,  0,  0, 33, 37, 92,  0};  // F

uint8_t tandberg2asciiseq[94] = {0xFF,							//  0 NULL
								0x08, 0xFF,						//  1 CURSOR LEFT
								0x1C, 0xFF,						//  3 CURSOR UP
								0x0B, 0xFF,						//  5 CURSOR DOWN
								0x1D, 0xFF,						//  7 CURSOR HOME
								0x18, 0xFF,						//  9 CURSOR RIGHT
								0x0D, 0xFF,						// 11 CR
								0x0A, 0xFF,						// 13 LF
								0x0D, 0x0A, 0xFF,				// 15 ENTER
								0x1B, 0xFF,						// 18 ESC
								0x1B, 0x5B, 0xFF,				// 20 CSI
								0x19, 0xFF,						// 23 EREASE PAGE
								0x04, 0xFF,						// 25 EREASE LINE
								0x09, 0xFF,						// 27 TAB ->
								0x1B, 0x5B, 0x5A, 0xFF,			// 29 TAB <-
								0x1B, 0x5B, 0x69, 0xFF,			// 33 PRINT (SHIFT)
								0x1B, 0x5B, 0x34, 0x69, 0xFF,	// 37 PRINT (CTRL)
								0x0C, 0xFF,						// 42 ROLL UP
								0x17, 0xFF,						// 44 ROLL DOWN
								0xCE, 0xFF,						// 46 SO
								0xCF, 0xFF,						// 48 SI
								0x1B, 0x5B, 0x4D, 0xFF,			// 50 DEL LINE
								0x1B, 0x5B, 0x4C, 0xFF,			// 54 INS LINE
								0x1B, 0x5B, 0x50, 0xFF,			// 58 DEL CHAR
								0x1B, 0x5B, 0x40, 0xFF,			// 62 INS CHAR
								0x30, 0xFF,						// 66 Numpad 0
								0x31, 0xFF,						// 68 Numpad 1
								0x32, 0xFF,						// 70 Numpad 2
								0x33, 0xFF,						// 72 Numpad 3
								0x34, 0xFF,						// 74 Numpad 4
								0x35, 0xFF,						// 76 Numpad 5
								0x36, 0xFF,						// 78 Numpad 6
								0x37, 0xFF,						// 80 Numpad 7
								0x38, 0xFF,						// 82 Numpad 8
								0x39, 0xFF,						// 84 Numpad 9
								0x2D, 0xFF,						// 86 Numpad -
								0x2E, 0xFF,						// 88 Numpad .
								0x20, 0xFF,						// 90 Numpad Space
								0x03, 0xFF						// 92 BREAK
								};

								

uint8_t hexvals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


void process_input(uint8_t dat)
{
	if(dat<0x80)
	{
		UART_Transmit(dat, 1);					// Tx RS232
	}
	else
	{
		uint8_t ptr = tandberg2asciilookup[dat&0x7F];
		while(tandberg2asciiseq[ptr] != 0xFF)
		{
			UART_Transmit(tandberg2asciiseq[ptr++], 1);
		}
	}
	
	if(dat<0x80 && dat>=20)						// Tx LCD
	{
		screen_write(dat);
	}
	else
	{
		screen_write('[');
		screen_write(hexvals[(dat&0xF0)>>4]);
		screen_write(hexvals[dat&0x0F]);
		screen_write(']');
	}	
	
	uint8_t atcode = tandberg2at[dat];		// Tx AT Tastatur
	if(atcode != 255)
	{
		if(atcode>=0x80)
		{
			USRT_AT_Transmit(44);
		}
		USRT_AT_Transmit(atcode&0x7F);
		USRT_AT_Transmit(atcode|0x80);
		if(atcode>=0x80)
		{
			USRT_AT_Transmit(44|0x80);
		}
	}
	uint8_t xtcode = tandberg2xt[dat];		// Tx XT Tastatur
	if(xtcode != 255)
	{
		if(xtcode>=0x80)
		{
			USRT_XT_Transmit(42);
		}
		USRT_XT_Transmit(xtcode&0x7F);
		USRT_XT_Transmit(xtcode|0x80);
		if(xtcode>=0x80)
		{
			USRT_XT_Transmit(42|0x80);
		}
	}
	
}

int main(void)
{
	init_uarts();
	_delay_ms(1000);
	init_usrts();
	init_screen();

	//screen_print("  Tandberg TDV2215  ");
	//screen_print("RS-232+XT/AT Adapter");
	//screen_write(0x7C);
	//screen_write(0x0A);
	
    while (1) 
    {
		if(UART_Check(1))
		{
			process_input(UART_Receive(1)&0x7F);
		}
		if(UART_Check(2))
		{
			process_input(UART_Receive(2));
		}
	}
}

