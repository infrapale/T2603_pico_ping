#include  <WiFi.h>
#include  <time.h>
#define   PIRPANA
#include  "secrets.h"

#define PIN_TX0         (0u)
#define PIN_RX0         (1u)
#define PING_INTERVAL_ms  (60*1000)

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

typedef struct
{
    uint16_t state;
    uint16_t prev_state;
    uint16_t retry_cntr;
    uint32_t next_try;
    time_t   now;
    struct tm timeinfo;
    char    buffer[64];
} ping_st;

ping_st ping = {0};

// Finnish timezone (automatic DST)
const char* TZ_FINLAND = "EET-2EEST,M3.5.0/03,M10.5.0/04";

void setup() {
    Serial1.setTX(PIN_TX0);   
    Serial1.setRX(PIN_RX0);

    Serial.begin(115200);
    delay(1500);
    Serial1.begin(9600);
    ping.state = 0;
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
            ping.state = 10;
            break;
        case 10:
            Serial.println("Connecting to WiFi...");
            WiFi.begin(ssid, password);
            ping.retry_cntr = 0;
            ping.state = 20;
            break;
        case 20:
            if(WiFi.status() == WL_CONNECTED)
            {
                Serial.printf("WiFi is connected, Retries= %d\n",ping.retry_cntr);
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                ping.state = 100;
            }
            else
            {
                delay(250);
                Serial.printf("-%d",ping.retry_cntr);
                ping.retry_cntr++;
                if(ping.retry_cntr > 40) 
                {
                    Serial.printf("WiFi Connection Reries was Exceeded: %d", ping.retry_cntr);
                    ping.state = 50;
                }
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
            if(ping.now > 100000) ping.state = 200;
            else{
                delay(100);
                if(ping.retry_cntr > 100) ping.state = 120;
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
            strftime(ping.buffer, sizeof(ping.buffer), "<NTP_TIME;%Y;%m;%d;%H;%M>", &ping.timeinfo);
            Serial.println(ping.buffer);    
            Serial1.println(ping.buffer);    
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