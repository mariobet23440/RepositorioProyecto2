/*
* PROYECTO 2 RECEPTOR - PROGRAMACI�N DE MICROCONTROLADORES
* FECHA DE CREACI�N: 3/05/25 19:35:08
* FECHA DE ENTREGA: 17/05/25 16:30:00
* AUTOR: Mario Alejandro Betancourt Franco (Universidad del Valle de Guatemala, 23440)
* MICROCONTROLADOR: ATMega328P (En un Arduino Uno)
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
#define MOTORREDUCTOR_X		0x5555  // 0101 0101 0101 0101 (En ascii es 77)
#define MOTORREDUCTOR_Y		0xAAAA  // 1010 1010 1010 1010
#define SERVOMOTOR_X		0x3333  // 0011 0011 0011 0011
#define SERVOMOTOR_Y		0xCCCC  // 1100 1100 1100 1100
#define EEPROM_1			0x0F0F  // 0000 1111 0000 1111
#define EEPROM_2			0xF0F0  // 1111 0000 1111 0000
#define EEPROM_3			0x9999  // 1001 1001 1001 1001
#define EEPROM_4			0x6666  // 0110 0110 0110 0110

// SAMPLE - �77AZ (MOTORREDUCTOR X - A)

/************************************************************************/
/* LIBRER�AS                                                            */
/************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "Libreria_Timer0PWM/Libreria_Timer0PWM.h"
#include "Libreria_Timer1PWM/LibreriaTimer1PWM.h"
#include "Libreria_UART/Libreria_UART.h"
#include "Libreria_HBridge/Libreria_HBridge.h"


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
	
// Arreglo de constantes
const uint16_t instrucciones[] = {
	MOTORREDUCTOR_X,
	MOTORREDUCTOR_Y,
	SERVOMOTOR_X,
	SERVOMOTOR_Y,
	EEPROM_1,
	EEPROM_2,
	EEPROM_3,
	EEPROM_4
};

// Nombres de Instrucciones
const char* const nombres_instrucciones[] = {
	"MOTORREDUCTOR_X",
	"MOTORREDUCTOR_Y",
	"SERVOMOTOR_X",
	"SERVOMOTOR_Y",
	"EEPROM_1",
	"EEPROM_2",
	"EEPROM_3",
	"EEPROM_4"
};

// Tama�o del arreglo
const size_t num_instrucciones = sizeof(instrucciones) / sizeof(instrucciones[0]);

/************************************************************************/
/* PROTOTIPOS DE FUNCIONES                                              */
/************************************************************************/
void setup(void);
void process_instruction(void);
void show_instruction_ASCII(void);
int8_t normalize_input(char valor);
void move_differential(int8_t traslacion, int8_t rotacion);


/************************************************************************/
/* SETUP Y MAINLOOP                                                     */
/************************************************************************/
void setup(void)
{
	init_timer1();
	init_timer0();
	UART_init();
	sei();  // Habilitar interrupciones globales
}

int main(void)
{
	setup();
	UART_sendString("PROGRAMACI�N DE MICROCONTROLADORES - PROYECTO 2 - RECEPTOR \r\n");
	show_instruction_ASCII();
	PORTB |= (1 << PORTB1);
	while(1);
	return 0;
}

/************************************************************************/
/* RUTINAS NO DE INTERRUPCI�N                                           */
/************************************************************************/
// Procesar Instrucciones (A partir del arreglo de datos recibido)
void process_instruction(void)
{
	// N�mero de 16 bits de caracteres de instrucci�n 
	uint16_t	instruction = ((uint16_t)received_data[0] << 8) | received_data[1];
	
	// Caracter de datos 
	char		data_char = received_data[2];
	
	// Logs de prueba para monitor serial
	UART_sendString("Instrucci�n recibida: ");
	
	switch(instruction)
	{
		case MOTORREDUCTOR_X:
		UART_sendString("MOTORREDUCTOR X - ");		
		TIMER0_PWMA_set_PW(data_char);
		break;
		
		case MOTORREDUCTOR_Y:
		UART_sendString("MOTORREDUCTOR Y - ");
		TIMER0_PWMB_set_PW(data_char);
		break;
		
		case SERVOMOTOR_X:
		UART_sendString("SERVOMOTOR X - ");
		TIMER1_PWMA_set_servo_PW(data_char);
		break;
		
		case SERVOMOTOR_Y:
		UART_sendString("SERVOMOTOR Y - ");
		TIMER1_PWMB_set_servo_PW(data_char);
		break;
		
		default:
		UART_sendString("INSTRUCCI�N INV�LIDA - ");
		break;
	}
	
	UART_sendChar(data_char);
	UART_sendString("\r\n");
	UART_sendString("\r\n");
}

