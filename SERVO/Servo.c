//Servo.c

#include "SERVO.h"

static volatile uint8_t  sw_pulso[4] = {150, 150, 150, 150};
static volatile uint16_t sw_tick     = 0;

ISR(TIMER2_COMPA_vect)
{
	if (sw_tick == 0)
	{
		PORTB |= (1<<PB1)|(1<<PB2)|(1<<PB3);
		PORTD |= (1<<PD6);
	}

	if (sw_tick == sw_pulso[0]) PORTB &= ~(1<<PB1);
	if (sw_tick == sw_pulso[1]) PORTB &= ~(1<<PB2);
	if (sw_tick == sw_pulso[2]) PORTB &= ~(1<<PB3);
	if (sw_tick == sw_pulso[3]) PORTD &= ~(1<<PD6);

	sw_tick++;
	if (sw_tick >= 2000) sw_tick = 0;
}

void SERVO_init(void)
{
	DDRB |= (1<<DDB1)|(1<<DDB2)|(1<<DDB3);
	DDRD |= (1<<DDD6);

	TCCR1A = 0;
	TCCR1B = 0;

	TCCR2A = (1<<WGM21);
	TCCR2B = (1<<CS21);
	OCR2A  = 19;
	TIMSK2 = (1<<OCIE2A);
}

void SERVO_set(uint8_t canal, float angulo)
{
	float pmin, pmax;

	switch (canal)
	{
		case OJO_IZQ_H:
		if (angulo < IZQ_H_MIN) angulo = IZQ_H_MIN;
		if (angulo > IZQ_H_MAX) angulo = IZQ_H_MAX;
		pmin = IZQ_H_PULSO_MIN; pmax = IZQ_H_PULSO_MAX;
		sw_pulso[0] = (uint8_t)(pmin + (angulo / 180.0f) * (pmax - pmin));
		break;

		case OJO_IZQ_V:
		if (angulo < IZQ_V_MIN) angulo = IZQ_V_MIN;
		if (angulo > IZQ_V_MAX) angulo = IZQ_V_MAX;
		pmin = IZQ_V_PULSO_MIN; pmax = IZQ_V_PULSO_MAX;
		sw_pulso[1] = (uint8_t)(pmin + (angulo / 180.0f) * (pmax - pmin));
		break;

		case OJO_DER_H:
		if (angulo < DER_H_MIN) angulo = DER_H_MIN;
		if (angulo > DER_H_MAX) angulo = DER_H_MAX;
		pmin = DER_H_PULSO_MIN; pmax = DER_H_PULSO_MAX;
		sw_pulso[2] = (uint8_t)(pmin + (angulo / 180.0f) * (pmax - pmin));
		break;

		case OJO_DER_V:
		if (angulo < DER_V_MIN) angulo = DER_V_MIN;
		if (angulo > DER_V_MAX) angulo = DER_V_MAX;
		pmin = DER_V_PULSO_MIN; pmax = DER_V_PULSO_MAX;
		sw_pulso[3] = (uint8_t)(pmin + (angulo / 180.0f) * (pmax - pmin));
		break;
	}
}