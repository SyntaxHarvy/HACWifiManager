#ifndef __AMPWIFI_MANAGER_PARAMETERS_H_
#define __AMPWIFI_MANAGER_PARAMETERS_H_


/* #region CONSTANT_DEFINITION */
#define MAX_WIFI_INFO_LIST              5                           // Maximum wifi information

#ifdef DEBUG_ESP_PORT

#define DEBUG_HAC_WIFI_PARAM                       Serial                                                          // Custom serial debug
#define DEBUG_MSG_HAC_WIFI_PARAM(msg, ...)                 { DEBUG_HAC_WIFI_PARAM.printf(msg, ## __VA_ARGS__); }         // Custom serial debug for Serial.printf
#define DEBUG_MSG_HAC_WIFI_PARAM_LN(msg)                   { DEBUG_HAC_WIFI_PARAM.println(msg); }                        // Custom serial debug for Serial.println
#define DEBUG_MSG_HAC_WIFI_PARAM_SERIAL_INIT()             { DEBUG_HAC_WIFI_PARAM.begin(115200); }                       // Serial initialization baud rate
#define DEBUG_CALLBACK_HAC_PARAM(msg)                      { HACWifiManagerParameters::_debug(msg); }

#else

#define DEBUG_MSG_HAC_WIFI_PARAM(msg, ...) 
#define DEBUG_MSG_HAC_WIFI_PARAM_LN(msg)
#define DEBUG_MSG_HAC_WIFI_PARAM_SERIAL_INIT()
#define DEBUG_CALLBACK_HAC_PARAM(msg)

#endif

/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
/* #endregion */

/* #region GLOBAL_DECLARATION */
//typedef std::function<void()> tListGenCbFnHaCSub;                      // Standard void function with non-return value
typedef std::function<void(const char *)> tListGenCbFnHaC1StrParamSub; // Standard void function with non-return value

/* #endregion */


/* #region CLASS_DECLARATION */
class HACWifiManagerParameters{                                   
    public:
        HACWifiManagerParameters();                         // Constructor
        ~HACWifiManagerParameters();                        // Desctructor
        void fromJson(const char *jsonStr);                 // Accept json string and convert it to HACWifiManagerParameters class
        String toJson();                                    // Convert HACWifiManagerParameters class into Json

        void setMode(uint8_t mode);
        uint8_t getMode();
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

        uint8_t getWifiListCount();

        void onDebug(tListGenCbFnHaC1StrParamSub fn);       // Debug related events
    private:
        /**
            * Struct which will hold the ssid and pass for each wifi Info.
            * The maximum number of the list will be limited by MAX_WIFI_INFO_LIST.
            * @param 
        */
        typedef struct WifiInfo {
            String ssid;		
            String pass;
        } t_wifiInfo;

        typedef struct NetworkInfo {
            String ip;		
            String sn;
            String gw;
            String pdns;
            String sdns;
        } t_networkInfo;


        std::vector<t_wifiInfo> _wifiInfo;
        t_networkInfo _networkInfo;
        uint8_t _mode;
        bool _multiWifiEnable;
        bool _dhcpNetworkEnable;
        uint8_t _totalWifiList;

        tListGenCbFnHaC1StrParamSub _onDebugFn;                        // Function callback declaration for debug event
        void _debug(const char *data);                              // Function prototype declaration for debug function
        
};

/* #endregion */
#endif