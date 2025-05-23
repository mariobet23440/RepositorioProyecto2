/************************************************************************/
/* CONTROLADOR PRINCIPAL FUSIONADO UART + MODO MANUAL                   */
/* Versión: 23/05/2025                                                  */
/************************************************************************/

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

/************************************************************************/
/* INCLUDES LIBRERÍAS                                                   */
/************************************************************************/
#include "Libreria_Timer0PWM.h"
#include "LibreriaTimer1PWM.h"
#include "Libreria_HBridge.h"
#include "LibreriaADC.h"
#include "Libreria_UART.h"
#include "Libreria_EEPROM.h"

/************************************************************************/
/* CONSTANTES                                                           */
/************************************************************************/
#define RXTX_START         0x30			// 0
#define RXTX_END           0x5A			// Z
#define MOTORREDUCTOR_X    0x4141		// AA
#define MOTORREDUCTOR_Y    0x4242		// BB
#define SERVOMOTOR_X       0x4343		// CC
#define SERVOMOTOR_Y       0x4444		// DD
#define EEPROM_READ        0x4545		// EE
#define EEPROM_WRITE       0x4646		// FF
#define MANUAL_ENABLE      0x4747		// GG
#define MANUAL_DISABLE     0x4848		// HH

#define FRAME_SIZE         3

#define EEPROM_ADDRESS1A   0x0000
#define EEPROM_ADDRESS1B   0x0001
#define EEPROM_ADDRESS2A   0x0002
#define EEPROM_ADDRESS2B   0x0003
#define EEPROM_ADDRESS3A   0x0004
#define EEPROM_ADDRESS3B   0x0005
#define EEPROM_ADDRESS4A   0x0006
#define EEPROM_ADDRESS4B   0x0007

/************************************************************************/
/* VARIABLES GLOBALES                                                   */
/************************************************************************/
char current_channel = 0;
char adc_value_chan0 = 0;
char adc_value_chan1 = 0;
char adc_value_chan2 = 0;
char adc_value_chan3 = 0;

volatile uint8_t received_data[FRAME_SIZE];
volatile uint8_t reception_index = 0;
volatile uint8_t frame_ready = 0;

char manual_mode_enabled = 1;

/************************************************************************/
/* FUNCIONES AUXILIARES                                                 */
/************************************************************************/

int8_t normalize_input(char valor) {
	return ((int16_t)valor - 128);
}

void move_differential(int8_t traslacion, int8_t rotacion) {
	int16_t vel_izq = traslacion - rotacion;
	int16_t vel_der = traslacion + rotacion;

	if (vel_izq > 255) vel_izq = 255;
	if (vel_izq < -255) vel_izq = -255;
	if (vel_der > 255) vel_der = 255;
	if (vel_der < -255) vel_der = -255;

	if (traslacion > -10 && traslacion < 10 && rotacion > -10 && rotacion < 10) {
		motorA_stop();
		motorB_stop();
		TIMER0_PWMA_set_PW(0);
		TIMER0_PWMB_set_PW(0);
		return;
	}

	if (vel_izq >= 0) {
		motorA_forward();
		TIMER0_PWMA_set_PW((uint8_t)vel_izq);
		} else {
		motorA_backward();
		TIMER0_PWMA_set_PW((uint8_t)(-vel_izq));
	}

	if (vel_der >= 0) {
		motorB_forward();
		TIMER0_PWMB_set_PW((uint8_t)vel_der);
		} else {
		motorB_backward();
		TIMER0_PWMB_set_PW((uint8_t)(-vel_der));
	}
}

void manual_mode_movement(char Mx, char My, char Sx, char Sy) {
	TIMER1_PWMA_set_servo_PW(Sx);
	TIMER1_PWMB_set_servo_PW(Sy);
	int8_t traslacion = normalize_input(Mx);
	int8_t rotacion = normalize_input(My);
	move_differential(traslacion, rotacion);
}

