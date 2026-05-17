/*
 * EEPROM.h
 *
 * Descripcion: Lectura y escritura de la EEPROM interna del ATmega328P
 *
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

void    writeEEPROM(uint16_t direccion, uint8_t dato);
uint8_t readEEPROM(uint16_t direccion);

#endif