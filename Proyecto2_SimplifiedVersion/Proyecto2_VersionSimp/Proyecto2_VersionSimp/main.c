/*
 * Proyecto2_VersionSimp.c
 *
 * Created: 21/05/2025 10:39:13
 * Author : mario
 */ 

/*
PROTOCOLO DE COMUNICACIONES
Las instrucciones se transmiten en un formato como el siguiente
[START] - [INS BYTE 1] - [INS BYTE 2] - [DATA] - [END]
Los caracteres de instrucción, inicio y fin fueron escogidos para que la *Distancia de Hamming* entre ellos
(El número de bits distintos entre un par de caracteres) sea la máxima posible. Al usar dos caracteres de
instrucción y caracteres de inicio y fin complementarios, es posible realizar un protocolo de comunicaciones
más robusto y dificil de malinterpretar.
*/

/************************************************************************/
/* LIBRERIAS EXTERNAS                                                   */
/************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

/************************************************************************/
/* LIBRERIAS PROPIAS (INCLUDES)                                         */
/************************************************************************/
#include "Libreria_Timer0PWM/Libreria_Timer0PWM.h"
#include "Libreria_Timer1PWM/LibreriaTimer1PWM.h"
#include "Libreria_UART/Libreria_UART.h"
#include "Libreria_HBridge/Libreria_HBridge.h"
#include "Libreria_ADC/LibreriaADC.h"
#include "Libreria_EEPROM/Libreria_EEPROM.h"

/************************************************************************/
/* CONSTANTES		                                                    */
/************************************************************************/
// Caracteres para framing de recepción de datos
#define RXTX_START			0xA5    // Marca de inicio de transmisión (¥)
#define RXTX_END			0x5A    // Complemento con máxima distancia de Hamming (Z)

// Constantes para pares de caracteres de instrucción
#define MOTORREDUCTOR_X		0x5555  // 0101 0101 0101 0101 (En ascii es 77)
#define MOTORREDUCTOR_Y		0xAAAA  // 1010 1010 1010 1010
#define SERVOMOTOR_X		0x3333  // 0011 0011 0011 0011
#define SERVOMOTOR_Y		0xCCCC  // 1100 1100 1100 1100
#define EEPROM_READ			0x0F0F  // 0000 1111 0000 1111
#define EEPROM_WRITE		0xF0F0  // 1111 0000 1111 0000
#define MANUAL_ENABLE		0x9999  // 1001 1001 1001 1001
#define MANUAL_DISABLE		0x6666  // 0110 0110 0110 0110

// Constante de punto medio
#define MIDPOINT			0x7F	// 255/2 (Valor medio)

// Direcciones en EEPROM
#define EEPROM_ADDRESS1A	0X0000	// Dirección 1A
#define EEPROM_ADDRESS1B	0X0001	// Dirección 1B
#define EEPROM_ADDRESS2A	0X0002	// Dirección 1A
#define EEPROM_ADDRESS2B	0X0003	// Dirección 1B
#define EEPROM_ADDRESS3A	0X0004	// Dirección 1A
#define EEPROM_ADDRESS3B	0X0005	// Dirección 1B
#define EEPROM_ADDRESS4A	0X0006	// Dirección 1A
#define EEPROM_ADDRESS4B	0X0007	// Dirección 1B

// Pines para LEDs indicadores de modos
#define MANUAL_MODE_LED		PORTD3
#define UART_MODE_LED		PORTD4
#define EEPROM_MODE_LED		PORTD5

/************************************************************************/
/* VARIABLES GLOBALES                                                   */
/************************************************************************/

// VARIABLES PARA ADC
// -- VARIABLES --
char current_channel = 0;	// Canal actual del ADC
char adc_value_chan0 = 0;	// Valor del canal 0 del ADC
char adc_value_chan1 = 0;	// Valor del canal 1 del ADC
char adc_value_chan2 = 0;	// Valor del canal 2 del ADC
char adc_value_chan3 = 0;	// Valor del canal 3 del ADC

// MODOS OPERACIONALES
char manual_mode_enabled = 0;	// Modo manual activado

// RECEPCIÓN DE DATOS 
volatile char reception_started = 0;		// Bandera de inicio de framing
volatile char ins_byte1_received = 0;		// Bandera de recepción del byte 1 de instrucción
volatile char ins_byte2_received = 0;		// Bandera de recepción del byte 2 de instrucción
volatile char data_byte_received = 0;		// Bandera de recepción del byte de datos
volatile char reception_ended = 1;			// Bandera de final de framing

// Arreglo de datos recibidos
volatile uint8_t received_data[3] = {0xA5, 0xCC, 0xFF};  // Ejemplo de 3 bytes hexadecimales

// Arreglo de constantes
const uint16_t instrucciones[] = {
	MOTORREDUCTOR_X,
	MOTORREDUCTOR_Y,
	SERVOMOTOR_X,
	SERVOMOTOR_Y,
	EEPROM_READ,
	EEPROM_WRITE,
	MANUAL_ENABLE,
	MANUAL_DISABLE
};

