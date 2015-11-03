#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#define CFG_HOLDER	0x00FF55A4	/* Change this value to load default configurations */
#define CFG_LOCATION	0x3C	/* Please don't change or if you know what you doing */
#define CLIENT_SSL_ENABLE

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST			"写入你自己的服务器地址" //or "mqtt.yourdomain.com"
#define MQTT_PORT			服务器端口，默认SSL打开，请修改DEFAULT_SECURITY为0关闭SSL
#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		60	 /*second*/

#define MQTT_CLIENT_ID		"DVES%08X"//根据芯片ID号进行自动生成
#define MQTT_USER			"MQTT用户名，自行填写"
#define MQTT_PASS			"MQTT密码"

#define MQTT_RECONNECT_TIMEOUT 	5	   /*second*/

#define DEFAULT_SECURITY	            1
#define QUEUE_BUFFER_SIZE		 		2048 /*±¾À´ÊÇ 2048*/

#define PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/
#endif
//PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
