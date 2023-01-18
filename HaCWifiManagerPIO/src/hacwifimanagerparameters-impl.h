/**
 *
 * @file HaCwifimanagerparameters-impl.h
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

/* #region SELF_HEADER */
#include "HaCwifimanagerparameters.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
/* #endregion */

/* #region CLASS_DEFINITION */
/**
     * HACWifiManagerParameters Constructor
     */
HACWifiManagerParameters::HACWifiManagerParameters() {}

/**
     * HACWifiManagerParameters Destructor
     */
HACWifiManagerParameters::~HACWifiManagerParameters()
{
     this->wifiInfo.clear();
     this->wifiInfo = std::vector<t_wifiInfo>();  
}

/**
     * Accept json string and convert it to HACWifiManagerParameters class *
     * @param jsonStr Wifi parameters in json format as const char *.
     */
void HACWifiManagerParameters::fromJson(const char *jsonStr)
{
     DEBUG_CALLBACK_HAC_PARAM(F("Decoding json data."));
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 8192);
     DeserializationError error = deserializeJson(doc, jsonStr);
     doc.shrinkToFit();

     if (!error)
     {
          this->_mode = doc[__MODE__].as<uint8_t>();
          this->_multiWifiEnable = doc[__ENABLE_MULTI_WIFI__].as<bool>();
          this->_dhcpStaNetworkEnable = doc[__ENABLE_DHCP_NETWORK_STA__].as<bool>();
          this->_dhcpApNetworkEnable = doc[__ENABLE_DHCP_NETWORK_AP__].as<bool>();

          if(doc[__HOST_NAME__].as<String>() != HAC_WFM_STRING_NULL) 
               this->_hostName = doc[__HOST_NAME__].as<String>();
          /* #region Debug */   
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG1, this->_hostName.c_str());       
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG2, this->_mode);
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG3, this->_multiWifiEnable);
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG4, this->_dhcpStaNetworkEnable);
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG5, this->_dhcpApNetworkEnable);
          /* #endregion */

          this->staNetworkInfo.ip = doc[__STA_NETWORK__][___IP___].as<String>();
          this->staNetworkInfo.sn = doc[__STA_NETWORK__][___SN___].as<String>();
          this->staNetworkInfo.gw = doc[__STA_NETWORK__][___GW___].as<String>();
          this->staNetworkInfo.pdns = doc[__STA_NETWORK__][___PDNS___].as<String>();
          this->staNetworkInfo.sdns = doc[__STA_NETWORK__][___SDNS___].as<String>();

          this->accessPointInfo.ssid = doc[___AP___][___SSID___].as<String>();
          this->accessPointInfo.pass = doc[___AP___][___PASS___].as<String>();

          this->apNetworkInfo.ip = doc[__AP_NETWORK__][___IP___].as<String>();
          this->apNetworkInfo.sn = doc[__AP_NETWORK__][___SN___].as<String>();
          this->apNetworkInfo.gw = doc[__AP_NETWORK__][___GW___].as<String>();
          this->apNetworkInfo.pdns = doc[__AP_NETWORK__][___PDNS___].as<String>();
          this->apNetworkInfo.sdns = doc[__AP_NETWORK__][___SDNS___].as<String>();


          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG6, this->accessPointInfo.ssid.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG7, this->accessPointInfo.pass.c_str());
          DEBUG_CALLBACK_HAC_PARAM(F("Network setup for Station"));
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG8, this->staNetworkInfo.ip.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG9, this->staNetworkInfo.sn.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG10, this->staNetworkInfo.gw.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG11, this->staNetworkInfo.pdns.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG12, this->staNetworkInfo.sdns.c_str());
          DEBUG_CALLBACK_HAC_PARAM(F("Network setup for AP"));
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG8, this->apNetworkInfo.ip.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG9, this->apNetworkInfo.sn.c_str());
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG10, this->apNetworkInfo.gw.c_str());
          
          /* #endregion */

          JsonObject wifiList = doc[___WIFILIST___].as<JsonObject>();

          for (JsonPair p : wifiList)
          {

               //Check point for maximum number of wifi list allowed
               //Any list above the MAX_WIFI_INFO_LIST will be ignored
               if (this->wifiInfo.size() > MAX_WIFI_INFO_LIST)
                    return;
               
               JsonObject rowData = p.value().as<JsonObject>();

               t_wifiInfo w;

               w.ssid = rowData[___SSID___].as<String>();
               w.pass = rowData[___PASSWORD___].as<String>();
               //Initialize the rssi to the lowest dbm value
               w.rssi = -127;

               this->wifiInfo.push_back(w);

               /* #region Debug */
               #if defined(DEBUG_ESP_PORT) || defined(HAC_ENABLE_DEBUG)
               uint8_t index = this->wifiInfo.size() - 1;
               #endif
               
               DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG13, index);
               DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG14, this->wifiInfo[index].ssid.c_str());
               DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG15, this->wifiInfo[index].pass.c_str());
               /* #endregion */
          }
     }
}

