/*
 * Proyecto2_Transmisor.c
 *
 * Fecha y Hora de creación: 8/05/2025 21:36:11
 * Autor : Mario Alejandro Betancourt Franco
 * Descripción: El siguiente código se sube al transmisor en el control del carro. 
 * Contiene instrucciones para la lectura de canales del ADC y transmisión de datos con UART
 */ 

// Frecuencia de reloj de sistema
#define F_CPU 16000000UL

// Librerías
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Libreria_ADC/LibreriaADC.h"
#include "Libreria_Timer0/LibreriaTimer0.h"
#include "Libreria_UART/Libreria_UART.h"

// Variables utilizadas
char current_channel = 0;	// Canal actual del ADC
char adc_value_chan0 = 0;	// Valor del canal 0 del ADC
char adc_value_chan1 = 0;	// Valor del canal 1 del ADC
char adc_value_chan2 = 0;	// Valor del canal 2 del ADC
char adc_value_chan3 = 0;	// Valor del canal 3 del ADC

// Prototipos de funciones
void DisplayInPORTBD(char data);
void send_data(char action);

// Setup
void setup(void)
{
	// Deshabilitar interrupciones globales
	cli();
	
	// Setup
	setup_adc();		// Inicializar ADC
	init_timer0();		// Inicializar Timer0
	UART_init();		// Inicializar UART
	
	// Solo para hacer ensayos, habilitar los últimos 4 bits de PORTD y los primeros 4 de PORTB
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5);
	DDRB = 0x0F;
	
	// Habilitar Interrupciones globales
	sei();
}

// Mainloop
int main(void)
{
	setup();
	UART_sendString("Si lees esto... ¡Comunicación Exitosa!\r\n");
		
    /* Replace with your application code */
    while (1) 
    {
    }
}

// Rutina No de Interrupción 1 - Protocolo de Comunicaciones
void send_data(char action)
{
	switch(action)
	{
		// 1: Enviar entrada X de motores DC
		case 1:
			UART_sendString("MX");
			UART_sendChar(adc_value_chan0);
			break;
		
		// 2: Enviar entrada Y de motores DC
		case 2:
			UART_sendString("MY");
			UART_sendChar(adc_value_chan1);
			break;
			
		// 3: Enviar entrada X de servomotores
		case 3:
			UART_sendString("SX");
			UART_sendChar(adc_value_chan2);
			break;
		
		// 4: Enviar entrada Y de servomotores
		case 4:
			UART_sendString("SY");
			UART_sendChar(adc_value_chan3);
			break;
	}
}

// Mostrar datos en PD2-PD5 y PB0-PB3 (SOLO TESTING LUEGO SACAR)
void DisplayInPORTBD(char data)
{
	// Limpiar los bits PD2 a PD5 de PORTD, sin afectar PD0 y PD1
	PORTD &= 0x03;  // 0x03 es 00000011, no afecta a PD2-PD5

	// Mostrar el nibble bajo de `data` (los 4 bits más bajos) en PORTD (PD2-PD5)
	PORTD |= (data & 0x0F) << 2;  // Escribir solo el nibble bajo (los 4 bits más bajos)

	// Mostrar el nibble alto de `data` (los 4 bits más altos) en PORTB (PB0-PB3)
	PORTB = (data & 0xF0) >> 4;  // Escribir el nibble alto en PB0-PB3
}


// Interrupción por conversión completa en el ADC - Lectura y Multiplexado de canales
ISR(ADC_vect)
{	
	// Hacer multiplexado de canal
	switch(current_channel)
	{
		case 0:
			adc_value_chan0 = ADCH;		// Guardar valor para canal 0
			current_channel = 1;
			adc_set_channel(1);			// Cambiar al canal 1
			break;
			
		case 1:
			adc_value_chan1 = ADCH;		// Guardar valor para canal 1
			current_channel = 2;
			adc_set_channel(2);			// Cambiar al canal 2
			break;
			
		case 2:
			adc_value_chan2 = ADCH;		// Guardar valor para canal 2
			current_channel = 3;
			adc_set_channel(3);			// Cambiar al canal 3
			break;
		
		case 3:
			adc_value_chan3 = ADCH;		// Guardar valor para canal 3
			current_channel = 0;
			adc_set_channel(0);			// Cambiar al canal 0
			break;
		
		default:
			adc_value_chan0 = ADCH;		// Guardar valor para canal 0
			current_channel = 0;
			adc_set_channel(0);			// En cualquier otro caso regresar al canal 0
			break;
	}
}