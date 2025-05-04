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
void TIMER1_PWM1_set_servo_PW(uint16_t value);
void TIMER1_PWM2_set_servo_PW(uint16_t value);

#endif /* LIBRERIATIMER1PWM_H_ */