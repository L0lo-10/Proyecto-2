// main.c
// Proyecto 2 - Rostro Animatronico
// Autor: Giancarlo Custodio Lara
//
// Pines:
//   D2  - LED Modo 1
//   D3  - LED Modo 2  (D2+D3 = Modo 3)
//   D4  - Boton cambiar modo
//   D5  - Boton confirmar / siguiente expresion
//   D9  - Servo ojo izquierdo horizontal
//   D10 - Servo parpado izquierdo
//   D11 - Servo ojo derecho horizontal
//   D6  - Servo parpado derecho
//   A0  - Pot ojo izquierdo horizontal
//   A1  - Pot parpado izquierdo
//   A2  - Pot ojo derecho horizontal
//   A3  - Pot parpado derecho
//
// Modos:
//   Modo 1 (ROJO)      - Manual
//   Modo 2 (AZUL)      - Guardar
//   Modo 3 (ROJO+AZUL)   - Reproducir Adafruit
//
//*************************************************************




#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "ADC/ADC.h"
#include "SERVO/SERVO.h"
#include "UART/UART.h"
#include "EEPROM/EEPROM.h"

/****************************************/
// Prototipos

void    setup(void);
void    set_leds(uint8_t modo);
uint8_t btn_presionado(uint8_t pin);
float   leerPot(uint8_t canal, float *anterior);
float   map(float x, float in_min, float in_max, float out_min, float out_max);
void    modo_manual(void);
void    modo_guardar(void);
void    modo_reproducir(void);
void    guardar_cara(uint8_t cara);
void    cargar_cara(uint8_t cara);

/****************************************/
// Pines utiles

#define LED1       PD2
#define LED2       PD3
#define BTN_MODO   PD4
#define BTN_BORRAR PD5

/****************************************/
// Modos

#define MODO_MANUAL     1
#define MODO_GUARDAR    2
#define MODO_REPRODUCIR 3

#define NUM_CARAS    4
#define ADC_DEADZONE 8

/****************************************/
// Expresiones fijas para modo 3
// [0]=IH  [1]=IV  [2]=DH  [3]=DV
// Vertical:   90=abierto / 180=cerrado
// Horizontal: 10=izquierda / 90=centro / 170=derecha

float EXP_DORMIDO[4] = {  90, 180,  90, 180 };
float EXP_VISCO[4]   = { 140,  90,  40,  90 };
float EXP_GUINO[4]   = {  90,  90,  90, 180 };

/****************************************/
// Variables

uint8_t modo_actual = MODO_MANUAL;
float   angulos[4]  = { 90, 90, 90, 90 };
float   adcAnt[4]   = { 512, 512, 512, 512 };

/****************************************/
// Main

int main(void)
{
    cli();

    DDRD  |=  (1<<LED1)|(1<<LED2);
    DDRD  &= ~((1<<BTN_MODO)|(1<<BTN_BORRAR));
    PORTD |=   (1<<BTN_MODO)|(1<<BTN_BORRAR);

    ADC_init();
    SERVO_init();
    initUART();

    sei();

    set_leds(MODO_MANUAL);
    writeString("\r\n=== ROSTRO ANIMATRONICO ===\r\n");
    writeString("MODO 1: Manual\r\n");

    while (1)
    {
        // D4: cambiar modo
        if (btn_presionado(BTN_MODO))
        {
            modo_actual++;
            if (modo_actual > MODO_REPRODUCIR) modo_actual = MODO_MANUAL;
            set_leds(modo_actual);

            if (modo_actual == MODO_MANUAL)
                writeString("\r\nMODO 1: Manual\r\n");
				
            else if (modo_actual == MODO_GUARDAR)
                writeString("\r\nMODO 2: Guardar\r\n");
				
            else
                writeString("\r\nMODO 3: Reproducir\r\n");
        }

        // D5: borrar caras guardadas (modos 2 y 3)
        if (modo_actual != MODO_MANUAL && btn_presionado(BTN_BORRAR))
        {
            for (uint16_t i = 0; i < (NUM_CARAS * 4); i++)
                writeEEPROM(i, 0xFF);
            writeString("Caras borradas.\r\n");
        }

        switch (modo_actual)
        {
            case MODO_MANUAL:     modo_manual();     break;
            case MODO_GUARDAR:    modo_guardar();    break;
            case MODO_REPRODUCIR: modo_reproducir(); break;
        }
    }
}

/****************************************/
// LEDs indicadores de modo

void set_leds(uint8_t modo)
{
    PORTD &= ~((1<<LED1)|(1<<LED2));
    if (modo == MODO_MANUAL)     PORTD |= (1<<LED1);
    if (modo == MODO_GUARDAR)    PORTD |= (1<<LED2);
    if (modo == MODO_REPRODUCIR) PORTD |= (1<<LED1)|(1<<LED2);
}

/****************************************/
// Boton con antirrebote

uint8_t btn_presionado(uint8_t pin)
{
    if (!(PIND & (1<<pin)))
    {
        _delay_ms(30);
        if (!(PIND & (1<<pin)))
        {
            while (!(PIND & (1<<pin)));
            _delay_ms(30);
            return 1;
        }
    }
    return 0;
}

