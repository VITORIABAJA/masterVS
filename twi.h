/*
 * twi.h
 *
 * Created: 27/05/2019 15:52:01
 *  Author: rasle
 */ 


#ifndef TWI_H_
#define TWI_H_

#define I2C_READ 0x01
#define I2C_WRITE 0x00

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

void	inic_twi_master();
uint8_t twi_master_start(uint8_t address);
uint8_t twi_master_send_byte(uint8_t data);
uint8_t twi_master_read_ack();
uint8_t twi_master_read_nack();
uint8_t twi_master_send(uint8_t address, uint8_t* data, uint16_t length);
uint8_t twi_master_read(uint8_t address, uint8_t* data, uint16_t length);
uint8_t twi_master_write_reg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length);
uint8_t twi_master_read_reg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length);
void	twi_slave_stop();

volatile uint8_t pos_dados;
volatile uint8_t dados_tx[0xFF];
volatile uint8_t dados_rx[0xFF];


#endif /* TWI_H_ */