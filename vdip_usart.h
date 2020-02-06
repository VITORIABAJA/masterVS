/*
 * vdip_usart.h
 *
 * Created: 27/05/2019 12:55:40
 *  Author: rasle
 */ 


#ifndef VDIP_USART_H_
#define VDIP_USART_H_


#define F_CPU 16000000ul

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"

/*
 *	Definições para tamanho de Arrays
 */


#define BAUD 9600UL
#define UBBR_9600 ((F_CPU/(BAUD<<4))-1)							//Define a velocidade da Serial (default do VDIP)
#define MYUBBR UBBR_9600
#define BAUD1 57600UL
#define UBBR_57600 ((F_CPU/(BAUD1<<4))-1)						//Define a velocidade da Serial
#define BAUD2 230400UL
#define UBBR_230400 ((F_CPU/(BAUD2<<4))-1)						//Define a velocidade da Serial
#define BAUD3 115200UL
#define UBBR_115200 ((F_CPU/(BAUD3<<4))-1)						//Define a velocidade da Serial

#define t_c_9600	6											//Tamanho do array comandobaud9600
#define t_c_57600	6											//Tamanho do array comandobaud57600
#define t_c_115200	6											//Tamanho do array comandobaud115200
#define t_c_230400	6											//Tamanho do array comandobaud230400
#define t_c_inic	4//6										//Tamanho do array comandoinicializa
#define t_c_abrir	11											//Tamanho do array comandoabrir
#define t_c_escr	7											//Tamanho do array comandoescrever
#define t_c_fech	11										    //Tamanho do array comandofechar
#define t_cabe		61											//Tamanho do array para cabeçario do arquivo AV#.txt


/*
	Mapeamento de Hardware
*/

#define CTS PINB3												//Entrada, quando em 0 permite o tráfego de comandos pela serial
#define	RTS PINB4												//Saida, habilita o ínicio da transmissão de comandos

/*
	Variaveis para comandos do VDIP1
*/


//void USART_Transmit(uint8_t data);
void inic_derial();
void inic_vdip();
void abrir_arquivo(uint8_t digito1, uint8_t digito2);
void escrever_cabecario();
void escrever_arquivo(uint8_t *data, uint8_t pos_INI, uint8_t pos_FIN);
void fechar_arquivo();
void baud_rate_9600();
void baud_rate_57600();
void baud_rate_230400();

#endif /* VDIP_USART_H_ */