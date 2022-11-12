/**
 *
 * @file hacwifimanager.h
 * @date 26.11.2021
 * @author Harvy Aronales Costiniano
 * 
 * Copyright (c) 2021 Harvy Aronales Costiniano. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __HACWIFI_MANAGER_H_
#define __HACWIFI_MANAGER_H_

/* #region CONSTANT_DEFINITION */

/* #region Debug */
#define HAC_DEBUG_PREFIX "[HACWIFIMANAGER]"
#define DEFAULT_HOST_NAME "HACWIFIMNGRHOST"
#define WIFI_SCAN_TIMEOUT 1000   
#define MAX_WIFI_SCAN_ATTEMPT 3

#if defined(DEBUG_ESP_PORT) || defined(HAC_ENABLE_DEBUG)

#define DEBUG_HAC_WIFI Serial // Custom serial debug
#define DEBUG_MSG_HAC_WIFI(msg, ...)               \
    {                                              \
        DEBUG_HAC_WIFI.printf(msg, ##__VA_ARGS__); \
    } // Custom serial debug for Serial.printf
#define DEBUG_MSG_HAC_WIFI_LN(msg)   \
    {                                \
        DEBUG_HAC_WIFI.println(msg); \
    } // Custom serial debug for Serial.println
#define DEBUG_MSG_HAC_WIFI_SERIAL_INIT() \
    {                                    \
        DEBUG_HAC_WIFI.begin(115200);    \
    } // Serial initialization baud rate
#define DEBUG_CALLBACK_HAC(msg)      \
    {                                \
        HaCWifiManager::_debug(msg); \
    }
#define DEBUG_CALLBACK_HAC2(msg, ...)               \
    {                                               \
        char buffer [255];                          \
			snprintf_P(                             \
				buffer, sizeof(buffer),             \
				msg,  \
				##__VA_ARGS__ \
			);                                      \
        HaCWifiManager::_debug((const char*)buffer); \
    }
#else

#define DEBUG_MSG_HAC_WIFI(msg, ...)
#define DEBUG_MSG_HAC_WIFI_LN(msg)
#define DEBUG_MSG_HAC_WIFI_SERIAL_INIT()
#define DEBUG_CALLBACK_HAC(msg)
#define DEBUG_CALLBACK_HAC2(msg, ...)

#endif

/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCWifiManagerStringConst.h"
#include "hacwifimanagerparameters.h"
#include "tick.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
extern "C" {
#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
}
#endif

#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#endif

#include <ArduinoJson.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
typedef std::function<void()> tListGenCbFnHaC;                      // Standard void function with non-return value
typedef std::function<void(const char *)> tListGenCbFnHaC1StrParam; // Standard void function with non-return value

enum WifiMode
{
    STA_ONLY = 1,    // Station mode only
    AP_ONLY = 2,     // Access point mode only
    BOTH_STA_AP = 3, // Both station and access point mode
};

/* #endregion */

/* #region CLASS_DECLARATION */
class HaCWifiManager
{
public:
    HaCWifiManager(); // Constructor
    void setup(const char *wifiJsonStr);
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
    void setup(); // Function called on setting up the wifi manager Core

    void loop(); // Function called at the loop routine of the wifi
                 // manager

    //Public Functions
    void setMode(WifiMode mode);
    WifiMode getMode();
    void setEnableMultiWifi(bool enable = false);
    bool getEnableMultiWifi();
    void setEnableDHCPNetwork(bool enableSta = false, bool enableAP = false);
    bool getEnableDHCPNetwork(NetworkType netWorkType);
    void setNetWorkInformation(const char *ip = "10.0.1.100",
                    const char *sn = "255.255.255.0",
                    const char *gw = "0.0.0.0",
                    const char *pdns = "0.0.0.0",
                    const char *sdns = "0.0.0.0",
                    const char *apIp = "10.0.10.1",
                    const char *apSn = "255.255.255.0",
                    const char *apGw = "10.0.10.1"                           

    );
    void setAPInfo( const char *ssid = "mydefaultAP", 
                    const char *pass = "mydefaultAPPass"
                    );

    void setHostName(const char *hostName);
    String getHostName();
    void getWifiConfigJson(char *jsonConfig, uint16_t size);

    void addWifiList(const char *ssid, const char *pass);
    bool editWifiList(const char *oldSsid, const char *oldPass,
                      const char *newSsid, const char *newPass);
    bool removeWifiList(const char *ssid);

    
    void shutdownAP();
    void shutdownSTA();
    void shutdown();


    void getStaIP(char *ip);
    void getStaSubnetMask(char *sn);
    void getGateway(char *gw);
    void getAPIP(char *ip);
    void getAPSubnet(char *sn);
    void getAPGateway(char *gw);
    void getDNS1(char *dns1);
    void getDNS2(char *dns2);
    void getSTAWifiSSID(char *ssid);
    void getSTAWifiPassword(char *wifiPass);
    void getAPWifiSSID(char *ssid);
    void getAPWifiPassword(char *ssid);



    /* Note: Added as per issue #7, https://github.com/SyntaxHarvy/HACWifiManager/issues/7 */
    void setWifiOptions(
        bool persistent = false,
        WiFiSleepType wifiSleepStyle = WIFI_NONE_SLEEP,
        float outputPower = 16.5,
        WiFiPhyMode wifiPhyMode = WIFI_PHY_MODE_11G
    );

    // Event Function Declaration
    void onDebug(tListGenCbFnHaC1StrParam fn);      // Debug related events
    void onError(tListGenCbFnHaC1StrParam fn);      // Error related events
    void onSTAReady(tListGenCbFnHaC1StrParam fn);      // Event called when wifi successfully setup
    void onSTADisconnect(tListGenCbFnHaC1StrParam fn); // Event called when wifi loss connection for
    void onSTALoop(tListGenCbFnHaC1StrParam fn);    // either Access point or Station mode
    void onAPReady(tListGenCbFnHaC1StrParam fn);      // Event called when wifi successfully setup
    void onAPDisconnect(tListGenCbFnHaC1StrParam fn); // Event called when wifi loss connection for
    void onAPLoop(tListGenCbFnHaC1StrParam fn);    // either Access point or Station mode
                                                    // point mode
    void onAPNewConnection(tListGenCbFnHaC1StrParam fn);
private:

    HACWifiManagerParameters _wifiParam;       // Private declaration of AMPWifiManagerData
    tListGenCbFnHaC1StrParam _onDebugFn;       // Function callback declaration for onDebug event
    tListGenCbFnHaC1StrParam _onErrorFn;       // Function callback declaration for onError event
    tListGenCbFnHaC1StrParam _onSTAReadyFn;       // Function callback declaration for onready event
    tListGenCbFnHaC1StrParam _onSTADisconnectFn;  // Function callback declaration for onDisconnect event
    tListGenCbFnHaC1StrParam _onSTALoopFn; // Function callback declaration for onServiceLoop event
    tListGenCbFnHaC1StrParam _onAPReadyFn;       // Function callback declaration for onready event
    tListGenCbFnHaC1StrParam _onAPDisconnectFn;  // Function callback declaration for onDisconnect event
    tListGenCbFnHaC1StrParam _onAPLoopFn; // Function callback declaration for onServiceLoop event
    tListGenCbFnHaC1StrParam _onAPNewConnectionFn; // Function callback declaration for onServiceLoop event

    bool _onReadyStateSTAFlagOnce = false;
    bool _onReadyStateAPFlagOnce = false;
    bool _apFlagStarted = false;
    bool _manualStaNetworkSetupSuccess = false;
    bool _manualApNetworkSetupSuccess = false;
    bool _wifiScanFail = false;
    bool _initMdnsFlagOnce = false;
    enum WifiMode _wifiMode; // Enum Wifi Mode
    char _defaultAccessPointID[132];
    char _defaultAccessPointPass[132];
    char _wifiMacAddress[128];
    Tick _wifiScanTimer;
    Tick _staStartupTimer;
    Tick _staWatchdogTimer;
    uint8_t _wifiScanCountAttempt = 0;
    uint8_t _previousAPClientCount = 0;


    void _debug(const char *data); // Function prototype declaration for debug function
    void _debug(const __FlashStringHelper* data);
    void _printError(uint8_t errorCode);
    void _initWifiManager();
    void _initStation(bool isStartUp = true);
    void _setupSTAMultiWifi(bool isStartUp = true);
    bool _scanWifiListRssi(uint8_t totalAP);
    void _sortWifiRssi();
    void _setupSTASingleWifi(bool isStartUp = true);
    bool _setupNetworkManually(NetworkType netWorkType);
    void _startStation(const char *ssid, const char *pass);
    void _startAccessPoint();
    void _generateAccessPointInformation();
    int _generateWifiMacStrLower();
};
/* #endregion */

#include "HaCWifiManager-impl.h"

#endif
