/*
 * ADC.c
 */

#include "ADC.h"

void ADC_init(void)
{
    ADMUX  |=  (1<<REFS0);    // Referencia AVCC
    ADMUX  &= ~(1<<REFS1);
    ADMUX  &= ~(1<<ADLAR);    // Resultado alineado a la derecha (10 bits)
    ADCSRA |=  (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);  // Prescaler 128
    ADCSRA |=  (1<<ADEN);     // Encender ADC
}

uint16_t ADC_read(uint8_t canal)
{
    canal &= 0b00001111;                  // Limitar a 4 bits
    ADMUX  = (ADMUX & 0xF0) | canal;     // Seleccionar canal
    ADCSRA |= (1<<ADSC);                  // Iniciar conversion
    while (ADCSRA & (1<<ADSC));           // Esperar conversion
    return ADC;
}