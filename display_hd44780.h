/*
 * display_hd44780.h
 *
 * Created: 22/08/2019 23:10:20
 *  Author: rasle
 */ 


#ifndef DISPLAY_HD44780_H_
#define DISPLAY_HD44780_H_


#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#define BAUD 9600UL
#define UBBR_9600 ((F_CPU/(BAUD<<4))-1)							//Define a velocidade da Serial (default do VDIP)
#define MYUBBR UBBR_9600

#define DB4		4            /**< pin for 4bit data bit 0  */
#define DB5		5            /**< pin for 4bit data bit 1  */
#define DB6		6            /**< pin for 4bit data bit 2  */
#define DB7		7            /**< pin for 4bit data bit 3  */
#define RS      0            /**< pin  for RS line         */
#define RW      1            /**< pin  for RW line         */
#define EN		2            /**< pin  for Enable line     */
#define BK		3            /**< pin  for backlight             */

#define CONF_1D_1C_1B 0x0FU
#define CONF_1D_1C_0B 0x0EU
#define CONF_1D_0C_0B 0x0CU
#define CONF_0D_0C_0B 0x08U
#define LCD_DDRAM 7
#define LINHA0 0x00
#define LINHA1 0x40
#define LINHA2 0x14
#define LINHA3 0x54

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "twi.h"

uint8_t dataport;
uint8_t pin_dados[4];
uint8_t en;
uint8_t rw;
uint8_t rs;
uint8_t bk;

void pcf8574_send(uint8_t byte );

void pulso();

void send_comando(uint8_t byte);

void send_dados(uint8_t byte);

void pos_cur_xy(uint8_t x, uint8_t y);

void inic_h44780();

void send_string_display(char palavra[100]);

void send_char_display(uint8_t entrada);

void send_number(uint16_t n);

void clear_disp();

void home_disp();

void reset_bk();

void set_bk();

void conf_disp(uint8_t conf);

void tela_inicial();

void tela_Abrir(uint8_t digito1, uint8_t digito2);

void tela_Fechar(uint8_t digito1, uint8_t digito2);

void tela_dados();

void tela_atualiza_dados(uint8_t *dados,uint8_t linha);

void inicia_VB();

void abrindo_VB();

void fecha_VB();

void dados_VB();

void atualiza_VB(uint8_t *dados);

#endif /* DISPLAY_HD44780_H_ */