// Mostrar ASCII de instrucciones
void show_instruction_ASCII(void)
{
	UART_sendString("INSTRUCCIONES (EN ASCII) \r\n");
	UART_sendString("Para ejecutar las siguientes instrucciones, coloque el caracter mostrado dos veces \r\n");
	for (size_t i = 0; i < num_instrucciones; ++i) {
		UART_sendString(nombres_instrucciones[i]);		// Mostrar nombre de instrucci�n
		UART_sendString(" : ");							// Espacio
		UART_sendChar(instrucciones[i] >> 8);			// Mostrar instrucci�n
		UART_sendString("\r\n");
	}
	UART_sendString("\r\n");
	UART_sendString("Para iniciar y terminar frames de 3 bytes \r\n");
	UART_sendString("Inicio: � \r\n");
	UART_sendString("Fin: Z \r\n");
	UART_sendString("\r\n");
	UART_sendString("Ingrese un caracter para accionar el sistema. \r\n");
}

// Normalizar entrada (0 a 255 -> -128 a 127)
int8_t normalize_input(char valor)
{
	return ((int16_t)valor - 128); // Convierte 0�255 en -128 a 127
}

// Movimiento de ruedas diferenciales - Permite traslaci�n y rotaci�n con entradas anal�gicas
void move_differential(int8_t traslacion, int8_t rotacion)
{
	int16_t vel_izq = traslacion - rotacion;
	int16_t vel_der = traslacion + rotacion;

	// Saturaci�n
	if (vel_izq > 255) vel_izq = 255;
	if (vel_izq < -255) vel_izq = -255;
	if (vel_der > 255) vel_der = 255;
	if (vel_der < -255) vel_der = -255;

	// Motor izquierdo
	if (vel_izq >= 0) {
		motorA_forward();
		TIMER0_PWMA_set_PW((uint8_t)vel_izq);
		} else {
		motorA_backward();
		TIMER0_PWMA_set_PW((uint8_t)(-vel_izq));
	}

	// Motor derecho
	if (vel_der >= 0) {
		motorB_forward();
		TIMER0_PWMB_set_PW((uint8_t)vel_der);
		} else {
		motorB_backward();
		TIMER0_PWMB_set_PW((uint8_t)(-vel_der));
	}
}



/************************************************************************/
/* INTERRUPCI�N POR RECEPCI�N UART                                      */
/************************************************************************/
ISR(USART_RX_vect)
{
	// Guardar caracter (Y limpiar el buffer)
	char data = UDR0;
	//UART_sendChar(data);
	
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
		
		// Indicador de prueba
		UART_sendString("Marca de Inicio de Frame: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
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
		
		// Indicador de prueba
		UART_sendString("Primer Caracter de Instrucci�n Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
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
		
		// Indicador de prueba
		UART_sendString("Segundo Caracter de Instrucci�n Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
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
		
		// Indicador de prueba
		UART_sendString("Caracter de Datos Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
	
	// Fin de transmisi�n
	else if((data == RXTX_END) && (data_byte_received))
	{
		// Bajar la bandera de recepci�n del byte de dato
		data_byte_received = 0;
		
		// Levantar la bandera de recepci�n terminada
		reception_ended = 1;
		
		// Indicador de prueba
		UART_sendString("Fin de Frame de Informaci�n: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
		UART_sendString("Arreglo de Datos: [");
		UART_sendChar(received_data[0]);
		UART_sendString(", ");
		UART_sendChar(received_data[1]);
		UART_sendString(", ");
		UART_sendChar(received_data[2]);
		UART_sendString("] ");
		UART_sendString("\r\n");
		
		// PROCESAR INSTRUCCI�N!!!!
		process_instruction();
	}
	else
	{
		// Indicador de prueba
		UART_sendString("Caracter Inv�lido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
	
}