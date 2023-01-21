/**
 *
 * @file hacwifimanager-impl.h
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
#include "HaCwifimanager.h"
/* #endregion */

/* #region CLASS_DEFINITION */
/**
     * Constructor.
     */
HaCWifiManager::HaCWifiManager() {
     
     this->_wifiParam = new HACWifiManagerParameters();
     //wifi parameter onDebug callback
     this->_wifiParam->onDebug([&](const char *msg){
          DEBUG_CALLBACK_HAC(msg);     
     });
 
}

/**
     * Constructor.
     */
HaCWifiManager::~HaCWifiManager() {
     if(this->_wifiParam) delete this->_wifiParam;
}

/**
     * Called when setting up the library.     
     * Note: Wifi parameters passed as json data.
     * @param wifiJsonStr Wifi Parameters in Json String format
     */
void HaCWifiManager::setup(const char *wifiJsonStr)
{
     if(!this->_wifiParam)return;

     //Setup the wifi parameters from Json format string
     DEBUG_CALLBACK_HAC(F("Setting up wifi parameters from json data."));
     //Process the json data
     DynamicJsonDocument doc(__ESP_HEAPS_BLOCK_SIZE__ - 8192);
     DeserializationError error = deserializeJson(doc, wifiJsonStr);
     doc.shrinkToFit();
     
     if (!error)
     {
          DEBUG_CALLBACK_HAC(F("Data is a valid json."));
          //Set wifiParam hostname
          //this->_generateWifiMacStrLower();
         
          this->_wifiParam->setHostName(DEFAULT_HOST_NAME);
          //Json data is a valid json format hence it will be safe to be process by the wifi parameter class
          
          this->_wifiParam->fromJson(wifiJsonStr);

          //Setting up wifi Core
          this->setup();
     }
     else
     {
          DEBUG_CALLBACK_HAC(F("Failed to resolve wifi parameters, due to invalid json format!"));
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
          this->setHostName(DEFAULT_HOST_NAME);
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
     for(auto entry: this->_wifiParam->wifiInfo)
          tmp.push_back(entry);
     this->_wifiParam->clearWifiList();
     this->addWifiList(defaultSSID, defaultPass);
     for(auto entry: tmp)
          this->_wifiParam->wifiInfo.push_back(entry);
     tmp.clear();

     //Setup wifimanager
     this->setup();

}

/**
     * Called when setting up the wifi manager core
     */

void HaCWifiManager::setup()
{
     if(!this->_wifiParam)return;

     //Check mode validity
     if (this->_wifiParam->getMode() < STA_ONLY || this->_wifiParam->getMode() > BOTH_STA_AP)
     {
          this->_printError(1);
          DEBUG_CALLBACK_HAC(F("Invalid wifi mode!"));
          return;
     }
     //Check and ssid or password pair
     if (this->_wifiParam->getWifiListCount() == 0)
     {
          this->_printError(2);
          DEBUG_CALLBACK_HAC(F("There is no ssid configured!"));
          return;
     }


     //Initialize watchdog timer
     this->_staWatchdogTimer = Tick(30000);
     this->_staStartupTimer = Tick(1000);
     this->_wifiScanTimer = Tick((unsigned long)WIFI_SCAN_TIMEOUT);

     //Wifi is ready for start up
     DEBUG_CALLBACK_HAC(F("Initializing  manager.."));
     this->_initWifiManager();
}


/**
     * Setting wifi mode.     
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     */
void HaCWifiManager::setMode(WifiMode mode)
{
     if(!this->_wifiParam)return;

     this->_wifiParam->setMode(mode);
}

/**
     * Getting wifi mode.  
     * @return Wifimode.        
     */
WifiMode HaCWifiManager::getMode()
{
     if(!this->_wifiParam)return(WifiMode)0;

     return (WifiMode)this->_wifiParam->getMode();
}


/**
     * Getting station ip .  
     * @param ip IP in char pointer
     */
void HaCWifiManager::getStaIP(char *ip)
{     
     strcpy(ip, &(WiFi.localIP().toString())[0]);
}

/**
     * Getting station subnet .  
     * @param sn Subnet pointer address
     */
void HaCWifiManager::getStaSubnetMask(char *sn)
{     
     strcpy(sn, &(WiFi.subnetMask().toString())[0]);
}

/**
     * Getting station gateway .  
     * @param gw Gateway pointer address
     */
void HaCWifiManager::getGateway(char *gw)
{     
     strcpy(gw, &(WiFi.gatewayIP().toString())[0]);
}

/**
     * Getting Access Point IP.  
     * @param ip Access point IP
     */
void HaCWifiManager::getAPIP(char *ip)
{  
     __TCP_INFO__ info;      
     __TCP_ADAPTER_GET_IP_INFO(__TCP_ADAPTER_INTERFACE__, &info);     

     strcpy(ip, &(IPAddress(info.ip.addr).toString())[0]);
}

/**
     * Getting station gateway .  
     * @param sn Access point subnet pointer
     */
void HaCWifiManager::getAPSubnet(char *sn)
{  
     __TCP_INFO__ info;  
     __TCP_ADAPTER_GET_IP_INFO(__TCP_ADAPTER_INTERFACE__, &info);   

     strcpy(sn, &(IPAddress(info.netmask.addr).toString())[0]);
}

/**
     * Getting station gateway .  
     * @param gw Access point gateway pointer
     */
void HaCWifiManager::getAPGateway(char *gw)
{  
     __TCP_INFO__ info;  
     __TCP_ADAPTER_GET_IP_INFO(__TCP_ADAPTER_INTERFACE__, &info);   

     strcpy(gw, &(IPAddress(info.gw.addr).toString())[0]);
}

/**
     * Getting network dns1.  
     * @param dns1 DNS server1 
     */
void HaCWifiManager::getDNS1(char *dns1)
{  
     __TCP_INFO__ info;

     #ifdef ESP8266
     info.ip.addr = dns_getserver(1)->addr;
     #endif
     #ifdef ESP32
     info.ip.addr = dns_getserver(1)->u_addr.ip4.addr;
     #endif
     
     strcpy(dns1, &(IPAddress(info.ip.addr).toString())[0]);
}

/**
     * Getting network dns2.  
     * @param dns2 DNS server2 
     */
void HaCWifiManager::getDNS2(char *dns2)
{  
     __TCP_INFO__ info;
     
     #ifdef ESP8266
     info.ip.addr = dns_getserver(2)->addr;
     #endif
     #ifdef ESP32
     info.ip.addr = dns_getserver(2)->u_addr.ip4.addr;
     #endif
     
     strcpy(dns2, &(IPAddress(info.ip.addr).toString())[0]);
}

/**
     * Getting station ssid.  
     * @param ssid Station ssid pointer address
     */
void HaCWifiManager::getSTAWifiSSID(char *ssid)
{  
     strcpy(ssid, &(WiFi.SSID())[0]);
}

/**
     * Getting Access Point ssid.   
     * @param apPassword Access point ssid pointer address
     */
void HaCWifiManager::getAPWifiSSID(char *ssid)
{  
     strcpy(ssid, &(WiFi.softAPSSID())[0]);
}

/**
     * Setting multi wifi enable flag.     
     * @param enable Enable multi wifi flag.
     */
void HaCWifiManager::setEnableMultiWifi(bool enable)
{
     if(!this->_wifiParam)return;

     this->_wifiParam->setEnableMultiWifi(enable);
}

/**
     * Getting multi wifi enable flag.   
     * @return enable multi wifi flag       
     */
bool HaCWifiManager::getEnableMultiWifi()
{
     if(!this->_wifiParam)return false;

     return this->_wifiParam->getEnableMultiWifi();
}

/**
     * Setting enable DHCP flag.     
     * @param enableSta Enable DHCP for station.
     * @param enableAp Enable DHCP for static.
     */
void HaCWifiManager::setEnableDHCPNetwork(bool enableSta, bool enableAp)
{
     if(!this->_wifiParam)return;

     this->_wifiParam->setEnableDHCPNetwork(enableSta, enableAp);
}


/**
     * Getting DHCP Enable flag.   
     * @param netWorkType Two network type for station and access point.
     * @return DHCP enable flag for the network selected.       
     */
bool HaCWifiManager::getEnableDHCPNetwork(NetworkType netWorkType)
{
     if(!this->_wifiParam)return false;
     return this->_wifiParam->getEnableDHCPNetwork(netWorkType);
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
     if(!this->_wifiParam)return;

     this->_wifiParam->staNetworkInfo.ip = String(ip);
     this->_wifiParam->staNetworkInfo.gw = String(gw);
     this->_wifiParam->staNetworkInfo.sn = String(sn);
     this->_wifiParam->staNetworkInfo.pdns = String(pdns);
     this->_wifiParam->staNetworkInfo.sdns = String(sdns);
     this->_wifiParam->apNetworkInfo.ip = String(apIp);
     this->_wifiParam->apNetworkInfo.sn = String(apSn);
     this->_wifiParam->apNetworkInfo.gw = String(apGw);

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
     if(!this->_wifiParam)return;

     this->_wifiParam->accessPointInfo.ssid = String(ssid);
     this->_wifiParam->accessPointInfo.pass = String(pass);

}

/**
     * Setting device host name
     * @param hostName Device network host name
     */
void HaCWifiManager::setHostName(const char *hostName)
{
     if(!this->_wifiParam)return;

     this->_wifiParam->setHostName(hostName);
}

/**
     * Getting device host name
     * @return Device network host name
     */
const char * HaCWifiManager::getHostName()
{
     if(!this->_wifiParam)return "";

     return this->_wifiParam->getHostName();
}

/**
     * Getting the wifimanager configuration in Json string
     */
void HaCWifiManager::getWifiConfigJson(char *jsonConfig, uint16_t size)
{
     if(!this->_wifiParam)return;

     this->_wifiParam->toJson(jsonConfig, size);
}

/**
     * Adding wifi info to the wifi info list
     * @param ssid Wifi SSID
     * @param pass Wifi Password          
     */
void HaCWifiManager::addWifiList(const char *ssid, const char *pass)
{  
     if(!this->_wifiParam)return;

     this->_wifiParam->addWifiList(ssid, pass);
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
     if(!this->_wifiParam)return false;

     return this->_wifiParam->editWifiList(oldSsid, oldPass, newSsid, newPass);
}

/**
     * Removing wifi info from wifi list info
     * @param ssid Wifi SSID        
     */
bool HaCWifiManager::removeWifiList(const char *ssid)
{
     if(!this->_wifiParam)return false;

     return this->_wifiParam->removeWifiList(ssid);
}

/**
     * Shutdown Wifi Station.          
     */
void HaCWifiManager::shutdownSTA()
{
     WiFi.disconnect();
     WiFi.mode(WIFI_OFF);

     #ifdef ESP8266
     WiFi.forceSleepBegin();
     #endif
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

#ifdef ESP8266
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
          DEBUG_CALLBACK_HAC(F("Invalid wifi sleep style."));
          this->_printError(21);
          return;
     }
     WiFi.setSleepMode(wifiSleepStyle);     
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG100, wifiSleepStyle);
     //Setting wifi Physical Mode
     if(wifiPhyMode < WIFI_PHY_MODE_11B || wifiPhyMode > WIFI_PHY_MODE_11N)
     {
          DEBUG_CALLBACK_HAC(F("Invalid wifi physical mode."));
          this->_printError(22);
          return;
     }
     WiFi.setPhyMode(wifiPhyMode);
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG101, wifiPhyMode);

     //Setting wifi persistent enable
     WiFi.persistent(persistent);
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG102, persistent);

     //Setting wifi power output
     WiFi.setOutputPower(outputPower);
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG103, outputPower);

}
#endif

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

          //Initialize MDNS once
          if(!this->_initMdnsFlagOnce)
          {
               char hostName[30];
               memset(hostName, '\0', 30);
               strcpy(hostName, this->getHostName());
               //If host name is empty then set the default hostname
               if(hostName[0] == '\0') strcpy(hostName, DEFAULT_HOST_NAME);

               if(MDNS.begin(hostName))
               {
                    DEBUG_CALLBACK_HAC(F("MDNS Started."));
                    DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG116, hostName);
                    this->_initMdnsFlagOnce = true;
               }
               else
               {
                    DEBUG_CALLBACK_HAC(F("MDNS failed to start."));
                    this->_printError(23);
               }              
          }
          //Saving to file
          this->_save();

          //Destroying parameters
          if(this->_wifiParam)
          {
               DEBUG_CALLBACK_HAC(F("Destroying wifi parameters.."));
               delete this->_wifiParam;
          }

     }
     //Wifi station onDisconnect event
     if (WiFi.status() != WL_CONNECTED && this->_onReadyStateSTAFlagOnce)
     {
          //this->_initParam();

          if (this->_onSTADisconnectFn)
               this->_onSTADisconnectFn(WiFi.SSID().c_str());
          this->_onReadyStateSTAFlagOnce = false;

          //Set the rssi for the current ssid to the lowest dbM value
          //in order to put it lowest on the new scanning
          this->_wifiParam->wifiInfo[0].rssi = -127;

          //if multiwifi is enabled then reinitialized the wifi multimode setup
          if (this->_wifiParam->getEnableMultiWifi())
               this->_setupSTAMultiWifi();

          
     }
     //Wifi onSTATLoop event
     if (this->_onReadyStateSTAFlagOnce)
     {
          //TO DO: Pass json data for the status of the STA
          if (this->_onSTALoopFn)
               this->_onSTALoopFn("");
          
          //If MDNS initialized, then start MDNS loop
          #ifdef ESP8266
          if(this->_initMdnsFlagOnce) MDNS.update();  
          #endif
     }
     //Access point onReady event
     if (this->_apFlagStarted && !this->_onReadyStateAPFlagOnce)
     {
          //Destroying parameters
          if(this->_wifiParam && this->_wifiParam->getMode() == AP_ONLY)
          {
               DEBUG_CALLBACK_HAC(F("Destroying wifi parameters.."));
               delete this->_wifiParam;
          }

          this->_onReadyStateAPFlagOnce = true;          
          if (this->_onAPReadyFn)
               this->_onAPReadyFn(WiFi.softAPSSID().c_str());
     }
     //Access point onDisconnect event
     if (!this->_apFlagStarted && this->_onReadyStateAPFlagOnce)
     {
          //this->_initParam();
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
 * @param data Debug message .
 */
void HaCWifiManager::_debug(const __FlashStringHelper* data)
{
    char buffer[128];
    memset(buffer, '\0', sizeof(buffer));
    strcpy_P(buffer, (const char*) data);
    if (this->_onDebugFn)
        this->_onDebugFn(String(String(HAC_DEBUG_PREFIX) + " " + String(buffer)).c_str());
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
     if(!this->_wifiParam)return;

     switch (this->_wifiParam->getMode())
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
          #ifdef ESP8266
          WiFi.forceSleepBegin();
          #endif
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
     if(!this->_wifiParam)this->_initParam();
     if(!this->_wifiParam) return;

     DEBUG_CALLBACK_HAC(F("Initializing station.."));
     //Check if it is multi or single wifi
     if (this->_wifiParam->getEnableMultiWifi())
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
     if (this->_wifiParam->getWifiListCount() == 1)
     {
          this->_setupSTASingleWifi(isStartup);
          return;
     }
     DEBUG_CALLBACK_HAC(F("Setting up STA multi wifi.."));

     //Clean previous scan
     WiFi.scanDelete();
     DEBUG_CALLBACK_HAC(F("Deleted previous scan."));
     //Remove previous wifi ssid&password
     WiFi.disconnect();
     DEBUG_CALLBACK_HAC(F("Previous wifi session ssid/password."));
     //Start wifi scan
     WiFi.scanNetworks(true);
     DEBUG_CALLBACK_HAC(F("Start wifi scan in async mode"));

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
                                                DEBUG_CALLBACK_HAC(F("Wifi scan failed or timeout."));
                                                this->_wifiScanTimer.stop();
                                           }
                                           return;
                                      }

                                      //Continue scanning until count return other than 255 and 0
                                      if (count != 255 && count > 0)
                                           this->_wifiScanTimer.stop();
                                      else
                                           return;
                                   
                                      DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG104, count);

                                      if (!this->_scanWifiListRssi(count))
                                      {
                                           DEBUG_CALLBACK_HAC(F("Warning: None of ssid listed was found from the scanning."));
                                           this->_printError(12);
                                      }
                                      else
                                           this->_sortWifiRssi();

                                      this->_setupSTASingleWifi();

                                      DEBUG_CALLBACK_HAC(F("Multiwifi setup done."));
                                 });
     this->_wifiScanTimer.begin();
}
/**
     * Scan wifi rssi.     
     * @param totalAP- Total number of access point scanned
     */
