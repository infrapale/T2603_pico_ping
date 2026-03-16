/*****************************************************************************
T2603_Picolina_Ping_RFM69 
*******************************************************************************

HW: Pi Pico W + RFM69

*******************************************************************************
https://github.com/infrapale/T2310_RFM69_TxRx
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
*******************************************************************************



*******************************************************************************
**/

#include    "main.h"
#include    "secrets.h"
#include    <RH_RF69.h>
#include    "Rfm69Modem.h"
#include    "atask.h"
#include    "io.h"
#include    "ping.h"

#define PIN_TX0             (0u)
#define PIN_RX0             (1u)

#define IO_TICK_INTERVAL    (100)

#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h
RH_RF69         rf69(PIN_RFM_CS, PIN_RFM_IRQ);
Rfm69Modem      rfm69_modem(&rf69,  PIN_RFM_RESET, -1 );
modem_data_st   modem_data = {MY_MODULE_TAG, MY_MODULE_ADDR};

main_ctrl_st ctrl = {0};

void modem_task(void);
atask_st modem_handle              = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};


void setup() {
    Serial1.setTX(PIN_TX0);   
    Serial1.setRX(PIN_RX0);

    Serial.begin(115200);
    delay(1500);
    Serial1.begin(9600);
    uint8_t key[] = RFM69_KEY;
    atask_initialize();
    ping_initialize();
    rfm69_modem.initialize(MY_MODULE_TAG, MY_MODULE_ADDR, key);
    rfm69_modem.radiate(__APP__);
    atask_add_new(&modem_handle);
}

void setup1(){
    io_initialize();
    ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
}

void loop() 
{
    atask_run();
}

void loop1()
{
    if(millis() > ctrl.next_io_tick){
        ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
        io_task();
    }
}

void modem_task(void)
{
    rfm69_modem.modem_task();
}


