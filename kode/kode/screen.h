/*
 * screen.h
 *
 * Created: 19.04.2018 04.23.26
 *  Author: medlem
 */ 


#ifndef SCREEN_H_
#define SCREEN_H_

void screen_write(uint8_t);
void screen_print(char*);
void screen_placeCursor(uint8_t, uint8_t);
void init_screen();
void screen_home();
void screen_clear();

#endif /* SCREEN_H_ */