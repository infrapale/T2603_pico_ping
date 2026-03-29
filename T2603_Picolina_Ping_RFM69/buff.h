#ifndef __BUFF_H__
#define __BUFF_H__

void buff_write_u8(uint8_t *buff, uint8_t value);

uint8_t buff_read_u8(uint8_t *buff);

void buff_write_u16(uint8_t *buff, uint16_t value);

uint16_t buff_read_u16(uint8_t *buff);

void buff_write_u32(uint8_t *buff, uint32_t value);

uint32_t buff_read_u32(uint8_t *buff);


#endif