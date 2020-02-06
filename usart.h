/*
 * usart.h
 *
 * Created: 27/05/2019 12:59:21
 *  Author: rasle
 */ 


#ifndef USART_H_
#define USART_H_

#define USARTLIB_H_
#define FOSC 16000000UL
#define BAUD 9600UL
#define UBBR_9600 ((FOSC/(BAUD<<4))-1)							//Define a velocidade da Serial (default do VDIP)
#define BAUD1 57600UL
#define UBBR_57600 ((F_CPU/(BAUD1<<4))-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void inic_usart_8_1(uint16_t MYUBBR);

void usart_send(uint8_t data);

uint8_t usart_receive();


ISR (USART_RX_vect);

ISR (USART_TX_vect);

ISR (USART_UDRE_vect);

#endif /* USART_H_ */