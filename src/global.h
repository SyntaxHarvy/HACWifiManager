#ifndef __HACWIFI_GLOBAL_H_
#define __HACWIFI_GLOBAL_H_
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
extern "C" {
#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
}

#define __LITTLEFS__ LittleFS
#define __ESP_HEAPS_BLOCK_SIZE__ ESP.getMaxFreeBlockSize()
#define __TCP_ADAPTER_INTERFACE__ SOFTAP_IF
#define __TCP_ADAPTER_GET_IP_INFO(interface, ipInfo) \
{ \
wifi_get_ip_info(interface, ipInfo); \
}
#define __TCP_INFO__ struct ip_info
#endif

#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <LITTLEFS.h>
#include "lwip/dns.h"

#define __LITTLEFS__ LITTLEFS
#define __ESP_HEAPS_BLOCK_SIZE__ ESP.getMaxAllocHeap()
#define __TCP_ADAPTER_INTERFACE__ TCPIP_ADAPTER_IF_AP
#define __TCP_ADAPTER_GET_IP_INFO(interface, ipInfo)  \
{ \
tcpip_adapter_get_ip_info(interface, ipInfo); \
}
#define __TCP_INFO__ tcpip_adapter_ip_info_t

#define FORMAT_LITTLEFS_IF_FAILED true
#endif

#include <ArduinoJson.h>

#endif