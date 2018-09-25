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

        switch (write_mode){
                case BITS8:
                        //we only use 4-bit for this device, so drop the 4 least significant bits and write to gpio     
			cs_log_printf("Writing %d to LCD\n",lcd_data);
                        gpio_mask |= ((lcd_data & 0x0F) << 2);
                        digitalWriteAll(gpio_state & gpio_mask);
                        break;
                case BITS4:
                        //write MSB first, then LSB
                        cs_log_printf("Writing %d to LCD\n",lcd_data);
                        gpio_mask |= ((lcd_data & 0x0F) << 2);
                        digitalWriteAll(gpio_state & gpio_mask);

                        digitalWrite(LCDRS, 1);
                        digitalWrite(LCDE,1);
                        mgos_msleep(1);
                        digitalWrite(LCDE,0);
                        //lcd_busy_wait();
			mgos_msleep(1);

                        gpio_mask |= ((lcd_data & 0xF0) >> 2);
                        digitalWriteAll(gpio_state & gpio_mask);
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
	
	switch (write_mode){
		case BITS8:
			//we only use 4-bit for this device, so drop the 4 least significant bits and write to gpio	
			cs_log_printf("Writing %d to LCD\n",inst);
			gpio_mask |= ((inst & 0x0F) << 2);
			digitalWriteAll(gpio_state & gpio_mask);
			break;
		case BITS4:
			//write MSB first, then LSB
			cs_log_printf("Writing %d to LCD\n",inst);
			gpio_mask |= ((inst & 0x0F) << 2);
                        digitalWriteAll(gpio_state & gpio_mask);
		
		        digitalWrite(LCDRS, 0);
		        digitalWrite(LCDE,1);
			mgos_msleep(1);
		        digitalWrite(LCDE,0);
		        //lcd_busy_wait();
			mgos_msleep(1);

			gpio_mask |= ((inst & 0xF0) >> 2);
                        digitalWriteAll(gpio_state & gpio_mask);
		default:
			break;
	}
    	
    	digitalWrite(LCDRS, 0); 
    	digitalWrite(LCDE,1);
	mgos_msleep(1);
    	digitalWrite(LCDE,0);
	//lcd_busy_wait();
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

void init_lcd()
//initialize LCD
{
//make sure we've waited 50ms after power on
	mgos_msleep(50);
//startup procedure
	write_inst(INITIALIZE,BITS8);
	mgos_msleep(20);
	write_inst(INITIALIZE,BITS8);
        mgos_msleep(20);
        write_inst(INITIALIZE,BITS8);
        mgos_msleep(20);
	write_inst(CONFIGURE_SCREEN_4BIT,BITS4);
	//lcd_busy_wait();
	mgos_msleep(1);
	write_inst(SCREEN_CLEAR,BITS4);
	//lcd_busy_wait();
	mgos_msleep(1);

}


