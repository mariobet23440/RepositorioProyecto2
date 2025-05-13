/*
* PROYECTO 2 RECEPTOR - PROGRAMACIÓN DE MICROCONTROLADORES
* FECHA DE CREACIÓN: 3/05/25 19:35:08
* FECHA DE ENTREGA: 17/05/25 16:30:00
* AUTOR: Mario Alejandro Betancourt Franco (Universidad del Valle de Guatemala, 23440)
* MICROCONTROLADOR: ATMega328P (En un Arduino Nano)
* DESCRIPCIÓN: Programa para controlar un carro detector de obstáculos. El programa ofrece soporte para el control de
* dos motorreductores en las ruedas y dos servomotores. Adicionalmente utiliza un sensor ultrasónico para detectar obstáculos,
* alertando de la presencia de objetos a una distancia menor a los 30 cm en la línea de visión (LOS).
*/

// Librerías
#define F_CPU 16000000UL  // Definir F_CPU si tu microcontrolador corre a 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Libreria_Timer1PWM/LibreriaTimer1PWM.h"
#include "Libreria_Timer2PWM/LibreriaTimer2PWM.h"
#include "Libreria_UART/Libreria_UART.h"

// Variables
uint16_t uart_data = 0;
char uart_action = 0;
char prev_answer_act = 0;

// Prototipos de funciones
void react(char action, char data);

void setup(void)
{
	UART_init();
	init_timer1();
	init_timer2();
	sei();  // Habilitar interrupciones globales
}

int main(void)
{
	setup();
	TIMER1_PWM1_set_servo_PW(127);  // valor medio
	TIMER1_PWM2_set_servo_PW(200);  // valor hacia extremo
	//TIMER2_PWMA_set_PW(125);
	//TIMER2_PWMB_set_PW(125);

	while (1)
	{
		UART_sendString("Hola mundo\r\n");
		_delay_ms(1000);
	}
}

// Interrupción por recepción UART
ISR(USART_RX_vect)
{
	char data = UDR0;

	//UART_sendChar(data);  // Eco inmediato del carácter recibido

	if (prev_answer_act)
	{
		uart_data = data;

		// Indicar que se recibió un dato
		UART_sendString("Dato recibido: ");
		UART_sendChar(uart_data);
		UART_sendString("\r\n");
		
		// Hacer algo con el dato
		react(uart_action, uart_data);

		// Bajar la bander
		prev_answer_act = 0;
	}
	else
	{
		uart_action = data;
		prev_answer_act = 1;

		// Indicar que se recibió una acción
		UART_sendString("Acción recibida: ");
		UART_sendChar(uart_action);
		UART_sendString("\r\n");
	}
}

// Función de reacción ante comandos UART
void react(char action, char data)
{
	switch (action)
	{
		case '1':
		TIMER1_PWM1_set_servo_PW(data);
		break;

		case '2':
		TIMER1_PWM2_set_servo_PW(data);
		break;
		
		case '3':
		TIMER2_PWMA_set_PW(data);
		break;
		
		case '4':
		TIMER2_PWMB_set_PW(data);
		break;

		default:
		break;
	}
}
