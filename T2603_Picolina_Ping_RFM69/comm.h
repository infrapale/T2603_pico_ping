#ifndef __COMM_H__
#define __COMM_H__

#define MSG_TAG_LEN     8
#define COMM_BUFF_LEN   64

typedef enum
{
    COMM_INDX_0   = 0,
    COMM_INDX_1,
    COMM_INDX_2,
    COMM_INDX_NBR_OF,
} comm_indx_et;

typedef enum 
{
    COMM_MSG_UNDEFINED = 0,
    COMM_MSG_FACTORY,
    COMM_MSG_PRINT,
    COMM_MSG_NBR_OF,
} comm_msg_type_et;

typedef struct
{
    char tx_buff[COMM_BUFF_LEN];
    char rx_buff[COMM_BUFF_LEN];
    bool rx_avail;
} comm_st;

void comm_initialize(void);

void comm_task(void);

#endif