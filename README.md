# HACWifiManager
Event driven wifimanager for esp8266 and esp32 arduino application.

The **HACWifiManager** allows you to create easily a callbacks for the following events:
- onDebug
- onError
- onSTAReady
- onSTADisconnect
- onSTALoop
- onAPReady
- onAPDisconnect
- onAPLoop
- onAPNewConnection

This will make your wifi application easy to handle during disconnection and reconnection as you can put all your different handles for each specific events. E.g for Mqtt, you can stop the mqtt loop handle when the **onSTADisconnect** fired and resume it when the **onSTALoop** is raise again. 

## Installation
- **Download**: https://github.com/SyntaxHarvy/HACWifiManager/archive/refs/heads/main.zip
- Arduino Ide:
1. Go to Sketch >> Include Library >> Add .zip library
2. Select the download zip file.
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
