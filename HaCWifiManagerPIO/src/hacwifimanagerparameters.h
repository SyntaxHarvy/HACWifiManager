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

#define DEBUG_CALLBACK_HAC_PARAM(msg)
#define DEBUG_CALLBACK_HAC_PARAM2(msg, ...)

#endif

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCWifiManagerStringConst.h"

/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include "global.h"
#include <ArduinoJson.h>
#include <vector>
/* #endregion */

/* #region GLOBAL_DECLARATION */
#define __MODE__ "mode"
#define __ENABLE_MULTI_WIFI__ "enable_multi_wifi"
#define __ENABLE_DHCP_NETWORK_STA__ "enable_dhcp_network_sta"
#define __ENABLE_DHCP_NETWORK_AP__ "enable_dhcp_network_ap"
#define __HOST_NAME__ "host_name"
#define __STA_NETWORK__ "sta_network"
#define __AP_NETWORK__ "ap_network"
#define __TOTAL_WIFI_LIST__ "total_wifi_list"
#define ___IP___ "ip"
#define ___SN___ "sn"
#define ___GW___ "gw"
#define ___PDNS___ "pdns"
#define ___SDNS___ "sdns"
#define ___AP___ "ap"
#define ___SSID___ "ssid"
#define ___PASS___ "pass"
#define ___PASSWORD___ "password"
#define ___WIFILIST___ "wifilist"
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
    void toJson(char *jsonConfig, uint16_t size);    // Convert HACWifiManagerParameters class into Json

    void setMode(uint8_t mode);
    uint8_t getMode();
    void setEnableMultiWifi(bool enable = false);
    bool getEnableMultiWifi();
    void setEnableDHCPNetwork(bool enableSta = false, bool enableAp = false);
    bool getEnableDHCPNetwork(NetworkType netWorkType);
    void setHostName(const char *hostName);
    const char * getHostName();

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
    char *_hostName;

    tListGenCbFnHaC1StrParamSub _onDebugFn; // Function callback declaration for debug event

    void _debug(const char *data);
    void _debug(const __FlashStringHelper *data); // Function prototype declaration for debug function
    bool _wifiExists(t_wifiInfo w, tListGenCbFnHaC1IntParamSub fn);

};

/* #endregion */

#include "HaCwifimanagerparameters-impl.h"

#endif