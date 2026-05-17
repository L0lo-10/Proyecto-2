/*
 * UART.h
 *
 * Descripcion: Comunicacion serial 9600 
*/

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BUF_SIZE 64

void    initUART(void);
void    writeChar(char c);
void    writeString(char *string);
void    writeUint8(uint8_t valor);
uint8_t UART_disponible(void);
char    UART_leerChar(void);
uint8_t UART_leerLinea(char *buf, uint8_t maxLen);

#endif