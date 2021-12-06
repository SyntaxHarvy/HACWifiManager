/**
 *
 * @file hacwifimanager.cpp
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
#include "hacwifimanager.h"
/* #endregion */

/* #region CLASS_DEFINITION */
/**
     * Constructor.
     */
HaCWifiManager::HaCWifiManager() {
     //wifi parameter onDebug callback
     this->_wifiParam.onDebug([&](const char *msg){
          DEBUG_CALLBACK_HAC(msg);     
     });
}

/**
     * Called when setting up the library.     
     * Note: Wifi parameters passed as json data.
     * @param wifiJsonStr Wifi Parameters in Json String format
     */
void HaCWifiManager::setup(const char *wifiJsonStr)
{
     //Setup the wifi parameters from Json format string
     DEBUG_CALLBACK_HAC("Setting up wifi parameters from json data.");
     //Process the json data
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 8192);
     DeserializationError error = deserializeJson(doc, wifiJsonStr);
     doc.shrinkToFit();
     if (!error)
     {
          DEBUG_CALLBACK_HAC("Data is a valid json.");
          //Set wifiParam hostname
          this->_generateWifiMacStrLower();
          this->_wifiParam.setHostName(String("hacwfm" + String(this->_wifiMacAddress) ).c_str());
          //Json data is a valid json format hence it will be safe to be process by the wifi parameter class
          this->_wifiParam.fromJson(wifiJsonStr);

          /* #region Debug */
          //Read back the value from the wifiParam as json string
          DEBUG_CALLBACK_HAC(String("Reading back json value from wifiParam class =>" + String(this->_wifiParam.toJson())).c_str());
          /* #endregion */

          //Setting up wifi Core
          this->setup();
     }
     else
     {
          DEBUG_CALLBACK_HAC("Failed to resolve wifi parameters, due to invalid json format!");
     }
}

/**
     * Called when setting up the library.
     * Note: Wifi parameters passed as json data.
     * @param defaultSSID const char * wifi default ssid.
     * @param defaultPass const char * wifi default password.
     * @param hostName Device network host name.
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     * @param enableMultiWifi bool enable multi wifi.
     * @param enableDHCPNetwork bool enable DHCP Network.
     * @param ip const char * Static Network ip. Note, enableDHCPNetwork shall be false before assigning a network ip.
     * @param sn const char * Static Network sn. Note, enableDHCPNetwork shall be false before assigning a network sn.
     * @param gw const char * Static Network gw. Note, enableDHCPNetwork shall be false before assigning a network gw.
     * @param pdns const char * Static Network pdns. Note, enableDHCPNetwork shall be false before assigning a network pdns.
     * @param sdns const char * Static Network sdns. Note, enableDHCPNetwork shall be false before assigning a network sdns.
     * @param apSsid const char * Access point SSID.
     * @param apPass const char * Access point Password.
     * @param apIp Access Point static ip.
     * @param apSn Access Point static subnet.
     * @param apGw Access Point static gateway.
     */
void HaCWifiManager::setup(
    const char *defaultSSID,
    const char *defaultPass,
    const char *hostName,
    WifiMode mode,
    bool enableMultiWifi,
    bool enableStaDHCPNetwork,
    bool enableApDHCPNetwork,
    const char *staIp,
    const char *staGw,
    const char *staSn,
    const char *staPdns,
    const char *staSdns,
    const char *apSsid,
    const char *apPass,
    const char *apIp,
    const char *apSn,
    const char *apGw
    )
{

     //Set wifiParam hostname
     if(String(hostName) == "")
     {
          this->_generateWifiMacStrLower();
          this->setHostName(String("hacwfm" + String(this->_wifiMacAddress) ).c_str());
     }
     else
          this->setHostName(hostName);
     

     //Set wifi mode
     this->setMode(mode);
     //Set enable multi wifi
     this->setEnableMultiWifi(enableMultiWifi);
     //Set Enable DHCP
     this->setEnableDHCPNetwork(enableStaDHCPNetwork, enableApDHCPNetwork);
     //Set network information
     this->setNetWorkInformation(staIp, staSn, staGw, staPdns, staSdns, apIp, apSn, apGw);
     //Set Access point information
     this->setAPInfo(apSsid, apPass);
     //Set first object of Wifi information list
     std::vector<t_wifiInfo> tmp;
     for(auto entry: this->_wifiParam.wifiInfo)
          tmp.push_back(entry);
     this->_wifiParam.clearWifiList();
     this->addWifiList(defaultSSID, defaultPass);
     for(auto entry: tmp)
          this->_wifiParam.wifiInfo.push_back(entry);
     tmp.clear();

     //Setup wifimanager
     this->setup();

     /* #region Debug */
     DEBUG_CALLBACK_HAC("Parameters in Json");
     DEBUG_CALLBACK_HAC(this->_wifiParam.toJson().c_str());
     /* #endregion */

}

