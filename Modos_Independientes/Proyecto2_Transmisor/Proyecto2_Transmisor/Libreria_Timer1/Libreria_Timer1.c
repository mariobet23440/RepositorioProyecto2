/*
 * LibreriaTimer1PWM.c - LIBRERIA PWM CON TIMER1 PARA ADC
 *
 * Creada: 11/04/2025 18:32:51
 * Autor: Mario Alejandro Betancourt Franco
 * Descripci�n: Librer�a para PWM en TIMER1. Se utiliza para disparar conversiones en el m�dulo ADC
 * sin interferir con la comunicaci�n UART.
 */ 

#include "Libreria_Timer1.h"

// Inicializaci�n de Timer1
void init_timer1(void)
{
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Fast PWM, OC1A y OC1B no invertido
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Prescaler de 8, Fast PWM
	ICR1 = 39999;							// Establecer TOP a 39999 (Frecuencia de 50 Hz, 20ms)
	DDRB |= (1 << DDB1) | (1 << DDB2);		// Configurar PB1 y PB2 como salida (OC1A y OC1B)
}
