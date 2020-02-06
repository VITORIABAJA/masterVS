/*
 * amostragem.c
 *
 * Created: 03/06/2019 00:40:17
 *  Author: rasle
 */ 

#include "amostragem.h"

uint8_t	veloc_msb	=0;
uint8_t	veloc_lsb	=0;
uint8_t desloc_msb	=0;
uint8_t	desloc_lsb	=0;
uint8_t rotacao_msb	=0;
uint8_t rotacao_lsb	=0;

volatile uint16_t tempo		= 0;
volatile uint16_t vel_kmh	= 0;
volatile uint16_t dente		= 0;
volatile uint16_t desloc	= 0;
volatile uint16_t inicio	= 0;
volatile uint16_t distancia	= 0;
volatile uint16_t homo_rpm	= 0;
volatile uint16_t motor_rpm	= 0;


ISR(TIMER1_OVF_vect){
	TCNT1	= 55535;
	//PORTB |= (1<<1);
	
	//TempoM TempoL
	//Vel
	//DeslocamentoM Deslocamento L
	//MotorM MotorL
	tempo	+= 4;
	if (tempo>=65535)
	{
		tempo=0;
	}
	//desloc	+= 7;
	
	twi_master_start( (0x2<<1)+1 );
	
	veloc_msb	= twi_master_read_ack();
	veloc_lsb	= twi_master_read_ack();

	desloc_msb	= twi_master_read_ack();
	desloc_lsb	= twi_master_read_ack();
	
	rotacao_msb	= twi_master_read_ack();
	rotacao_lsb	= twi_master_read_nack();
	
	twi_master_stop();
	
	if (tempo<=5)
	{
		inicio = (uint16_t) (desloc_msb<<8)		|	(desloc_lsb);
	}
	distancia = (uint16_t) (desloc_msb<<8)		|	(desloc_lsb);
	
	dados_main[0] = (tempo);
	dados_main[1] = (uint16_t) (veloc_msb<<8)		|	(veloc_lsb);
	
	if(distancia>=inicio)
	{
		dados_main[2] =	(distancia - inicio);
	}
	else{
		dados_main[2] =	(distancia+65535- inicio);
	}
	
	dados_main[3] = (uint16_t) ( (veloc_msb<<8)		|	(veloc_lsb) )*(9.25926);
	dados_main[4] =	(uint16_t) (rotacao_msb<<8)		|	(rotacao_lsb);
	flag_dados	  = 1;
}

void inic_timer1(){
	/*
		Tempo de Overflow de 0,04s
		TOVF = PRESCALER(65535 - TCNT1)/16Mhz
		TOVF = 64(65535 - 55535)/16000000
	*/
	TIMSK1 	= 0b00000001;					//Habilita Interrupção
	TCNT1	= 55535;						//10000 incrementos
	TCCR1B	= 0b00000011;					//Prescaler 64
}

void inic_leds(){
	DDRB |= (3<<1);					//Seta como saida as portas PB0 (14) e PB1 (15)
	//PORTB &= ~(3<<1);
}

void inic_amostragem(){
	uint8_t i;
	tempo		= 0;
	desloc		= 0;
	inicio	= 0;
	distancia	= 0;
	vel_kmh		= 0;
	dente		= 0;
	homo_rpm	= 0;
	motor_rpm	= 0;
	flag_dados	= 0;
	for (i=0; i<5; i++)
	{
		dados_main[i]=0;
	}
	inic_timer1();
}

void fim_amostragem(){
	TCCR1B = 0;
	TIMSK1 = 0;
}