/**
     * Called when setting up the wifi manager core
     */

void HaCWifiManager::setup()
{
     //Check mode validity
     if (this->_wifiParam.getMode() < STA_ONLY || this->_wifiParam.getMode() > BOTH_STA_AP)
     {
          this->_printError(1);
          DEBUG_CALLBACK_HAC("Invalid wifi mode!");
          return;
     }
     //Check and ssid or password pair
     if (this->_wifiParam.getWifiListCount() == 0)
     {
          this->_printError(2);
          DEBUG_CALLBACK_HAC("There is no ssid configured!");
          return;
     }

     //Initialize watchdog timer
     this->_staWatchdogTimer = Tick(30000);
     this->_staStartupTimer = Tick(1000);
     this->_wifiScanTimer = Tick((unsigned long)WIFI_SCAN_TIMEOUT);

     //Wifi is ready for start up
     DEBUG_CALLBACK_HAC("Initializing  manager..");
     this->_initWifiManager();
}

/**
     * Setting wifi mode.     
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     */
void HaCWifiManager::setMode(WifiMode mode)
{
     this->_wifiParam.setMode(mode);
}

/**
     * Getting wifi mode.  
     * @return Wifimode.        
     */
WifiMode HaCWifiManager::getMode()
{
     return (WifiMode)this->_wifiParam.getMode();
}

/**
     * Getting station ip in string.  
     * @return IP in string format.        
     */
String HaCWifiManager::getStaIP()
{
     return WiFi.localIP().toString();
}

/**
     * Getting Access Point ip in string.  
     * @return IP in string format.        
     */
String HaCWifiManager::getAPIP()
{
     return WiFi.softAPIP().toString();
}


/**
     * Setting multi wifi enable flag.     
     * @param enable Enable multi wifi flag.
     */
void HaCWifiManager::setEnableMultiWifi(bool enable)
{
     this->_wifiParam.setEnableMultiWifi(enable);
}

/**
     * Getting multi wifi enable flag.   
     * @return enable multi wifi flag       
     */
bool HaCWifiManager::getEnableMultiWifi()
{
     return this->_wifiParam.getEnableMultiWifi();
}

/**
     * Setting enable DHCP flag.     
     * @param enableSta Enable DHCP for station.
     * @param enableAp Enable DHCP for static.
     */
void HaCWifiManager::setEnableDHCPNetwork(bool enableSta, bool enableAp)
{
     this->_wifiParam.setEnableDHCPNetwork(enableSta, enableAp);
}

/**
     * Getting DHCP Enable flag.   
     * @param netWorkType Two network type for station and access point.
     * @return DHCP enable flag for the network selected.       
     */
bool HaCWifiManager::getEnableDHCPNetwork(NetworkType netWorkType)
{
     return this->_wifiParam.getEnableDHCPNetwork(netWorkType);
}

/**
     * Setting enable DHCP flag.  
     * Note, this network settings will be available only if the DHCP set to off   
     * @param ip Station IP.
     * @param sn Station Subnet.
     * @param gw Station Gateway.
     * @param pdns Station Primary DNS.
     * @param sdns Station Secondary DNS.
     * @param apIp Access point ip
     * @param apSn Access point Subnet
     * @param apGw Access point gateway
     */
