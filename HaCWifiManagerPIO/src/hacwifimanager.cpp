/* #region SELF_HEADER */
#include "hacwifimanager.h"
/* #endregion */

/* #region CLASS_DEFINITION */
/**
     * Constructor.
     */
HaCWifiManager::HaCWifiManager() {}

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
          //Initialize _wifiParam debug callback function
          this->_wifiParam.onDebug([&](const char *msg)
                                   { DEBUG_CALLBACK_HAC(msg); });
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
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     * @param enableMultiWifi bool enable multi wifi.
     * @param enableDHCPNetwork bool enable DHCP Network.
     * @param ip const char * Static Network ip. Note, enableDHCPNetwork shall be false before assigning a network ip.
     * @param sn const char * Static Network sn. Note, enableDHCPNetwork shall be false before assigning a network sn.
     * @param gw const char * Static Network gw. Note, enableDHCPNetwork shall be false before assigning a network gw.
     * @param pdns const char * Static Network pdns. Note, enableDHCPNetwork shall be false before assigning a network pdns.
     * @param sdns const char * Static Network sdns. Note, enableDHCPNetwork shall be false before assigning a network sdns.
     */
void HaCWifiManager::setup(
    const char *defaultSSID,
    const char *defaultPass,
    WifiMode mode,
    bool enableMultiWifi,
    bool enableDHCPNetwork,
    const char *ip,
    const char *gw,
    const char *sn,
    const char *pdns,
    const char *sdns)
{

     //TO DO: Another way of setting up wifi manager
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

     //Wifi is ready for start up
     DEBUG_CALLBACK_HAC("Starting up wifi.");

     //Check if it is multi or single wifi
     if (this->_wifiParam.getEnableMultiWifi())
          this->_setupMultiWifi();
     else
          this->_setupSingleWifi();
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
     */
WifiMode HaCWifiManager::getMode()
{
     return (WifiMode)this->_wifiParam.getMode();
}

/**
     * Loop routine of the library.
     * Note: loop routine      
     * @param none
     */
void HaCWifiManager::loop()
{
     //Wifi onReady event
     if (WiFi.status() == WL_CONNECTED && !this->_onReadyStateFlagOnce)
     {
          if (this->_onReadyFn)
               this->_onReadyFn(WiFi.SSID().c_str());
          this->_onReadyStateFlagOnce = true;
     }
     //Wifi onDisconnect event
     if (WiFi.status() != WL_CONNECTED && this->_onReadyStateFlagOnce)
     {
          if (this->_onDisconnectFn)
               this->_onDisconnectFn(WiFi.SSID().c_str());
          this->_onReadyStateFlagOnce = false;
     }
     //Wifi onServiceLoop event
     if (this->_onReadyStateFlagOnce)
     {
          //TO DO: Pass json data for the status of the STA
          if (this->_onServiceLoopFn)
               this->_onServiceLoopFn("Wifi loop");
     }
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

void HaCWifiManager::onReady(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onReadyFn = fn;
}

/**
     * On Service Loop Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onServiceLoop(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onServiceLoopFn = fn;
}

/**
     * On Disconnect Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onDisconnect(tListGenCbFnHaC1StrParamSub fn)
{
     this->_onDisconnectFn = fn;
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
     * Setting multi wifi mode.     
     */
void HaCWifiManager::_setupMultiWifi()
{
     //If there is only one ssid on the list then set it up as a single wifi
     if (this->_wifiParam.getWifiListCount() == 1)
     {
          this->_setupSingleWifi();
          return;
     }
     DEBUG_CALLBACK_HAC("Setting up multi wifi..");
}
/**
     * Setting single wifi mode.     
     */
void HaCWifiManager::_setupSingleWifi(bool isStartUp)
{
     DEBUG_CALLBACK_HAC("Setting up single wifi..");

     switch (this->_wifiParam.getMode())
     {
     case STA_ONLY:
          //Stop access which is point currently running
          WiFi.softAPdisconnect(true);
          if (isStartUp)
          {
               delay(1000);
               this->_setupStation(this->_wifiParam.wifiInfo[0].ssid.c_str(),
                    this->_wifiParam.wifiInfo[0].pass.c_str());
          }
          break;
     case AP_ONLY:
          WiFi.softAPdisconnect(true);
          WiFi.disconnect(); 
          WiFi.mode(WIFI_OFF);
          WiFi.forceSleepBegin();
          if (isStartUp)
          {
               delay(1000);
               this->_setupAccessPoint();
          }
          break;
     case BOTH_STA_AP:
          break;
     default:
          break;
     }
}

/**
     * Setting station.    
     * @param ssid-const char* wifi station ssid
     * @param pass-const char* wifi station pass 
     */
void HaCWifiManager::_setupStation(const char *ssid, const char *pass)
{
     //Check if network set as DHCP
     if (!this->_wifiParam.getEnableDHCPNetwork())
          this->_manualNetworkSetupSuccess = this->_setupNetworkManually();

     /* #region Debug */
     DEBUG_CALLBACK_HAC("Setting up station..");
     DEBUG_CALLBACK_HAC("WIFI_STA = true");
     DEBUG_CALLBACK_HAC(String("SSID = " + String(ssid)).c_str());
     DEBUG_CALLBACK_HAC(String("PASS = " + String(pass)).c_str());
     /* #endregion */
     //Setup wifi network

     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, pass);
}

/**
     * Setting network manually.     
     */
bool HaCWifiManager::_setupNetworkManually()
{
     DEBUG_CALLBACK_HAC("Setting up network manually..");
     IPAddress ip, gw, sn, pdns, sdns;
     if (!ip.fromString(this->_wifiParam.networkInfo.ip))
     {
          this->_printError(3);
          DEBUG_CALLBACK_HAC(String("Failed to setup IP(" + String(this->_wifiParam.networkInfo.ip) + ") manually..").c_str());
          return false;
     }
     if (!sn.fromString(this->_wifiParam.networkInfo.sn))
     {
          this->_printError(4);
          DEBUG_CALLBACK_HAC(String("Failed to setup subnet(" + String(this->_wifiParam.networkInfo.sn) + ") manually..").c_str());
          return false;
     }
     if (!gw.fromString(this->_wifiParam.networkInfo.gw))
     {
          this->_printError(5);
          DEBUG_CALLBACK_HAC(String("Failed to setup gateway(" + String(this->_wifiParam.networkInfo.gw) + ") manually..").c_str());
          return false;
     }
     if (!pdns.fromString(this->_wifiParam.networkInfo.pdns))
     {
          this->_printError(6);
          DEBUG_CALLBACK_HAC(String("Failed to setup primary dns(" + String(this->_wifiParam.networkInfo.pdns) + ") manually..").c_str());
          return false;
     }
     if (!sdns.fromString(this->_wifiParam.networkInfo.sdns))
     {
          this->_printError(7);
          DEBUG_CALLBACK_HAC(String("Failed to setup secondary dns(" + String(this->_wifiParam.networkInfo.sdns) + ") manually..").c_str());
          return false;
     }
     if (!WiFi.config(ip, gw, sn, pdns, sdns))
     {
          this->_printError(8);
          DEBUG_CALLBACK_HAC("Network manual configuration failed.");
          return false;
     }

     DEBUG_CALLBACK_HAC("Network successfully setup manually.");

     return true;
}

/**
     * Function that will setup access point.     
     */
void HaCWifiManager::_setupAccessPoint()
{

     //Check if Access point is provided from the parameters
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

     if (!WiFi.softAP(this->_wifiParam.accessPointInfo.ssid.c_str(),this->_wifiParam.accessPointInfo.pass.c_str(), 6, 0, 5))
     {
          this->_printError(10);
          DEBUG_CALLBACK_HAC("Failed to setup access point.");
          return;
     }

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

     if (this->_setWifiMacAddress())
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
     */

int HaCWifiManager::_setWifiMacAddress()
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