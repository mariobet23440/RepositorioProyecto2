// Inclusión de librerías estándar de AVR
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

// Inclusión de librerías propias
#include "Libreria_Timer0PWM/Libreria_Timer0PWM.h"   // PWM para motores (puente H)
#include "Libreria_Timer1PWM/LibreriaTimer1PWM.h"    // PWM para servomotores
#include "Libreria_HBridge/Libreria_HBridge.h"       // Control del puente H (dirección motores)
#include "Libreria_ADC/LibreriaADC.h"                // Lectura de entradas analógicas (potenciómetros, joysticks)
#include "Libreria_UART/Libreria_UART.h"			 // Librería UART

// ==========================
// VARIABLES GLOBALES
// ==========================

// Canal actual del ADC (0 a 3)
char current_channel = 0;

// Variables que almacenan el valor leído por el ADC (8 bits cada una)
char adc_value_chan0 = 0;
char adc_value_chan1 = 0;
char adc_value_chan2 = 0;
char adc_value_chan3 = 0;

// ==========================
// FUNCIONES AUXILIARES
// ==========================

/**
 * Convierte un valor de 0 a 255 (entrada del ADC)
 * a un rango de -128 a 127 para simplificar
 * el control de velocidad en motores diferenciales.
 */
int8_t normalize_input(char valor) {
	return ((int16_t)valor - 128);
}

/**
 * Controla las ruedas diferenciales de un robot.
 * El parámetro `traslacion` se refiere al movimiento hacia adelante/atrás.
 * El parámetro `rotacion` indica el giro (izquierda/derecha).
 */
void move_differential(int8_t traslacion, int8_t rotacion) {
	// Cálculo de velocidades para cada rueda
	int16_t vel_izq = traslacion - rotacion;
	int16_t vel_der = traslacion + rotacion;

	// Limitación (saturación) de las velocidades a -255 a 255
	if (vel_izq > 255) vel_izq = 255;
	if (vel_izq < -255) vel_izq = -255;
	if (vel_der > 255) vel_der = 255;
	if (vel_der < -255) vel_der = -255;
	
	// ZONA MUERTA: si ambos valores están cerca de 0, detener motores
	if (traslacion > -10 && traslacion < 10 && rotacion > -10 && rotacion < 10) {
		motorA_stop();
		motorB_stop();
		TIMER0_PWMA_set_PW(0);
		TIMER0_PWMB_set_PW(0);
		return;
	}

	// Control de dirección y velocidad para el motor izquierdo
	if (vel_izq >= 0) {
		motorA_forward();  // Adelante
		TIMER0_PWMA_set_PW((uint8_t)vel_izq);
	} else {
		motorA_backward(); // Atrás
		TIMER0_PWMA_set_PW((uint8_t)(-vel_izq));
	}

	// Control de dirección y velocidad para el motor derecho
	if (vel_der >= 0) {
		motorB_forward();  // Adelante
		TIMER0_PWMB_set_PW((uint8_t)vel_der);
	} else {
		motorB_backward(); // Atrás
		TIMER0_PWMB_set_PW((uint8_t)(-vel_der));
	}
	
	
}

/**
 * Función que aplica los valores leídos del ADC
 * para controlar tanto los servomotores como los motorreductores.
 * 
 * - Mx, My: para control de motorreductores (movimiento base)
 * - Sx, Sy: para control de servomotores (movimiento cabeza o brazos, por ejemplo)
 */
void manual_mode_movement(char Mx, char My, char Sx, char Sy) {
	// PWM para servos: ajustar ángulo de servomotores
	TIMER1_PWMA_set_servo_PW(Sx); // Servo X
	TIMER1_PWMB_set_servo_PW(Sy); // Servo Y

	// Movimiento diferencial (con ruedas)
	int8_t traslacion = normalize_input(Mx);
	int8_t rotacion   = normalize_input(My);
	move_differential(traslacion, rotacion);
}

// ==========================
// CONFIGURACIÓN INICIAL
// ==========================

/**
 * Función de configuración del sistema.
 * Inicializa todos los periféricos necesarios.
 */
void setup(void) {
	cli(); // Deshabilitar interrupciones globales

	// Inicialización de periféricos
	init_timer0();          // PWM para motores
	init_timer1();          // PWM para servos
	init_HBridgePins();     // Pines de dirección del puente H
	setup_adc();            // Configuración del ADC
	UART_init();

	sei(); // Habilitar interrupciones globales
}

// ==========================
// FUNCIÓN PRINCIPAL
// ==========================

/**
 * Función principal del programa.
 * Ejecuta un bucle infinito donde lee entradas analógicas
 * y mueve los motores en base a eso.
 */
int main(void) {
	setup(); // Inicializar el sistema

	uint8_t modo_prueba = 0; // Cambia a 0 para usar el modo joystick/manual

	if (modo_prueba) {
		// ========== MODO PRUEBA DE MOTORES ==========
		TIMER1_PWMA_set_servo_PW(100);
		TIMER1_PWMB_set_servo_PW(100);


		// Giro hacia adelante
		motorA_forward();
		motorB_forward();
		TIMER0_PWMA_set_PW(200); // PWM medio-alto
		TIMER0_PWMB_set_PW(200);
		_delay_ms(2000); // Espera 2 segundos

		// Giro hacia atrás
		motorA_backward();
		motorB_backward();
		TIMER0_PWMA_set_PW(200);
		TIMER0_PWMB_set_PW(200);
		_delay_ms(2000); // Espera 2 segundos

		// Detener motores
		motorA_stop();
		motorB_stop();
		TIMER0_PWMA_set_PW(0);
		TIMER0_PWMB_set_PW(0);

		while (1); // Mantener detenido después de prueba
	}
	else {
		// ========== MODO MANUAL (CONTROL ANALÓGICO) ==========
		while (1) {
			manual_mode_movement(adc_value_chan0, adc_value_chan1, adc_value_chan2, adc_value_chan3);
		}
	}

	return 0;

}

// ==========================
// INTERRUPCIONES
// ==========================

/**
 * Interrupción del ADC.
 * Se encarga de cambiar de canal cada vez que se completa una conversión.
 * Esto permite leer múltiples canales secuencialmente.
 */
ISR(ADC_vect) {
	switch(current_channel) {
		case 0:
			adc_value_chan0 = ADCH;       // Leer canal 0
			current_channel = 1;
			adc_set_channel(1);           // Cambiar a canal 1
			break;
		case 1:
			adc_value_chan1 = ADCH;       // Leer canal 1
			current_channel = 2;
			adc_set_channel(2);
			break;
		case 2:
			adc_value_chan2 = ADCH;       // Leer canal 2
			current_channel = 3;
			adc_set_channel(3);
			break;
		case 3:
			adc_value_chan3 = ADCH;       // Leer canal 3
			current_channel = 0;
			adc_set_channel(0);           // Regresar al canal 0
			break;
	}
}


