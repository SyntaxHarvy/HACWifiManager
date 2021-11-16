#ifndef __AMPWIFI_MANAGER_H_
#define __AMPWIFI_MANAGER_H_

/* #region CONSTANT_DEFINITION */
#define MAX_WIFI_INFO_LIST              5                           // Maximum wifi information

#define ON_DEBUG_EVENTS_AMPWF           if(DEBUG_AMPWF) AMPWifiManager
#ifdef  ON_DEBUG_EVENTS_AMPWF
#define DEBUG_CALLBACK_AMPWF(msg)       { ON_DEBUG_EVENTS_AMPWF::_debug(msg); }
#elif
#define DEBUG_CALLBACK_AMPWF(msg) 
#endif

/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <ESP8266WiFi.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
typedef std::function<void()> tListGenCbFnAmp;                      // Standard void function with non-return value
typedef std::function<void(const char *)> tListGenCbFnAmp1StrParam; // Standard void function with non-return value

enum WifiMode { 
    STA_ONLY = 0,                                                   // Station mode only    
    AP_ONLY  = 1,                                                   // Access point mode only
    BOTH     = 2,                                                   // Both station and access point mode
};

/* #endregion */



class AMPWifiManager{
    public:
        AMPWifiManager(const char * wifiJsonStr);                   // Constructor
        void setup(const char * ssid, const char *pass);            // Function called on setting up the wifi manager with default ssid/pass
        void setup();                                               // Function called on setting up the wifi manager with no parameter
        void loop();                                                // Function called at the loop routine of the wifi
                                                                    // manager

        //Public Functions
        void setMode(uint8_t wifiMode);                             // Setting wifi mode according to the mode listed on enumWifiMode
        void setMultiWifi(bool enable = true);                      // Setting multi wifi or just one ssid
        void setNetworkDHCP(bool enable = true);                    // Setting network type to DHCP if enable else static
                                                                    // IP, GWs, Gatemash & DBS will be set accordingly from Json string if set to static                                                
                                                                    
        void setWifiParameters(const char * jsonStr);               // Setting wifi parameters based on the json string passed
        const char* getWifiParametersJson();                        // Get wifi parameters in json format                                                           

        // Event Function Declaration
        void onDebug(tListGenCbFnAmp1StrParam fn);                  // Debug related events
        void onReady(tListGenCbFnAmp fn);                           // Event called when wifi successfully setup
        void onDisconnect(tListGenCbFnAmp fn);                      // Event called when wifi loss connection for 
                                                                    // either Access point or Station mode
        void onReconnect(tListGenCbFnAmp fn);                       // Event called when wifi establishing connection
        void onStaMode(tListGenCbFnAmp fn);                         // Event called when wifi established on station mode
        void onAPMode(tListGenCbFnAmp fn);                          // Event called when wifi established on Access 
                                                                    // point mode
    private:
        /**
            * Class which is the data holder of the AmpWifiManager            
            * @param 
        */
        class AMPWifiManagerParameters{                                   
            public:
                AMPWifiManagerParameters();                         // Constructor
                void fromJson(const char *jsonStr);                 // Accept json string and convert it to AMPWifiManagerParameters class
                void toJson(const char *jsonStr);                   // Convert AMPWifiManagerParameters class into Json
            private:
                /**
                    * Struct which will hold the ssid and pass for each wifi Info.
                    * The maximum number of the list will be limited by MAX_WIFI_INFO_LIST.
                    * @param 
                */
                typedef struct WifiInfo {
	                const char * ssid;		
                    const char * pass;
                } t_wifiInfo;
                t_wifiInfo _wifiInfo[MAX_WIFI_INFO_LIST];
                uint8_t _mode;
                bool _multiWifiEnable;
                bool _dhcpNetworkEnable;

        };

        AMPWifiManagerParameters _wifiParam;                        // Private declaration of AMPWifiManagerData
        tListGenCbFnAmp1StrParam _onDebugFn;                        // Function callback declaration for debug event
        tListGenCbFnAmp _onReadyFn;                                 // Function callback declaration for onready event
        tListGenCbFnAmp _onDisconnectFn;                            // Function callback declaration for onDisconnect event
        tListGenCbFnAmp _onReconnectFn;                             // Function callback declaration for onReconnect event
        tListGenCbFnAmp _onStaModeFn;                               // Function callback declaration for onStaMode event
        tListGenCbFnAmp _onAPModeFn;                                // Function callback declaration for onAPMode event

        const char * _defaultSsid;                                  // Default ssid 
        const char * _defaultSsidPassword;                          // Default ssid password
        enum WifiMode _wifiMode;                                    // Enum Wifi Mode

        void _debug(const char *data);                              // Function prototype declaration for debug function


};


#endif

