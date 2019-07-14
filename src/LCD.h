#include "gpio.h"

//LCD1602 GPIO mapping
#define LCDRS  IO_A7
#define LCDE   IO_A6
#define D4     IO_A5
#define D5     IO_A4
#define D6     IO_A3
#define D7     IO_A2

//Commands
typedef enum {	//flip bits because I wired the LCD weirdly
        SCREEN_CLEAR=0x80, //0x01
        CURSOR_RETURN=0x40, //0x02
        SET_CURSOR_LEFT=0x20, //0x04
        SET_CURSOR_LEFT_APPOINT=0xA0, //0x05 
        SET_CURSOR_RIGHT=0x60, //0x06
        SET_CURSOR_RIGHT_APPOINT=0xE0, //0x07
	SET_DISPLAY_ON=0x30, //0x0C
        SHIFT_CURSOR_LEFT=0x08, //0x10
        SHIFT_CURSOR_RIGHT=0x28, //0x14
        SHIFT_DISPLAY_LEFT=0x18, //0x18
        SHIFT_DISPLAY_RIGHT=0x38, //0x1C
        INITIALIZE=0x0C, //0x30
        CONFIGURE_SCREEN_8BIT=0x1C, //0x38
	CONFIGURE_SCREEN_4BIT=0x14 //0x28
} LCD_instr;

typedef enum {
	BITS8,
	BITS4
} mode;

void write_line(char lcd_data[]);
void write_char(char lcd_data, mode write_mode);
void write_inst(LCD_instr inst, mode write_mode);
void init_lcd();
