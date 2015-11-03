/* main.c -- MQTT client example
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/
#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
#include "smart.h"
#include "md5.h"

void MD5(void);
unsigned char decrypt[16];
MQTT_Client mqttClient;

void h8_str(char* str,unsigned char h8 )
{
	*str = h8>>4;
	if(*str < 10)
		*str += 0x30;
	else
		*str += 0x57;
	*++str = h8&0x0f;
	if((h8 &0x0f) < 10)
		*str += 0x30;
	else
		*str += 0x57;
}
void data8_str(char* str,unsigned char *data,unsigned char n )
{
	unsigned char i;
	for(i=0;i<n;i++)
	{
	  h8_str(&str[i<<1],data[i]);
	}
}

void CheckWifi(void)
{
	uint8 ModeFlag,ssidFlag;
	struct station_config config[5];
	uint8 a = wifi_station_get_auto_connect();
	if(a) wifi_station_set_auto_connect(0);
	int i = wifi_station_get_ap_info(config);
	uint8_t *ssid = config[0].ssid;
	uint8_t *pass = config[0].password;
	if(config[0].ssid)
	{
		WIFI_Connect(ssid,pass,wifiConnectCb);
		wifi_station_set_auto_connect(1);
	}else{
		os_printf("SmartConfig Start¡£¡£¡£");
		smartconfig_start(smartconfig_done);
	}
}

void user_rf_pre_init(void)
{
}

void sysInfoInit()
{
	MD5();
	sysCfg.cfg_holder = CFG_HOLDER;
	data8_str(sysCfg.device_id,decrypt,16 );
	os_sprintf(sysCfg.mqtt_host, "%s", MQTT_HOST);
	sysCfg.mqtt_port = MQTT_PORT;
	os_sprintf(sysCfg.mqtt_user, "%s", MQTT_USER);
	os_sprintf(sysCfg.mqtt_pass, "%s", MQTT_PASS);
	sysCfg.security = DEFAULT_SECURITY;				/* default non ssl */
	sysCfg.mqtt_keepalive = MQTT_KEEPALIVE;
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	sysInfoInit();
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);
	MQTT_InitLWT(&mqttClient, sysCfg.device_id, "Oh~~ I am offline", 0, 0);
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);
	CheckWifi();
	INFO("\r\nSystem started ...\r\n");
}

void MD5()
{
	MD5_CTX md5;
	MD5Init(&md5);
	unsigned char encrypt[16];
	MD5Update(&md5,(unsigned char*)MQTT_CLIENT_ID,strlen((unsigned char*)MQTT_CLIENT_ID));
	MD5Final(&md5,decrypt);
}
