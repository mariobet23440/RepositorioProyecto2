/*
 * Libreria_UART.c
 *
 * Created: 29/04/2025 20:56:55
 *  Author: mario
 */ 


#include "Libreria_UART.h"

// Inicialización UART a 9600 baudios
void UART_init(void) {
	// Configura TX como salida, RX como entrada
	DDRD |= (1 << PD1);   // TX
	DDRD &= ~(1 << PD0);  // RX

	// Configura baud rate a 9600 (F_CPU = 16 MHz)
	// UBRR = (F_CPU / (16 * BAUD)) - 1 = 103
	UBRR0 = 103;

	// Habilita recepción, transmisión e interrupción de recepción
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8 bits, 1 bit de stop, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Enviar un carácter
void UART_sendChar(char c) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera buffer libre
	UDR0 = c;
}

// Enviar una cadena de texto
void UART_sendString(const char* str) {
	while (*str)
	{
		UART_sendChar(*str++);
	}
}