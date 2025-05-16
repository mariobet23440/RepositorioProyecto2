/*
 * Proyecto2_Transmisor.c
 *
 * Fecha y Hora de creaci�n: 8/05/2025 21:36:11
 * Autor : Mario Alejandro Betancourt Franco
 * Descripci�n: El siguiente c�digo se sube al transmisor en el control del carro. 
 * Contiene instrucciones para la lectura de canales del ADC y transmisi�n de datos con UART
 */ 

// -- CONSTANTES --
// Frecuencia de reloj de sistema
#define F_CPU 16000000UL

// Constantes para protocolo de comunicaciones
#define MOTORREDUCTOR_X  0x5555  // 0101 0101 0101 0101
#define MOTORREDUCTOR_Y  0xAAAA  // 1010 1010 1010 1010
#define SERVOMOTOR_X     0x3333  // 0011 0011 0011 0011
#define SERVOMOTOR_Y     0xCCCC  // 1100 1100 1100 1100
#define EEPROM_1         0x0F0F  // 0000 1111 0000 1111
#define EEPROM_2         0xF0F0  // 1111 0000 1111 0000
#define EEPROM_3         0x9999  // 1001 1001 1001 1001
#define EEPROM_4         0x6666  // 0110 0110 0110 0110

/*
NOTA: Aqu� una curiosidad. Estas constantes son n�meros de 16 bits, con distancias de Hamming de 8 bits m�nimo.
Esto facilita la correcci�n de errores en el receptor y dificulta malinterpretaciones por caracteres que se
parecen entre s�.
*/

// -- LIBRER�AS --
// Librer�as externas
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Librer�as Propias
#include "Libreria_ADC/LibreriaADC.h"
#include "Libreria_Timer1/Libreria_Timer1.h"
#include "Libreria_UART/Libreria_UART.h"

// -- VARIABLES --
char current_channel = 0;	// Canal actual del ADC
char adc_value_chan0 = 0;	// Valor del canal 0 del ADC
char adc_value_chan1 = 0;	// Valor del canal 1 del ADC
char adc_value_chan2 = 0;	// Valor del canal 2 del ADC
char adc_value_chan3 = 0;	// Valor del canal 3 del ADC


// -- PROTOTIPOS DE FUNCIONES --
void send_formated_data(uint16_t label, char data);


// -- SETUP --
void setup(void)
{
	// Deshabilitar interrupciones globales
	cli();
	
	// Setup
	setup_adc();		// Inicializar ADC
	//init_timer0();		// Inicializar Timer0
	UART_init();		// Inicializar UART
	
	// Habilitar Interrupciones globales
	sei();
}

// -- MAINLOOP --
int main(void)
{
	setup();
		
    // Replace with your application code
    while (1) 
    {
		send_formated_data(MOTORREDUCTOR_X, adc_value_chan0);
		send_formated_data(MOTORREDUCTOR_Y, adc_value_chan1);
		send_formated_data(SERVOMOTOR_X, adc_value_chan2);
		send_formated_data(SERVOMOTOR_Y, adc_value_chan3);
    }
}

// -- RUTINAS NO DE INTERRUPC��N -- 
// Env�o de datos formateados
void send_formated_data(uint16_t label, char data)
{
	// Enviar byte alto del label
	UART_sendChar((label >> 8) & 0xFF);
	// Enviar byte bajo del label
	UART_sendChar(label & 0xFF);
	// Enviar el dato
	UART_sendChar(data);
}


// -- RUTINAS DE INTERRUPCI�N -- 
// Interrupci�n por conversi�n completa en el ADC - Lectura y Multiplexado de canales
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