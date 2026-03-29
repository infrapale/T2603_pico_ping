#ifndef __EEPROM_H__
#define __EEPROM_H__

#define BUFF_LEN  16
void eeprom_initialize(void);

void eeprom_load_main_data(void);

void eeprom_save_main_data(void);
#endif