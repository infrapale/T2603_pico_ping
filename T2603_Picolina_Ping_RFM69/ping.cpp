#include    <WiFi.h>
#include    <time.h>

#include    "main.h"
#include    "secrets.h"
#include    "io.h"
#include    "atask.h"
#include    "ping.h"
#include    "Rfm69Modem.h"
#include    "eeprom.h"


#define     PING_INTERVAL_ms  (60*1000)

typedef struct
{
    uint16_t state;
    uint16_t prev_state;
    uint16_t retry_cntr;
    uint32_t next_try;
    uint32_t wait_until;
    uint8_t   task_indx;
    time_t   now;
    struct tm timeinfo;
    char    buffer[64];
    uint32_t next_io_tick;
} ping_st;


ping_st ping = {0};

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

extern Rfm69Modem       rfm69_modem;
extern main_data_st     main_data;

// Finnish timezone (automatic DST)
const char* TZ_FINLAND = "EET-2EEST,M3.5.0/03,M10.5.0/04";

// Function prototypes
void ping_task(void);
//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st ping_th              =   {"Ping Task      ", 100,     0,     0,  255,    0,  1,  ping_task };

void ping_initialize(void)
{
    ping.task_indx =  atask_add_new(&ping_th);
}

void ping_task(void)
{
    // if(ping.state != ping.prev_state){
    //     Serial.printf("State %d -> %d\n", ping.prev_state, ping.state);
    //     ping.prev_state = ping.state;
    // }
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
                //Serial.printf("%d\n",ping.retry_cntr);
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
            ping.state = 300;
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
            ping.wait_until = millis() + 10000;
            digitalWrite(PIN_RELAY, LOW);
            io_led_flash(COLOR_YELLOW, BLINK_JITTER_3, BLINK_FOREVER); 
            main_data.router_restart_cntr++;
            eeprom_save_main_data();
            ping_print_main_data();
            ping.state = 310;
            break;
        case 310:
            if(millis() > ping.wait_until){
                digitalWrite(PIN_RELAY, HIGH);
                ping.state = 0;
            }
            break;
    }

}

void ping_print_main_data(void)
{
    Serial.printf("Pico Restarts: %d, Router Restarts: %d\n",
        main_data.restart_cntr,
        main_data.router_restart_cntr);
}