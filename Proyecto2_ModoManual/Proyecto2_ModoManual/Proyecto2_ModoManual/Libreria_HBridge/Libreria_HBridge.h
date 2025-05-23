/*
 * Libreria_HBridge.h
 *
 * Created: 21/05/2025 09:14:09
 *  Author: mario
 */ 


#ifndef LIBRERIA_HBRIDGE_H_
#define LIBRERIA_HBRIDGE_H_

#include <avr/io.h>
void init_HBridgePins(void);
void motorA_forward(void);
void motorA_backward(void);
void motorA_stop(void);
void motorB_forward(void);
void motorB_backward(void);
void motorB_stop(void);



#endif /* LIBRERIA_HBRIDGE_H_ */