void HaCWifiManager::setNetWorkInformation(
                    const char *ip,
                    const char *sn,
                    const char *gw,
                    const char *pdns,
                    const char *sdns,
                    const char *apIp,
                    const char *apSn,
                    const char *apGw
    )
{
     this->_wifiParam.staNetworkInfo.ip = String(ip);
     this->_wifiParam.staNetworkInfo.gw = String(gw);
     this->_wifiParam.staNetworkInfo.sn = String(sn);
     this->_wifiParam.staNetworkInfo.pdns = String(pdns);
     this->_wifiParam.staNetworkInfo.sdns = String(sdns);
     this->_wifiParam.apNetworkInfo.ip = String(apIp);
     this->_wifiParam.apNetworkInfo.sn = String(apSn);
     this->_wifiParam.apNetworkInfo.gw = String(apGw);

}

/**
     * Setting Access Point information.  
     * Note, Default ssid = 'hacAP' and default pass = 'appass'
     * @param ssid Access Point SSID.
     * @param pass Access Point Password.
     */
void HaCWifiManager::setAPInfo(
     const char *ssid, 
     const char *pass)
{
     this->_wifiParam.accessPointInfo.ssid = String(ssid);
     this->_wifiParam.accessPointInfo.pass = String(pass);

}

/**
     * Setting device host name
     * @param hostName Device network host name
     */
void HaCWifiManager::setHostName(const char *hostName)
{
     this->_wifiParam.setHostName(hostName);
}

/**
     * Getting device host name
     * @return Device network host name
     */
String HaCWifiManager::getHostName()
{
     return this->_wifiParam.getHostName();
}

/**
     * Getting the wifimanager configuration in Json string
     * @return Wifi manager configuration in Json string
     */
String HaCWifiManager::getWifiConfigJson()
{
     return this->_wifiParam.toJson();
}

/**
     * Adding wifi info to the wifi info list
     * @param ssid Wifi SSID
     * @param pass Wifi Password          
     */
void HaCWifiManager::addWifiList(const char *ssid, const char *pass)
{     
     this->_wifiParam.addWifiList(ssid, pass);
}

/**
     * Editing wifi info from the wifiinfo list
     * @param ssid Wifi SSID
     * @param pass Wifi Password   
     * @return True if edit is successful else False       
     */
bool HaCWifiManager::editWifiList(const char *oldSsid, const char *oldPass,
                                            const char *newSsid, const char *newPass)
{
     return this->_wifiParam.editWifiList(oldSsid, oldPass, newSsid, newPass);
}

/**
     * Removing wifi info from wifi list info
     * @param ssid Wifi SSID        
     */
bool HaCWifiManager::removeWifiList(const char *ssid)
{
     return this->_wifiParam.removeWifiList(ssid);
}

/**
     * Shutdown Wifi Station.          
     */
void HaCWifiManager::shutdownSTA()
{
     WiFi.disconnect();
     WiFi.mode(WIFI_OFF);
     WiFi.forceSleepBegin();
}

/**
     * Shutdown Wifi Access Point.          
     */
void HaCWifiManager::shutdownAP()
{
     WiFi.softAPdisconnect(true);
     this->_apFlagStarted = false;
}

/**
     * Shutdown Wifi manager.          
     */
void HaCWifiManager::shutdown()
{
     this->shutdownSTA();
     this->shutdownAP();
}

/**
     * Setting Wifi Options.
     * Note: Added as per issue #7, https://github.com/SyntaxHarvy/HACWifiManager/issues/7
     * @param persistent Wifi Persistent
     * @param wifiSleepStyle Wifi Sleep Style
     * @param outputPower Wifi Output power  
     * @param wifiPhyMode Wifi Physical Mode        
     */
