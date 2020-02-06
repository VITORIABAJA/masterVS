/*
* botoes.c
*
* Created: 30/04/2017 21:14:04
*  Author: Guilherme Fardin
*/

#define  F_CPU 16000000ul

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "botao.h"
#include "vdip_usart.h"


/*
* Mapeamento de Hardware
*/


/*
* Variaveis auxiliares
*/

uint8_t flag_botao  = 0;
uint8_t Dig1_arquivo = 0;
uint8_t Dig2_arquivo = 0;

uint8_t pos1_eeprom  = 48;
uint8_t pos2_eeprom  = 48;

extern volatile uint8_t flag_Dis;

void debounce(uint8_t sinal)
{
	/*
	* Filtra as oscilações de sinal causandas pela acomodação mecanica das chaves
	*/

	uint8_t filtro = 0;
	//Trata a borda de descida

	while (filtro < 2)
	{
		if (PIND & (1 << sinal))	{ filtro   = 0; }
		
		else						{ filtro  += 1; }
	}                    
 		
	filtro = 0;
	//Trata a borda de subida
// 	while (filtro < 200)
// 	{
// 		if (PIND & (1 << sinal))	{ filtro  += 1; }
// 		
// 		else						{ filtro   = 0; }
// 	}                    
}

void inic_botao()  
{
// 	PORTB |= (1 << Bot_Play);								//Pull-up para o botão play
// 	PORTB |= (1 << Bot_Stop);								//Pull-up para o botão stop
// 
// 	PCICR = 0b00000001;										//Habilita a interrupção do pinos PCINT0 a PCINT7
// 	
// 	//PCMSK0 = 0b00100000;									//Habilita a interrupção do pino PCINT5 PB5(Play),
// 															//Desabilita a interrupção do pino PCINT4 PB4(Stop)
// 	
// 	PCMSK0 = 0b00000010;									//Habilita a interrupção do pino PCINT5 PB1(Play),
// 															//Desabilita a interrupção do pino PCINT4 PB2(Stop)

	PORTD |= (1 << Bot_Play);								//Pull-up para o botão play
	PORTD |= (1 << Bot_Stop);								//Pull-up para o botão stop
	EICRA = 0b00001010;										//Habilita INT0 e INT1 para borda de descida
	EIMSK = 0b00000001;					
	
	DDRB |= (1 << led_play);								//Habilita a saida para o led
	
}

void inic_eeprom()
{
	if(eeprom_read_byte(&pos1_eeprom) < 48 || eeprom_read_byte(&pos1_eeprom) > 57)
	{
		eeprom_write_byte(&pos1_eeprom, 48);
	}
	if(eeprom_read_byte(&pos2_eeprom) < 48 || eeprom_read_byte(&pos2_eeprom) > 57)
	{
		eeprom_write_byte(&pos2_eeprom, 48);
	}
}															 //Inicializa a eeprom com '0' caso esteja fora de um digito

ISR(INT0_vect){
	if(!flag_botao){
		EIMSK &= ~(1<<INT0);
		debounce(Bot_Play);
		Dig1_arquivo = eeprom_read_byte(&pos1_eeprom);     //Lê a memória eeprom da posição pos1_eeprom = 0
		Dig2_arquivo = eeprom_read_byte(&pos2_eeprom);     //Lê a memória eeprom da posição pos2_eeprom = 0
			
		
		//abrir_arquivo(Dig2_arquivo,Dig1_arquivo);
		//escrever_cabecario();
		//fechar_arquivo();

		abrindo_VB();
		flag_Dis = 0;
		
		inic_amostragem();
		
		PORTB |= (1 << led_play);						//Começa a gravação
		flag_botao = 1;									//Permite começar a gravar
		
		dados_VB();
		
		EIMSK |= (1 << INT1);							//Habilita a interrupção do botao stop
	}
}

ISR(INT1_vect){
	if(flag_botao)
	{
		
		flag_botao = 0;
		
		EIMSK &= ~(1 << INT1);							//desabilita a interrupção pelo botão
		PORTB &= ~(1 << led_play);						//Indica o fim da gravação
		debounce(Bot_Stop);
		
		fim_amostragem();
		
		fecha_VB();			//Atualiza o display com a tela fechar indicando o nome do arquivo
		
		Dig1_arquivo ++;								//Incrementa a variavel a ser usado para criar o arquivo seguinte
		if(Dig1_arquivo > 57)							//Se dig1 for 10
		{
			Dig1_arquivo = 48;							//coloca '0' na unidade
			Dig2_arquivo ++;							//Incrementa a dezena
			
			if (Dig2_arquivo > 57)				         //Se dig2 for 10
			{
				Dig2_arquivo = 48;				        //coloca '0' na dezena
														//Incrementa os digitos de 00 a 99
			}
		}
		
		eeprom_write_byte(&pos1_eeprom, Dig1_arquivo);
		eeprom_write_byte(&pos2_eeprom, Dig2_arquivo);     //Grava as variaveis na eeprom, garantindo a sequencia dos arquivos
		
		_delay_ms(3000);
		inicia_VB();					//Recomeça o programa
		
		EIMSK |= (1 << INT0);          //Habilita a interrupção pelo botao.
	}
}