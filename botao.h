/*
* botoes.h
*
* Created: 30/04/2017 21:40:14
*  Author: Guilherme Fardin
*/


#ifndef BOTAO_H_
#define BOTAO_H_

// #define Bot_Play PINB2				//Bot�o para iniciar a aquisi��o de dados
// #define Bot_Stop PINB1				//Bot�o para fim da aquisi��o de dados
#define Bot_Play PIND2					//Bot�o para iniciar a aquisi��o de dados
#define Bot_Stop PIND3					//Bot�o para fim da aquisi��o de dados
#define led_play PINB2					//Saida para indicar o inicio de grava��o

#include <avr/io.h>
#include <avr/interrupt.h>
#include "amostragem.h"
//#include "display_hd44780.h"

void debounce(uint8_t sinal);
void inic_botao();
void inic_eeprom();
ISR(INT0_vect);
ISR(INT1_vect);

#endif /* BOTAO_H_ */