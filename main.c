/*
* teste.c
*
* Created: 01/10/2017 17:39:59
*  Author: Guilherme Fardin
*/ 

//#define F_CPU 16000000ul

#define d1 3000
#define d2 10000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "vdip_usart.h"
#include "botao.h"
//#include "display_tw12864.h"
#include "amostragem.h"
#include "twi.h"

/*
* Array com toda a formatação do Arquivo
*/
	
						  //  0     1     2     3     4     5     6
uint8_t arquivo[35] =		{0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x09,	//ttt,tt	tab
						  //  7     8     9    10    11    12    13
							0x00, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x09,	//vvvv,v	tab
						  // 14    15    16    17    18    19    20 
							0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x09,	//ddd,dd	tab
						  // 21    22    23    24    25    26    27
							0x00, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x09,	//hhhh,h	tab
						  // 28    29    30    31    32    33    34
							0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x10};	//MMMMM		CR NL

/*
* Variaveis auxiliares
*/
				
uint16_t divisores[33] =   {10000,	1000,	100,	1,	10,	1,	1,					//ttt,tt	tab
			
							10000,	1000,	100,	10,	1,	1,	1,					//vvvv,v	tab

							10000,	1000,	100,	1,	10,	1,	1,					//ddd,dd	tab

							10000,	1000,	100,	10,	1,	1,	1,					//hhhh,h	tab

							10000,	1000,	100,	10,	1};							//MMMMM		CR NL

uint8_t		j			= 0;											//Indice do array de dados
uint8_t		indice		= 0;								            //Indice do array de formatação do arquivo
uint32_t	aux_cal;													//Variavel de auxilio a calculos
uint8_t		flag_Dis	= 0;

/*
* Variaveis exportadas dos outros arquivos.c
*/

volatile uint16_t dados_main[5]	=	{0,	0,	0,	0,	0};
extern volatile uint8_t flag_dados;

extern volatile uint8_t Dig1_arquivo;
extern volatile uint8_t Dig2_arquivo;
extern volatile uint8_t flag_botao;

int main(void)
{ 
DDRB |= (1<<1);
inic_usart_8_1(UBBR_9600);
//inic_vdip();
//baud_rate_57600();
inic_usart_8_1(UBBR_57600);
inic_botao();
inic_eeprom();
inic_twi_master();
tela_inicial();

sei();

   while(1)
   {
		
		if((flag_dados == 1) && (flag_botao == 1))
		{
 		  PORTB |= (1<<1);	
		
								 /*
								* Formatando o arquivo, separando os digitos de cada dado, inserindo TABs e pulando linha
								*
								*  Formatação do arquivo para teste
								*
								* Tempo(s) Velocidade(km/h) Deslocamento(m)  Homocinética(rpm) Motor(rpm)
								* ttt,tt  vvvv,v    ddd,dd    hhhh,h    MMMMM
								*/ 
 
			aux_cal = dados_main[j];										//Dado de tempo a ser separado por digitos
 
			for(indice = 0; indice < 33; indice++)
			{
	  
				if(indice == 6 || indice == 13 || indice == 20 || indice == 27)
				{
					arquivo[indice] = 0x09;									//Adicionando TAB no arquivo
					j++;													//Deslocando o indice do array DadosBrutos selecionando o dado
					aux_cal = dados_main[j];								//Depois do TAB próximo dado será separado por digitos
				}
				else if (indice == 3 || indice == 11 || indice == 17 || indice == 25)
				{
					arquivo[indice] = ',';									//Adicionando virgula no arquivo
				} 
				else
				{
					arquivo[indice] = (aux_cal / divisores[indice]) + 48;	//Adicionando os digitos de cada dado
					aux_cal   = aux_cal % divisores[indice];
				}
			}																//Executa os calculos, separando cada digito das variaveis
  
// 			abrir_arquivo(Dig2_arquivo,Dig1_arquivo);
// 			escrever_arquivo(&arquivo,0,34);
// 			fechar_arquivo();
			
			flag_dados = 0;
			j = 0;
			
			atualiza_VB(&arquivo);
			_delay_ms(1000);
 		}
		PORTB &= ~(1<<1);
	}
}
