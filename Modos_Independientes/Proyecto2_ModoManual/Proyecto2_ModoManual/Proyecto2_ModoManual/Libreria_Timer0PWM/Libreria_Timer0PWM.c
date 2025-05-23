/*
 * LibreriaTimer0PWM.c - LIBRERÍA PWM CON TIMER0
 *
 * Created: 21/05/2025
 * Author: Mario Alejandro Betancourt Franco
 * Descripción: Librería para PWM en TIMER0. Permite usar OC0A y OC0B para controlar
 * intensidad de LEDs o potencia de motores DC. Frecuencia ajustada con prescaler.
 */

#include "Libreria_Timer0PWM.h"

// INICIALIZACIÓN DE TIMER0 - FASTPWM
void init_timer0(void)
{
	// Fast PWM, OC0A y OC0B no invertidos
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);

	// Limpiar bits del prescaler
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));

	// Establecer prescaler = 128 (no está directamente soportado por hardware)
	// Usamos 256 mejor (estándar en AVR), para obtener 244 Hz
	TCCR0B |= (1 << CS02); // prescaler = 256

	// Salidas PWM
	DDRD |= (1 << DDD6); // OC0A - Motor A
	DDRD |= (1 << DDD5); // OC0B - Motor B
}


// Establecer ancho de pulso en OC0A
void TIMER0_PWMA_set_PW(uint8_t value)
{
    OCR0A = 255 - value;
}

// Establecer ancho de pulso en OC0B
void TIMER0_PWMB_set_PW(uint8_t value)
{
    OCR0B = 255 - value;
}
