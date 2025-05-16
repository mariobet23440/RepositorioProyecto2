/*
* PROYECTO 2 RECEPTOR - PROGRAMACI�N DE MICROCONTROLADORES
* FECHA DE CREACI�N: 3/05/25 19:35:08
* FECHA DE ENTREGA: 17/05/25 16:30:00
* AUTOR: Mario Alejandro Betancourt Franco (Universidad del Valle de Guatemala, 23440)
* MICROCONTROLADOR: ATMega328P (En un Arduino Nano)
* DESCRIPCI�N: Programa para controlar un carro a control remoto. El programa ofrece soporte para el control de
* dos motorreductores en las ruedas y dos servomotores a trav�s de comandos recibidos por UART. 
*/

/*
PROTOCOLO DE COMUNICACIONES
Las instrucciones se transmiten en un formato como el siguiente
[START] - [INS BYTE 1] - [INS BYTE 2] - [DATA] - [END]
Los caracteres de instrucci�n, inicio y fin fueron escogidos para que la *Distancia de Hamming* entre ellos
(El n�mero de bits distintos entre un par de caracteres) sea la m�xima posible. Al usar dos caracteres de
instrucci�n y caracteres de inicio y fin complementarios, es posible realizar un protocolo de comunicaciones
m�s robusto y dificil de malinterpretar.
*/

/************************************************************************/
/* CONSTANTES                                                           */
/************************************************************************/
#define F_CPU 16000000UL  // Frecuencia de microcontrolador

// Caracteres para framing de recepci�n de datos
#define RXTX_START			0xA5    // Marca de inicio de transmisi�n (�)
#define RXTX_END			0x5A    // Complemento con m�xima distancia de Hamming (Z)

// Constantes para pares de caracteres de instrucci�n
#define MOTORREDUCTOR_X		0x5555  // 0101 0101 0101 0101
#define MOTORREDUCTOR_Y		0xAAAA  // 1010 1010 1010 1010
#define SERVOMOTOR_X		0x3333  // 0011 0011 0011 0011
#define SERVOMOTOR_Y		0xCCCC  // 1100 1100 1100 1100
#define EEPROM_1			0x0F0F  // 0000 1111 0000 1111
#define EEPROM_2			0xF0F0  // 1111 0000 1111 0000
#define EEPROM_3			0x9999  // 1001 1001 1001 1001
#define EEPROM_4			0x6666  // 0110 0110 0110 0110

/************************************************************************/
/* LIBRER�AS                                                            */
/************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "Libreria_Timer1PWM/LibreriaTimer1PWM.h"
#include "Libreria_Timer2PWM/LibreriaTimer2PWM.h"
#include "Libreria_UART/Libreria_UART.h"

/************************************************************************/
/* VARIABLES GLOBALES                                                   */
/************************************************************************/
volatile char reception_started = 0;		// Bandera de inicio de framing
volatile char ins_byte1_received = 0;		// Bandera de recepci�n del byte 1 de instrucci�n
volatile char ins_byte2_received = 0;		// Bandera de recepci�n del byte 2 de instrucci�n
volatile char data_byte_received = 0;		// Bandera de recepci�n del byte de datos
volatile char reception_ended = 1;			// Bandera de final de framing

// Arreglo de datos recibidos
volatile uint8_t received_data[3] = {0xA5, 0xCC, 0xFF};  // Ejemplo de 3 bytes hexadecimales

/************************************************************************/
/* SETUP Y MAINLOOP                                                                     */
/************************************************************************/
void setup(void)
{
	UART_init();
	//init_timer1();
	//init_timer2();
	sei();  // Habilitar interrupciones globales
}

int main(void)
{
	setup();
	printf("Tilin \r\n");
	while(1);
	return 0;
}

/************************************************************************/
/* INTERRUPCI�N POR RECEPCI�N UART                                      */
/************************************************************************/
ISR(USART_RX_vect)
{
	// Guardar caracter (Y limpiar el buffer)
	char data = UDR0;
	UART_sendChar(data);
	
	// Inicializar framing (Lo que tiene m�s protecci�n)
	if ((data == RXTX_START) && (!reception_started) && (reception_ended))
	{
		// Bajar la bandera de fin de framing
		reception_ended = 0;
		
		// Levantar la bandera de inicio de framing
		reception_started = 1;
		
		// Limpiar todos los valores del arreglo
		for (uint8_t i = 0; i < 3; i++) {
			received_data[i] = 0;
		}
		
		UART_sendChar('1');
	}
	
	// Recepci�n del primer byte de instrucci�n
	else if(reception_started)
	{
		// Bajar la bandera de inicio de framing
		reception_started = 0;
		
		// Guardar el dato en la posici�n 0 del arreglo
		received_data[0] = data;
		
		// Levantar la bandera de recepci�n del primer byte de instrucci�n 
		ins_byte1_received = 1;
		
		UART_sendChar('2');
	}
	
	// Recepci�n del segundo byte de instrucci�n
	else if(ins_byte1_received)
	{
		// Bajar la bandera de recepci�n del primer byte de instrucci�n
		ins_byte1_received = 0;
		
		// Guardar el dato en la posici�n 1 del arreglo
		received_data[1] = data;
		
		// Levantar la bandera de recepci�n del segundo byte de instrucci�n
		ins_byte2_received = 1;
		
		UART_sendChar('3');
	}
	
	// Recepci�n de caracter de dato
	else if(ins_byte2_received)
	{
		// Bajar la bandera de recepci�n del segundo byte de instrucci�n
		ins_byte2_received = 0;
		
		// Guardar el byte en la posici�n 2 del arreglo
		received_data[2] = data;
		
		// Levantar la bandera de caracter de dato recibido
		data_byte_received = 1;
		
		UART_sendChar('4');
	}
	
	// Fin de transmisi�n
	else if((data == RXTX_END) && (data_byte_received))
	{
		// Bajar la bandera de recepci�n del byte de dato
		data_byte_received = 0;
		
		// Levantar la bandera de recepci�n terminada
		reception_ended = 1;
		
		UART_sendChar('5');
	}
	else
	{
		UART_sendChar('6');
	}
	
}