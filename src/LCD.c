/*
 * Driver for HD4470 display using Mongoose OS and MCP23017 gpio expander
 *
 * Marcus Niekraszewicz
 * 2018
 *
 */

#include "gpio.h"
#include "LCD.h"
#include "mgos.h"
#include "stdint.h"

/////////////////////////////////////////
//Wait until the LCD is not busy
//
//in:	none
//out:	none
////////////////////////////////////////
void lcd_busy_wait()
//do nothing until LCD is no longer busy
//(not used in this implementation)
{
	pinMode(D7, INPUT);

	digitalWrite(LCDRS, 0);
	digitalWrite(LCDE, 1);

	while (digitalRead(D7) == 1)
	{
		cs_log_printf("LCD Busy\n");
		digitalWrite(LCDE, 0);
		digitalWrite(LCDE, 1);
		mgos_msleep(1);
	}	
	
	digitalWrite(LCDE, 0);
	pinMode(D7, OUTPUT);
}

///////////////////////////////////////////////////////////
//Write a character to the LCD
//
//in:	lcd_data: the register to write character data from
//	write_mode: either BITS8 or BITS4
//out:	none
///////////////////////////////////////////////////////////
void write_char(char lcd_data, mode write_mode)
//write ASCII character to 8 bit LCD data pins and call hardware write
{
    	uint16_t gpio_state=digitalReadAll();     //current state of GPIO pins
        uint16_t gpio_mask=0xFFC3;              //mask pins A5-A2 used for LCD data
	uint8_t gpio_write;
	
	uint8_t lcd_data_rev=0;

	//first reverse the bits, because I wired the display weirdly
	for (int i = 0; i < 8; i++) { 
        	if((lcd_data & (1 << i))) {
           		lcd_data_rev |= 1 << (7 - i);   
		}
   	} 
        cs_log_printf("Writing character %d to LCD\n",lcd_data);
	cs_log_printf("Writing bits %d to LCD\n",lcd_data_rev);

	lcd_data=lcd_data_rev;

        switch (write_mode){
                case BITS8:
                        //we only use 4-bit for this device, so drop the 4 least significant bits and write to gpio     
			gpio_write=(gpio_state & gpio_mask)|((lcd_data & 0x0F) << 2);
			digitalWriteAll(gpio_write);
                        break;
                case BITS4:
                        //write MSB first, then LSB
                        gpio_write=(gpio_state & gpio_mask)|((lcd_data & 0x0F) << 2);
                        digitalWriteAll(gpio_write);

                        digitalWrite(LCDRS, 1);
                        digitalWrite(LCDE,1);
                        mgos_msleep(1);
                        digitalWrite(LCDE,0);
			mgos_msleep(1);

                        gpio_write=(gpio_state & gpio_mask)|((lcd_data & 0xF0) >> 2);
			digitalWriteAll(gpio_write);
			break;

                default:
                        break;
        }

    	digitalWrite(LCDRS, 1); 
    	digitalWrite(LCDE,1);
	mgos_msleep(1);
    	digitalWrite(LCDE,0);
	//lcd_busy_wait();
	mgos_msleep(1);
}


////////////////////////////////////////
//write an instruction to the LCD
//in: 	instr: the instruction to write
//	write_mode: either BITS8 or BITS4
//out: 	none
////////////////////////////////////////
void write_inst(LCD_instr inst,mode write_mode)
//write instruction to 8 bit LCD data pins and call hardware write
{
	uint16_t gpio_state=digitalReadAll();	//current state of GPIO pins
	uint16_t gpio_mask=0xFFC3;		//mask pins A5-A2 used for LCD data
	uint8_t gpio_write;
	
	switch (write_mode){
		case BITS8:
			//we only use 4-bit for this device, so drop the 4 least significant bits and write to gpio	
			cs_log_printf("Writing instruction %d to LCD\n",inst);
			gpio_write=(gpio_state & gpio_mask)|((inst & 0x0F) << 2);
			digitalWriteAll(gpio_write);
			break;
		case BITS4:
			//write MSB first, then LSB
			cs_log_printf("Writing instruction %d to LCD\n",inst);

			gpio_write=(gpio_state & gpio_mask)|((inst & 0x0F) << 2);
                        digitalWriteAll(gpio_write);
		
		        digitalWrite(LCDRS, 0);
		        digitalWrite(LCDE,1);
			mgos_msleep(1);
		        digitalWrite(LCDE,0);
			mgos_msleep(1);

			gpio_write=(gpio_state & gpio_mask)|((inst & 0xF0) >> 2);
                        digitalWriteAll(gpio_write);
		default:
			break;
	}
    	
    	digitalWrite(LCDRS, 0); 
    	digitalWrite(LCDE,1);
	mgos_msleep(1);
    	digitalWrite(LCDE,0);
	mgos_msleep(1);
}

////////////////////////////////////////////////////
//Write a line to the LCD
//
//in:	lcd_data: a string of characters to write
//out: 	none
///////////////////////////////////////////////////
void write_line(char lcd_data[])
{
	//reset line first
	write_inst(CURSOR_RETURN,BITS4);
	for (int i=0;lcd_data[i]!='\0';i++){
		write_char(lcd_data[i],BITS4);
	}
}

////////////////////////////////////////////////////
//Initialize the LCD
///////////////////////////////////////////////////
void init_lcd()
{
//initialize GPIO pins as output
	pinMode(LCDRS,OUTPUT);
	pinMode(LCDE,OUTPUT);
	pinMode(D4,OUTPUT);
	pinMode(D5,OUTPUT);
	pinMode(D6,OUTPUT);
	pinMode(D7,OUTPUT);

//make sure we've waited 50ms after power on
	mgos_msleep(50);

//startup procedure
	write_inst(INITIALIZE,BITS8);
	mgos_msleep(20);
	write_inst(INITIALIZE,BITS8);
        mgos_msleep(20);
        write_inst(INITIALIZE,BITS8);
        mgos_msleep(20);
	write_inst(CONFIGURE_SCREEN_4BIT,BITS8);
	mgos_msleep(10);
	write_inst(CONFIGURE_SCREEN_4BIT,BITS4);
	mgos_msleep(5);
	write_inst(SCREEN_CLEAR,BITS4);
	mgos_msleep(5);

}


