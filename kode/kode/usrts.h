/*
 * usrts.h
 *
 * Created: 02.05.2018 23:26:53
 *  Author: medlem
 */ 


#ifndef USRTS_H_
#define USRTS_H_

void init_usrts();

int USRT_AT_Check();
uint8_t USRT_AT_Receive();
uint8_t USRT_AT_Transmit(uint8_t);
void USRT_AT_Flush();

int USRT_XT_Check();
uint8_t USRT_XT_Receive();
uint8_t USRT_XT_Transmit(uint8_t);
void USRT_XT_Flush();
#endif /* USRTS_H_ */