bool HaCWifiManager::_scanWifiListRssi(uint8_t totalAP)
{
     if(!this->_wifiParam)return false;

     String ssid;
     int32_t rssi;
     uint8_t encType;
     uint8_t *bssid;
     int32_t channel;
     #ifdef ESP8266
     bool hidden;
     #endif
     bool atleastOneSsidListFoundFlag = false;

     DEBUG_CALLBACK_HAC(F("Scanning Wifi AP Rssi.."));
     for (uint8_t i = 0; i < totalAP; i++)
     {
          // Get network information
          #ifdef ESP8266
          WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel, hidden);
          #endif
          #ifdef ESP32
          WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel);
          #endif

          // Check if the WiFi network contains an entry in Wifiinfo list
          uint8_t j = 0;
          for (auto entry : this->_wifiParam->wifiInfo)
          {
               // Check SSID
               if (ssid == entry.ssid)
               {
                    // Known network
                    atleastOneSsidListFoundFlag = true;
                    this->_wifiParam->wifiInfo[j].rssi = WiFi.RSSI(i);
                    DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG105, ssid.c_str());
                    DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG106, this->_wifiParam->wifiInfo[j].rssi);
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
     if(!this->_wifiParam)return;

     DEBUG_CALLBACK_HAC(F("Sorting Wifi AP based on Rssi"));
     for (uint8_t i = 0; i < this->_wifiParam->getWifiListCount(); i++)
     {
          for (uint8_t j = 0; j < this->_wifiParam->getWifiListCount(); j++)
          {
               if (this->_wifiParam->wifiInfo[i].rssi > this->_wifiParam->wifiInfo[j].rssi)
               {
                    t_wifiInfo tmp;

               tmp = this->_wifiParam->wifiInfo[i];
                    this->_wifiParam->wifiInfo[i] = this->_wifiParam->wifiInfo[j];
                    this->_wifiParam->wifiInfo[j] = tmp;
               }
          }
     }

     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG107, this->_wifiParam->wifiInfo[0].ssid.c_str());
}

