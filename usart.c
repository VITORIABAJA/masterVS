/*
 * usart.c
 *
 * Created: 27/05/2019 12:59:09
 *  Author: Rasley de Paula Forde
 */ 

#include "usart.h"

void inic_usart_8_1(uint16_t MYUBBR){
	/*
		Usa-se o transmissor Serial para comunicação com o VDIP e o receptor para coletar os dados de Velocidade, Rotação e tempo
	*/
	
	//UBRR0	= (uint8_t) UBBR_9600;								//Bps 9600
	//UCSR0C	= 0b00000110;										//Formato da serial: 8data, 1stop bit
	//UCSR0B	= 0b00001000;	
	
// 	UBRR0H	= (unsigned char) (UBBR_9600>>8);	
// 	UBRR0L	= (unsigned char) (UBBR_9600);
	
	UBRR0H	= (unsigned char) (MYUBBR>>8);
	UBRR0L	= (unsigned char) (MYUBBR);
	
	
	//enable transmitter & receiver
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	//set frame - 8bits (011)
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	
}

void usart_send(uint8_t data)
{
	/* Put data into buffer, sends the data */
	UDR0 = data;
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1 << 5)) )
	;
}

uint8_t usart_receive()
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)))
	;
	/* Get and return received data from buffer */
	return UDR0;
}


ISR (USART_RX_vect){
	
}

ISR (USART_TX_vect){
	
}

ISR (USART_UDRE_vect){
	
}