/*
 * LibreriaTimer0PWM.c - LIBRERÍA PWM CON TIMER0
 *
 * Created: 21/05/2025
 * Author: Mario Alejandro Betancourt Franco
 * Descripción: Librería para PWM en TIMER0. Permite usar OC0A y OC0B para controlar
 * intensidad de LEDs o potencia de motores DC. Frecuencia ajustada con prescaler.
 */

#include "Libreria_Timer0PWM.h"

// Inicialización de TIMER0 - PWM
void init_timer0(void)
{
    // Modo Fast PWM, TOP = 255, salida no invertida en OC0A y OC0B
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (1 << CS01); // Prescaler de 8 (~7.8kHz con F_CPU = 16MHz)

    DDRD |= (1 << DDD6); // PD6 como salida (OC0A)
    DDRD |= (1 << DDD5); // PD5 como salida (OC0B)
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
