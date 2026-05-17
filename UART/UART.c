/*
 * UART.c
 *
 */

#include "UART.h"

static volatile char    rx_buf[UART_BUF_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

ISR(USART_RX_vect)
{
	char c = UDR0;

	// Eco inmediato: el usuario ve lo que escribe mientras tipea
	if (c >= 32 && c < 127 && (UCSR0A & (1<<UDRE0)))
	UDR0 = c;

	uint8_t next = (rx_head + 1) % UART_BUF_SIZE;
	if (next != rx_tail)
	{
		rx_buf[rx_head] = c;
		rx_head = next;
	}
}

void initUART(void)
{
	DDRD &= ~(1<<DDD0);
	DDRD |=  (1<<DDD1);
	UCSR0A = 0;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	UBRR0  = 103;
}

void writeChar(char c)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void writeString(char *string)
{
	for (uint8_t i = 0; *(string + i) != '\0'; i++)
	writeChar(*(string + i));
}

void writeUint8(uint8_t valor)
{
	char buf[4];
	uint8_t i = 0;
	if (valor == 0) { writeChar('0'); return; }
	while (valor > 0) { buf[i++] = '0' + (valor % 10); valor /= 10; }
	while (i > 0) writeChar(buf[--i]);
}

uint8_t UART_disponible(void)
{
	return (rx_head != rx_tail);
}

char UART_leerChar(void)
{
	if (!UART_disponible()) return 0;
	char c = rx_buf[rx_tail];
	rx_tail = (rx_tail + 1) % UART_BUF_SIZE;
	return c;
}

uint8_t UART_leerLinea(char *buf, uint8_t maxLen)
{
	static char    linebuf[UART_BUF_SIZE];
	static uint8_t idx = 0;

	while (UART_disponible() && idx < (UART_BUF_SIZE - 1))
	{
		char c = UART_leerChar();

		if (c == '\n' || c == '\r')
		{
			while (UART_disponible() &&
			(rx_buf[rx_tail] == '\n' || rx_buf[rx_tail] == '\r'))
			rx_tail = (rx_tail + 1) % UART_BUF_SIZE;

			linebuf[idx] = '\0';
			uint8_t n = idx < (maxLen - 1) ? idx : (maxLen - 1);
			for (uint8_t i = 0; i < n; i++) buf[i] = linebuf[i];
			buf[n] = '\0';
			idx = 0;
			return 1;
		}

		linebuf[idx++] = c;
	}

	buf[0] = '\0';
	return 0;
}