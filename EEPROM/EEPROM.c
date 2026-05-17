/*
 * EEPROM.c
 */

#include "EEPROM.h"

void writeEEPROM(uint16_t direccion, uint8_t dato)
{
	while (EECR & (1<<EEPE));
	EEAR = direccion;
	EEDR = dato;
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
}

uint8_t readEEPROM(uint16_t direccion)
{
	while (EECR & (1<<EEPE));
	EEAR = direccion;
	EECR |= (1<<EERE);
	return EEDR;
}