/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"
#include "gpio.h"
#include "LCD.h"
#include "SHT20.h"

enum mgos_app_init_result mgos_app_init(void) {
  
  gpio_init();
  init_lcd();
  LOG(LL_INFO, ("App initialized"));

  char LCDBuff[33];
  
  float temp = sht20_read_temp();
  float hum = sht20_read_humidity();

  mgos_msleep(2000);

  temp = sht20_read_temp();
  hum = sht20_read_humidity();

  sprintf( LCDBuff, "Temp: %10.2f\nHumidity: %6.2f", temp, hum );
  write_line( LCDBuff);

  return MGOS_APP_INIT_SUCCESS;
}
