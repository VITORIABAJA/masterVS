/*
 * display_hd44780.c
 *
 * Created: 22/08/2019 23:09:56
 *  Author: rasle
 */ 

#include "display_hd44780.h"



void pcf8574_send(uint8_t byte ){
	twi_master_start((0x3F<<1));
	twi_master_send_byte(byte);
	twi_master_stop();
}

void pulso(){
	dataport |= en;
	pcf8574_send(dataport);
	_delay_us(1);
	dataport &= ~en;
	pcf8574_send(dataport);
	_delay_us(1);
}

void send_comando(uint8_t byte){
	uint8_t h_nibble = (byte&0xF0);
	uint8_t l_nibble = (byte&0x0F)<<4;
	
	dataport = h_nibble;			//Digito
	pcf8574_send(dataport);
	pulso();
	dataport = l_nibble;
	pcf8574_send(dataport);
	pulso();
	_delay_ms(2);
}

void send_dados(uint8_t byte){
	
	uint8_t h_nibble = (byte&0xF0);
	uint8_t l_nibble = (byte&0x0F)<<4;
	
	dataport = rs | h_nibble;			//Digito
	pcf8574_send(dataport);
	pulso();
	dataport = bk | rs | l_nibble;
	pcf8574_send(dataport);
	pulso();
	_delay_ms(2);
}

void pos_cur_xy(uint8_t x, uint8_t y){
	if ( y==0 )
	send_comando((1<<LCD_DDRAM)| (LINHA0+x) );
	else if ( y==1)
	send_comando((1<<LCD_DDRAM)| (LINHA1+x) );
	else if ( y==2)
	send_comando((1<<LCD_DDRAM)+LINHA2+x);
	else /* y==3 */
	send_comando((1<<LCD_DDRAM)+LINHA3+x);
}

void inic_h44780(){
	
	dataport		= 0;
	pin_dados[0]	= (1<<DB4);
	pin_dados[1]	= (1<<DB5);
	pin_dados[2]	= (1<<DB6);
	pin_dados[3]	= (1<<DB7);
	en	= (1<<EN);
	rw	= (1<<RW);
	rs	= (1<<RS);
	bk	= (1<<BK);
	
	_delay_ms(100);
	
	dataport = 0x03;
	pcf8574_send(dataport);		//Function Set - 8Bit
	pulso();
	_delay_us(4500);			//Delay 4.1ms
	
	pcf8574_send(dataport);		//Function Set - 8Bit
	pulso();
	_delay_us(150);				//Delay 100us
	
	pcf8574_send(dataport);		//Function Set - 8Bit
	pulso();
	_delay_us(150);				//Delay 100us
	
	//Set for 4 bits
	dataport = (0x02);
	pcf8574_send(dataport);
	pulso();
	_delay_us(150);				//Delay 100us
	
	//Function Set - Entry mode, 2 lines, font 10 bit
	dataport = (0x02);
	pcf8574_send(dataport);
	pulso();
	dataport = (1<<DB7) | (1<<DB6);
	pcf8574_send(dataport);
	pulso();
	_delay_us(2000);			//Delay 60us
	
	//Display Off
	send_comando(0x08);
	
	//Display clear
	clear_disp();	
	
	//Entry Mode Set
	send_comando(0b0110);
	
	//display on, cursor on, blink char
	conf_disp(CONF_1D_0C_0B);
	
	//Home
	send_comando(0x02);

	
	//backlight on
	dataport = bk;
	pcf8574_send(dataport);
	pulso();
	dataport = bk;
	pcf8574_send(dataport);
	pulso();
	_delay_ms(2);
}

void send_string_display(char palavra[100]){
	unsigned char j = 0;
	while(palavra[j] != '\0'){
		send_dados(palavra[j]);
		j++;
	}
}

void send_char_display(uint8_t entrada){
	char palavra[10];
	itoa(entrada,palavra,10);
	
	unsigned char j=0;
	while(palavra[j] != '\0'){
		send_dados(palavra[j]);
		j++;
	}
	
	_delay_ms(2);
}

void send_number(uint16_t n){
	if(n>999){
		send_dados(48+n/1000);
		send_dados(48+(n%1000)/100);
		send_dados(48+(n%100)/10);
		send_dados(48+n%10);
	}
	else if(n>99){
		send_dados(48+n/100);
		send_dados(48+(n%100)/10);
		send_dados(48+n%10);
	}
	else if(n>9){
		send_dados(48+n/10);
		send_dados(48+n%10);
	}
	else{
		send_dados(48+n);
	}
	_delay_ms(2);
}

void clear_disp(){
	send_comando(0x01);
}

void home_disp(){
	send_comando(0x02);
}

void reset_bk(){
	dataport &= ~bk;			//Digito
	pcf8574_send(dataport);
	pulso();
	dataport &= ~bk;
	pcf8574_send(dataport);
	pulso();
	_delay_ms(2);
}

