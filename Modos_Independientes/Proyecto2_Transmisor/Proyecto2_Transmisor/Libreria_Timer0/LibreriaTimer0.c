/*
 * LibreriaTimer0.c
 *
 * Created: 8/05/2025 22:09:26
 *  Author: mario
 */ 

#include "LibreriaTimer0.h"

// Inicialización de TIMER 0 - Disparo de conversión
void init_timer0(void)
{
	// Operamos el temporizador en modo NORMAL
	TCCR0B |= (1 << CS01) | (1 << CS00);	// Prescaler de 64
	TCNT0 = 0;
	
	// Habilitar interrupciones por overflow de TIMER0
	TIMSK0 |= (1 << TOIE0);
}

void reset_timer0(void)
{
	TCNT0 = 0;
}
