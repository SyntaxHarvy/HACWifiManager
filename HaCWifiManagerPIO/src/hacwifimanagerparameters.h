/**
 *
 * @file HaCwifimanagerparameters.h
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

#ifndef __HACWIFI_MANAGER_PARAMETERS_H_
#define __HACWIFI_MANAGER_PARAMETERS_H_

/* #region CONSTANT_DEFINITION */
#define MAX_WIFI_INFO_LIST 5 // Maximum wifi information

#if defined(DEBUG_ESP_PORT) || defined(HAC_ENABLE_DEBUG)

#define DEBUG_HAC_WIFI_PARAM Serial // Custom serial debug
#define DEBUG_MSG_HAC_WIFI_PARAM(msg, ...)               \
    {                                                    \
        DEBUG_HAC_WIFI_PARAM.printf(msg, ##__VA_ARGS__); \
    } // Custom serial debug for Serial.printf
#define DEBUG_MSG_HAC_WIFI_PARAM_LN(msg)   \
    {                                      \
        DEBUG_HAC_WIFI_PARAM.println(msg); \
    } // Custom serial debug for Serial.println
#define DEBUG_MSG_HAC_WIFI_PARAM_SERIAL_INIT() \
    {                                          \
        DEBUG_HAC_WIFI_PARAM.begin(115200);    \
    } // Serial initialization baud rate
#define DEBUG_CALLBACK_HAC_PARAM(msg)          \
    {                                          \
        HACWifiManagerParameters::_debug(msg); \
    }
#define DEBUG_CALLBACK_HAC_PARAM2(msg, ...)               \
    {                                               \
        char buffer [255];                          \
			snprintf_P(                             \
				buffer, sizeof(buffer),             \
				msg,  \
				##__VA_ARGS__ \
			);                                      \
        HACWifiManagerParameters::_debug((const char*)buffer); \
    }
#else

#define DEBUG_MSG_HAC_WIFI_PARAM(msg, ...)
#define DEBUG_MSG_HAC_WIFI_PARAM_LN(msg)
#define DEBUG_MSG_HAC_WIFI_PARAM_SERIAL_INIT()
#define DEBUG_CALLBACK_HAC_PARAM(msg)
#define DEBUG_CALLBACK_HAC_PARAM2(msg, ...)

#endif

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCWifiManagerStringConst.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
/* #endregion */

/* #region GLOBAL_DECLARATION */
// typedef std::function<void()> tListGenCbFnHaCSub;                      // Standard void function with non-return value
typedef std::function<void(const char *)> tListGenCbFnHaC1StrParamSub; // Standard void function with non-return value
typedef std::function<void(uint8_t)> tListGenCbFnHaC1IntParamSub;      // Standard void function with non-return value
typedef struct WifiInfo
{
    String ssid;
    String pass;
    int8_t rssi;
} t_wifiInfo;

typedef struct NetworkInfo
{
    String ip;
    String sn;
    String gw;
    String pdns;
    String sdns;
} t_networkInfo;

enum NetworkType
{
    NETWORK_STATION = 1, // Network for Station
    NETWORK_AP = 2,      // Network for Access point
};

/* #endregion */

/* #region CLASS_DECLARATION */
class HACWifiManagerParameters
{
public:
    std::vector<t_wifiInfo> wifiInfo;
    t_wifiInfo accessPointInfo;
    t_networkInfo staNetworkInfo;
    t_networkInfo apNetworkInfo;

    HACWifiManagerParameters();         // Constructor
    ~HACWifiManagerParameters();        // Desctructor
    void fromJson(const char *jsonStr); // Accept json string and convert it to HACWifiManagerParameters class
    void toJson(String *jsonConfig);    // Convert HACWifiManagerParameters class into Json

    void setMode(uint8_t mode);
    uint8_t getMode();
    void setEnableMultiWifi(bool enable = false);
    bool getEnableMultiWifi();
    void setEnableDHCPNetwork(bool enableSta = false, bool enableAp = false);
    bool getEnableDHCPNetwork(NetworkType netWorkType);
    void setHostName(const char *hostName);
    String getHostName();

    void clearWifiList();
    void addWifiList(const char *ssid, const char *pass);
    bool editWifiList(const char *oldSsid, const char *oldPass,
                      const char *newSsid, const char *newPass);
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

    void onDebug(tListGenCbFnHaC1StrParamSub fn); // Debug related events
private:
    /**
     * Struct which will hold the ssid and pass for each wifi Info.
     * The maximum number of the list will be limited by MAX_WIFI_INFO_LIST.
     * @param
     */
    uint8_t _mode;
    bool _multiWifiEnable;
    bool _dhcpStaNetworkEnable;
    bool _dhcpApNetworkEnable;
    String _hostName;

    tListGenCbFnHaC1StrParamSub _onDebugFn; // Function callback declaration for debug event

    void _debug(const char *data);
    void _debug(const __FlashStringHelper *data); // Function prototype declaration for debug function
    bool _wifiExists(t_wifiInfo w, tListGenCbFnHaC1IntParamSub fn);

};

/* #endregion */

#include "HaCwifimanagerparameters-impl.h"

#endif