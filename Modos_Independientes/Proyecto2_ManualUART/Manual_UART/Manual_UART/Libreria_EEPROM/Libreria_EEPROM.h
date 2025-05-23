/*
 * Libreria_EEPROM.h
 *
 * Created: 21/05/2025 11:44:32
 *  Author: mario
 */ 


#ifndef LIBRERIA_EEPROM_H_
#define LIBRERIA_EEPROM_H_

#include <avr/io.h>
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);



#endif /* LIBRERIA_EEPROM_H_ */