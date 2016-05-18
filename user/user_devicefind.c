/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_devicefind.c
 *
 * Description: Find your hardware's information while working any mode.
 *
 * Modification history:
 *     2014/3/12, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "espconn.h"
#include "user_json.h"
#include "user_devicefind.h"

const char *device_find_request = "Are You Espressif IOT Smart Device?";
const char *device_find_response_ok = "I'm Plug.";

/*---------------------------------------------------------------------------*/
LOCAL struct espconn ptrespconn;

/******************************************************************************
 * FunctionName : user_devicefind_recv
 * Description  : Processing the received data from the host
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_devicefind_recv(void *arg, char *pusrdata, unsigned short length)
{
#if DEBUG_MODE
    os_printf("Start: user_devicefind_recv \n");
#endif
    char DeviceBuffer[40] = {0};
    char Device_mac_buffer[60] = {0};
    char hwaddr[6];
    remot_info *premot = NULL;
    struct ip_info ipconfig;

    if (wifi_get_opmode() != STATION_MODE) { //get WiFi current operating mode
        wifi_get_ip_info(SOFTAP_IF, &ipconfig);
        wifi_get_macaddr(SOFTAP_IF, hwaddr);

        if (!ip_addr_netcmp((struct ip_addr *)ptrespconn.proto.udp->remote_ip, &ipconfig.ip, &ipconfig.netmask)) {
            wifi_get_ip_info(STATION_IF, &ipconfig); // Get IP info of WiFi station or soft-AP interface
            wifi_get_macaddr(STATION_IF, hwaddr); // Get MAC address
        }
    } else {
        wifi_get_ip_info(STATION_IF, &ipconfig);
        wifi_get_macaddr(STATION_IF, hwaddr);
    }

    if (pusrdata == NULL) {
        return;
    }

    if (length == os_strlen(device_find_request) &&
            os_strncmp(pusrdata, device_find_request, os_strlen(device_find_request)) == 0) {
        os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, device_find_response_ok,
                   MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

        os_printf("%s\n", DeviceBuffer);
        length = os_strlen(DeviceBuffer);
        if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK) // Get the information about a TCP connection or UDP transmission.
        	return;
        os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
        ptrespconn.proto.udp->remote_port = premot->remote_port;
        espconn_sent(&ptrespconn, DeviceBuffer, length); // [@deprecated] This API is deprecated, please use espconn_send instead. // espconn_send - Send data through network
    } else if (length == (os_strlen(device_find_request) + 18)) {
        os_sprintf(Device_mac_buffer, "%s " MACSTR , device_find_request, MAC2STR(hwaddr));
        os_printf("%s", Device_mac_buffer);

        if (os_strncmp(Device_mac_buffer, pusrdata, os_strlen(device_find_request) + 18) == 0) {
            //os_printf("%s\n", Device_mac_buffer);
            length = os_strlen(DeviceBuffer);
            os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, device_find_response_ok,
                       MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

            os_printf("%s\n", DeviceBuffer);
            length = os_strlen(DeviceBuffer);
			if (espconn_get_connection_info(&ptrespconn, &premot, 0) != ESPCONN_OK)
				return;
			os_memcpy(ptrespconn.proto.udp->remote_ip, premot->remote_ip, 4);
			ptrespconn.proto.udp->remote_port = premot->remote_port;
            espconn_sent(&ptrespconn, DeviceBuffer, length);
        } else {
            return;
        }
    }
#if DEBUG_MODE
    os_printf("End: user_devicefind_recv \n");
#endif
}

/******************************************************************************
 * FunctionName : user_devicefind_init
 * Description  : the espconn struct parame init
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_devicefind_init(void)
{
#if DEBUG_MODE
    os_printf("Start: user_devicefind_init \n");
#endif
    ptrespconn.type = ESPCONN_UDP;
    ptrespconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
    ptrespconn.proto.udp->local_port = 1025;
    espconn_regist_recvcb(&ptrespconn, user_devicefind_recv);
    espconn_create(&ptrespconn);
#if DEBUG_MODE
    os_printf("End: user_devicefind_init \n");
#endif
}
