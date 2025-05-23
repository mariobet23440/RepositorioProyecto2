/*
 * Libreria_HBridge.c - LIBRERÍA PARA MÓDULO PUENTE H
 *
 * Created: 21/05/2025 09:13:53
 * Autor: Mario Alejandro Betancourt Franco
 * Descripción: Librería para funciones de control para módulo puente H (L298N)
 */ 

/*
ASIGNACIÓN DE PINES
IN1 - PD2 (Pin Digital 2 en Arduino Uno)
IN2 - PD3 (Pin Digital 3 en Arduino Uno)
ENA - PD5 (Pin Digital 5 en Arduino Uno - OC0B)
IN3 - PD4 (Pin Digital 2 en Arduino Uno)
IN4 - PD7 (Pin Digital 3 en Arduino Uno)
ENB - PD6 (Pin Digital 5 en Arduino Uno - OC0A)
*/

#include "Libreria_HBridge.h"

// Constante
const char midpoint = 255/2; // Punto medio entre 255

// Inicialización de pines de salida
void init_HBridgePins(void)
{
	// Configura IN1 e IN2 como salidas
	DDRD |= (1 << PD2) | (1 << PD3);
	
	// Configura IN3 e IN4 como salidas
	DDRD |= (1 << PD4) | (1 << PD7);
}

// Control de dirección - Motor A
void motorA_forward(void) {
	PORTD |= (1 << PD2);   // IN1 = HIGH
	PORTD &= ~(1 << PD3);  // IN2 = LOW
}

void motorA_backward(void) {
	PORTD &= ~(1 << PD2);  // IN1 = LOW
	PORTD |= (1 << PD3);   // IN2 = HIGH
}

void motorA_stop(void) {
	PORTD &= ~((1 << PD2) | (1 << PD3));  // IN1 = LOW, IN2 = LOW
}

// Control de dirección - Motor B
void motorB_forward(void) {
	PORTD |= (1 << PD4);   // IN3 = HIGH
	PORTD &= ~(1 << PD7);  // IN4 = LOW
}

void motorB_backward(void) {
	PORTD &= ~(1 << PD4);  // IN3 = LOW
	PORTD |= (1 << PD7);   // IN4 = HIGH
}

void motorB_stop(void) {
	PORTD &= ~((1 << PD4) | (1 << PD7));  // IN3 = LOW, IN3 = LOW
}



