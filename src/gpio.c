/*
 * Driver for MCP23017 for GPIO functions over I2C
 *
 * Marcus Niekraszewicz
 * 2018
 */

#include "mgos.h"
#include "mgos_i2c.h"
#include "gpio.h"
#include "stdint.h"

//////////////////////////////////////////////////////////////////////
//Constants
//////////////////////////////////////////////////////////////////////

#define I2C_ADRS 0x21	//device address + R/W bit: 0100001x

//MCP23017 registers
#define IODIRA 0x00	//I/O direction 
#define IODIRB 0x10
#define IPOLA 0x01	//Input polarity
#define IPOLB 0x11	
#define GPINTENA 0x02	//Interrupt-on-change enable
#define GPINTENB 0x12	
#define DEFVALA 0x03	//Interrupt default value
#define DEFVALB 0x13	
#define INTCONA 0x04	//Interrupt control
#define INTCONB 0x14	
#define IOCONA 0x05	//Device configuration
#define IOCONB 0x15	
#define GPPUA 0x06	//Pull-up resistor configuration
#define GPPUB 0x16	
#define INTFA 0x07	//Interrupt flag
#define INTFB 0x17	
#define INTCAPA 0x08	//Interrupt capture
#define INTCAPB 0x18	
#define GPIOA 0x09	//Port value
#define GPIOB 0x19	
#define OLATA 0x0A	//Output latch value
#define OLATB 0x1A	

//////////////////////////////////////////////////////////////////////

//Return I2C address for writing
uint8_t write_adrs(void){
	return (I2C_ADRS);
}

//Return I2C address for reading
uint8_t read_adrs(void){
	return (I2C_ADRS);
}

//Read all pins
//Returns a bitmask of GPIO [B7..B0,A7..A0]
uint16_t digitalReadAll(void){

	struct mgos_i2c *i2c = mgos_i2c_get_global();

	//read bank A
	int gpio_a = mgos_i2c_read_reg_b(i2c,read_adrs(),GPIOA);
	if (gpio_a==-1){
		LOG(LL_ERROR,("Error reading GPIO bank A state"));
		return 0;
	}
	
	//read bank B
	int gpio_b = mgos_i2c_read_reg_b(i2c,read_adrs(),GPIOB);
	if (gpio_b==-1){
                LOG(LL_ERROR,("Error reading GPIO bank B state"));
		return 0;
        }

	uint8_t out_b=gpio_b&0xFF;
	uint8_t out_a=gpio_a&0xFF;
	return ((out_b << 8) | out_a);

}

//Write to all pins
//pin_mask: bitmask to write, in format {GPIO bank B,GPIO bank A}
void digitalWriteAll(uint16_t pin_mask){
	
	struct mgos_i2c *i2c = mgos_i2c_get_global();
        uint8_t pins_b = (pin_mask >> 8);    //mask for GPIO bank B
        uint8_t pins_a = pin_mask & 0x00FF;        //mask for GPIO bank A

	cs_log_printf("Writing %d to bank A\n",pins_a);
	cs_log_printf("Writing %d to bank B\n",pins_b);

        //write bank A
        _Bool gpio_a = mgos_i2c_write_reg_b(i2c,write_adrs(),OLATA,pins_a);
        if (!gpio_a){
                LOG(LL_ERROR,("Error writing GPIO bank A state"));
        }

        //write bank B
        _Bool gpio_b = mgos_i2c_write_reg_b(i2c,write_adrs(),OLATB,pins_b);
        if (!gpio_b){
                LOG(LL_ERROR,("Error writing GPIO bank B state"));
        }

}

//initialize the device
void gpio_init(void){

	//BANK=1
	//MIRROR=1
	//SEQOP=1
	//DISSLW=0
	//HAEN=1
	//ODR=1
	//INTPOL=1

	uint8_t conf_reg = 0b11101110;
	void *i2c = mgos_i2c_get_global();

	if (i2c==NULL){
		LOG(LL_ERROR,("I2C not configured"));
	}

	if(mgos_i2c_write_reg_b(i2c,write_adrs(),0x0A,conf_reg)==1){	//write to 0x0A first, depending on initial config will either set IOCON or OLATA
		if(mgos_i2c_write_reg_b(i2c,write_adrs(),IOCONA,conf_reg)==0){
			LOG(LL_ERROR,("Error writing GPIO config"));
		}
	}else{
		LOG(LL_ERROR,("Error writing GPIO config"));
	}

	digitalWriteAll(0x0000);
			
}

//Arduino-style pin mode select
//pin: the pin to set
//mode: INPUT/OUTPUT
void pinMode(uint16_t pin, _Bool mode){

	uint8_t dir_reg = ((pin >> 8)&0xF0);    //direction register, IODIRA or IODIRB
	uint8_t write_mask = pin&0x00FF;	//bit mask for writing to register
	struct mgos_i2c *i2c = mgos_i2c_get_global();

        uint8_t cur_mask = mgos_i2c_read_reg_b(i2c,read_adrs(),dir_reg);             //current register state

	switch(mode){
		//Input -> set bits
		case INPUT:
			mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask|write_mask));
			break;
		//Output -> unset bits
		case OUTPUT:
                        mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask&(~write_mask)));
			break;

	}

	
}

//Arduino-style pin write
//pin: the pin to write
//level: HIGH/LOW
void digitalWrite(uint16_t pin, _Bool level){

	uint8_t dir_reg = ((pin >> 8)|0x01);    	//output latch register, OLATA or OLATB
        uint8_t write_mask = pin&0x00FF;        //bit mask for writing to register
        struct mgos_i2c *i2c = mgos_i2c_get_global();

        uint8_t cur_mask = mgos_i2c_read_reg_b(i2c,read_adrs(),dir_reg);             //current register state


        switch(level){
                //Input -> set bits
                case HIGH:
                        mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask|write_mask));
			break;
                //Output -> unset bits
                case LOW:
                        mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask&(~write_mask)));
			break;

        }



}

//Arduino-style pin read
//pin: the pin to read
//returns HIGH/LOW
_Bool digitalRead(uint16_t pin){

	uint8_t dir_reg = (pin >> 8);           //IO register, GPIOA or GPIOB
        uint8_t read_mask = pin&0x00FF;        //bit mask for reading from register
        struct mgos_i2c *i2c = mgos_i2c_get_global();

        uint8_t cur_mask = mgos_i2c_read_reg_b(i2c,read_adrs(),dir_reg);             //current register state

	return ((cur_mask&read_mask) && 1);

}