/**
     * Convert HACWifiManagerParameters class into Json *
     * @param jsonStr Wifi parameters in json format as const char *.
     */
void HACWifiManagerParameters::toJson(char * jsonConfig, uint16_t size)
{

     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
     doc[__MODE__] = this->_mode;
     doc[__ENABLE_MULTI_WIFI__] = this->_multiWifiEnable;
     doc[__ENABLE_DHCP_NETWORK_STA__] = this->_dhcpStaNetworkEnable;
     doc[__ENABLE_DHCP_NETWORK_AP__] = this->_dhcpApNetworkEnable;
     doc[__HOST_NAME__] = this->_hostName;
     doc[__TOTAL_WIFI_LIST__] = this->wifiInfo.size();
     doc[__STA_NETWORK__][___IP___] = this->staNetworkInfo.ip;
     doc[__STA_NETWORK__][___SN___] = this->staNetworkInfo.sn;
     doc[__STA_NETWORK__][___GW___] = this->staNetworkInfo.gw;
     doc[__STA_NETWORK__][___PDNS___] = this->staNetworkInfo.pdns;
     doc[__STA_NETWORK__][___SDNS___] = this->staNetworkInfo.sdns;
     doc[__AP_NETWORK__][___IP___] = this->apNetworkInfo.ip;
     doc[__AP_NETWORK__][___SN___] = this->apNetworkInfo.sn;
     doc[__AP_NETWORK__][___GW___] = this->apNetworkInfo.gw;
     doc[___AP___][___SSID___] = this->accessPointInfo.ssid;
     doc[___AP___][___PASS___] = this->accessPointInfo.pass;

     for (uint8_t i = 0; i < this->wifiInfo.size(); i++)
     {
          doc[___WIFILIST___][String(i)][___SSID___] = this->wifiInfo[i].ssid;
          doc[___WIFILIST___][String(i)][___PASSWORD___] = this->wifiInfo[i].pass;
     }

     doc.shrinkToFit();
     serializeJson(doc, jsonConfig, size);
}

/**
     * Setting wifi mode.     
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     */
void HACWifiManagerParameters::setMode(uint8_t mode)
{
     this->_mode = mode;
}

/**
     * Getting wifi mode.    
     * @return Wifimode      
     */
uint8_t HACWifiManagerParameters::getMode()
{
     return this->_mode;
}

/**
     * Getting wifi list count.          
     */
uint8_t HACWifiManagerParameters::getWifiListCount()
{
     return this->wifiInfo.size();
}

/**
     * Setting multiwifi enable mode.          
     */
void HACWifiManagerParameters::setEnableMultiWifi(bool enable)
{
     this->_multiWifiEnable = enable;
}

/**
     * Getting multiwifi enable mode. 
     * @return Enable Multiwifi flag        
     */
bool HACWifiManagerParameters::getEnableMultiWifi()
{
     return this->_multiWifiEnable;
}

/**
     * Setting DHCP enable mode.     
     * @param enableSta Enable DHCP for station
     * @param enableAP Enable DHCP for access point    
     */
void HACWifiManagerParameters::setEnableDHCPNetwork(bool enableSta, bool enableAp)
{
     this->_dhcpStaNetworkEnable = enableSta;
     this->_dhcpApNetworkEnable = enableAp;
}

/**
     * Getting DHCP enable mode.  
     * @param netWorkType Two network type for station and access point.        
     * @return DHCP enable flag for the network selected.
     */
bool HACWifiManagerParameters::getEnableDHCPNetwork(NetworkType netWorkType)
{
     switch (netWorkType)
     {
     case NETWORK_STATION:
          return this->_dhcpStaNetworkEnable;
          break;
     case NETWORK_AP:
          return this->_dhcpApNetworkEnable;
          break;
     
     default:
          break;
     }

     return false;
}

/**
     * Setting DHCP enable mode.     
     * @param hostName Device Network Host Name    
     */
void HACWifiManagerParameters::setHostName(const char *hostName)
{
     this->_hostName = String(hostName);
}
/**
     * Setting DHCP enable mode.     
     * @return String Device Network Host Name    
     */
String HACWifiManagerParameters::getHostName()
{
     return this->_hostName;
}


/**
     * Clear wifi info list        
     */
void HACWifiManagerParameters::clearWifiList()
{
     this->wifiInfo.clear();
     this->wifiInfo = std::vector<t_wifiInfo>();
}

