/*
 * Driver for MCP23017 for GPIO functions over I2C
 *
 * Marcus Niekraszewicz
 * 2018
 */

#include "mgos.h"
#include "mgos_i2c.h"

//////////////////////////////////////////////////////////////////////
//Constants
//////////////////////////////////////////////////////////////////////

#define I2C_ADRS 0x42	//device address=0100001x

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

//GPIO pins
//[REG,BTIMASK]
#define IO_A0 0x0901
#define IO_A1 0x0902
#define IO_A2 0x0904
#define IO_A3 0x0908
#define IO_A4 0x0910
#define IO_A5 0x0920
#define IO_A6 0x0940
#define IO_A7 0x0980
#define IO_B0 0x1901
#define IO_B1 0x1902
#define IO_B2 0x1904
#define IO_B3 0x1908
#define IO_B4 0x1910
#define IO_B5 0x1920
#define IO_B6 0x1940
#define IO_B7 0x1980

//Arduino style pin mode
const enum io_mode{
	INPUT,
	OUTPUT
};

//Arduino style pin levels
const enum io_level{
	HIGH,
	LOW
};

//////////////////////////////////////////////////////////////////////

//Return I2C address for writing
uint8_t write_adrs(void){
	return (I2C_ADRS&0xFE);
}

//Return I2C address for reading
uint8_t read_adrs(void){
	return (I2C_ADRS|0x01);
}

//Read all pins
//Returns a bitmask of GPIO [B7..B0,A7..A0]
uint16_t digitalReadReg(void){

	void *i2c = mgos_i2c_get_global();

	//read bank A
	uint8_t gpio_a = mgos_i2c_read_reg_b(i2c,read_adrs(),GPIOA);
	if (gpio_a==0){
		LOG(LL_ERROR,("Error reading GPIO bank A state"));
	}
	
	//read bank B
	uint8_t gpio_b = = mgos_i2c_read_reg_b(i2c,read_adrs(),GPIOB);
	if (gpio_b==0){
                LOG(LL_ERROR,("Error reading GPIO bank B state"));
        }

	mgos_i2c_close(i2c);

	return ((gpio_b << 8) | gpio_a);

}

//Write to all pins
//pin_mask: bitmask to write, in format {GPIO bank B,GPIO bank A}
void digitalWriteAll(uint16_t pin_mask){

}

//initialize the device
void gpio_init(void){

	//BANK=1
	//MIRROR=1
	//SEQOP=1
	//DISSLW=0
	//HAEN=1
	//ODR=1

	uint8_t conf_reg = 0b11101110;
	void *i2c = mgos_i2c_get_global();

	if(mgos_i2c_write_reg_b(i2c,write_adrs(),IOCONA,conf_reg)==0){
		if(mgos_i2c_write_reg_b(i2c,write_adrs(),IOCONB,conf_reg)==0){
			LOG(LL_ERROR,("Error writing GPIO config"));
		}
	}
			
	mgos_i2c_close(i2c);

}

//Arduino-style pin mode select
//pin: the pin to set
//mode: INPUT/OUTPUT
void pinMode(uint16_t pin,io_mode mode){

	uint8_t dir_reg = ((pin >> 8)&0xF0);    //direction register, IODIRA or IODIRB
	uint8_t write_mask = pin|0x00FF;	//bit mask for writing to register
	void *i2c = mgos_i2c_get_global();

        uint8_t cur_mask = mgos_i2c_read_reg_b(i2c,read_adrs(),dir_reg);             //current register state


	switch(mode){
		//Input -> set bits
		case INPUT:
			mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask|write_mask));
		//Output -> unset bits
		case OUTPUT:
                        mgos_i2c_write_reg_b(i2c,write_adrs(),dir_reg,(cur_mask&(~write_mask)));

	}

	mgos_i2c_close(i2c);
	
}

//Arduino-style pin write
//pin: the pin to write
//level: HIGH/LOW
void digitalWrite(uint16_t pin,io_level level){

}

//Arduino-style pin read
//pin: the pin to read
//returns HIGH/LOW
io_level digitalRead(uint16_t pin){

}