void HaCWifiManager::setWifiOptions(bool persistent,
                                    WiFiSleepType wifiSleepStyle,
                                    float outputPower,
                                    WiFiPhyMode wifiPhyMode
)
{
     //Setting wifi sleep style
     if(wifiSleepStyle < WIFI_NONE_SLEEP || wifiSleepStyle > WIFI_MODEM_SLEEP)
     {
          DEBUG_CALLBACK_HAC("Invalid wifi sleep style.");
          this->_printError(21);
          return;
     }
     WiFi.setSleepMode(wifiSleepStyle);
     DEBUG_CALLBACK_HAC(String("Wifi sleep style =>" + String(wifiSleepStyle)).c_str());
     //Setting wifi Physical Mode
     if(wifiPhyMode < WIFI_PHY_MODE_11B || wifiPhyMode > WIFI_PHY_MODE_11N)
     {
          DEBUG_CALLBACK_HAC("Invalid wifi physical mode.");
          this->_printError(22);
          return;
     }
     WiFi.setPhyMode(wifiPhyMode);
     DEBUG_CALLBACK_HAC(String("Wifi physical mode =>" + String(wifiPhyMode)).c_str());

     //Setting wifi persistent enable
     WiFi.persistent(persistent);
     DEBUG_CALLBACK_HAC(String("Wifi persistent enable =>" + String(persistent)).c_str());

     //Setting wifi power output
     WiFi.setOutputPower(outputPower);
     DEBUG_CALLBACK_HAC(String("Wifi output power =>" + String(outputPower)).c_str());

}

/**
     * Loop routine of the library.
     * Note: loop routine      
     * @param none
     */
void HaCWifiManager::loop()
{
     //Wifi Station onReady event
     if (WiFi.status() == WL_CONNECTED && !this->_onReadyStateSTAFlagOnce)
     {
          if (this->_onSTAReadyFn)
               this->_onSTAReadyFn(WiFi.SSID().c_str());
          this->_onReadyStateSTAFlagOnce = true;
     }
     //Wifi station onDisconnect event
     if (WiFi.status() != WL_CONNECTED && this->_onReadyStateSTAFlagOnce)
     {
          if (this->_onSTADisconnectFn)
               this->_onSTADisconnectFn(WiFi.SSID().c_str());
          this->_onReadyStateSTAFlagOnce = false;

          //Set the rssi for the current ssid to the lowest dbM value
          //in order to put it lowest on the new scanning
          this->_wifiParam.wifiInfo[0].rssi = -127;

          //if multiwifi is enabled then reinitialized the wifi multimode setup
          if (this->_wifiParam.getEnableMultiWifi())
               this->_setupSTAMultiWifi();
     }
     //Wifi onSTATLoop event
     if (this->_onReadyStateSTAFlagOnce)
     {
          //TO DO: Pass json data for the status of the STA
          if (this->_onSTALoopFn)
               this->_onSTALoopFn("");
     }
     //Access point onReady event
     if (this->_apFlagStarted && !this->_onReadyStateAPFlagOnce)
     {
          this->_onReadyStateAPFlagOnce = true;          
          if (this->_onAPReadyFn)
               this->_onAPReadyFn(WiFi.softAPSSID().c_str());
     }
     //Access point onDisconnect event
     if (!this->_apFlagStarted && this->_onReadyStateAPFlagOnce)
     {
          this->_onReadyStateAPFlagOnce = false;          
          if (this->_onAPDisconnectFn)
               this->_onAPDisconnectFn(WiFi.softAPSSID().c_str());
     }
     //Access point onAPLoop event
     if (this->_onReadyStateAPFlagOnce)
     {          
          if (this->_onAPLoopFn)
               this->_onAPLoopFn("");
     }
     //Access point new client connection event
     if(WiFi.softAPgetStationNum() != this->_previousAPClientCount)
     {
          this->_previousAPClientCount = WiFi.softAPgetStationNum();
          //TO DO: Pass json data for the status of the STA
          if (this->_onAPNewConnectionFn)
               this->_onAPNewConnectionFn(String("{\"no_client\": " + String(WiFi.softAPgetStationNum()) + "}").c_str());

     }
     //All timers
     this->_wifiScanTimer.handle();
     this->_staStartupTimer.handle();
     this->_staWatchdogTimer.handle();
}

