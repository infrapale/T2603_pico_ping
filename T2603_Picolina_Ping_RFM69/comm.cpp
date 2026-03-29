#include "main.h"
#include "io.h"
#include "comm.h"
#include "atask.h"
#include "ping.h"


extern main_data_st main_data;


void comm_task(void);
bool comm_parse(char *msg);

//                      123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st comm_h  =   {"Comm   SM      ", 100,    0,     0,  255,    0,   1,  comm_task };

const char msg_tag[COMM_MSG_NBR_OF][MSG_TAG_LEN] =
{
    [COMM_MSG_UNDEFINED]  = "XXXXXXX",
    [COMM_MSG_FACTORY]    = "factory",
    [COMM_MSG_PRINT]      = "print",
};

comm_st comm[COMM_INDX_NBR_OF];

void comm_initialize(void)
{
  // Serial1.setTX(PIN_TX0);   
  // Serial1.setRX(PIN_RX0);
  // Serial2.setTX(PIN_TX1);   
  // Serial2.setRX(PIN_RX1);
  // Serial1.begin(9600);
  // Serial1.begin(9600);

  //atask_add_new(&comm_h);

}

comm_msg_type_et comm_read_parse(void)
{
    //Serial.println("comm_read_parse");
    comm_msg_type_et tag_type = COMM_MSG_UNDEFINED;
    int  cmp;
    if (Serial.available())
    {
        String RxStr;
        Serial.println("rx0 is available");
        RxStr = Serial.readStringUntil('\n');
        if (RxStr.length()> 0)
        {
            comm[COMM_INDX_0].rx_avail = true;
            RxStr.trim();
            RxStr.toCharArray(comm[COMM_INDX_0].rx_buff, COMM_BUFF_LEN);
            Serial.println(comm[COMM_INDX_0].rx_buff);
            
            uint8_t len = strlen(comm[COMM_INDX_0].rx_buff);
           
            for(uint8_t i = 0; (i < COMM_MSG_NBR_OF) && (tag_type == COMM_MSG_UNDEFINED); i++)
            {
                //if(strncmp(msg_tag[i],comm[COMM_INDX_0].rx_buff,MSG_TAG_LEN) == 0) tag_type = (comm_msg_type_et)i;
                cmp = strcmp(msg_tag[i],comm[COMM_INDX_0].rx_buff);
                Serial.printf("-%s-%s-%d\n",msg_tag[i], comm[COMM_INDX_0].rx_buff,cmp );
                if(cmp == 0) tag_type = (comm_msg_type_et)i;
                Serial.printf("CMP:  %d = %d\n", i, cmp);
            } 
        }
    } 
    return tag_type;
}

comm_msg_type_et comm_get_type(char *tag)
{
    comm_msg_type_et tag_type = COMM_MSG_UNDEFINED;
    for(uint8_t i = 0; (i < COMM_MSG_NBR_OF) && (tag_type == COMM_MSG_UNDEFINED); i++)
    {
        if(strncmp(msg_tag[i],tag,MSG_TAG_LEN) == 0) tag_type = (comm_msg_type_et)i;
    } 
    return tag_type;
}


void comm_task(void)
{
    static comm_msg_type_et tag_type;
    bool is_ok;
    switch(comm_h.state)
    {
        case 0:
            comm[COMM_INDX_0].rx_avail = false;
            comm_h.state = 10;
            break;
        case 10:
            tag_type = comm_read_parse();
            if(comm[COMM_INDX_0].rx_avail) {
                io_led_flash(COLOR_YELLOW, BLINK_FAST, 100);
                comm_h.state = 100;
            }    
            else comm_h.state = 10;
            break;
        case 100:
            Serial.printf("Message type: %d\n", (int)tag_type);
            switch(tag_type)
            {
                case COMM_MSG_UNDEFINED:
                    Serial.println("Commands:");
                    Serial.println("factory  (reset)");
                    Serial.println("print");
                    break;
                case COMM_MSG_FACTORY:
                    main_data.restart_cntr = 0;
                    main_data.router_restart_cntr = 0;
                    break;
                case COMM_MSG_PRINT:
                    ping_print_main_data();
                    break;
            }

            comm[COMM_INDX_0].rx_avail = false;
            comm_h.state = 10;
            break;
    }
}

