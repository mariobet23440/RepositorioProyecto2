/*
 * Libreria_UART.h
 *
 * Created: 29/04/2025 20:57:21
 *  Author: mario
 */ 


#ifndef LIBRERIA_UART_H_
#define LIBRERIA_UART_H_


#include <avr/io.h>
void UART_init(void) ;
void UART_sendChar(char c);
void UART_sendString(const char* str);



#endif /* LIBRERIA_UART_H_ */