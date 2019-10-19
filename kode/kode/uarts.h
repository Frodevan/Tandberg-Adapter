/*
 * uarts.h
 *
 * Created: 18.04.2018 22:15:15
 *  Author: medlem
 */ 


#ifndef UARTS_H_
#define UARTS_H_

void init_uarts();
int UART_Check(uint8_t);
uint8_t UART_Receive(uint8_t);
void UART_Transmit(uint8_t, uint8_t);

#endif /* UARTS_H_ */