/**
 *
 * @file hacwifimanagerparameters.cpp
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
#include "hacwifimanagerparameters.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
/* #endregion */

/* #region CLASS_DEFINITION */
/**
     * HACWifiManagerParameters Desstructor     *
     */
HACWifiManagerParameters::HACWifiManagerParameters() {}

/**
     * HACWifiManagerParameters Constructor     *
     */
HACWifiManagerParameters::~HACWifiManagerParameters()
{
     this->wifiInfo.clear();
}

/**
     * Accept json string and convert it to HACWifiManagerParameters class *
     * @param jsonStr Wifi parameters in json format as const char *.
     */
void HACWifiManagerParameters::fromJson(const char *jsonStr)
{
     DEBUG_CALLBACK_HAC_PARAM("Decoding json data.");
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 8192);
     DeserializationError error = deserializeJson(doc, jsonStr);
     doc.shrinkToFit();
     if (!error)
     {
          this->_mode = doc["mode"].as<uint8_t>();
          this->_multiWifiEnable = doc["enable_multi_wifi"].as<bool>();
          this->_dhcpStaNetworkEnable = doc["enable_dhcp_network_sta"].as<bool>();
          this->_dhcpStaNetworkEnable = doc["enable_dhcp_network_ap"].as<bool>();

          if(doc["host_name"].as<String>() != "null") 
               this->_hostName = doc["host_name"].as<String>();
          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM(String("Hostname = " + this->_hostName).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("Mode = " + String(this->_mode)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("Enable Multi Wifi = " + String(this->_multiWifiEnable)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("Enable DHCP Station = " + String(this->_dhcpStaNetworkEnable)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("Enable DHCP Access Point = " + String(this->_dhcpApNetworkEnable)).c_str());
          /* #endregion */

          this->staNetworkInfo.ip = doc["sta_network"]["ip"].as<String>();
          this->staNetworkInfo.sn = doc["sta_network"]["sn"].as<String>();
          this->staNetworkInfo.gw = doc["sta_network"]["gw"].as<String>();
          this->staNetworkInfo.pdns = doc["sta_network"]["pdns"].as<String>();
          this->staNetworkInfo.sdns = doc["sta_network"]["sdns"].as<String>();

          this->accessPointInfo.ssid = doc["ap"]["ssid"].as<String>();
          this->accessPointInfo.pass = doc["ap"]["pass"].as<String>();

          this->apNetworkInfo.ip = doc["ap_network"]["ip"].as<String>();
          this->apNetworkInfo.sn = doc["ap_network"]["sn"].as<String>();
          this->apNetworkInfo.gw = doc["ap_network"]["gw"].as<String>();
          this->apNetworkInfo.pdns = doc["ap_network"]["pdns"].as<String>();
          this->apNetworkInfo.sdns = doc["ap_network"]["sdns"].as<String>();


          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM(String("AP SSID = " + String(this->accessPointInfo.ssid)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("AP PASS = " + String(this->accessPointInfo.pass)).c_str());
          DEBUG_CALLBACK_HAC_PARAM("Network setup for Station");
          DEBUG_CALLBACK_HAC_PARAM(String("IP = " + String(this->staNetworkInfo.ip)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("SN = " + String(this->staNetworkInfo.sn)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("GW = " + String(this->staNetworkInfo.gw)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("PDNS = " + String(this->staNetworkInfo.pdns)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("SDNS = " + String(this->staNetworkInfo.sdns)).c_str());
          DEBUG_CALLBACK_HAC_PARAM("Network setup for AP");
          DEBUG_CALLBACK_HAC_PARAM(String("IP = " + String(this->apNetworkInfo.ip)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("SN = " + String(this->apNetworkInfo.sn)).c_str());
          DEBUG_CALLBACK_HAC_PARAM(String("GW = " + String(this->apNetworkInfo.gw)).c_str());
          /* #endregion */

          JsonObject wifiList = doc["wifilist"].as<JsonObject>();

          for (JsonPair p : wifiList)
          {

               //Check point for maximum number of wifi list allowed
               //Any list above the MAX_WIFI_INFO_LIST will be ignored
               if (this->wifiInfo.size() > MAX_WIFI_INFO_LIST)
                    return;
               
               JsonObject rowData = p.value().as<JsonObject>();

               t_wifiInfo w;

               w.ssid = rowData["ssid"].as<String>();
               w.pass = rowData["password"].as<String>();
               //Initialize the rssi to the lowest dbm value
               w.rssi = -127;

               this->wifiInfo.push_back(w);

               /* #region Debug */
               uint8_t index = this->wifiInfo.size() - 1;
               DEBUG_CALLBACK_HAC_PARAM(String("Wifi Index = " + String(index)).c_str());
               DEBUG_CALLBACK_HAC_PARAM(String("ssid = " + this->wifiInfo[index].ssid).c_str());
               DEBUG_CALLBACK_HAC_PARAM(String("password = " + this->wifiInfo[index].pass).c_str());
               /* #endregion */
          }
     }
}

/**
     * Convert HACWifiManagerParameters class into Json *
     * @param jsonStr Wifi parameters in json format as const char *.
     */
String HACWifiManagerParameters::toJson()
{

     String tmp = "";
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
     doc["mode"] = this->_mode;
     doc["enable_multi_wifi"] = this->_multiWifiEnable;
     doc["enable_dhcp_network_sta"] = this->_dhcpStaNetworkEnable;
     doc["enable_dhcp_network_ap"] = this->_dhcpApNetworkEnable;
     doc["host_name"] = this->_hostName;
     doc["total_wifi_list"] = this->wifiInfo.size();
     doc["sta_network"]["ip"] = this->staNetworkInfo.ip;
     doc["sta_network"]["sn"] = this->staNetworkInfo.sn;
     doc["sta_network"]["gw"] = this->staNetworkInfo.gw;
     doc["sta_network"]["pdns"] = this->staNetworkInfo.pdns;
     doc["sta_network"]["sdns"] = this->staNetworkInfo.sdns;
     doc["ap_network"]["ip"] = this->apNetworkInfo.ip;
     doc["ap_network"]["sn"] = this->apNetworkInfo.sn;
     doc["ap_network"]["gw"] = this->apNetworkInfo.gw;
     doc["ap"]["ssid"] = this->accessPointInfo.ssid;
     doc["ap"]["pass"] = this->accessPointInfo.pass;

     

     for (uint8_t i = 0; i < this->wifiInfo.size(); i++)
     {
          doc["wifilist"][String(i)]["ssid"] = this->wifiInfo[i].ssid;
          doc["wifilist"][String(i)]["password"] = this->wifiInfo[i].pass;
     }

     doc.shrinkToFit();
     serializeJson(doc, tmp);

     return tmp;
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
     * @return Device Network Host Name    
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
          DEBUG_CALLBACK_HAC_PARAM("Wifi just added already exist.");
          return;
     } 

     DEBUG_CALLBACK_HAC_PARAM(String("Total wifi =>" + String(this->wifiInfo.size())).c_str());
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
          //Copy tmp to wifiInfo
          for (auto entry : tmp)
               this->wifiInfo.push_back(entry);
          //Clear temporary vector array
          tmp.clear();
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
     * Note, this is the debug of HACWifiManagerParameters which is a private class of HaCWifiManager
     * @param data const char *.
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
          DEBUG_CALLBACK_HAC_PARAM(String("New ssid = " + w.ssid + " Comparing to =" +  entry.ssid).c_str());
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