/**
     * Setting single wifi mode.     
     */
void HaCWifiManager::_setupSTASingleWifi(bool isStartUp)
{
     if(!this->_wifiParam)this->_initParam();
     if(!this->_wifiParam)return;

     DEBUG_CALLBACK_HAC(F("Setting up single STA wifi.."));
     if (isStartUp)
     {
          delay(1000);
          this->_startStation(this->_wifiParam->wifiInfo[0].ssid.c_str(),
                              this->_wifiParam->wifiInfo[0].pass.c_str());
     }
     else //After startup, wifi setup will be done asynchronously
     {          
          this->_staStartupTimer.onTick([&]()
                                        {
                                             //if(!this->_wifiParam) this->_initParam();
                                             this->_startStation(this->_wifiParam->wifiInfo[0].ssid.c_str(),
                                                                 this->_wifiParam->wifiInfo[0].pass.c_str());
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
     if(!this->_wifiParam)return;

     //Check if network set as DHCP for station     
     if (!(this->_wifiParam->getEnableDHCPNetwork(NETWORK_STATION)))
          this->_manualStaNetworkSetupSuccess = this->_setupNetworkManually(NETWORK_STATION);

     /* #region Debug */
     DEBUG_CALLBACK_HAC(F("Setting up station.."));
     DEBUG_CALLBACK_HAC(F("WIFI_STA = true"));
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG114, ssid);
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG108, pass);
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
     if(!this->_wifiParam)return false;

     t_networkInfo netInfo;
     if(netWorkType == NETWORK_STATION) 
          netInfo = this->_wifiParam->staNetworkInfo;     
     if(netWorkType == NETWORK_AP) 
          netInfo = this->_wifiParam->apNetworkInfo;     

     DEBUG_CALLBACK_HAC(F("Setting up network manually.."));
     IPAddress ip, gw, sn, pdns, sdns;
     if (!ip.fromString(netInfo.ip))
     {
          this->_printError(3);
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG109, netInfo.ip.c_str());
          return false;
     }
     if (!sn.fromString(netInfo.sn))
     {
          this->_printError(4);
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG110, netInfo.sn.c_str());
          return false;
     }
     if (!gw.fromString(netInfo.gw))
     {
          this->_printError(5);
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG111, netInfo.gw.c_str());
          return false;
     }
     if (!pdns.fromString(netInfo.pdns) && netWorkType == NETWORK_STATION)
     {
          this->_printError(6);
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG112, netInfo.pdns.c_str());
          return false;
     }
     if (!sdns.fromString(netInfo.sdns) && netWorkType == NETWORK_STATION)
     {
          this->_printError(7);
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG113, netInfo.sdns.c_str());
          return false;
     }

     if(netWorkType == NETWORK_STATION)
     {
          if (!WiFi.config(ip, gw, sn, pdns, sdns))
          {
               this->_printError(81);
               DEBUG_CALLBACK_HAC(F("Network manual configuration failed for Station."));
               return false;
          }
     }
     if(netWorkType == NETWORK_AP)
     {
          if (!WiFi.softAPConfig(ip, gw, sn))
          {
               this->_printError(82);
               DEBUG_CALLBACK_HAC(F("Network manual configuration failed for Access Point."));
               return false;
          }
     }


     DEBUG_CALLBACK_HAC(F("Network successfully setup manually."));

     return true;
}

