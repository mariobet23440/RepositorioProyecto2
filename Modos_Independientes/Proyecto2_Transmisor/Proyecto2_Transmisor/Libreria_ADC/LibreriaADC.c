/*
 * LibreriaADC.c
 *
 * Created: 11/04/2025 13:14:17
 *  Author: mario
 */ 

#include "LibreriaADC.h"

void setup_adc(void)
{
	// Formato de datos y voltaje de referencia
	ADMUX |= (1 << ADLAR) | (1 << REFS0);   // Ajuste a la izquierda (ADLAR), Referencia AVCC (REFS0)
	
	// Habilitación de ADC e interrupciones por conversión completada
	ADCSRA |= (1 << ADEN) | (1 << ADIE);    // Habilitar ADC y habilitar interrupción ADC
	
	// Prescaler
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Prescaler de 64
	
	// Configuraciones para Auto Trigger
	ADCSRA |= (1 << ADATE);					// Habilitar Auto Trigger
	ADCSRB |= (1 << ADTS2) | (1 << ADTS1);	// Trigger Source Timer/Counter0 Overflow
	
	// Iniciar conversión
	ADCSRA |= (1 << ADSC);
}

// Establecer canal en ADC
void adc_set_channel(unsigned char channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Limpiar bits de canal y asignar nuevo canal
}