/**
     * Debug Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onDebug(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onDebugFn = fn;
}

/**
     * On Error Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onError(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onErrorFn = fn;
}

/**
     * On Ready Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onSTAReady(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onSTAReadyFn = fn;
}

/**
     * On Disconnect Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onSTADisconnect(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onSTADisconnectFn = fn;
}

/**
     * On Station mode Loop Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onSTALoop(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onSTALoopFn = fn;
}

/**
     * On AP ready Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onAPReady(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onAPReadyFn = fn;
}

/**
     * On AP disconnect Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onAPDisconnect(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onAPDisconnectFn = fn;
}

/**
     * On AP mode Loop Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onAPLoop(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onAPLoopFn = fn;
}

/**
     * AP new connection Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onAPNewConnection(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onAPNewConnectionFn = fn;
}


/**
     * Debug function.     
     * @param data const char .
     */

void HaCWifiManager::_debug(const char *data)
{
     if (this->_onDebugFn)
          this->_onDebugFn(String(String(HAC_DEBUG_PREFIX) + " " + String(data)).c_str());
}

/**
     * Print error function.     
     * @param data uint8_t .
     */

void HaCWifiManager::_printError(uint8_t errorCode)
{
     if (this->_onErrorFn)
          this->_onErrorFn(String(errorCode).c_str());
}

/**
     * Setting up wifi configuration based on the mode   
     */
void HaCWifiManager::_initWifiManager()
{
     switch (this->_wifiParam.getMode())
     {
     case STA_ONLY:
          WiFi.softAPdisconnect(true);
          WiFi.mode(WIFI_STA);
          this->_initStation();
          break;
     case AP_ONLY:
          WiFi.softAPdisconnect(true);
          WiFi.disconnect();
          WiFi.mode(WIFI_OFF);
          WiFi.forceSleepBegin();
          this->_startAccessPoint();
          break;
     case BOTH_STA_AP:
          WiFi.mode(WIFI_AP_STA);
          this->_initStation();
          delay(1000);
          this->_startAccessPoint();
          break;
     default:
          break;
     }
}

/**
     * Setting up wifi configuration based on the mode   
     */
void HaCWifiManager::_initStation(bool isStartUp)
{
     DEBUG_CALLBACK_HAC("Initializing station..");
     //Check if it is multi or single wifi
     if (this->_wifiParam.getEnableMultiWifi())
          this->_setupSTAMultiWifi(isStartUp);
     else
          this->_setupSTASingleWifi(isStartUp);
}

/**
     * Setting multi wifi mode.     
     */
void HaCWifiManager::_setupSTAMultiWifi(bool isStartup)
{
     //If there is only one ssid on the list then set it up as a single wifi
     if (this->_wifiParam.getWifiListCount() == 1)
     {
          this->_setupSTASingleWifi(isStartup);
          return;
     }
     DEBUG_CALLBACK_HAC("Setting up STA multi wifi..");

     //Clean previous scan
     WiFi.scanDelete();
     DEBUG_CALLBACK_HAC("Deleted previous scan.");
     //Remove previous wifi ssid&password
     WiFi.disconnect();
     DEBUG_CALLBACK_HAC("Previous wifi session ssid/password.");
     //Start wifi scan
     WiFi.scanNetworks(true);
     DEBUG_CALLBACK_HAC("Start wifi scan in async mode");

     this->_wifiScanCountAttempt = 0;
     
     this->_wifiScanTimer.onTick([&]()
                                 {
                                      int8_t count = WiFi.scanComplete();
                                      if (count < 0)
                                      {
                                           this->_wifiScanCountAttempt++;

                                           //Raise error if wifi scan fail for max attempt
                                           if (this->_wifiScanCountAttempt > MAX_WIFI_SCAN_ATTEMPT)
                                           {
                                                this->_wifiScanFail = true;
                                                this->_printError(11);
                                                DEBUG_CALLBACK_HAC("Wifi scan failed or timeout.");
                                                this->_wifiScanTimer.stop();
                                           }
                                           return;
                                      }

                                      //Continue scanning until count return other than 255 and 0
                                      if (count != 255 && count > 0)
                                           this->_wifiScanTimer.stop();
                                      else
                                           return;

                                      DEBUG_CALLBACK_HAC(String("Wifi scan success. Total AP found = " + String(count)).c_str());

                                      if (!this->_scanWifiListRssi(count))
                                      {
                                           DEBUG_CALLBACK_HAC("Warning: None of ssid listed was found from the scanning.");
                                           this->_printError(12);
                                      }
                                      else
                                           this->_sortWifiRssi();

                                      this->_setupSTASingleWifi();

                                      DEBUG_CALLBACK_HAC("Multiwifi setup done.");
                                 });
     this->_wifiScanTimer.begin();
}
/**
     * Scan wifi rssi.     
     * @param totalAP- Total number of access point scanned
     */
