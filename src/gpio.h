/*
 * Driver for MCP23017 for GPIO functions over I2C
 *
 * Marcus Niekraszewicz
 * 2018
 */

//////////////////////////////////////////////////////////////////////
//Constants
//////////////////////////////////////////////////////////////////////

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
#define INPUT 1
#define OUTPUT 0

//Arduino style pin levels
#define HIGH 1
#define LOW 0

///////////////////////////////////////////////////////////////////////


//initialize the device with a configuration compatible with this driver
//In: none
//Out: none
void gpio_init(void);

//Read all GPIO pins
//In: 	none
//Out: 	A bitmask representing the GPIO pin levels [B7..B0,A7..A0]
uint16_t digitalReadAll(void);

//Write to all GPIO pins
//In: 	pin_mask: bitmask to write to GPIO pins [B7..B0,A7..A0]
//Out: 	none
void digitalWriteAll(uint16_t pin_mask);

//Arduino-style pin mode select
//In:	pin: the GPIO pin to set the mode of (IO_A1, IO_A2, etc)
//	mode: INPUT/OUTPUT
//Out: 	none
void pinMode(uint16_t pin,bool mode);

//Arduino-style pin write
//In: 	pin: the GPIO pin for writing to (IO_A0, IO_A1, etc)
//	level: HIGH/LOW
//Out:	none
void digitalWrite(uint16_t pin,bool level);

//Arduino-style pin read
//In:	pin: the GPIO pin to read from (IO_A1, IO_A2, etc)
//Out:	HIGH/LOW
bool digitalRead(uint16_t pin);
