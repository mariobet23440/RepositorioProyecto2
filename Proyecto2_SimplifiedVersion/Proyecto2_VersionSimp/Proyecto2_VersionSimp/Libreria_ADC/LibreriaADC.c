/*
 * LibreriaADC.c
 *
 * Created: 11/04/2025 13:14:17
 *  Author: mario
 */ 

#include "LibreriaADC.h"

void setup_adc(void)
{
	ADMUX  = (1 << REFS0) | (1 << ADLAR);  // AVCC, resultado a la izquierda
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1);
	ADCSRB = 0x00;                         // Modo de Trigger Libre
	ADCSRA |= (1 << ADSC);                 // Iniciar la conversión
}


// Establecer canal en ADC
void adc_set_channel(unsigned char channel)
{
	ADMUX = (1 << ADLAR) | (1 << REFS0) | (channel & 0x0F); // Limpiar bits de canal y asignar nuevo canal
}
