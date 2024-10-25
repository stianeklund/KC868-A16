// EthernetManager.h
#pragma once
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_netif.h"

class EthernetManager {
public:
    EthernetManager();
    esp_err_t init();
private:
    esp_eth_handle_t eth_handle_;
};