void set_bk(){
	dataport |=	bk;		//Digito
	pcf8574_send(dataport);
	pulso();
	dataport |= bk;
	pcf8574_send(dataport);
	pulso();
	_delay_ms(2);
}

void conf_disp(uint8_t conf){
	send_comando(conf);
}

void tela_inicial()
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Teste AV");
	send_char_display(1);
	send_char_display(2);
}

void tela_Abrir(uint8_t digito1, uint8_t digito2)
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Abrindo: AV");
	pos_cur_xy(12, 0);
	send_char_display((digito1-48));
	pos_cur_xy(13, 0);
	send_char_display((digito2-48));
	pos_cur_xy(14, 0);
	send_string_display("...");
	pos_cur_xy(0, 2);
	send_string_display("Inicia: ");

	uint8_t i;
	//char i_c[10];
	for(i = 6; i > 0; i--)
	{
		pos_cur_xy(8, 2);
		send_char_display(i);// + 47);
		_delay_ms(1000);
	}
}

void tela_Fechar(uint8_t digito1, uint8_t digito2)
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Fechando AV");
	pos_cur_xy(11, 0);
	send_char_display(digito1-48);
	pos_cur_xy(12, 0);
	send_char_display(digito2-48);
	pos_cur_xy(13, 0);
	send_string_display("...");
}

void tela_dados()
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("d1: ");
	pos_cur_xy(13, 0);
	send_string_display("seg");
	pos_cur_xy(0, 2);
	send_string_display("d2: ");
	pos_cur_xy(13, 2);
	send_string_display("km/h");
}

void tela_atualiza_dados(uint8_t *dados,uint8_t linha)
{
	if(linha == 1)
	{
		pos_cur_xy(4, 0);
		// 		send_char_display(dados[0]-48);
		//
		// 		pos_cursor(col*5, lin*0);
		// 		send_char_display(dados[1]-48);
		//
		// 		pos_cursor(col*6, lin*0);
		// 		send_char_display(dados[2]-48);
		//
		// 		pos_cursor(col*7, lin*0);
		// 		send_string_display(",");//dados[3]);
		//
		// 		pos_cursor(col*8, lin*0);
		// 		send_char_display(dados[4]-48);
		//
		// 		pos_cursor(col*9, lin*0);
		// 		send_char_display(dados[5]-48);
		
		send_dados(dados[0]);
		send_dados(dados[1]);
		send_dados(dados[2]);
		send_dados(dados[3]);
		send_dados(dados[4]);
		send_dados(dados[5]);
	}
	if(linha == 2)
	{
		pos_cur_xy(4, 2);
		// 		send_char_display(dados[8]-48);
		//
		// 		pos_cursor(col*5, lin*2);
		// 		send_char_display(dados[9]-48);
		//
		// 		pos_cursor(col*6, lin*2);
		// 		send_char_display(dados[10]-48);
		//
		// 		pos_cursor(col*7, lin*2);
		// 		send_string_display(",");//dados[11]);
		//
		// 		pos_cursor(col*8, lin*2);
		// 		send_char_display(dados[12]-48);
		send_dados(dados[8]);
		send_dados(dados[9]);
		send_dados(dados[10]);
		send_dados(dados[11]);
		send_dados(dados[12]);
	}
}

void inicia_VB()
{
	clear_disp();
	pos_cur_xy(4, 0);
	send_string_display("Vitoria Baja");
	pos_cur_xy(8, 2);
	send_string_display("VB19");
}

void abrindo_VB(){
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Iniciando VB19");
	pos_cur_xy(1, 0);
	send_string_display("...");

	uint8_t i;
	//char i_c[10];
	for(i = 6; i > 0; i--)
	{
		pos_cur_xy(18, 0);
		send_char_display(i);// + 47);
		_delay_ms(1000);
	}
	};

void fecha_VB()
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Desligando VB19");
	pos_cur_xy(15, 0);
	send_string_display("...");
	_delay_ms(3000);
}

void dados_VB()
{
	clear_disp();
	pos_cur_xy(0, 0);
	send_string_display("Vel ");
	pos_cur_xy(10, 0);
	send_string_display("km/h");
	pos_cur_xy(0, 2);
	send_string_display("Rot ");
	pos_cur_xy(10, 2);
	send_string_display("RPM");
}

void atualiza_VB(uint8_t *dados)
{
		pos_cur_xy(4, 0);		
		send_dados(dados[8]);
		send_dados(dados[9]);
		send_dados(dados[10]);
		send_dados(dados[11]);
		send_dados(dados[12]);
		
		pos_cur_xy(4, 2);
		send_dados(dados[28]);
		send_dados(dados[29]);
		send_dados(dados[30]);
		send_dados(dados[31]);
		send_dados(dados[32]);
}
