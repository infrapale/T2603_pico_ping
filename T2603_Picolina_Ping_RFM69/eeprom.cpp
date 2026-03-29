#include "main.h"
#include "Adafruit_EEPROM_I2C.h"
#include "eeprom.h"
#include "buff.h"

#define EEPROM_ADDR 0x50  //
#define EEPROM_MAIN_DATA_ADDR 0x00

extern main_data_st main_data;
Adafruit_EEPROM_I2C i2ceeprom;

uint8_t eebuff[BUFF_LEN];

void eeprom_initialize(void) {
    
    if (i2ceeprom.begin(EEPROM_ADDR, &Wire1)) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
        Serial.println("Found I2C EEPROM");
    } else {
        Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
        while (1) delay(10);
    }
}

void eeprom_load_main_data(void)
{
    for(uint8_t i = 0; i < BUFF_LEN; i++)
    {
        eebuff[i] = i2ceeprom.read(EEPROM_MAIN_DATA_ADDR + i);
    }
    main_data.restart_cntr = buff_read_u16(&eebuff[0]);
    main_data.router_restart_cntr = buff_read_u16(&eebuff[2]);
}

void eeprom_save_main_data(void)
{
    buff_write_u16(&eebuff[0], main_data.restart_cntr);
    buff_write_u16(&eebuff[2], main_data.router_restart_cntr);

    for(uint8_t i = 0; i < BUFF_LEN; i++)
    {
        i2ceeprom.write(EEPROM_MAIN_DATA_ADDR + i, eebuff[i]);
    }
}

void eeprom_test(void)
{
    // Read the first byte
    uint8_t test = i2ceeprom.read(0x0);
    Serial.print("Restarted "); Serial.print(test); Serial.println(" times");
    // Test write ++
    test++;
    i2ceeprom.write(0x0, test);

    // Try to determine the size by writing a value and seeing if it changes the first byte
    Serial.println("Testing size!");

    uint32_t max_addr;
    for (max_addr = 1; max_addr < 0xFFFF; max_addr++) {
      if (i2ceeprom.read(max_addr) != test)
        continue; // def didnt wrap around yet

      // maybe wraped? try writing the inverse
      if (! i2ceeprom.write(max_addr, (byte)~test)) {
          Serial.print("Failed to write address 0x");
          Serial.println(max_addr, HEX);
      }

      // read address 0x0 again
      uint8_t val0 = i2ceeprom.read(0);

      // re-write the old value
      if (! i2ceeprom.write(max_addr, test)) {
          Serial.print("Failed to re-write address 0x");
          Serial.println(max_addr, HEX);
      }    

      // check if addr 0 was changed
      if (val0 == (byte)~test) {
        Serial.println("Found max address");
        break;
      }
    }
    Serial.print("This EEPROM can store ");
    Serial.print(max_addr);
    Serial.println(" bytes");
      
    // dump the memory
    uint8_t val;
    for (uint16_t addr = 0; addr < max_addr; addr++) {
      val = i2ceeprom.read(addr);
      if ((addr % 32) == 0) {
        Serial.print("\n 0x"); Serial.print(addr, HEX); Serial.print(": ");
      }
      Serial.print("0x"); 
      if (val < 0x10) 
        Serial.print('0');
      Serial.print(val, HEX); Serial.print(" ");
    }
}

