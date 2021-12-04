# HACWifiManager
Event driven wifimanager for esp8266 and esp32 arduino application.

The **HACWifiManager** allows you to create easily a callbacks for the following events:
- **onDebug**, 
    If **DEBUG_ESP_PORT** is define then this event will return all the debug define within the library. This will be usefull when troubleshooting and debugging.
- **onError**, 
    This event will be raised when any error occured within the library during startup and normal operations.
- **onSTAReady**, 
    This event will be raised when the station first establised a connection to the wifi access point or router.
- **onSTADisconnect**, 
    This event will be raised when station is disconnected.
- **onSTALoop**, 
    This event will be raised periodically similar to the loop functions. Additionally this event will only be raised when station has no error or no disconnection.
- **onAPReady**, 
    This event will be raised when the library access point successfully setup and ready for connection.
- **onAPDisconnect**, 
    This event will be raised when Access Point is shutdown.
- **onAPLoop**, 
    This event will be raised periodically like loop functions when the access point has no error or no disconnection.
- **onAPNewConnection**, 
    This event will be raised when a new client is connected to the local access point.

This will make your wifi application easy to handle during disconnection and reconnection as you can put all your different handles for each specific events. E.g for Mqtt, you can stop the mqtt loop handle when the **onSTADisconnect** fired and resume it when the **onSTALoop** is raise again.

A json format parameters can also be pass on the setup function for the initialization of the wifi. 

Libary setup function overloads:

- With Json string parameter

```cpp
void setup(const char *wifiJsonStr);
```
- With default parameters

```cpp
void setup(
    const char *defaultSSID,
    const char *defaultPass,
    const char *hostName = "hacwfmhost",
    WifiMode mode = STA_ONLY,
    bool enableMultiWifi = false,
    bool enableStaDHCPNetwork = true,
    bool enableApDHCPNetwork = true,
    const char *staIp = "10.0.1.100",
    const char *staGw = "255.255.255.0",
    const char *staSn = "0.0.0.0",
    const char *staPdns = "0.0.0.0",
    const char *staSdns = "0.0.0.0",
    const char *apSsid = "mydefaultAP",
    const char *apPass = "mydefaultAPPass",
    const char *apIp = "192.168.4.1",
    const char *apSn = "255.255.255.0",
    const char *apGw = "0.0.0.0"        
    );
```

- Without parameters but its initialization will be done externally prior to calling the setup function

```cpp
void setup();
```

## Installation
- **Download**: https://github.com/SyntaxHarvy/HACWifiManager/archive/refs/heads/main.zip
- Arduino Ide:
1. Go to Sketch >> Include Library >> Add .zip library
2. Select the downloaded zip file.
- Others:
1. Unzip and modify the folder name to **HACWifiManager**
2. Move the modified folder on the designated library of the IDE you are using.

## Dependency
1. ESP8266WiFi
2. ArduinoJson

## How to use

- Include the **HACWifiManager** library to your project(Refer to library example).

```cpp
#include <hacwifimanager.h>
```

- Instantiate:

```cpp
HaCWifiManager gHaCWifiManager;
```

- Define the callback functions prototype:

```cpp
void onDebugCB(const char *msg);                // onDebug Event
void onErrorCB(const char *msg);                // onError Event
void onSTAReadyCB(const char *msg);             // onSTAReady Event
void onSTADisconnectCB(const char *msg);        // onSTADisconnect Event
void onSTALoopCB(const char *msg);              // onSTALoop Event
void onAPReadyCB(const char *msg);              // onAPReady Event
void onAPDisconnectCB(const char *msg);         // onAPDisconnect Event
void onAPLoopCB(const char *msg);               // onAPLoop Event
void onAPNewConnectionCB(const char *msg);      // onAPNewConnection Event
```

### Different Options of Setting-up.

#### **Option 1** : Using json format parameters

- Declaration of Json at the global:
```cpp
static const char wifidata[] PROGMEM =
  R"(   {
       "mode" : 3,
       "enable_multi_wifi" : true,
       "enable_dhcp_network_sta" : true,
       "enable_dhcp_network_ap" : true,
       "host_name" : "hacwfmhost",
       "wifilist" : {
           "0" : {"ssid": "ssid1", "password" : "pass1"},            
           "1" : {"ssid": "ssid2", "password" : "pass2"}            
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

```

- Initialization of the events
```cpp
gHaCWifiManager.onDebug(onDebugCB);                     //onDebug Event
gHaCWifiManager.onDebug(onDebugCB);                     //onError Event
gHaCWifiManager.onSTAReady(onSTAReadyCB);               //onSTAReady Event
gHaCWifiManager.onSTADisconnect(onSTADisconnectCB);     //onSTADisconnect Event
gHaCWifiManager.onSTALoop(onSTALoopCB);                 //onSTALoop Event
gHaCWifiManager.onAPReady(onAPReadyCB);                 //onAPReady Event
gHaCWifiManager.onAPDisconnect(onAPDisconnectCB);       //onAPLoop Event
gHaCWifiManager.onAPLoop(onAPLoopCB);                   //onAPLoop Event
gHaCWifiManager.onAPNewConnection(onAPNewConnectionCB); //onAPNewConnection Event      
```