bool HaCWifiManager::_scanWifiListRssi(uint8_t totalAP)
{
     String ssid;
     int32_t rssi;
     uint8_t encType;
     uint8_t *bssid;
     int32_t channel;
     bool hidden;
     bool atleastOneSsidListFoundFlag = false;

     DEBUG_CALLBACK_HAC("Scanning Wifi AP Rssi..");
     for (uint8_t i = 0; i < totalAP; i++)
     {
          // Get network information
          WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel, hidden);
          // Check if the WiFi network contains an entry in Wifiinfo list
          uint8_t j = 0;
          for (auto entry : this->_wifiParam.wifiInfo)
          {
               // Check SSID
               DEBUG_CALLBACK_HAC(String("scan ssid = " + ssid + " listed ssid = " + entry.ssid).c_str());
               if (ssid == entry.ssid)
               {
                    // Known network
                    atleastOneSsidListFoundFlag = true;
                    this->_wifiParam.wifiInfo[j].rssi = WiFi.RSSI(i);
                    DEBUG_CALLBACK_HAC(String("Wifi listed ssid = " + ssid + " rssi = " + String(this->_wifiParam.wifiInfo[j].rssi)).c_str());
               }
               j++;
          }
     }

     return atleastOneSsidListFoundFlag;
}

/**
     * Sort wifi list from strongest to weakest rssi.     
     * @param totalAP- Total number of access point scanned
     */
void HaCWifiManager::_sortWifiRssi()
{
     DEBUG_CALLBACK_HAC("Sorting Wifi AP based on Rssi");
     for (uint8_t i = 0; i < this->_wifiParam.getWifiListCount(); i++)
     {
          for (uint8_t j = 0; j < this->_wifiParam.getWifiListCount(); j++)
          {
               if (this->_wifiParam.wifiInfo[i].rssi > this->_wifiParam.wifiInfo[j].rssi)
               {
                    t_wifiInfo tmp;

                    tmp = this->_wifiParam.wifiInfo[i];
                    this->_wifiParam.wifiInfo[i] = this->_wifiParam.wifiInfo[j];
                    this->_wifiParam.wifiInfo[j] = tmp;
               }
          }
     }
     DEBUG_CALLBACK_HAC(String("Strongest RSSI =>" + this->_wifiParam.wifiInfo[0].ssid).c_str());
}

/**
     * Setting single wifi mode.     
     */
void HaCWifiManager::_setupSTASingleWifi(bool isStartUp)
{
     DEBUG_CALLBACK_HAC("Setting up single STA wifi..");
     if (isStartUp)
     {
          delay(1000);
          this->_startStation(this->_wifiParam.wifiInfo[0].ssid.c_str(),
                              this->_wifiParam.wifiInfo[0].pass.c_str());
     }
     else //After startup, wifi setup will be done asynchronously
     {          
          this->_staStartupTimer.onTick([&]()
                                        {
                                             this->_startStation(this->_wifiParam.wifiInfo[0].ssid.c_str(),
                                                                 this->_wifiParam.wifiInfo[0].pass.c_str());
                                             this->_staStartupTimer.stop();
                                        });
          this->_staStartupTimer.begin();
     }
}

