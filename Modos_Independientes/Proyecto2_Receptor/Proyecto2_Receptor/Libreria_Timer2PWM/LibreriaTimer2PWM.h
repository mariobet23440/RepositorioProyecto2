/*
 * LibreriaTimer2PWM.h
 *
 * Created: 11/04/2025 18:44:07
 *  Author: mario
 */ 


#ifndef LIBRERIATIMER2PWM_H_
#define LIBRERIATIMER2PWM_H_

#include <avr/io.h>
void init_timer2(void);
void TIMER2_PWMA_set_PW(uint8_t value);
void TIMER2_PWMB_set_PW(uint8_t value);



#endif /* LIBRERIATIMER2PWM_H_ */