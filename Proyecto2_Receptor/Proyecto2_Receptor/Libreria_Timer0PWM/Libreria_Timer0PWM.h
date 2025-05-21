/*
 * Libreria_Timer0PWM.h
 *
 * Created: 21/05/2025 09:21:43
 *  Author: mario
 */ 


#ifndef LIBRERIA_TIMER0PWM_H_
#define LIBRERIA_TIMER0PWM_H_

#include <avr/io.h>
void init_timer0(void);
void TIMER0_PWMA_set_PW(uint8_t value);
void TIMER0_PWMB_set_PW(uint8_t value);


#endif /* LIBRERIA_TIMER0PWM_H_ */