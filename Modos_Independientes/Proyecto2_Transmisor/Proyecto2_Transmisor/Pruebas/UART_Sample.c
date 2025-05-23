/*
 * CFile1.c
 *
 * Created: 15/05/2025 14:46:14
 *  Author: mario
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void UART_init(void) {
	UBRR0H = (103 >> 8);
	UBRR0L = 103;

	UCSR0B = (1 << TXEN0); // Habilitar transmisi�n
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos, sin paridad, 1 bit de stop
}

void UART_sendChar(char c) {
	// Parpadeo r�pido para indicar que estamos esperando buffer
	while (!(UCSR0A & (1 << UDRE0))) {
		PORTB ^= (1 << PB0); // LED parpadeando r�pido
		_delay_ms(100);
	}

	// Cuando el buffer est� libre, enviamos el car�cter
	UDR0 = c;
	// Parpadeo lento indicando que el dato fue enviado
	PORTB &= ~(1 << PB0);
	_delay_ms(500); // Parpadeo lento
	PORTB |= (1 << PB0);
	_delay_ms(500); // Parpadeo lento
}

void UART_sendString(const char* str) {
	while (*str) {
		UART_sendChar(*str++);
	}
}

int main(void) {
	// Configura PB0 como salida para LED de prueba
	DDRB |= (1 << PB0);

	// Indicamos que el c�digo lleg� al principio
	PORTB |= (1 << PB0); // Enciende LED al principio
	_delay_ms(1000); // Espera un segundo
	PORTB &= ~(1 << PB0); // Apaga LED

	// Inicializa UART
	UART_init();

	// Env�a una cadena de prueba
	UART_sendString("A");

	// Si UART est� funcionando correctamente, el LED parpadear� lento
	while (1) {
		// El LED se mantendr� prendido o parpadeando lentamente
		PORTB ^= (1 << PB0);
		_delay_ms(1000); // Parpadeo cada 1 segundo
	}
}
