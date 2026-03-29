#include "main.h"
#include "buff.h"

void buff_write_u8(uint8_t *buff, uint8_t value)
{
    buff[0] = value;
}

uint8_t buff_read_u8(uint8_t *buff)
{
    return buff[0];
}

void buff_write_u16(uint8_t *buff, uint16_t value)
{
    buff[0] = (value >> 0) & 0xFF;
    buff[1] = (value >> 8) & 0xFF;
}

uint16_t buff_read_u16(uint8_t *buff)
{
    return ((uint16_t)buff[0] << 0) |
           ((uint16_t)buff[1] << 8);
}

void buff_write_u32(uint8_t *buff, uint32_t value)
{
    buff[0] = (value >> 0) & 0xFF;
    buff[1] = (value >> 8) & 0xFF;
    buff[2] = (value >> 16) & 0xFF;
    buff[3] = (value >> 24) & 0xFF;
}

uint32_t buff_read_u32(uint8_t *buff)
{
    return ((uint32_t)buff[0] << 0)  |
           ((uint32_t)buff[1] << 8)  |
           ((uint32_t)buff[2] << 16) |
           ((uint32_t)buff[3] << 24);
}
