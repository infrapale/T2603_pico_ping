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

#include    <WiFi.h>
#include    <time.h>
#include    "main.h"
#include    "secrets.h"
#include    <RH_RF69.h>
#include    "atask.h"
#include    "Rfm69Modem.h"
#include    "io.h"

#define PIN_TX0         (0u)
#define PIN_RX0         (1u)
#define PING_INTERVAL_ms  (60*1000)
#define IO_TICK_INTERVAL    (100)

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

typedef struct
{
    uint16_t state;
    uint16_t prev_state;
    uint16_t retry_cntr;
    uint32_t next_try;
    uint32_t wait_until;
    time_t   now;
    struct tm timeinfo;
    char    buffer[64];
    uint32_t next_io_tick;
} ping_st;


ping_st ping = {0};

// Finnish timezone (automatic DST)
const char* TZ_FINLAND = "EET-2EEST,M3.5.0/03,M10.5.0/04";

#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h
RH_RF69         rf69(PIN_RFM_CS, PIN_RFM_IRQ);
Rfm69Modem      rfm69_modem(&rf69,  PIN_RFM_RESET, PIN_LED_BLUE );
modem_data_st   modem_data = {MY_MODULE_TAG, MY_MODULE_ADDR};


void modem_task(void);
atask_st modem_handle              = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};



void setup() {
    Serial1.setTX(PIN_TX0);   
    Serial1.setRX(PIN_RX0);

    Serial.begin(115200);
    delay(1500);
    Serial1.begin(9600);
    ping.state = 0;
    uint8_t key[] = RFM69_KEY;
    rfm69_modem.initialize(MY_MODULE_TAG, MY_MODULE_ADDR, key);
    rfm69_modem.radiate(__APP__);
    atask_initialize();
    atask_add_new(&modem_handle);
}

void setup1(){
    io_initialize();
    ping.next_io_tick = millis() + IO_TICK_INTERVAL;
}

void loop() 
{
    if(ping.state != ping.prev_state){
        Serial.printf("State %d -> %d\n", ping.prev_state, ping.state);
        ping.prev_state = ping.state;
    }
    switch(ping.state)
    {
        case 0:
            ping.state = 5;
            ping.wait_until = millis() + 3200;
            io_led_flash(COLOR_BLUE, BLINK_JITTER_1, 40);            
            io_led_flash(COLOR_YELLOW, BLINK_JITTER_2, 40);            
            io_led_flash(COLOR_RED, BLINK_JITTER_3, 40);
            break;
        case 5:
            if(millis() > ping.wait_until) ping.state = 10;
        case 10:
            io_led_flash(COLOR_YELLOW, BLINK_FAST, 50);
            Serial.printf("Connecting to WiFi...%s\n", ssid);
            WiFi.begin(ssid, password);
            ping.retry_cntr = 0;
            ping.state = 20;
            break;
        case 20:
            if(WiFi.status() == WL_CONNECTED)
            {
                io_led_flash(COLOR_YELLOW, BLINK_1_FLASH, BLINK_FOREVER);                
                Serial.printf("WiFi is connected, Retries= %d\n",ping.retry_cntr);
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                ping.state = 100;
            }
            else {
                ping.wait_until = millis() + 250;
                ping.state = 30;
            }
            break;
        case 30:
            if(millis() > ping.wait_until)
            {
                Serial.printf("-%d",ping.retry_cntr);
                ping.retry_cntr++;
                if(ping.retry_cntr > 40) 
                {
                    Serial.printf("WiFi Connection Retries was Exceeded: %d", ping.retry_cntr);
                    ping.state = 50;
                    io_led_flash(COLOR_RED, BLINK_SOS, BLINK_FOREVER);
                }
                else ping.state = 20;
            }
            break;           
        case 50:
            ping.state = 0;
            break;
        case 100:
          // Set timezone
            setenv("TZ", TZ_FINLAND, 1);
            tzset();

            // Configure NTP servers
            configTime(0, 0, "pool.ntp.org", "time.nist.gov");
            Serial.println("Waiting for NTP time...");
            ping.state = 110;
            ping.retry_cntr = 100;
            break;
        case 110:
            // Wait up to 10 seconds for NTP sync
            time(&ping.now);
            if(ping.now > 100000) {
                io_led_flash(COLOR_BLUE, BLINK_JITTER_1, 40);
                ping.state = 200;
            }
            else{
                ping.wait_until = millis() + 100;
                ping.state = 112;
            }
            break;
        case 112:
             if(millis() > ping.wait_until){
                if(ping.retry_cntr > 100) ping.state = 120;
                else ping.state = 110;
             }
            break;
        case 120:  
            Serial.println("NTP FAILED (no internet?)");
            Serial1.println("<NTP_FAIL;0>");
            ping.state = 300;
            break;

            break;
        case 200:
            // Convert to local Finnish time
            localtime_r(&ping.now, &ping.timeinfo);
            Serial.println("NTP OK — Internet is working!");
            strftime(ping.buffer, sizeof(ping.buffer), "%Y-%m-%d %H:%M:%S (%Z)", &ping.timeinfo);
            Serial.print("Local Finnish time: ");
            Serial.println(ping.buffer);    
            // <##T1T1=;2026;03;12;10;12>
            strftime(ping.buffer, sizeof(ping.buffer), "<##C1T1=;%Y;%m;%d;%H;%M>", &ping.timeinfo);
            Serial.println(ping.buffer);    
            Serial1.println(ping.buffer);    
            rfm69_modem.radiate(ping.buffer);
            ping.state = 210;
            ping.next_try = millis() + PING_INTERVAL_ms;
            break;
        case 210:
            if(millis() > ping.next_try){
                Serial.println("Testing again..");
                ping.state = 100;
            }
            break;    
        case 300:
            ping.state = 10;
            break;

    }
}

void loop1()
{
    if(millis() > ping.next_io_tick){
        ping.next_io_tick = millis() + IO_TICK_INTERVAL;
        io_task();
    }
}

void modem_task(void)
{
    rfm69_modem.modem_task();
}