// Nombres de Instrucciones
const char* const nombres_instrucciones[] = {
	"MOTORREDUCTOR_X",
	"MOTORREDUCTOR_Y",
	"SERVOMOTOR_X",
	"SERVOMOTOR_Y",
	"EEPROM_READ",
	"EEPROM_WRITE",
	"MANUAL_ENABLE",
	"MANUAL_DISABLE"
};

// Tamaño del arreglo
const size_t num_instrucciones = sizeof(instrucciones) / sizeof(instrucciones[0]);

/************************************************************************/
/* PROTOTIPOS DE FUNCIONES                                              */
/************************************************************************/
void process_instruction_uart(void);
void show_instruction_ASCII(void);
int8_t normalize_input(char valor);
void move_differential(int8_t traslacion, int8_t rotacion);
void manual_mode_movement(char Mx, char My, char Sx, char Sy);


/************************************************************************/
/* SETUP Y MAINLOOP                                                     */
/************************************************************************/
void setup(void)
{
	// Inicializar cosas en librerías
	init_timer0();			// Timer0
	init_timer1();			// Timer1
	init_HBridgePins();		// Puente H
	UART_init();			// UART
	setup_adc();			// ADC
}

int main(void)
{
    setup();
    UART_sendString("PROGRAMACIÓN DE MICROCONTROLADORES - PROYECTO 2 - RECEPTOR \r\n");
    show_instruction_ASCII();
    PORTB |= (1 << PORTB1);
    while(1)
	{
		if(manual_mode_enabled)
		{
			manual_mode_movement(adc_value_chan0, adc_value_chan1, adc_value_chan2, adc_value_chan3);
		}
	}
	
    return 0;
}

/************************************************************************/
/* RUTINAS NO DE INTERRUPCIÓN                                           */
/************************************************************************/
void manual_mode_movement(char Mx, char My, char Sx, char Sy)
{
	// Mover servomotores
	TIMER1_PWMA_set_servo_PW(Sx);
	TIMER1_PWMB_set_servo_PW(Sy);
	
	// Mover motorreductores (ruedas diferenciales)
	int8_t traslacion = normalize_input(Mx);
	int8_t rotacion   = normalize_input(My);
	move_differential(traslacion, rotacion);
}


// Procesar Instrucciones (A partir del arreglo de datos recibido) - SOLO CUANDO MODO UART ESTÁ ACTIVADO!!!
// Esta rutina es exclusiva para ISR(USART_RX_vect) y no se usa para ninguna otra parte del código
void process_instruction_uart(void)
{
	// Número de 16 bits de caracteres de instrucción
	uint16_t	instruction = ((uint16_t)received_data[0] << 8) | received_data[1];
	
	// Caracter de datos
	char		data_char = received_data[2];
	
	// Logs de prueba para monitor serial
	UART_sendString("Instrucción recibida: ");
	
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
		
		// LECTURA DE EEPROM - Mostrar Posiciones en Servomotores
		case EEPROM_READ:
		UART_sendString("EEPROM READ - ");
		
		char eeprom_dataA = 0;
		char eeprom_dataB = 0;
		
		// Leemos dos posiciones dependiendo del valor de la entrada
		switch(data_char)
		{
			case 1:
			UART_sendString("Leyendo datos en dirección No. 1");
			eeprom_dataA = EEPROM_read(EEPROM_ADDRESS1A);
			eeprom_dataB = EEPROM_read(EEPROM_ADDRESS1B);
			break;
			
			case 2:
			UART_sendString("Leyendo datos en dirección No. 2");
			eeprom_dataA = EEPROM_read(EEPROM_ADDRESS2A);
			eeprom_dataB = EEPROM_read(EEPROM_ADDRESS2B);
			break;
			
			case 3:
			UART_sendString("Leyendo datos en dirección No. 3");
			eeprom_dataA = EEPROM_read(EEPROM_ADDRESS3A);
			eeprom_dataB = EEPROM_read(EEPROM_ADDRESS3B);
			break;
			
			case 4:
			UART_sendString("Leyendo datos en dirección No. 4");
			eeprom_dataA = EEPROM_read(EEPROM_ADDRESS4A);
			eeprom_dataB = EEPROM_read(EEPROM_ADDRESS4B);
			break;
			
			default:
			UART_sendString("Dirección Inválida");
			break;
		}
		
		UART_sendString("\r\n");
		UART_sendString("Datos Recibidos: ");
		UART_sendChar(eeprom_dataA);
		UART_sendString(" - ");
		UART_sendChar(eeprom_dataB);
		UART_sendString("\r\n");
		
		// Mostramos las posiciones en los servomotores
		TIMER1_PWMA_set_servo_PW(eeprom_dataA);
		TIMER1_PWMB_set_servo_PW(eeprom_dataB);
		break;
		
		// Escribir en EEPROM
		case EEPROM_WRITE:
		UART_sendString("EEPROM WRITE - ");
		
		
		switch(data_char)
		{
			case 1:
			UART_sendString("Guardando datos en dirección No. 1");
			EEPROM_write(EEPROM_ADDRESS1A, adc_value_chan0);
			EEPROM_write(EEPROM_ADDRESS1B, adc_value_chan1);
			break;
			
			case 2:
			UART_sendString("Guardando datos en dirección No. 2");
			EEPROM_write(EEPROM_ADDRESS2A, adc_value_chan0);
			EEPROM_write(EEPROM_ADDRESS2B, adc_value_chan1);
			break;
			
			case 3:
			UART_sendString("Guardando datos en dirección No. 3");
			EEPROM_write(EEPROM_ADDRESS3A, adc_value_chan0);
			EEPROM_write(EEPROM_ADDRESS3B, adc_value_chan1);
			break;
			
			case 4:
			UART_sendString("Guardando datos en dirección No. 4");
			EEPROM_write(EEPROM_ADDRESS4A, adc_value_chan0);
			EEPROM_write(EEPROM_ADDRESS4B, adc_value_chan1);
			break;
			
			default:
			UART_sendString("Dirección Inválida");
		}
		
		UART_sendString("\r\n");
		UART_sendString("Datos Escritos: ");
		UART_sendChar(adc_value_chan0);
		UART_sendString(" - ");
		UART_sendChar(adc_value_chan1);
		UART_sendString("\r\n");
		break;
		
		// Habilitar y deshabilitar modo manual
		case MANUAL_ENABLE:
		UART_sendString("MANUAL ENABLE - Habilitando modo manual");
		manual_mode_enabled = 1;
		break;
		
		case MANUAL_DISABLE:
		UART_sendString("MANUAL ENABLE - Deshabilitando modo manual");
		manual_mode_enabled = 0;
		break;
		
		default:
		UART_sendString("INSTRUCCIÓN INVÁLIDA - ");
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
		UART_sendString(nombres_instrucciones[i]);		// Mostrar nombre de instrucción
		UART_sendString(" : ");							// Espacio
		UART_sendChar(instrucciones[i] >> 8);			// Mostrar instrucción
		UART_sendString("\r\n");
	}
	UART_sendString("\r\n");
	UART_sendString("Para iniciar y terminar frames de 3 bytes \r\n");
	UART_sendString("Inicio: ¥ \r\n");
	UART_sendString("Fin: Z \r\n");
	UART_sendString("\r\n");
	UART_sendString("Ingrese un caracter para accionar el sistema. \r\n");
}

