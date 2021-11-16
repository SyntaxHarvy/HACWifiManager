/* #region CONSTANT_DEFINITION */
#if(AMP_SERIAL_DEBUG_ENABLE)
#define DEBUG_AMP_WIFI                       Serial                                                 // Custom serial debug
#define DEBUG_MSG(msg, ...)                 { DEBUG_AMP_WIFI.printf(msg, ## __VA_ARGS__); }         // Custom serial debug for Serial.printf
#define DEBUG_MSG_LN(msg)                   { DEBUG_AMP_WIFI.println(msg); }                        // Custom serial debug for Serial.println
#else
#define DEBUG_MSG_AMP_WIFI(msg, ...) 
#define DEBUG_MSG_AMP_WIFI_LN(msg)
#endif

/* #endregion */

/* #region SELF_HEADER */
#include "ampwifimanager.h"
/* #endregion */

/* #region CLASS_DEFINITION */
/*
    Constructor
*/
AMPWifiManager::AMPWifiManager(){}

/**
     * Called when setting up the library.
     * Note: setup routine      
     * @param 
     */
/*void AMPWifiManager::setup(){

}*/

/**
     * Loop routine of the library.
     * Note: loop routine      
     * @param none
     */
void AMPWifiManager::loop(){
    
}

/* #endregion */