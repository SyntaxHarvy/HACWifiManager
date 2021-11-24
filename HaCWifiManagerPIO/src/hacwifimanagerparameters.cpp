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
HACWifiManagerParameters::HACWifiManagerParameters(){}    

/**
     * HACWifiManagerParameters Constructor     *
     */
HACWifiManagerParameters::~HACWifiManagerParameters(){
    this->wifiInfo.clear();
}    

/**
     * Accept json string and convert it to HACWifiManagerParameters class *
     * @param jsonStr Wifi parameters in json format as const char *.
     */
void HACWifiManagerParameters::fromJson(const char *jsonStr){         
    DEBUG_CALLBACK_HAC_PARAM("Decoding json data.");  
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 8192);
     DeserializationError error = deserializeJson(doc, jsonStr);
     doc.shrinkToFit();
     if (!error) {   
          this->_mode = doc["mode"].as<uint8_t>();             
          this->_multiWifiEnable = doc["enable_multi_wifi"].as<bool>();             
          this->_dhcpNetworkEnable = doc["enable_dhcp_network"].as<bool>();  

          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM(String("Mode = " + String(this->_mode)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("Enable Multi Wifi = " + String(this->_multiWifiEnable)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("Enable DHCP = " + String(this->_dhcpNetworkEnable)).c_str());         
          /* #endregion */

          
          this->networkInfo.ip = doc["network"]["ip"].as<String>();  
          this->networkInfo.sn = doc["network"]["sn"].as<String>();
          this->networkInfo.gw = doc["network"]["gw"].as<String>();
          this->networkInfo.pdns = doc["network"]["pdns"].as<String>();
          this->networkInfo.sdns = doc["network"]["sdns"].as<String>();
          
          this->accessPointInfo.ssid = doc["ap"]["ssid"].as<String>();
          this->accessPointInfo.pass = doc["ap"]["pass"].as<String>();

          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM(String("IP = " + String(this->networkInfo.ip)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("SN = " + String(this->networkInfo.sn)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("GW = " + String(this->networkInfo.gw)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("PDNS = " + String(this->networkInfo.pdns)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("SDNS = " + String(this->networkInfo.sdns)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("AP SSID = " + String(this->accessPointInfo.ssid)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("AP PASS = " + String(this->accessPointInfo.pass)).c_str());         
          /* #endregion */

          JsonObject wifiList = doc["wifilist"].as<JsonObject>(); 
          
          for (JsonPair p : wifiList) {

               //Check point for maximum number of wifi list allowed
               //Any list above the MAX_WIFI_INFO_LIST will be ignored
               if(this->wifiInfo.size() > MAX_WIFI_INFO_LIST) return;
               
               this->_totalWifiList = this->wifiInfo.size() + 1;     
               JsonObject rowData = p.value().as<JsonObject>();
               
               t_wifiInfo w;
               
               w.ssid = rowData["ssid"].as<String>();
               w.pass = rowData["password"].as<String>();

               this->wifiInfo.push_back(w);

               /* #region Debug */
               uint8_t index = this->wifiInfo.size()-1;
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
String HACWifiManagerParameters::toJson(){    
     
     String tmp = "";
     DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
     doc["mode"] = this->_mode;
     doc["enable_multi_wifi"] = this->_multiWifiEnable;
     doc["enable_dhcp_network"] = this->_dhcpNetworkEnable;
     doc["total_wifi_list"] = this->_totalWifiList;
     doc["network"]["ip"] = this->networkInfo.ip;
     doc["network"]["sn"] = this->networkInfo.sn;
     doc["network"]["gw"] = this->networkInfo.gw;
     doc["network"]["pdns"] = this->networkInfo.pdns;
     doc["network"]["sdns"] = this->networkInfo.sdns;
     
     for(uint8_t i=0; i<this->_totalWifiList; i++){
         doc["wifilist"][String(i)]["ssid"] =  this->wifiInfo[i].ssid; 
         doc["wifilist"][String(i)]["password"] =  this->wifiInfo[i].pass;
     }

     doc.shrinkToFit();                 
     serializeJson(doc, tmp); 
     
     return tmp; 
}

/**
     * Setting wifi mode.     
     * @param mode WifiMode wifi mode such as STA_ONLY, AP_ONLY & BOTH.
     */
void HACWifiManagerParameters::setMode(uint8_t mode){
     this->_mode = mode;    
}

/**
     * Getting wifi mode.          
     */
uint8_t HACWifiManagerParameters::getMode(){
     return this->_mode;
}

/**
     * Getting wifi list count.          
     */
uint8_t HACWifiManagerParameters::getWifiListCount(){
     return this->wifiInfo.size();
}

/**
     * Setting multiwifi enable mode.          
     */
void HACWifiManagerParameters::setEnableMultiWifi(bool enable){
     this->_multiWifiEnable = enable;
}

/**
     * Getting multiwifi enable mode.          
     */
bool HACWifiManagerParameters::getEnableMultiWifi(){
     return this->_multiWifiEnable;
}

/**
     * Setting DHCP enable mode.          
     */
void HACWifiManagerParameters::setEnableDHCPNetwork(bool enable){
     this->_dhcpNetworkEnable = enable;
}

/**
     * Getting DHCP enable mode.          
     */
bool HACWifiManagerParameters::getEnableDHCPNetwork(){
     return this->_dhcpNetworkEnable;
}

/**
     * Debug Callback function.          * 
     * @param fn Standard non return function with a const * char parameter*.
     */

void HACWifiManagerParameters::onDebug(tListGenCbFnHaC1StrParamSub fn){
    this->_onDebugFn = fn;    
}

/**
     * Debug function.     
     * Note, this is the debug of HACWifiManagerParameters which is a private class of HaCWifiManager
     * @param data const char *.
     */
void HACWifiManagerParameters::_debug(const char *data){
     if(this->_onDebugFn) this->_onDebugFn(data);
}    


/* #endregion */