/************************************************************************/
/* COMUNICACIÓN UART - Procesamiento de Frame                           */
/************************************************************************/
void process_instruction_uart(void) {
	uint16_t instruction = ((uint16_t)received_data[0] << 8) | received_data[1];
	char data_char = received_data[2];

	char buffer[8];

	UART_sendString("\r\n[UART] RX START: ");
	itoa(RXTX_START, buffer, 16);
	UART_sendString("0x");
	UART_sendString(buffer);

	UART_sendString(", INSTR: 0x");
	itoa(instruction, buffer, 16);
	UART_sendString(buffer);

	UART_sendString(", DATA: 0x");
	itoa(data_char, buffer, 16);
	UART_sendString(buffer);

	UART_sendString(", RX END: ");
	itoa(RXTX_END, buffer, 16);
	UART_sendString("0x");
	UART_sendString(buffer);
	UART_sendString("\r\n");

	switch (instruction) {
		case MOTORREDUCTOR_X:
		UART_sendString("[LOG] Ejecutando: MOTORREDUCTOR_X\r\n");
		TIMER0_PWMA_set_PW(data_char);
		break;
		case MOTORREDUCTOR_Y:
		UART_sendString("[LOG] Ejecutando: MOTORREDUCTOR_Y\r\n");
		TIMER0_PWMB_set_PW(data_char);
		break;
		case SERVOMOTOR_X:
		UART_sendString("[LOG] Ejecutando: SERVOMOTOR_X\r\n");
		TIMER1_PWMA_set_servo_PW(data_char);
		break;
		case SERVOMOTOR_Y:
		UART_sendString("[LOG] Ejecutando: SERVOMOTOR_Y\r\n");
		TIMER1_PWMB_set_servo_PW(data_char);
		break;
		case EEPROM_READ:
		UART_sendString("[LOG] Ejecutando: EEPROM_READ\r\n");
		UART_sendString("[LOG] Leyendo EEPROM bloque: ");
		UART_sendChar(data_char);
		UART_sendString("\r\n");
		// EEPROM read cases...
		break;
		case EEPROM_WRITE:
		UART_sendString("[LOG] Ejecutando: EEPROM_WRITE\r\n");
		UART_sendString("[LOG] Escribiendo EEPROM bloque: ");
		UART_sendChar(data_char);
		UART_sendString("\r\n");
		// EEPROM write cases...
		break;
		case MANUAL_ENABLE:
		UART_sendString("[LOG] Modo manual ACTIVADO\r\n");
		manual_mode_enabled = 1;
		break;
		case MANUAL_DISABLE:
		UART_sendString("[LOG] Modo manual DESACTIVADO\r\n");
		manual_mode_enabled = 0;
		break;
		default:
		UART_sendString("[ERROR] Instrucción desconocida\r\n");
		break;
	}
}


/************************************************************************/
/* CONFIGURACIÓN Y MAIN LOOP                                            */
/************************************************************************/
void setup(void) {
	cli();
	init_timer0();
	init_timer1();
	init_HBridgePins();
	setup_adc();
	UART_init();
	
	TIMER1_PWMA_set_servo_PW(127);
	TIMER1_PWMA_set_servo_PW(127);
	sei();
}

int main(void) {
	setup();

	while (1) {
		if (frame_ready) {
			process_instruction_uart();
			frame_ready = 0;
		}

		if (manual_mode_enabled) {
			manual_mode_movement(adc_value_chan0, adc_value_chan1, adc_value_chan2, adc_value_chan3);
		}
	}
}

/************************************************************************/
/* INTERRUPCIONES                                                       */
/************************************************************************/
ISR(ADC_vect) {
	switch (current_channel) {
		case 0: adc_value_chan0 = ADCH; current_channel = 1; adc_set_channel(1); break;
		case 1: adc_value_chan1 = ADCH; current_channel = 2; adc_set_channel(2); break;
		case 2: adc_value_chan2 = ADCH; current_channel = 3; adc_set_channel(3); break;
		case 3: adc_value_chan3 = ADCH; current_channel = 0; adc_set_channel(0); break;
	}
}

ISR(USART_RX_vect) {
	char data = UDR0;

	if (!frame_ready) {
		if (data == RXTX_START) {
			reception_index = 0;
			} else if (data == RXTX_END && reception_index == FRAME_SIZE) {
			frame_ready = 1;
			} else if (reception_index < FRAME_SIZE) {
			received_data[reception_index++] = data;
		}
	}
}


