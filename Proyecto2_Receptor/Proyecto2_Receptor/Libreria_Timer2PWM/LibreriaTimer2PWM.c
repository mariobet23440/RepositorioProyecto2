/*
 * LibreriaTimer2PWM.c - LIBRERIA PWM CON TIMER2
 *
 * Created: 11/04/2025 18:44:56
 * Author: Mario Alejandro Betancourt Franco
 * Descripción: Librería para PWM en TIMER2. Permite usar OC2A y OC2B para controlar
 * intensidad de LEDs o potencia de motores DC (La frecuencia )
 */ 

#include "LibreriaTimer2PWM.h"
static uint8_t manual_pwm_pin = DDD3;

// Inicialización de TIMER2 - PWM Manual
void init_timer2(void)
{
	// Modo Normal, TOP = 255
	TCCR2B |= (1 << CS21); // Prescaler de 8
	
	// Habilitación de interrupciones por overflow de timer y output compare match
	TIMSK2 |= (1 << OCIE2A) | (1 << TOIE2);
	
	DDRD |= (1 << manual_pwm_pin); // Salida de PWM
}

// Establecer ancho de pulso en TIMER2
void TIMER2_PWMA_set_PW(uint8_t value)
{
	OCR2A = 255 - value;
}

// Establecer ancho de pulso en TIMER2
void TIMER2_PWMB_set_PW(uint8_t value)
{
	OCR2B = 255 - value;
}



