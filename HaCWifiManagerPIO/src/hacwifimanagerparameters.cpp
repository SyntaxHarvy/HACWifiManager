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
    this->_wifiInfo.clear();
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

          
          this->_networkInfo.ip = doc["network"]["ip"].as<String>();  
          this->_networkInfo.sn = doc["network"]["sn"].as<String>();
          this->_networkInfo.gw = doc["network"]["gw"].as<String>();
          this->_networkInfo.pdns = doc["network"]["pdns"].as<String>();
          this->_networkInfo.sdns = doc["network"]["sdns"].as<String>();
          
          /* #region Debug */
          DEBUG_CALLBACK_HAC_PARAM(String("IP = " + String(this->_networkInfo.ip)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("SN = " + String(this->_networkInfo.sn)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("GW = " + String(this->_networkInfo.gw)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("PDNS = " + String(this->_networkInfo.pdns)).c_str());         
          DEBUG_CALLBACK_HAC_PARAM(String("SDNS = " + String(this->_networkInfo.sdns)).c_str());         
          /* #endregion */

          JsonObject wifiList = doc["wifilist"].as<JsonObject>(); 
          
          for (JsonPair p : wifiList) {

               //Check point for maximum number of wifi list allowed
               //Any list above the MAX_WIFI_INFO_LIST will be ignored
               if(this->_wifiInfo.size() > MAX_WIFI_INFO_LIST) return;
               
               this->_totalWifiList = this->_wifiInfo.size() + 1;     
               JsonObject rowData = p.value().as<JsonObject>();
               
               t_wifiInfo w;
               
               w.ssid = rowData["ssid"].as<String>();
               w.pass = rowData["password"].as<String>();

               this->_wifiInfo.push_back(w);

               /* #region Debug */
               uint8_t index = this->_wifiInfo.size()-1;
               DEBUG_CALLBACK_HAC_PARAM(String("Wifi Index = " + String(index)).c_str());      
               DEBUG_CALLBACK_HAC_PARAM(String("ssid = " + this->_wifiInfo[index].ssid).c_str());      
               DEBUG_CALLBACK_HAC_PARAM(String("password = " + this->_wifiInfo[index].pass).c_str());                     
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
     doc["network"]["ip"] = this->_networkInfo.ip;
     doc["network"]["sn"] = this->_networkInfo.sn;
     doc["network"]["gw"] = this->_networkInfo.gw;
     doc["network"]["pdns"] = this->_networkInfo.pdns;
     doc["network"]["sdns"] = this->_networkInfo.sdns;
     
     for(uint8_t i=0; i<this->_totalWifiList; i++){
         doc["wifilist"][String(i)]["ssid"] =  this->_wifiInfo[i].ssid; 
         doc["wifilist"][String(i)]["password"] =  this->_wifiInfo[i].pass;
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
     return this->_wifiInfo.size();
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