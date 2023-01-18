/**
 *
 * @file HaCWifiManagerStringConst.h
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

#ifndef __HAC_MANAGER_VERBOSEMSG_H_
#define __HAC_MANAGER_VERBOSEMSG_H_

#include <Arduino.h>

#define HAC_WFM_STRING_NULL "null"

#define HAC_WFM_STRING_CONST1 "{\"no_client\": "
#define HAC_WFM_STRING_CONST2 "}"


#if defined(DEBUG_ESP_PORT) || defined(HAC_ENABLE_DEBUG)
/* #region Verbose Message String Constant */

const char HAC_WFM_VERBOSE_MSG1[] PROGMEM = "Hostname = %s";
const char HAC_WFM_VERBOSE_MSG2[] PROGMEM = "Mode = %d";
const char HAC_WFM_VERBOSE_MSG3[] PROGMEM = "Enable Multi Wifi = %d";
const char HAC_WFM_VERBOSE_MSG4[] PROGMEM = "Enable DHCP Station = %d";
const char HAC_WFM_VERBOSE_MSG5[] PROGMEM = "Enable DHCP Access Point = %d";
const char HAC_WFM_VERBOSE_MSG6[] PROGMEM = "AP SSID = %s";
const char HAC_WFM_VERBOSE_MSG7[] PROGMEM = "AP PASS = %s";
const char HAC_WFM_VERBOSE_MSG8[] PROGMEM = "IP = %s";
const char HAC_WFM_VERBOSE_MSG9[] PROGMEM = "SN = %s"; 
const char HAC_WFM_VERBOSE_MSG10[] PROGMEM = "GW = %s";  
const char HAC_WFM_VERBOSE_MSG11[] PROGMEM = "PDNS = %s";  
const char HAC_WFM_VERBOSE_MSG12[] PROGMEM = "SDNS = %s"; 
const char HAC_WFM_VERBOSE_MSG13[] PROGMEM = "Wifi Index = %d";
const char HAC_WFM_VERBOSE_MSG14[] PROGMEM = "SSID = %s";
const char HAC_WFM_VERBOSE_MSG15[] PROGMEM = "password = %s";
const char HAC_WFM_VERBOSE_MSG16[] PROGMEM = "Total wifi = %d";
const char HAC_WFM_VERBOSE_MSG17[] PROGMEM = "New ssid = %s Comparing to = %s";
const char HAC_WFM_VERBOSE_MSG18[] PROGMEM = "Start writing to file %s..";
const char HAC_WFM_VERBOSE_MSG19[] PROGMEM = "Wifi Manager Configuration : %s ";
const char HAC_WFM_VERBOSE_MSG20[] PROGMEM = "Failed to open file %s for writing..";
const char HAC_WFM_VERBOSE_MSG21[] PROGMEM = "Failed to save file %s!!";
const char HAC_WFM_VERBOSE_MSG22[] PROGMEM = "Successfully save the file %s!!";
const char HAC_WFM_VERBOSE_MSG23[] PROGMEM = "Failed to open file %s for reading..";
const char HAC_WFM_VERBOSE_MSG24[] PROGMEM = "Start reading from file %s..";

const char HAC_WFM_VERBOSE_MSG100[] PROGMEM = "Wifi sleep style = %d";
const char HAC_WFM_VERBOSE_MSG101[] PROGMEM = "Wifi physical mode = %d";
const char HAC_WFM_VERBOSE_MSG102[] PROGMEM = "Wifi persistent enable = %d";
const char HAC_WFM_VERBOSE_MSG103[] PROGMEM = "Wifi output power = %f";
const char HAC_WFM_VERBOSE_MSG104[] PROGMEM = "Wifi scan success. Total AP found = %d";
const char HAC_WFM_VERBOSE_MSG105[] PROGMEM = "Known network found = %s";
const char HAC_WFM_VERBOSE_MSG106[] PROGMEM = "RSSI = %d";
const char HAC_WFM_VERBOSE_MSG107[] PROGMEM = "Strongest RSSI = %s";
const char HAC_WFM_VERBOSE_MSG108[] PROGMEM = "PASS = %s";
const char HAC_WFM_VERBOSE_MSG109[] PROGMEM = "Failed to setup IP manually = %s";
const char HAC_WFM_VERBOSE_MSG110[] PROGMEM = "Failed to setup Subnet manually = %s";
const char HAC_WFM_VERBOSE_MSG111[] PROGMEM = "Failed to setup Gateway manually = %s";
const char HAC_WFM_VERBOSE_MSG112[] PROGMEM = "Failed to setup Pri. DNS manually = %s";
const char HAC_WFM_VERBOSE_MSG113[] PROGMEM = "Failed to setup Sec. DNS manually = %s";
const char HAC_WFM_VERBOSE_MSG114[] PROGMEM = "AP SSID = %s";
const char HAC_WFM_VERBOSE_MSG115[] PROGMEM = "AP PASS = %s";
const char HAC_WFM_VERBOSE_MSG116[] PROGMEM = "MDNS = %s.local";


/* #endregion */



#endif

#endif