/**
     * Setting station.    
     * @param ssid-const char* wifi station ssid
     * @param pass-const char* wifi station pass 
     */
void HaCWifiManager::_startStation(const char *ssid, const char *pass)
{
     //Check if network set as DHCP for station     
     if (!(this->_wifiParam.getEnableDHCPNetwork(NETWORK_STATION)))
          this->_manualStaNetworkSetupSuccess = this->_setupNetworkManually(NETWORK_STATION);

     /* #region Debug */
     DEBUG_CALLBACK_HAC("Setting up station..");
     DEBUG_CALLBACK_HAC("WIFI_STA = true");
     DEBUG_CALLBACK_HAC(String("SSID = " + String(ssid)).c_str());
     DEBUG_CALLBACK_HAC(String("PASS = " + String(pass)).c_str());
     /* #endregion */
     //Start wifi network
     WiFi.begin(ssid, pass);

     //Start the station watchdog timer which shall check if the connection
     //established 30secs later after station startup     
     this->_staWatchdogTimer.onTick([&]()
                                   {
                                        if (!this->_onReadyStateSTAFlagOnce)
                                             this->_initStation(false);
                                        this->_staWatchdogTimer.stop();
                                   });
     this->_staWatchdogTimer.begin();
}

/**
     * Setting network manually.     
     */
bool HaCWifiManager::_setupNetworkManually(NetworkType netWorkType)
{
     t_networkInfo netInfo;
     if(netWorkType == NETWORK_STATION) 
          netInfo = this->_wifiParam.staNetworkInfo;     
     if(netWorkType == NETWORK_AP) 
          netInfo = this->_wifiParam.apNetworkInfo;     

     DEBUG_CALLBACK_HAC("Setting up network manually..");
     IPAddress ip, gw, sn, pdns, sdns;
     if (!ip.fromString(netInfo.ip))
     {
          this->_printError(3);
          DEBUG_CALLBACK_HAC(String("Failed to setup IP(" + String(netInfo.ip) + ") manually..").c_str());
          return false;
     }
     if (!sn.fromString(netInfo.sn))
     {
          this->_printError(4);
          DEBUG_CALLBACK_HAC(String("Failed to setup subnet(" + String(netInfo.sn) + ") manually..").c_str());
          return false;
     }
     if (!gw.fromString(netInfo.gw))
     {
          this->_printError(5);
          DEBUG_CALLBACK_HAC(String("Failed to setup gateway(" + String(netInfo.gw) + ") manually..").c_str());
          return false;
     }
     if (!pdns.fromString(netInfo.pdns) && netWorkType == NETWORK_STATION)
     {
          this->_printError(6);
          DEBUG_CALLBACK_HAC(String("Failed to setup primary dns(" + String(netInfo.pdns) + ") manually..").c_str());
          return false;
     }
     if (!sdns.fromString(netInfo.sdns) && netWorkType == NETWORK_STATION)
     {
          this->_printError(7);
          DEBUG_CALLBACK_HAC(String("Failed to setup secondary dns(" + String(netInfo.sdns) + ") manually..").c_str());
          return false;
     }

     if(netWorkType == NETWORK_STATION)
     {
          if (!WiFi.config(ip, gw, sn, pdns, sdns))
          {
               this->_printError(81);
               DEBUG_CALLBACK_HAC("Network manual configuration failed for Station.");
               return false;
          }
     }
     if(netWorkType == NETWORK_AP)
     {
          if (!WiFi.softAPConfig(ip, gw, sn))
          {
               this->_printError(82);
               DEBUG_CALLBACK_HAC(netInfo.ip.c_str());
               DEBUG_CALLBACK_HAC(netInfo.gw.c_str());
               DEBUG_CALLBACK_HAC(netInfo.sn.c_str());
               DEBUG_CALLBACK_HAC("Network manual configuration failed for Access Point.");
               return false;
          }
     }


     DEBUG_CALLBACK_HAC("Network successfully setup manually.");

     return true;
}

