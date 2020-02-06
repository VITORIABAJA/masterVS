/*
 * vdip_usart.c
 *
 * Created: 23/04/2017 17:53:30
   *  Author: Guilherme Fardin
 */ 
#include "vdip_usart.h"

uint8_t comandobaud9600[t_c_9600]		=	{ 0x14, 0x20, 0x38, 0x41, 0x00, 0x0D};													//Set Baud Rate 9600
uint8_t comandobaud57600[t_c_57600]		=	{ 0x14, 0x20, 0x34, 0xC0, 0x00, 0x0D};													//Set Baud Rate 57600
uint8_t comandobaud115200[t_c_115200]	=	{ 0x14, 0x20, 0x1A, 0x00, 0x00, 0x0D};													//Set Baud Rate 230400
uint8_t comandobaud230400[t_c_230400]	=	{ 0x14, 0x20, 0x0D, 0x00, 0x00, 0x0D};													//Set Baud Rate 230400
uint8_t comandoinicializa[t_c_inic]		=	{ 0x10, 0x0D, 'E', 0x0D};//, 0x91, 0x0D};// 0x14, 0x20, '04', 0xC0, 0x00, 0x0D};		//Habilita comandos curtos, em bytes, e echo E
uint8_t comandoabrir[t_c_abrir]			=	{ 0x09, 0x20, 'a', 'v', '0', '0', '.', 't', 'x', 't', 0x0D};							//Abre ou cria um arquivo, com nome de AV#.txt, 0 <#< 100
uint8_t comandoescrever[t_c_escr]		=	{ 0x08, 0x20, 0x00, 0x00, 0x00, 0x01, 0x0D};											//Comando de iniciar a escrita de até 256 byte
uint8_t comandofechar[t_c_fech]			=	{ 0x0A, 0x20, 'a', 'v', '0', '0', '.', 't', 'x', 't', 0x0D};							//Fecha arquivo AV#.txt
uint8_t comandocabecario[t_cabe]		=	{ 0x08, 0x20, 0x00, 0x00, 0x00, 53, 0x0D,	//54, 0x0D,									//Formata o cabeçario do arquivo
										'T',  'e',  'm',  'p',  'o',  '(',  's',  ')',  0x09,
										'V',  'e',  'l',  '(',  'K',  'm',  '/',  'h',  ')',  0x09,
										'D',  'i',  's',  't',  'a',  'n',  'c',  'i',  'a',  '(',  'm',  ')',  0x09,
										'H',  'o',  'm',  'o',  '(',  'R',  'p',  'm',  ')',  0x09,
										'M',  'o',  't',  'o',  'r',  '(',  'R',  'p',  'm',  ')',  0x0D};//,  0x10};			//Aqui deveria terminar com 0x0D

void inic_vdip()
{
	/*
		Configura-se o VDIP para comandos curtos, em bytes, com echo'E' tendo o Baud Rate de 9600 bps (default)
	*/

	uint8_t i = 0;												//Variavel auxialiar
	DDRB	&=	~(1<<CTS);
	DDRB	|= (1 << RTS);										//PIND2 saida e PIND0 entrada
	PORTB	|= (1 << CTS);										//Resistor PULL-UP em PIND0
	PORTB	&= ~(1 << RTS);										//Habilita a comunicação
	
	_delay_ms(100);												//Aguarda estabilização
	
	while(i < t_c_inic)
	{
		if ((PINB & (1 << CTS)) == 0)							//Somente transmisse se o VDIP permite
		{
			usart_send(comandoinicializa[i]);
			i ++;
		}
	}															//Transmite via Serial os comando necessários para iniciar o VDIP
	_delay_ms(100);
}

void abrir_arquivo(uint8_t digito1, uint8_t digito2)
{
	comandoabrir[4]  = digito1;
	comandoabrir[5]	 = digito2;
	comandofechar[4] = digito1;
	comandofechar[5] = digito2;												//Atualizando o numero do arquivo txt, para abrir e fechar o mesmo arq

	uint8_t i = 0;															//Variavel local auxiliar
	while (i < t_c_abrir)
	{
		if ((PINB & (1 << CTS)) == 0)										//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandoabrir[i]);								//Transmite o comando para o Vdip
			i ++;															//Desloca o array do comandoabrir
		}
	}																		//Transmite todo o comandoabrir
}

void escrever_cabecario()
{
	uint8_t i = 0;

	while (i < t_cabe)
	{
		if ((PINB & (1 << CTS)) == 0)										//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandocabecario[i]);							//Transmite o comando para o Vdip
			i ++;															//Desloca o array do comandoescrever
		}
	}
	_delay_ms(10);
}

void escrever_arquivo(uint8_t *data, uint8_t pos_INI, uint8_t pos_FIN)
{
	uint8_t i = 0;															//Variavel local auxiliar

	comandoescrever[5] = pos_FIN - pos_INI + 1;								//Quantidade de dados que serão escritos no arquivo

	/*
	 *	Primeiramente, insere os comandos indicando que vai ser escrever no arquivo
	 */

	while (i < t_c_escr)
	{
		if ((PINB & (1 << CTS)) == 0)										//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandoescrever[i]);								//Transmite o comando para o Vdip
			i ++;															//Desloca o array do comandoescrever
		}
	}

	/*
	 *	Em seguida, se escreve cada caracter para o arquivo
	 */

	i = 0;																	//Inicializa o indice para o ponteiro data
	while(i < comandoescrever[5])
	{
		if ((PINB & (1 << CTS)) == 0)										//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(data[pos_INI + i]);								//Transmite o comando para o Vdip
			i ++;															//Desloca o array do ponteiro data
		}
	}
}

void fechar_arquivo()
{
	uint8_t i = 0;															//Variavel local auxiliar

	while (i < t_c_fech)
	{
		if ((PINB & (1 << CTS)) == 0)										//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandofechar[i]);								//Transmite o comando para o Vdip
			i ++;															//Desloca o array do comandofechar
		}
	}
}

void baud_rate_57600()
{
	uint8_t i = 0;

	while (i < t_c_57600)
	{
		if ((PINB & (1 << CTS)) == 0)									//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandobaud57600[i]);							//Transmite o comando para o Vdip
			i ++;														//Desloca o array do comandoescrever
		}
	}
	_delay_ms(50);														//Espera estabilizar
}

void baud_rate_9600()
{
	uint8_t i = 0;

	while (i < t_c_9600)
	{
		if ((PINB & (1 << CTS)) == 0)									//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandobaud9600[i]);								//Transmite o comando para o Vdip
			i ++;														//Desloca o array do comandoescrever
		}
	}
	_delay_ms(50);														//Espera estabilizar
}


void baud_rate_115200()
{
	uint8_t i = 0;

	while (i < t_c_115200)
	{
		if ((PINB & (1 << CTS)) == 0)									//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandobaud115200[i]);								//Transmite o comando para o Vdip
			i ++;														//Desloca o array do comandoescrever
		}
	}
	_delay_ms(100);														//Espera estabilizar
}

void baud_rate_230400()
{
	uint8_t i = 0;

	while (i < t_c_230400)
	{
		if ((PINB & (1 << CTS)) == 0)									//Espera o Vdip liberar a transmissão de comandos
		{
			usart_send(comandobaud230400[i]);								//Transmite o comando para o Vdip
			i ++;														//Desloca o array do comandoescrever
		}
	}
	_delay_ms(100);														//Espera estabilizar
}