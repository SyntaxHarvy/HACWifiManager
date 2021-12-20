/**
 *
 * @file BasicWifiMngrSetup.ino
 * @date 26.11.2021
 * @author Harvy Aronales Costiniano
 *
 * Basic setup of wifi manager without using Json parameter
 * This example will explore other parameters define on the constructor
 * Setup function parameters define as below:
 *     void setup(
        const char *defaultSSID,              - Required
        const char *defaultPass,              - Required
        const char *hostName = "hacwfmhost",  - Optional with default value
        WifiMode mode = STA_ONLY,             - Optional with default value
        bool enableMultiWifi = true,          - Optional with default value
        bool enableDHCPNetwork = true,        - Optional with default value
        const char *staIp = "10.0.1.100",     - Optional with default value
        const char *staGw = "255.255.255.0",  - Optional with default value
        const char *staSn = "0.0.0.0",        - Optional with default value
        const char *staPdns = "0.0.0.0",      - Optional with default value
        const char *staSdns = "0.0.0.0",      - Optional with default value
        const char *apSsid = "hacAP",         - Optional with default value
        const char *apPass = "appass",        - Optional with default value
        const char *apIp = "10.0.10.1",       - Optional with default value
        const char *apSn = "255.255.255.0",   - Optional with default value
        const char *apGw = "10.0.10.1"        - Optional with default value        
        )
 * 
 * Wifi Manager Mode:
 * enum WifiMode
  {
    STA_ONLY = 1,    // Station mode only
    AP_ONLY = 2,     // Access point mode only
    BOTH_STA_AP = 3, // Both station and access point mode
  }
 * Wifi Manager Network Type:
 * enum NetworkType
  {
    NETWORK_STATION = 1, // Network for Station
    NETWORK_AP  = 2,     // Network for Access point
  }
 * This example code is in the public domain.
 * https://github.com/SyntaxHarvy/HACWifiManager
 */

#include <HaCWifiManager.h>

//Instantiate wifimanager
HaCWifiManager gHaCWifiManager;

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

  //Setup will start the wifimanager
  //Visit : https://github.com/SyntaxHarvy/HACWifiManager.git
  //For the detail of the setup parameters
  //Note: First wifi ssid/pass is define on the setup function
  
  gHaCWifiManager.addWifiList("ssid2", "password2");
  gHaCWifiManager.addWifiList("ssid3", "pass3");
  gHaCWifiManager.addWifiList("ssid4", "pass4");
  gHaCWifiManager.addWifiList("ssid5", "pass5");
  
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

  
  //Note: First wifi ssid/pass is define on the setup function
  //Option 1:
  /*gHaCWifiManager.setup("ssid1",                   //default Station SSID
                       "password1",            //default Station Password
                       "myHostName",              //Device hostname
                       BOTH_STA_AP,               //Wifi manager will act as both station & access point only 
                       true,                      //Enable multi wifi  
                       false,                     //Enable DCHP for Station
                       false,                     //Enable DHCP for Access point
                       "10.0.0.14",               //Station IP Address
                       "255.255.255.0",           //Station subnet
                       "10.0.0.1",                //Station gateway
                       "0.0.0.0",                 //Station primary dns
                       "0.0.0.0",                 //Station secondary dns
                       "myapssid",                //AP ssid
                       "myappassword",            //AP password
                       "192.168.100.1",           //AP IP Address      
                       "255.255.255.0",           //AP subnet         
                       "192.168.100.1"            //AP gateway
                       );
                       */
 //Option 2:
 gHaCWifiManager.setEnableMultiWifi(true);        //Enable multiwifi
 gHaCWifiManager.setMode(BOTH_STA_AP);            //Wifi manager will act as both station & access point only
 gHaCWifiManager.setEnableDHCPNetwork(false, false);//First parameter is DHCP enable for station while 2nd parameter is the DHCP enable for Access point
 gHaCWifiManager.setNetWorkInformation(
                       "10.0.0.14",               //Station IP Address
                       "255.255.255.0",           //Station subnet
                       "10.0.0.1",                //Station gateway
                       "0.0.0.0",                 //Station primary dns
                       "0.0.0.0",                 //Station secondary dns
                       "192.168.100.1",           //AP IP Address      
                       "255.255.255.0",           //AP subnet         
                       "192.168.100.1"            //AP gateway
  );
  gHaCWifiManager.setAPInfo("myapssid", "myappassword");
  gHaCWifiManager.setup();
  //Print the wifimanager configuration in Json format
   String jsonConfig;
  gHaCWifiManager.getWifiConfigJson(&jsonConfig);
  Serial.printf("Wifi Manager Configuration : %s \n", jsonConfig.c_str());

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
