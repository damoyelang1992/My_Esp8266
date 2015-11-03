**esp_mqtt**
==========
This is MQTT client library for ESP8266, port from: [MQTT client library for Contiki](https://github.com/esar/contiki-mqtt) (thanks)



**Features:**
 * Support subscribing, publishing, authentication, will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).
 * Support multiple connection (to multiple hosts).
 * Support SSL connection (max 1024 bit key size)
 * Easy to setup and use
 * 以下是我修改的部分：
 * wifi自动连接
 * mqtt自动连接
 * 支持SmartConfig，第一次使用会向串口发送两次“LIG:20”/"LIG:0",然后开始SmartConfig配置
 * wifi断开之后每10秒重试连接一次，连接十次之后开启SmartConfig，等待手机连接
 * 使用环境：windows+eclipse
 * 硬件与服务器之间采用SSL连接，用户名密码更安全
 * 硬件序列号采用MD5加密，防止被穷举

**编译环境搭建:**

Instructions for installing and configuring the Unofficial Development Kit for Espressif ESP8266:

 1. To [download the Windows](http://programs74.ru/get.php?file=EspressifESP8266DevKit) (98Mb) and install my Unofficial Development Kit for Espressif ESP8266.
 2. [Download](http://www.oracle.com/technetwork/java/javase/downloads/index.html) and install the Java Runtime x86 (jre-7u72-windows-i586.exe)
 3. [Download](http://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/luna/SR1/eclipse-cpp-luna-SR1-win32.zip) and install Eclipse Luna x86 to develop in C ++ (eclipse-cpp-luna-SR1-win32.zip). Unpack the archive to the root of drive C.
 4. [Download](http://sourceforge.net/projects/mingw/files/Installer/) and install MinGW. Run mingw-get-setup.exe, the installation process to select without GUI, ie uncheck "... also install support for the graphical user interface".
 5. [Download](http://programs74.ru/get.php?file=EspressifESP8266DevKitAddon) the (84Mb) my scripts to automate the installation of additional modules for MinGW.
 6. Run from my file install-mingw-package.bat. He will establish the basic modules for MinGW, installation should proceed without error.
 7. Start the Eclipse Luna from the directory c:\eclipse\eclipse.exe
 8. In Eclipse, select File -> Import -> General -> Existing Project into Workspace, in the line Select root directory, select the directory C:\Espressif\examples and import work projects.
 9. Further, the right to select the Make Target project, such as hello-world and run the target All the assembly, while in the console window should display the progress of the build. To select the target firmware flash.
 

**Compile:**

Make sure to add PYTHON PATH and compile PATH to Eclipse environment variable if using Eclipse

for Windows:

```bash
git clone https://github.com/damoyelang1992/My_Esp8266
BUILD_BASE	= build
FW_BASE		= firmware

XTENSA_TOOLS_ROOT ?= c:/Espressif/xtensa-lx106-elf/bin

SDK_BASE	?= c:\Espressif\ESP8266_SDK

SDK_TOOLS	?= c:/Espressif/utils
ESPTOOL		?= $(SDK_TOOLS)/esptool.exe
ESPPORT		?= COM4
ESPBAUD		?= 115200
```

for Mac or Linux:

```bash
git clone https://github.com/damoyelang1992/My_Esp8266
cd esp_mqtt
#clean
make clean
#make
make SDK_BASE="/opt/Espressif/ESP8266_SDK" FLAVOR="release" all
#flash
make ESPPORT="/dev/ttyUSB0" flash
```

**Usage**
```c
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

MQTT_Client mqttClient;

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
		os_printf("LIG:20;");
		os_delay_us(500000);
		os_printf("LIG:0;");
		os_delay_us(500000);
		os_printf("LIG:20;");
		os_delay_us(500000);
		os_printf("LIG:0;");
		smartconfig_start(smartconfig_done);
	}
}

void user_rf_pre_init(void)
{
}

void sysInfoInit()
{
	sysCfg.cfg_holder = CFG_HOLDER;
	os_sprintf(sysCfg.device_id, MQTT_CLIENT_ID, system_get_chip_id());
	os_sprintf(sysCfg.mqtt_host, "%s", MQTT_HOST);
	sysCfg.mqtt_port = MQTT_PORT;
	os_sprintf(sysCfg.mqtt_user, "%s", MQTT_USER);
	os_sprintf(sysCfg.mqtt_pass, "%s", MQTT_PASS);
	sysCfg.security = DEFAULT_SECURITY;	/* default non ssl */
	sysCfg.mqtt_keepalive = MQTT_KEEPALIVE;
//	os_printf(sysCfg.device_id);
}

void user_init(void)
{
	uart_init(BIT_RATE_9600, BIT_RATE_9600);
//	CFG_Load();
	sysInfoInit();
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);
	MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);
	CheckWifi();
	INFO("\r\nSystem started ...\r\n");
}
```

**Publish message and Subscribe**

```c
/* TRUE if success */
BOOL MQTT_Subscribe(MQTT_Client *client, char* topic, uint8_t qos);

BOOL MQTT_Publish(MQTT_Client *client, const char* topic, const char* data, int data_length, int qos, int retain);

```

**Already support LWT: (Last Will and Testament)**

```c

/* Broker will publish a message with qos = 0, retain = 0, data = "offline" to topic "/lwt" if client don't send keepalive packet */
MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);

```

#Default configuration

See: **include/user_config.h**

If you want to load new default configurations, just change the value of CFG_HOLDER in **include/user_config.h**

**Define protocol name in include/user_config.h**

```c
#define PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/
```

In the Makefile, it will erase section hold the user configuration at 0x3C000

```bash
flash: firmware/0x00000.bin firmware/0x40000.bin
		$(vecho) "eagle.flash.bin-------->0x00000"
		$(vecho) "eagle.irom0text.bin---->0x40000"
```
The BLANKER is the blank.bin file you find in your SDKs bin folder.

**Example projects using esp_mqtt:**<br/>
- [https://github.com/eadf/esp_mqtt_lcd](https://github.com/eadf/esp_mqtt_lcd)

**Limited:**<br/>
- Not fully supported retransmit for QoS1 and QoS2

**Status:** *Pre release.*

[https://github.com/tuanpmt/esp_mqtt/releases](https://github.com/tuanpmt/esp_mqtt/releases)

[MQTT Broker for test](https://github.com/mcollina/mosca)

[MQTT Client for test](https://chrome.google.com/webstore/detail/mqttlens/hemojaaeigabkbcookmlgmdigohjobjm?hl=en)

[For more infomation click here](http://www.esp8266.com/viewtopic.php?f=9&t=820)

**Contributing:**

***Feel free to contribute to the project in any way you like!***

**Requried:**

SDK esp_iot_sdk_v0.9.4_14_12_19 or higher

**Authors:**
秦飞 南通 基于tuan PM 和 CHERTS的windows软件包以及例程 非常感谢

E-mail：iqinfei@163.com

**LICENSE - "MIT License"**

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