- Calling the library setup function with Json parameter.

```cpp
gHaCWifiManager.setup(String(wifidata).c_str());
```

- Library setup can be readback as well into Json string.

```cpp
 Serial.printf("Wifi Manager Configuration : %s \n", gHaCWifiManager.getWifiConfigJson().c_str());
```

#### **Option 2** : Using default parameter with require ssid and password

```cpp
gHaCWifiManager.setup( "ssid1",                   //default Station SSID
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
```

#### **Option 3** : Without parameters but its initialization will be done externally prior to calling the setup function

- Adding access point ssid/pass on the list

```cpp
gHaCWifiManager.addWifiList("ssid1", "ssid1");
gHaCWifiManager.addWifiList("ssid2", "ssid2");
gHaCWifiManager.addWifiList("ssid3", "pass3");
gHaCWifiManager.addWifiList("ssid4", "pass4");
gHaCWifiManager.addWifiList("ssid5", "pass5");
```

- Enable/Disable Multiwifi function

```cpp
gHaCWifiManager.setEnableMultiWifi(true);
```

- Set wifi mode

Note: Wifi different mode defined by WifiMode enum

```cpp
enum WifiMode
{
    STA_ONLY = 1,    // Station mode only
    AP_ONLY = 2,     // Access point mode only
    BOTH_STA_AP = 3, // Both station and access point mode
};
```

Setting setMode function.

```cpp
gHaCWifiManager.setMode(BOTH_STA_AP);
```

- Setting DHCP for Station and Access Point

Note: First parameter is the enable for Station while Access point for the second parameter

```cpp
 gHaCWifiManager.setEnableDHCPNetwork(false, false);
```

- Setting network IP information if DHCP is disabled.

```cpp
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
```

- Setting Access Point ssid/pass.

```cpp
gHaCWifiManager.setAPInfo("myapssid", "myappassword");
```

- Finally calling the library setup function

```cpp
gHaCWifiManager.setup();
```

### Loop Handling

- Calling the library loop function at the arduino loop routine

```cpp
gHaCWifiManager.loop();
```

## Public Function Definitions

- **setMode**

```cpp
void setMode(WifiMode mode);
```

- **getMode**

```cpp
WifiMode getMode();
```

- **setEnableMultiWifi**

```cpp
void setEnableMultiWifi(bool enable = false);
```

- **getEnableMultiWifi**

```cpp
bool getEnableMultiWifi();
```

- **setEnableDHCPNetwork**

```cpp
void setEnableDHCPNetwork(bool enableSta = false, bool enableAP = false);
```

- **getEnableDHCPNetwork**

```cpp
bool getEnableDHCPNetwork(NetworkType netWorkType);
```

- **setNetWorkInformation**

```cpp
void setNetWorkInformation(const char *ip = "10.0.1.100",
                const char *sn = "255.255.255.0",
                const char *gw = "0.0.0.0",
                const char *pdns = "0.0.0.0",
                const char *sdns = "0.0.0.0",
                const char *apIp = "10.0.10.1",
                const char *apSn = "255.255.255.0",
                const char *apGw = "10.0.10.1"                           

);
```

- **setAPInfo**

```cpp
void setAPInfo( const char *ssid = "mydefaultAP", 
                const char *pass = "mydefaultAPPass"
                );
```

- **setHostName**

```cpp
void setHostName(const char *hostName);
```

- **getHostName**

```cpp
String getHostName();
```

- **getWifiConfigJson**

```cpp
String getWifiConfigJson();
```

- **addWifiList**

```cpp
void addWifiList(const char *ssid, const char *pass);
```

- **editWifiList**

```cpp
bool editWifiList(const char *oldSsid, const char *oldPass,
                      const char *newSsid, const char *newPass);
```


- **removeWifiList**

```cpp
bool removeWifiList(const char *ssid);
```

- **shutdownAP**

```cpp
void shutdownAP();
```

- **shutdownSTA**

```cpp
void shutdownSTA();
```

- **shutdown**

```cpp
void shutdown();
```

- **getStaIP**

```cpp
String getStaIP();
```

- **getAPIP**

```cpp
String getAPIP();
```

- **Events**

```cpp
void onDebug(tListGenCbFnHaC1StrParam fn); 
void onError(tListGenCbFnHaC1StrParam fn); 
void onSTAReady(tListGenCbFnHaC1StrParam fn);
void onSTADisconnect(tListGenCbFnHaC1StrParam fn);
void onSTALoop(tListGenCbFnHaC1StrParam fn);    
void onAPReady(tListGenCbFnHaC1StrParam fn);    
void onAPDisconnect(tListGenCbFnHaC1StrParam fn);
void onAPLoop(tListGenCbFnHaC1StrParam fn);    
                                                   
void onAPNewConnection(tListGenCbFnHaC1StrParam fn);

```
