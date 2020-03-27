/*
 * Driver for SHT20 in MongooseOS
 *
 * Marcus Niekraszewicz
 * 2019
 */

/////////////////////////////////////////
//Defines
/////////////////////////////////////////

#define SHT20_TRIGGER_T_MEASUREMENT_HOLD	0xE3
#define SHT20_TRIGGER_H_MEASUREMENT_HOLD	0xE5
#define SHT20_TRIGGER_T_MEASUREMENT_NOHOLD	0xF3
#define SHT20_TRIGGER_H_MEASUREMENT_NOHOLD	0xF5
#define SHT20_WRITE_USER_REG			0xE6
#define SHT20_READ_USER_REG			0xE7
#define SHT20_SOFT_RESET			0xFE

////////////////////////////////////////
//Public Functions
////////////////////////////////////////

void sht20_reset(void);

float sht20_read_temp(void);

float sht20_read_humidity(void);
