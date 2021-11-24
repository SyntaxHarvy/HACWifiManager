
#include <Arduino.h>


#include "hacwifimanager.h"

HaCWifiManager gHaCWifiManager;

static const char wifidata[] PROGMEM =
  R"(   {
       "mode" : 2,
       "enable_multi_wifi" : true,
       "enable_dhcp_network" : true,
       "wifilist" : {
           "0" : {"ssid": "xtrwifiphone", "password" : "xtrp@ssword1"}            
        },
        "ap" : {
          "ssid" : "mydefaultAP",
          "pass" : "mydefaultAPPass"
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
  
  Serial.begin(115200);
  Serial.println("Start");

  gHaCWifiManager.onDebug([&](const char *msg){
    //DEBUG_MSG_HAC_WIFI_LN(msg);
    Serial.println(msg);
  });

  gHaCWifiManager.onReady([&](const char *ssid){
    Serial.println("onReady event => MCU connected to wifi " + String(ssid));
  });

   gHaCWifiManager.onError([&](const char *errCode){
    Serial.println("onError event => Error Code  " + String(errCode));
  });

  gHaCWifiManager.onDisconnect([&](const char *ssid){
    Serial.println("onDisconnect event => Wifi " + String(ssid) + " disconnected");
  });

  gHaCWifiManager.onServiceLoop([&](const char *json){
    Serial.println("onServiceLoop event => " + String(json));
  });

  gHaCWifiManager.setup(String(wifidata).c_str());
}

void loop() {
  gHaCWifiManager.loop();
}