// Normalizar entrada (0 a 255 -> -128 a 127)
int8_t normalize_input(char valor)
{
	return ((int16_t)valor - 128); // Convierte 0–255 en -128 a 127
}

// Movimiento de ruedas diferenciales - Permite traslación y rotación con entradas analógicas
void move_differential(int8_t traslacion, int8_t rotacion)
{
	int16_t vel_izq = traslacion - rotacion;
	int16_t vel_der = traslacion + rotacion;

	// Saturación
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
/* RUTINAS DE INTERRUPCIÓN                                              */
/************************************************************************/
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


// RECEPCIÓN DE DATOS EN UART
ISR(USART_RX_vect)
{
	// Guardar caracter (Y limpiar el buffer)
	char data = UDR0;
	//UART_sendChar(data);
	
	// Inicializar framing (Lo que tiene más protección)
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
	
	// Recepción del primer byte de instrucción
	else if(reception_started)
	{
		// Bajar la bandera de inicio de framing
		reception_started = 0;
		
		// Guardar el dato en la posición 0 del arreglo
		received_data[0] = data;
		
		// Levantar la bandera de recepción del primer byte de instrucción
		ins_byte1_received = 1;
		
		// Indicador de prueba
		UART_sendString("Primer Caracter de Instrucción Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
	
	// Recepción del segundo byte de instrucción
	else if(ins_byte1_received)
	{
		// Bajar la bandera de recepción del primer byte de instrucción
		ins_byte1_received = 0;
		
		// Guardar el dato en la posición 1 del arreglo
		received_data[1] = data;
		
		// Levantar la bandera de recepción del segundo byte de instrucción
		ins_byte2_received = 1;
		
		// Indicador de prueba
		UART_sendString("Segundo Caracter de Instrucción Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
	
	// Recepción de caracter de dato
	else if(ins_byte2_received)
	{
		// Bajar la bandera de recepción del segundo byte de instrucción
		ins_byte2_received = 0;
		
		// Guardar el byte en la posición 2 del arreglo
		received_data[2] = data;
		
		// Levantar la bandera de caracter de dato recibido
		data_byte_received = 1;
		
		// Indicador de prueba
		UART_sendString("Caracter de Datos Recibido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
	
	// Fin de transmisión
	else if((data == RXTX_END) && (data_byte_received))
	{
		// Bajar la bandera de recepción del byte de dato
		data_byte_received = 0;
		
		// Levantar la bandera de recepción terminada
		reception_ended = 1;
		
		// Indicador de prueba
		UART_sendString("Fin de Frame de Información: ");
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
		
		// PROCESAR INSTRUCCIÓN!!!!
		process_instruction_uart();
	}
	else
	{
		// Indicador de prueba
		UART_sendString("Caracter Inválido: ");
		UART_sendChar(data);
		UART_sendString("\r\n");
	}
}

