/*
 * LibreriaTimer1PWM.h
 *
 * Created: 11/04/2025 18:33:07
 *  Author: mario
 */ 


#ifndef LIBRERIATIMER1PWM_H_
#define LIBRERIATIMER1PWM_H_

#include <avr/io.h>
void init_timer1(void);
void TIMER1_PWMA_set_servo_PW(uint8_t value);
void TIMER1_PWMB_set_servo_PW(uint8_t value);

#endif /* LIBRERIATIMER1PWM_H_ */