/**
     * Function that will setup access point.     
     */
void HaCWifiManager::_startAccessPoint()
{

     //Check if Access point is provided
     if (this->_wifiParam.accessPointInfo.ssid == "null")
     {
          this->_generateAccessPointInformation();
          this->_wifiParam.accessPointInfo.ssid = String(this->_defaultAccessPointID);
          this->_wifiParam.accessPointInfo.pass = String(this->_defaultAccessPointPass);
     }

     /* #region Debug */
     DEBUG_CALLBACK_HAC(String("AP SSID = " + this->_wifiParam.accessPointInfo.ssid).c_str());
     DEBUG_CALLBACK_HAC(String("AP PASS = " + this->_wifiParam.accessPointInfo.pass).c_str());
     /* #endregion */

     //Raise error if ap ssid or password is empty
     if (this->_wifiParam.accessPointInfo.ssid == "" || this->_wifiParam.accessPointInfo.pass == "")
     {
          this->_printError(9);
          DEBUG_CALLBACK_HAC("Invalid access point parameters.");
          return;
     }

     //Check DHCP enable network is set
     if (!(this->_wifiParam.getEnableDHCPNetwork(NETWORK_AP)))
          this->_manualApNetworkSetupSuccess = this->_setupNetworkManually(NETWORK_AP);



     if (!WiFi.softAP(this->_wifiParam.accessPointInfo.ssid.c_str(), this->_wifiParam.accessPointInfo.pass.c_str(), 6, 0, 5))
     {
          this->_printError(10);
          DEBUG_CALLBACK_HAC(this->_wifiParam.accessPointInfo.ssid.c_str());
          DEBUG_CALLBACK_HAC(this->_wifiParam.accessPointInfo.pass.c_str());
          DEBUG_CALLBACK_HAC("Failed to setup access point.");
          return;
     }
     this->_apFlagStarted = true;
     DEBUG_CALLBACK_HAC("Access point successfully setup.");
}

/**
     * Function that will generate the access point default information such as ssid and pass.     
     */
void HaCWifiManager::_generateAccessPointInformation()
{
     DEBUG_CALLBACK_HAC("Generating Access point information..");
     //Setting memory
     memset(this->_defaultAccessPointID, 0, sizeof(this->_defaultAccessPointID));
     memset(this->_defaultAccessPointPass, 0, sizeof(this->_defaultAccessPointPass));
     memset(this->_wifiMacAddress, 0, sizeof(this->_wifiMacAddress));

     if (this->_generateWifiMacStrLower())
     {
          sprintf(this->_defaultAccessPointID, "hac-%s", this->_wifiMacAddress);
          sprintf(this->_defaultAccessPointPass, "%s", this->_wifiMacAddress);
     }
     else
     {
          sprintf(this->_defaultAccessPointID, "hac-%u", ESP.getChipId());
          sprintf(this->_defaultAccessPointPass, "%u", ESP.getChipId());
     }
}
/**
     * Function that will convert mac address into lower case format
     * @return int value not equal to 0 when generation is successful
     */

int HaCWifiManager::_generateWifiMacStrLower()
{
     char temp[128];
     strcpy(temp, WiFi.macAddress().c_str());
     if (!strlen(temp))
          return 0;
     char *pointer = strtok(temp, ":");
     if (pointer == nullptr)
          return 0;

     strcpy(this->_wifiMacAddress, pointer);
     while ((pointer = strtok(nullptr, ":")) != nullptr)
     {
          strcat(this->_wifiMacAddress, pointer);
     }
     for (int i = 0; this->_wifiMacAddress[i]; i++)
     {
          this->_wifiMacAddress[i] = tolower(this->_wifiMacAddress[i]);
     }
     return 1;
}

/* #endregion */