/****************************************/
// Pot con zona muerta

float leerPot(uint8_t canal, float *anterior)
{
    float nuevo = ADC_read(canal);
    float diff  = nuevo - *anterior;
    if (diff < 0) diff = -diff;
    if (diff > ADC_DEADZONE) { *anterior = nuevo; return nuevo; }
    return *anterior;
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}

/****************************************/
// MODO 1: pots mueven los servos

void modo_manual(void)
{
    float adc;

    adc = leerPot(0, &adcAnt[0]);
    angulos[0] = map(adc, 0, 1023, IZQ_H_MIN, IZQ_H_MAX);

    adc = leerPot(1, &adcAnt[1]);
    angulos[1] = map(adc, 0, 1023, IZQ_V_MIN, IZQ_V_MAX);

    adc = leerPot(2, &adcAnt[2]);
    angulos[2] = map(adc, 0, 1023, DER_H_MIN, DER_H_MAX);

    adc = leerPot(3, &adcAnt[3]);
    angulos[3] = map(adc, 0, 1023, DER_V_MIN, DER_V_MAX);

    SERVO_set(OJO_IZQ_H, angulos[0]);
    SERVO_set(OJO_IZQ_V, angulos[1]);
    SERVO_set(OJO_DER_H, angulos[2]);
    SERVO_set(OJO_DER_V, angulos[3]);

    _delay_ms(20);
}

/****************************************/
// MODO 2: pots mueven + Adafruit guarda

void modo_guardar(void)
{
    // Pots siguen moviendo los servos
    float adc;

    adc = leerPot(0, &adcAnt[0]);
    angulos[0] = map(adc, 0, 1023, IZQ_H_MIN, IZQ_H_MAX);

    adc = leerPot(1, &adcAnt[1]);
    angulos[1] = map(adc, 0, 1023, IZQ_V_MIN, IZQ_V_MAX);

    adc = leerPot(2, &adcAnt[2]);
    angulos[2] = map(adc, 0, 1023, DER_H_MIN, DER_H_MAX);

    adc = leerPot(3, &adcAnt[3]);
    angulos[3] = map(adc, 0, 1023, DER_V_MIN, DER_V_MAX);

    SERVO_set(OJO_IZQ_H, angulos[0]);
    SERVO_set(OJO_IZQ_V, angulos[1]);
    SERVO_set(OJO_DER_H, angulos[2]);
    SERVO_set(OJO_DER_V, angulos[3]);

    // Revisar si llego un comando de guardar desde Adafruit
    static char buf[32];
    if (UART_leerLinea(buf, sizeof(buf)))
    {
        if (strncmp(buf, "new", 3) == 0 && buf[3] >= '1' && buf[3] <= '4')
        {
            uint8_t cara = buf[3] - '1';
            guardar_cara(cara);
            writeString("Cara "); writeChar(buf[3]); writeString(" guardada.\r\n");
        }
    }

    _delay_ms(20);
}

/****************************************/
// MODO 3: reproducir posiciones y expresiones
// Adafruit manda: load1-load4 o dormido/visco/guino

void modo_reproducir(void)
{
    static char buf[32];
    if (!UART_leerLinea(buf, sizeof(buf))) return;

    // Caras guardadas en EEPROM
    if (strncmp(buf, "load", 4) == 0 && buf[4] >= '1' && buf[4] <= '4')
    {
        uint8_t cara = buf[4] - '1';
        cargar_cara(cara);
        writeString("Cara "); writeChar(buf[4]); writeString(" cargada.\r\n");
        return;
    }

    // Expresiones fijas
    if (strcmp(buf, "dormido") == 0)
    {
        for (uint8_t i = 0; i < 4; i++) { angulos[i] = EXP_DORMIDO[i]; SERVO_set(i, angulos[i]); }
        writeString("Expresion: DORMIDO\r\n");
        return;
    }
    if (strcmp(buf, "visco") == 0)
    {
        for (uint8_t i = 0; i < 4; i++) { angulos[i] = EXP_VISCO[i]; SERVO_set(i, angulos[i]); }
        writeString("Expresion: VISCO\r\n");
        return;
    }
    if (strcmp(buf, "guino") == 0)
    {
        for (uint8_t i = 0; i < 4; i++) { angulos[i] = EXP_GUINO[i]; SERVO_set(i, angulos[i]); }
        writeString("Expresion: GUINO\r\n");
        return;
    }
}

/****************************************/
// EEPROM
// Cara 1 -> dir 0-3 / Cara 2 -> dir 4-7 / Cara 3 -> dir 8-11 / Cara 4 -> dir 12-15

void guardar_cara(uint8_t cara)
{
    uint16_t base = cara * 4;
    for (uint8_t i = 0; i < 4; i++)
        writeEEPROM(base + i, (uint8_t)angulos[i]);
}

void cargar_cara(uint8_t cara)
{
    uint16_t base = cara * 4;
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t val = readEEPROM(base + i);
        angulos[i]  = (val == 0xFF) ? 90.0f : (float)val;
        SERVO_set(i, angulos[i]);
    }
}