*******************************************************************************
UART Command Syntax

    rfm_send_msg_st *rx_msg = &send_msg; 
    Frame: <R1F1O2:L>\r\n
            \\\\\\\\______ value, message (optional)
             \\\\\\\______ action set: '=' get: '?' reply: ':'
              \\\\\\______ index
               \\\\\______ function
                \\\\______ sender addr
                 \\\______ sender tag
                  \\______ module_addr target
                   \______ module_tag target

*******************************************************************************

    module_tag  and sender tag identifies the type of module
      R Radio
      Y Relay
      K Keypad
      # All modules
    module_addr and sender addr
      0-9
      # All modules
    Radio Functions
      A Is messages available
      R Read Raw Message
      D Read Decoded Message
      T Transmit Raw Message
      J Send Desoced Message as json
      I Get last RSSI
      S Set parameter
    Index
      0-9
      0 is also used when N/A
    Action
      set: '=' 
      get: '?' 
      reply: ':'
    Value
      Total max 61 characters for RFM69
      '*'  Not applicable or relevant

*******************************************************************************
Sensor Radio Message:   {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                        {"Z":"Dock","S":"T_dht22","V":"8.7","R":"-"}
Relay Radio Message     {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
Sensor Node Rx Mesage:  <#X1N:OD1;Temp;25.0;->
Relay Node Rx Mesage:   <#X1N:RMH1;RKOK1;T;->
Time Message:           <##C1T1=;2026;03;12;10;12>
Relay Mesage            <#R12=x>   x:  0=off, 1=on, T=toggle
*******************************************************************************
