#ifndef __AMPWIFI_MANAGER_H_
#define __AMPWIFI_MANAGER_H_


/* #region CONSTANT_DEFINITION */

/* #region Debug */
#define HAC_DEBUG_PREFIX                        "[HACWIFIMANAGER]"
#define WIFI_MODE_ERROR_CODE                    "0x001"
#define WIFI_SSID_MISSING_ERROR_CODE            "0x002"

#ifdef DEBUG_ESP_PORT

#define DEBUG_HAC_WIFI                       Serial                                                          // Custom serial debug
#define DEBUG_MSG_HAC_WIFI(msg, ...)                 { DEBUG_HAC_WIFI.printf(msg, ## __VA_ARGS__); }         // Custom serial debug for Serial.printf
#define DEBUG_MSG_HAC_WIFI_LN(msg)                   { DEBUG_HAC_WIFI.println(msg); }                        // Custom serial debug for Serial.println
#define DEBUG_MSG_HAC_WIFI_SERIAL_INIT()             { DEBUG_HAC_WIFI.begin(115200); }                       // Serial initialization baud rate
#define DEBUG_CALLBACK_HAC(msg)                      { HaCWifiManager::_debug(msg); }

#else

#define DEBUG_MSG_HAC_WIFI(msg, ...) 
#define DEBUG_MSG_HAC_WIFI_LN(msg)
#define DEBUG_MSG_HAC_WIFI_SERIAL_INIT()
#define DEBUG_CALLBACK_HAC(msg)

#endif


/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "hacwifimanagerparameters.h"
/* #endregion */


/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
typedef std::function<void()> tListGenCbFnHaC;                      // Standard void function with non-return value
typedef std::function<void(const char *)> tListGenCbFnHaC1StrParam; // Standard void function with non-return value

enum WifiMode { 
    STA_ONLY = 1,                                                   // Station mode only    
    AP_ONLY  = 2,                                                   // Access point mode only
    BOTH     = 3,                                                   // Both station and access point mode
};

/* #endregion */


/* #region CLASS_DECLARATION */
class HaCWifiManager{
    public:
        HaCWifiManager();                                          // Constructor        
        void setup(const char * wifiJsonStr);
        void setup(
            const char * defaultSSID, 
            const char * defaultPass,
            WifiMode mode = STA_ONLY,
            bool enableMultiWifi = true,
            bool enableDHCPNetwork = true,
            const char * ip = "",
            const char * gw = "",
            const char * sn = "",
            const char * pdns = "",
            const char * sdns = ""
        );                                               
        void setup();                                               // Function called on setting up the wifi manager Core

        void loop();                                                // Function called at the loop routine of the wifi
                                                                    // manager

        //Public Functions
        void setMode(WifiMode mode);
        WifiMode getMode();
        void setEnableMultiWifi(bool enable = false);
        bool getEnableMultiWifi();
        void setEnableDHCPNetwork(bool enable = false);
        bool getEnableDHCPNetwork();

        void addWifiList(const char *ssid, const char *pass);
        bool editWifiList(const char * oldSsid, const char * oldPass, 
                          const char * newSsid, const char * newPass);
        bool removeWifiList(const char *ssid);

        void setNetworkIP(const char *ip);
        String getNetworkIp();
        void setNetworkSn(const char *sn);
        String getNetworkSn();
        void setNetworkGw(const char *gw);
        String getNetworkGw();
        void setNetworkPdns(const char *pdns);
        String getNetworkPdns();
        void setNetworkSdns(const char *sdns);
        String getNetworkSdns();

        // Event Function Declaration
        void onDebug(tListGenCbFnHaC1StrParam fn);                  // Debug related events
        void onError(tListGenCbFnHaC1StrParam fn);                  // Error related events
        void onReady(tListGenCbFnHaC fn);                           // Event called when wifi successfully setup
        void onDisconnect(tListGenCbFnHaC fn);                      // Event called when wifi loss connection for 
                                                                    // either Access point or Station mode
        void onReconnect(tListGenCbFnHaC fn);                       // Event called when wifi establishing connection
        void onStaMode(tListGenCbFnHaC fn);                         // Event called when wifi established on station mode
        void onAPMode(tListGenCbFnHaC fn);                          // Event called when wifi established on Access 
                                                                    // point mode
    private:
        /**
            * Class which is the data holder of the HACWifiManager            
            * @param 
        */

        HACWifiManagerParameters _wifiParam;                        // Private declaration of AMPWifiManagerData
        tListGenCbFnHaC1StrParam _onDebugFn;                        // Function callback declaration for onDebug event
        tListGenCbFnHaC1StrParam _onErrorFn;                        // Function callback declaration for onError event
        tListGenCbFnHaC _onReadyFn;                                 // Function callback declaration for onready event
        tListGenCbFnHaC _onDisconnectFn;                            // Function callback declaration for onDisconnect event
        tListGenCbFnHaC _onReconnectFn;                             // Function callback declaration for onReconnect event
        tListGenCbFnHaC _onStaModeFn;                               // Function callback declaration for onStaMode event
        tListGenCbFnHaC _onAPModeFn;                                // Function callback declaration for onAPMode event

        const char * _defaultSsid;                                  // Default ssid 
        const char * _defaultSsidPassword;                          // Default ssid password
        enum WifiMode _wifiMode;                                    // Enum Wifi Mode

        void _debug(const char *data);                              // Function prototype declaration for debug function


};
/* #endregion */

#endif



