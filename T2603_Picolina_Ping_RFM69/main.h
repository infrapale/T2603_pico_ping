#ifndef __MAIN_H__
#define __MAIN_H__
#include "WString.h"
#define   __APP__ ((char*)"T2603_Picolina_Ping_RFM69")

// WiFi Access Point
// #define PIRPANA
// #define LILLA_ASTRID
#define VILLA_ASTRID

#define DEBUG_PRINT 
#define SEND_TEST_MSG 
//#define ADA_M0_RFM69 
//#define ADA_PICO_FEATHER
#define T2601_PICO_RFM69
//#define PRO_MINI_RFM69
#include <Arduino.h>
#include "rfm69.h"

#ifdef  ADA_M0_RFM69
#define SerialX  Serial1
#else
#define SerialX Serial
#endif

#define TASK_NBR_OF  3
#define LED_INDICATION

#define MY_MODULE_TAG   'R'
#define MY_MODULE_ADDR  '1'

typedef struct
{
    char            tag;
    char            addr;         
} modem_data_st;




#endif