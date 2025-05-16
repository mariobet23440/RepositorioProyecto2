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

	// Configurar baud rate antes de activar transmisor/receptor
	UBRR0H = (103 >> 8);
	UBRR0L = 103;

	// Habilitar transmisor e interrupciones por data register vacío (UDRIE)
	UCSR0B = (1 << TXEN0) | (1 << UDRIE0);

	// Frame: 8 data bits, 1 stop bit, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
