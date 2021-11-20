/* #region SELF_HEADER */
#include "hacwifimanager.h"
/* #endregion */



/* #region CLASS_DEFINITION */
/**
     * Constructor.
     */
HaCWifiManager::HaCWifiManager(){}

/**
     * Called when setting up the library.     
     * Note: Wifi parameters passed as json data.
     * @param wifiJsonStr Wifi Parameters in Json String format
     */
void HaCWifiManager::setup(const char * wifiJsonStr){
     //Setup the wifi parameters from Json format string
     DEBUG_CALLBACK_HAC("Setting up wifi parameters from json data.");
     //Process the json data
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 8192);
     DeserializationError error = deserializeJson(doc, wifiJsonStr);
     doc.shrinkToFit();
     if (!error) {           
          DEBUG_CALLBACK_HAC("Data is a valid json.");
          //Initialize _wifiParam debug callback function
          this->_wifiParam.onDebug([&](const char * msg){
               DEBUG_CALLBACK_HAC(msg);     
          });
          //Json data is a valid json format hence it will be safe to be process by the wifi parameter class
          this->_wifiParam.fromJson(wifiJsonStr);

          /* #region Debug */
          //Read back the value from the wifiParam as json string
          DEBUG_CALLBACK_HAC(String("Reading back json value from wifiParam class =>" + String(this->_wifiParam.toJson())).c_str());                              
          /* #endregion */

          //Setting up wifi Core
          this->setup();

     }else{
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
            const char * defaultSSID, 
            const char * defaultPass,
            WifiMode mode,
            bool enableMultiWifi,
            bool enableDHCPNetwork,
            const char * ip,
            const char * gw,
            const char * sn,
            const char * pdns,
            const char * sdns){
     
     //TO DO: Another way of setting up wifi manager

}

/**
     * Called when setting up the wifi manager core
     */

void HaCWifiManager::setup(){
     //Check mode validity
     if(this->_wifiParam.getMode() < STA_ONLY || this->_wifiParam.getMode()  > BOTH){
        if(this->_onErrorFn) this->_onErrorFn(WIFI_MODE_ERROR_CODE);
        DEBUG_CALLBACK_HAC("Invalid wifi mode!");   
        return;
     }
     //Check and ssid or password pair
     if(this->_wifiParam.getWifiListCount() == 0){
        if(this->_onErrorFn) this->_onErrorFn(WIFI_SSID_MISSING_ERROR_CODE);
        DEBUG_CALLBACK_HAC("There is no ssid configured!");   
        return;
     }
     
     //Wifi is ready for start up
     DEBUG_CALLBACK_HAC("Starting up wifi.");
}

/**
     * Setting wifi mode.     
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     */
void HaCWifiManager::setMode(WifiMode mode){
     this->_wifiParam.setMode(mode);     
}

/**
     * Getting wifi mode.          
     */
WifiMode HaCWifiManager::getMode(){
     return (WifiMode)this->_wifiParam.getMode();     
}


/**
     * Loop routine of the library.
     * Note: loop routine      
     * @param none
     */
void HaCWifiManager::loop(){
   
}

/**
     * Debug Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onDebug(tListGenCbFnHaC1StrParamSub fn){
    this->_onDebugFn = fn;    
}

/**
     * On Error Callback function.           
     * @param fn Standard non return function with a const * char parameter.
     */

void HaCWifiManager::onError(tListGenCbFnHaC1StrParamSub fn){
    this->_onErrorFn = fn;    
}


/**
     * Debug function.     
     * @param data const char .
     */

void HaCWifiManager::_debug(const char * data){        
    if(this->_onDebugFn) this->_onDebugFn(String(String(HAC_DEBUG_PREFIX) + " " + String(data)).c_str());
}



/* #endregion */