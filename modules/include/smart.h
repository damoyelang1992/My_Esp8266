#ifndef USER_SMART_H_
#define USER_SMART_H_

#include "os_type.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "mqtt.h"

void wifiConnectCb(uint8_t status);

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata);

#endif /* USER_SMART_H_ */
