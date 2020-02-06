/*
 * twi.c
 *
 * Created: 27/05/2019 15:51:50
 *  Author: rasle
 */ 

#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

 
#include "twi.h"

#define F_SCL 100000UL // SCL frequency
#define Prescaler 1
#define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

void	inic_twi_master()
{
	TWBR = (uint8_t)TWBR_val;
}

uint8_t twi_master_start(uint8_t address)
{
	// reset TWI control register
	TWCR = 0;
	// transmit START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	
	// check if the start condition was successfully transmitted
	if((TWSR & 0xF8) != TW_START){ return 1; }
	
	// load slave address into data register
	TWDR = address;
	// start transmission of address
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	
	// check if the device has acknowledged the READ / WRITE mode
	uint8_t twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
	
	return 0;
}

uint8_t twi_master_send_byte(uint8_t dados)
{
	// load data into data register
	TWDR = dados;
	// start transmission of data
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){ return 1; }
	
	return 0;
}

uint8_t twi_master_read_ack()
{
	
	// start TWI module and acknowledge data after reception
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	// return received data from TWDR
	return TWDR;
}

uint8_t twi_master_read_nack()
{
	
	// start receiving without acknowledging reception
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	// return received data from TWDR
	return TWDR;
}

uint8_t twi_master_send(uint8_t address, uint8_t* dados, uint16_t length)
{
	if (twi_master_start(address | I2C_WRITE)) return 1;
	
	for (uint16_t i = 0; i < length; i++)
	{
		if (twi_master_send_byte(dados[i])) return 1;
	}
	
	twi_master_stop();
	
	return 0;
}

uint8_t twi_master_read(uint8_t address, uint8_t* dados, uint16_t length)
{
	if (twi_master_start(address | I2C_READ)) return 1;
	
	for (uint16_t i = 0; i < (length-1); i++)
	{
		dados[i] = twi_master_read_ack();
	}
	dados[(length-1)] = twi_master_read_nack();
	
	twi_master_stop();
	
	return 0;
}

uint8_t twi_master_write_reg(uint8_t devaddr, uint8_t regaddr, uint8_t* dados, uint16_t length)
{
	if (twi_master_start(devaddr | 0x00)) return 1;

	twi_master_send_byte(regaddr);

	for (uint16_t i = 0; i < length; i++)
	{
		if (twi_master_send_byte(dados[i])) return 1;
	}

	twi_master_stop();

	return 0;
}

uint8_t twi_master_read_reg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
	if (twi_master_start(devaddr)) return 1;

	twi_master_send_byte(regaddr);

	if (twi_master_start(devaddr | 0x01)) return 1;

	for (uint16_t i = 0; i < (length-1); i++)
	{
		data[i] = twi_master_read_ack();
	}
	data[(length-1)] = twi_master_read_nack();

	twi_master_stop();

	return 0;
}

void	twi_master_stop()
{
	// transmit STOP condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void inic_twi_slave(uint8_t address){
	// load address into TWI address register
	TWAR = (address << 1);
	// set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
}

void	twi_slave_stop(){
	// clear acknowledge and enable bits
	TWCR &= ~( (1<<TWEA) | (1<<TWEN) );
}

ISR(TWI_vect){
	
	// temporary stores the received data
	uint8_t dados;
	
	// own address has been acknowledged
	if( (TWSR & 0xF8) == TW_SR_SLA_ACK ){
		pos_dados = 0xFF;
		// clear TWI interrupt flag, prepare to receive next byte and acknowledge
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	
	else if( (TWSR & 0xF8) == TW_ST_SLA_ACK ){
		pos_dados = 0x00;
		TWDR = dados_tx[0];
		// clear TWI interrupt flag, prepare to receive next byte and acknowledge
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	
	else if( (TWSR & 0xF8) == TW_SR_DATA_ACK ){ // data has been received in slave receiver mode
		
		// save the received byte inside data
		dados = TWDR;
		
		// check wether an address has already been transmitted or not
		if(pos_dados == 0x08){
			
			pos_dados = 0;
			
			// clear TWI interrupt flag, prepare to receive next byte and acknowledge
			TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		}
		else{ // if a databyte has already been received
			
			// store the data at the current address
			dados_rx[pos_dados] = dados;
			
			// increment the buffer address
			pos_dados++;
			
			// if there is still enough space inside the buffer
			if(pos_dados < 0x08){
				// clear TWI interrupt flag, prepare to receive next byte and acknowledge
				TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			}
			else{
				// Don't acknowledge
				TWCR &= ~(1<<TWEA);
				// clear TWI interrupt flag, prepare to receive last byte.
				TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEN);
			}
		}
	}
	
	
	else if( (TWSR & 0xF8) == TW_ST_DATA_ACK ){ // device has been addressed to be a transmitter
		
		// copy data from TWDR to the temporary memory
		//data = TWDR;
		
		pos_dados++;
		
		// if no buffer read address has been sent yet
		//if( buffer_address == 0x08 ){
		//	buffer_address = 0;
		//}
		
		// copy the specified buffer address into the TWDR register for transmission
		
		// increment buffer read address
		
		
		// if there is another buffer address that can be sent
		if(pos_dados < 0x07){
			TWDR = dados_tx[pos_dados];
			// clear TWI interrupt flag, prepare to send next byte and receive acknowledge
			TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		}
		else{
			// Don't acknowledge
			TWCR &= ~(1<<TWEA);
			// clear TWI interrupt flag, prepare to receive last byte.
			TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEN);
		}
		
	}
	
	
	else{
		// if none of the above apply prepare TWI to be addressed again
		TWCR |= (1<<TWIE) | (1<<TWEA) | (1<<TWEN);
	}
}