/**
     * Function that will setup access point.     
     */
void HaCWifiManager::_startAccessPoint()
{
     if(!this->_wifiParam)return;

     //Check if Access point is provided
     if (this->_wifiParam->accessPointInfo.ssid == "null")
     {
          this->_wifiParam->accessPointInfo.ssid = String(___DEF_SSID___);
          this->_wifiParam->accessPointInfo.pass = String(___DEF_PASS___);
     }

     /* #region Debug */
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG114, this->_wifiParam->accessPointInfo.ssid.c_str());
     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG115, this->_wifiParam->accessPointInfo.pass.c_str());
     /* #endregion */

     //Raise error if ap ssid or password is empty
     if (this->_wifiParam->accessPointInfo.ssid == "" || this->_wifiParam->accessPointInfo.pass == "")
     {
          this->_printError(9);
          DEBUG_CALLBACK_HAC(F("Invalid access point parameters."));
          return;
     }

     //Check DHCP enable network is set
     if (!(this->_wifiParam->getEnableDHCPNetwork(NETWORK_AP)))
          this->_manualApNetworkSetupSuccess = this->_setupNetworkManually(NETWORK_AP);



     if (!WiFi.softAP(this->_wifiParam->accessPointInfo.ssid.c_str(), this->_wifiParam->accessPointInfo.pass.c_str(), 6, 0, 5))
     {
          this->_printError(10);
          DEBUG_CALLBACK_HAC(this->_wifiParam->accessPointInfo.ssid.c_str());
          DEBUG_CALLBACK_HAC(this->_wifiParam->accessPointInfo.pass.c_str());
          DEBUG_CALLBACK_HAC(F("Failed to setup access point."));
          return;
     }
     this->_apFlagStarted = true;
     DEBUG_CALLBACK_HAC(F("Access point successfully setup."));
}

