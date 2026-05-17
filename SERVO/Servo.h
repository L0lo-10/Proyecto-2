
//Servo.h

#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define OJO_IZQ_H   0
#define OJO_IZQ_V   1
#define OJO_DER_H   2
#define OJO_DER_V   3

#define IZQ_H_MIN         10
#define IZQ_H_MAX        170
#define IZQ_H_PULSO_MIN  100
#define IZQ_H_PULSO_MAX  200

#define IZQ_V_MIN         90
#define IZQ_V_MAX        180
#define IZQ_V_PULSO_MIN  100
#define IZQ_V_PULSO_MAX  230

#define DER_H_MIN         10
#define DER_H_MAX        170
#define DER_H_PULSO_MIN  100
#define DER_H_PULSO_MAX  200

#define DER_V_MIN         90
#define DER_V_MAX        180
#define DER_V_PULSO_MIN  100
#define DER_V_PULSO_MAX  230

void SERVO_init(void);
void SERVO_set(uint8_t canal, float angulo);

#endif