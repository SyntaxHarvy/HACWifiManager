#include <Arduino.h>
#include "ampwifimanager.h"

#define AMP_SERIAL_DEBUG_ENABLE             true                                                   // Toggle switch to turn on.off Custom Debug

AMPWifiManager gAMPWifiManager;

void setup() {
  //gAMPWifiManager.setup();
}

void loop() {
  gAMPWifiManager.loop();
}