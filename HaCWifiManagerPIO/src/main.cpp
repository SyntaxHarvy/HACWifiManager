
#include <Arduino.h>


#include "hacwifimanager.h"

HaCWifiManager gHaCWifiManager;

static const char wifidata[] PROGMEM =
  R"(   {
       "mode" : 1,
       "enable_multi_wifi" : true,
       "enable_dhcp_network" : true,
       "wifilist" : {
           "0" : {"ssid": "ssid1", "password" : "pass1"},
           "1" : {"ssid": "ssid2", "password" : "pass2"} 
        },
        "network": {
           "ip" : "10.0.0.56",
           "sn" : "255.255.255.0",
           "gw" : "10.0.0.1",
           "pdns" : "8.8.8.8",
           "sdns" : "8.8.8.1"
        }
    })";

void setup() {
  DEBUG_MSG_HAC_WIFI_SERIAL_INIT();
  DEBUG_MSG_HAC_WIFI_LN("Start");
  DEBUG_MSG_HAC_WIFI_LN(wifidata);  
  

  gHaCWifiManager.onDebug([&](const char *msg){
    //DEBUG_MSG_HAC_WIFI_LN(msg);
    Serial.println(msg);
  });
  gHaCWifiManager.setup(String(wifidata).c_str());
}

void loop() {
  gHaCWifiManager.loop();
}