/**
     * Adding wifi info to the wifi info list
     * @param ssid Wifi SSID
     * @param pass Wifi Password          
     */
void HACWifiManagerParameters::addWifiList(const char *ssid, const char *pass)
{
     //Remove the checking for empty password to allow a connection to non secure wifi AP
     //Note: Refer, https://github.com/SyntaxHarvy/HACWifiManager/issues/10
     if (String(ssid) == "")
          return;

     WifiInfo w;
     w.ssid = String(ssid);
     w.pass = String(pass);
     w.rssi = -127;

     //If wifi exist terminate from here
     if(this->_wifiExists(w, [&](uint8_t index){
          this->wifiInfo[index].pass = w.pass;
     }))
     {
          DEBUG_CALLBACK_HAC_PARAM(F("Wifi just added already exist."));
          return;
     } 

     DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG16, this->wifiInfo.size());
     this->wifiInfo.push_back(w);
}

/**
     * Editing wifi info from the wifiinfo list
     * @param ssid Wifi SSID
     * @param pass Wifi Password   
     * @return True if edit is successful else False       
     */
bool HACWifiManagerParameters::editWifiList(const char *oldSsid, const char *oldPass,
                                            const char *newSsid, const char *newPass)
{
     //Remove the checking for empty password to allow a connection to non secure wifi AP
     //Note: Refer, https://github.com/SyntaxHarvy/HACWifiManager/issues/10

     if (String(oldSsid) == "" ||          
         String(newSsid) == ""         
         )
          return false;
     
     //Check wether the SSID exist from the list
     //If it exist, the wifiExists function will update the
     //wifi info based from the new information set of wifi info
     WifiInfo wOld, wNew;
     wOld.ssid = String(oldSsid);
     wOld.pass = String(oldPass);
     wNew.ssid = String(newSsid);
     wNew.pass = String(newPass);

     if(!this->_wifiExists(wOld, [&](uint8_t index){
          this->wifiInfo[index].ssid = wNew.ssid;
          this->wifiInfo[index].pass = wNew.pass;
     })) return false;

     return true;
}

/**
     * Removing wifi info from wifi list info
     * @param ssid Wifi SSID   
     * @return True if removing wifi from the list is successful otherwise false.     
     */
bool HACWifiManagerParameters::removeWifiList(const char *ssid)
{
     t_wifiInfo w;
     w.ssid = String(ssid);
     
     if(!this->_wifiExists(w, [&](uint8_t index){
          std::vector<t_wifiInfo> tmp;
          for (auto entry : this->wifiInfo)
          {
               //Add to the temporary vector array if the ssid is different than to be remove
               if(entry.ssid != ssid) tmp.push_back(entry);
          }
          //Clear the wifiInfo
          this->wifiInfo.clear();
          this->wifiInfo = std::vector<t_wifiInfo>();
          //Copy tmp to wifiInfo
          for (auto entry : tmp)
               this->wifiInfo.push_back(entry);
          //Clear temporary vector array
          tmp.clear();
          tmp = std::vector<t_wifiInfo>();
     })) return false;

     return true;
}
/**
     * Debug Callback function.          * 
     * @param fn Standard non return function with a const * char parameter*.
     */

void HACWifiManagerParameters::onDebug(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onDebugFn = fn;
}

/**
 * Debug function.
 * @param data Debug message .
 */
void HACWifiManagerParameters::_debug(const __FlashStringHelper* data)
{
    char buffer[128];
    memset(buffer, '\0', sizeof(buffer));
    strcpy_P(buffer, (const char*) data);
    if (this->_onDebugFn)
        this->_onDebugFn(buffer);
}

/**
     * Debug function.     
     * Note, this is the debug of HACWifiManagerParameters which is a private class of HaCWifiManager
     * @param data Debug message.
     */
void HACWifiManagerParameters::_debug(const char *data)
{
     if (this->_onDebugFn)
          this->_onDebugFn(data);
}

/**
     * Check if wifi exists from the existing list
     * @param w Wifi info
     * @return True if the wifi exists else False
     */
bool HACWifiManagerParameters::_wifiExists(t_wifiInfo w, tListGenCbFnHaC1IntParamSub fn)
{
     uint8_t i = 0;
     for (auto entry : this->wifiInfo)
     {
          DEBUG_CALLBACK_HAC_PARAM2(HAC_WFM_VERBOSE_MSG17, w.ssid.c_str(), entry.ssid.c_str());
          if (entry.ssid == w.ssid)
          {
               //If it exist raised the function callback to initiate the action remotely
               if(fn) fn(i);               
               return true;
          }
          i++;
     }

     return false;
}


/* #endregion */