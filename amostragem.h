/*
 * amostragem.h
 *
 * Created: 03/06/2019 00:40:33
 *  Author: rasle
 */ 


#ifndef AMOSTRAGEM_H_
#define AMOSTRAGEM_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint16_t dados_main[5];
volatile uint8_t flag_dados;

void inic_timer1();
void inic_leds();
void inic_amostragem();
void fim_amostragem();
ISR(TIMER1_OVF_vect);

#endif /* AMOSTRAGEM_H_ */