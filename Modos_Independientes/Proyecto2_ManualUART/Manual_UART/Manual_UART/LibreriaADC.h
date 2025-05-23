/*
 * LibreriaADC.h
 *
 * Created: 11/04/2025 13:14:40
 *  Author: mario
 */ 


#ifndef LIBRERIAADC_H_
#define LIBRERIAADC_H_

#include <avr/io.h>
void setup_adc(void);
void adc_set_channel(unsigned char channel);


#endif /* LIBRERIAADC_H_ */