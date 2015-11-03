/*
 * wifi.c
 *
 *  Created on: Dec 30, 2014
 *      Author: Minh
 */
#include "wifi.h"
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "mqtt_msg.h"
#include "debug.h"
#include "user_config.h"
#include "config.h"
#include "smart.h"

uint8 ConNum = 0;
static ETSTimer WiFiLinker;
WifiCallback wifiCb = NULL;
static uint8_t wifiStatus = STATION_IDLE, lastWifiStatus = STATION_IDLE;
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;
//	os_printf("Check_Ip\n");
	os_timer_disarm(&WiFiLinker);
	wifi_get_ip_info(STATION_IF, &ipConfig);
	wifiStatus = wifi_station_get_connect_status();
	if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0)
	{
//		os_printf("ok");
		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&WiFiLinker, 5000, 0);
	}
	else
	{
		if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
		{
			if(ConNum<10)
			{
				ConNum++;
				INFO("STATION_WRONG_PASSWORD\r\n");
				wifi_station_connect();
			}else{
				/************这里即将填充SmartConfig功能*************/
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
//				os_delay_us(500000);
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
				INFO("SmartConfigStart。。。\r\n");
				ConNum=0;
//				os_timer_disarm(&WiFiLinker);
				smartconfig_start(smartconfig_done);
//				smartconfig_start(smartconfig_done);
			}
		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{
			if(ConNum<10)
			{
				ConNum++;
				INFO("STATION_NO_AP_FOUND\r\n");
				wifi_station_connect();
			}else{
				/************这里即将填充SmartConfig功能*************/
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
//				os_delay_us(500000);
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
				INFO("SmartConfigStart。。。\r\n");
				ConNum=0;
//				os_timer_disarm(&WiFiLinker);
				smartconfig_start(smartconfig_done);
//				smartconfig_start(smartconfig_done);
			}
//			ConNum++;
		}
		else if(wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
		{

			if(ConNum<10)
			{
				ConNum++;
				INFO("STATION_CONNECT_FAIL\r\n");
				wifi_station_connect();
			}else{
				/************这里即将填充SmartConfig功能*************/
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
//				os_delay_us(500000);
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
				INFO("SmartConfigStart。。。\r\n");
				ConNum=0;
//				os_timer_disarm(&WiFiLinker);
				smartconfig_start(smartconfig_done);
//				smartconfig_start(smartconfig_done);
			}
		}
		else
		{
			if(ConNum<10)
			{
				ConNum++;
				INFO("STATION_IDLE\r\n");
			}else{
				/************这里即将填充SmartConfig功能*************/
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
//				os_delay_us(500000);
//				os_printf("LIG:20;");
//				os_delay_us(500000);
//				os_printf("LIG:0;");
				INFO("SmartConfigStart。。。\r\n");
				ConNum=0;
//				os_timer_disarm(&WiFiLinker);
				smartconfig_start(smartconfig_done);
//				smartconfig_start(smartconfig_done);
			}
		}

		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&WiFiLinker, 5000, 0);
	}
	if(wifiStatus != lastWifiStatus){
		lastWifiStatus = wifiStatus;
		if(wifiCb)
			wifiCb(wifiStatus);
	}
}

void ICACHE_FLASH_ATTR WIFI_Connect(uint8_t* ssid, uint8_t* pass, WifiCallback cb)
{
	struct station_config stationConf;

	INFO("WIFI_INIT\r\n");
	wifi_set_opmode(STATION_MODE);
	wifi_station_set_auto_connect(FALSE);
	wifiCb = cb;

	os_memset(&stationConf, 0, sizeof(struct station_config));

	os_sprintf(stationConf.ssid, "%s", ssid);
	os_sprintf(stationConf.password, "%s", pass);

	wifi_station_set_config(&stationConf);

	os_timer_disarm(&WiFiLinker);
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&WiFiLinker, 1000, 0);

	wifi_station_set_auto_connect(TRUE);
	wifi_station_connect();
}

