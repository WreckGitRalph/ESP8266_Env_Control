/*
 * Driver for SHT20 in MongooseOS
 *
 * Marcus Niekraszewicz
 * 2019
 */

#include "SHT20.h"
#include "mgos.h"
#include "mgos_i2c.h"

/////////////////////////////////////////
//Defines
/////////////////////////////////////////

#define SHT20_I2C_ADDR                  0x40

/////////////////////////////////////////
//Private Functions
/////////////////////////////////////////

//Return I2C address for writing
uint8_t sht20_write_adrs(void){
        return (SHT20_I2C_ADDR);
}

//Return I2C address for reading
uint8_t sht20_read_adrs(void){
        return (SHT20_I2C_ADDR);
}

/////////////////////////////////////////
//Public Functions
/////////////////////////////////////////

//Perform a soft reset
void sht20_reset(void){

        struct mgos_i2c *i2c = mgos_i2c_get_global();
	uint8_t command = SHT20_SOFT_RESET;

        //send a reset command
        mgos_i2c_write( i2c, sht20_write_adrs(), &command, sizeof(command), true);

	return;
}

//Read the temperature
float sht20_read_temp(void){

        struct mgos_i2c *i2c = mgos_i2c_get_global();

        //read humidity
        uint16_t temp_val = mgos_i2c_read_reg_w( i2c, sht20_read_adrs(), SHT20_TRIGGER_T_MEASUREMENT_HOLD );

	cs_log_printf("Read %u from temperature sensor.\n", temp_val);

	float temp_calc = (float)(temp_val) * (float)(175.72/65536);

        return ( temp_calc - 46.85 );

}

//Read the humidity
float sht20_read_humidity(void){

	struct mgos_i2c *i2c = mgos_i2c_get_global();

        //read humidity
        uint16_t humidity_val = mgos_i2c_read_reg_w( i2c, sht20_read_adrs(), SHT20_TRIGGER_H_MEASUREMENT_HOLD );

	cs_log_printf("Read %u from humidity sensor.\n", humidity_val);

	float humidity_calc = (float)(humidity_val) * ((float)125/(float)65536);

	cs_log_printf("Calculated %f from humidity sensor.\n", humidity_calc);

	return ( (float)humidity_calc - (float)6 );
}

