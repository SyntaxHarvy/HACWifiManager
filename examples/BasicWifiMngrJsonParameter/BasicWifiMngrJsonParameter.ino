/**
 *
 * @file BasicWifiJsonParameter.ino
 * @date 26.11.2021
 * @author Harvy Aronales Costiniano
 *
 * Setting-up wifi manager from json
 * Json data format will be passed on the constructor of the wifimanager
 * setup function.
 * JSON FORMAT:
 * {
 *    "mode" : 1-3 Integer format(Station only =1, AP only = 2, both = 3),
 *    "enable_multi_wifi" : true or false,
 *    "enable_dhcp_network_sta": true or false,
 *    "enable_dhcp_network_ap": true or false,
 *    "host_name" : name of the host in string format e.g. "myhost",
 *    "wifilist" : 
 *    {
           "0" : {
                  "ssid": in string format e.g "myssid", 
                  "password" : in string format e.g "mypassword"
                 },            
           "1" : {
                  "ssid": in string format e.g "myssid2", 
                  "password" : in string format e.g "mypassword2"
                 },            
           .......            
      },
      "ap" : 
      {
          "ssid" : in string format e.g "mydefaultAP",
          "pass" : in string format e.g "mydefaultAPPass"
      },      
      "sta_network": 
      {
         "ip" : in string format e.g "10.0.0.56",
         "sn" : in string format e.g "255.255.255.0",
         "gw" : in string format e.g "10.0.0.1",
         "pdns" in string format e.g : "8.8.8.8",
         "sdns" in string format e.g : "8.8.8.1"
      },  
      "ap_network": 
      {
         "ip" : in string format e.g "10.0.10.51",
         "sn" : in string format e.g "255.255.255.0",
         "gw" : in string format e.g "10.0.10.1"
      }
 Note: 
    ip format for network string parameters should be valid.         
 *    
 * }
 * 
 * This example code is in the public domain.
 * https://github.com/SyntaxHarvy/HACWifiManager
 */

#include <HACWifiManager.h>

//Instantiate wifimanager
HaCWifiManager gHaCWifiManager;

//Construction of Json parameters
static const char wifidata[] PROGMEM =
  R"(   {
       "mode" : 3,
       "enable_multi_wifi" : true,
       "enable_dhcp_network_sta" : true,
       "enable_dhcp_network_ap" : true,
       "host_name" : "hacwfmhost",
       "wifilist" : {
           "0" : {"ssid": "ssid1", "password" : "password1"},            
           "1" : {"ssid": "ssid2", "password" : "password2"}            
        },
        "ap" : {
          "ssid" : "mydefaultAP",
          "pass" : "mydefaultAPPass"
        },
        "sta_network": {
           "ip" : "10.0.0.56",
           "sn" : "255.255.255.0",
           "gw" : "10.0.0.1",
           "pdns" : "8.8.8.8",
           "sdns" : "8.8.8.1"
        },
        "ap_network": {
           "ip" : "10.0.10.51",
           "sn" : "255.255.255.0",
           "gw" : "10.0.10.1"
        }
    })";

//Wifi Manager Events Callback function prototypes
void onDebugCB(const char *msg);                // onDebug Event
void onErrorCB(const char *msg);                // onError Event
void onSTAReadyCB(const char *msg);             // onSTAReady Event
void onSTADisconnectCB(const char *msg);        // onSTADisconnect Event
void onSTALoopCB(const char *msg);              // onSTALoop Event
void onAPReadyCB(const char *msg);              // onAPReady Event
void onAPDisconnectCB(const char *msg);         // onAPDisconnect Event
void onAPLoopCB(const char *msg);               // onAPLoop Event
void onAPNewConnectionCB(const char *msg);      // onAPNewConnection Event

void setup() {
  
  Serial.begin(115200);
  Serial.println("Start");

  //Define wifimanager various events callback
  //onDebug Event
  gHaCWifiManager.onDebug(onDebugCB);
  //onError Event
  gHaCWifiManager.onError(onErrorCB);
  //onSTAReady Event
  gHaCWifiManager.onSTAReady(onSTAReadyCB);
  //onSTADisconnect Event
  gHaCWifiManager.onSTADisconnect(onSTADisconnectCB);
  //onSTALoop Event
  gHaCWifiManager.onSTALoop(onSTALoopCB);
  //onAPReady Event
  gHaCWifiManager.onAPReady(onAPReadyCB);
  //onAPDisconnect Event
  gHaCWifiManager.onAPDisconnect(onAPDisconnectCB);
  //onAPLoop Event
  gHaCWifiManager.onAPLoop(onAPLoopCB);
  //onAPNewConnection Event
  gHaCWifiManager.onAPNewConnection(onAPNewConnectionCB);

  //Setting wifi Options
  //Default settings
  //gHaCWifiManager.setWifiOptions();

  //Custom settings
  gHaCWifiManager.setWifiOptions(
                        false,                    //Persistent
                        WIFI_NONE_SLEEP,          //Sleep style
                        16.5,                     //Output power  
                        WIFI_PHY_MODE_11G         //Wifi Physical mode
  );


 //Setup will start the wifimanager
 gHaCWifiManager.setup(String(wifidata).c_str());
}

void loop() {
  gHaCWifiManager.loop();
}

//Wifi Manager Events Callback Function

void onDebugCB(const char *msg){
  Serial.printf("%s \n", msg);
}

void onErrorCB(const char *msg){
  Serial.println("Error =>" + String(msg));
}

void onSTAReadyCB(const char *msg){
  //TO DO: Add here on station ready handle 
  Serial.println("Station ready. Msg =>" + String(msg));
  Serial.println("Station IP =>" + gHaCWifiManager.getStaIP());
}
void onSTADisconnectCB(const char *msg){
  //TO DO: Add here on station disconnect handle 
  Serial.println("Station disconnected. Msg =>" + String(msg));
}
void onSTALoopCB(const char *msg){
  //TO DO: Add here all the services loop which are wifi dependent e.g mqtt.loop etc..
  //Serial.println("Station loop =>" + String(msg));
}
void onAPReadyCB(const char *msg){
  //TO DO: Add here all AP ready handle
  Serial.println("Access point is ready. =>" + String(msg));
  Serial.println("Access Point IP =>" + gHaCWifiManager.getAPIP());
}
void onAPDisconnectCB(const char *msg){
  //TO DO: Add here on AP disconnect handle 
  Serial.println("Access point disconnected =>" + String(msg));
}
void onAPLoopCB(const char *msg){
  //TO DO: Add here all the services loop which are wifi dependent e.g webserver.handle etc..
  //Serial.println("Access point loop =>" + String(msg));
}
void onAPNewConnectionCB(const char *msg){
  //TO DO: Add here all the services loop which are wifi dependent e.g webserver.handle etc..
  Serial.println("New client connected on the Access Point =>" + String(msg));
}