/**
     * Initialize parameter.     
     */
void HaCWifiManager::_initParam()
{
     this->_wifiParam = new HACWifiManagerParameters();

     char *data = new char[2000];
     memset(data, '\0', 2000);

     this->_read(data);

     if(data[0] == '\0') 
          DEBUG_CALLBACK_HAC(F("Invalid parameters retrieved.."));
     
     this->_wifiParam->fromJson(&data[0]);

     delete[] data;
}

/**
     * Save parameters.     
     */
void HaCWifiManager::_save()
{
     char *data = new char[1000];
     memset(data, '\0', 1000);

     this->_read(data);

     char *wifiConfig = new char[1000];
     memset(wifiConfig, '\0', 1000);
     this->getWifiConfigJson(wifiConfig, 2000);
     
     if(strcmp(&data[0], &wifiConfig[0]) == 0 && data[0] != '\0')
     {
          DEBUG_CALLBACK_HAC(F("Save is not required as file data is similar to current parameters.."));
          delete[] data;
          delete[] wifiConfig;
          return;
     } 
     
     if(!__LITTLEFS__.begin()){    
          DEBUG_CALLBACK_HAC(F("An Error has occurred while mounting LITTLEFS!"));        
          return;
     }

     DEBUG_CALLBACK_HAC(F("Successfully mounted the littlefs.")); 

     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG18, ___FILE_NAME___); 
     File file = __LITTLEFS__.open(___FILE_NAME___, "w+");
     if(!file){
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG20, ___FILE_NAME___);
          return;
     }       

     if(!file.print(&wifiConfig[0]))
     {
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG21, ___FILE_NAME___);
          return;
     }

     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG22, ___FILE_NAME___);

     delete[] wifiConfig;
     delete[] data;

     file.close();
     __LITTLEFS__.end();

}

/**
     * Read parameters.  
     * @param data parameter buffer   
     */
void HaCWifiManager::_read(char *data)
{
     if(!__LITTLEFS__.begin()){    
          DEBUG_CALLBACK_HAC(F("An Error has occurred while mounting LITTLEFS!"));        
          return;
     }

     DEBUG_CALLBACK_HAC(F("Successfully mounted the littlefs.")); 

     DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG24, ___FILE_NAME___); 
     File file = __LITTLEFS__.open(___FILE_NAME___, "r");
     if(!file){
          DEBUG_CALLBACK_HAC2(HAC_WFM_VERBOSE_MSG23, ___FILE_NAME___);
          return;
     }  

     uint16_t i=0;
     while(file.available()) data[i++] = (char)file.read();

     data[i++] = '\0'; 
          
     file.close();

     __LITTLEFS__.end();

}
/* #endregion */