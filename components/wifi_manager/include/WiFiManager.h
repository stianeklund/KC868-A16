// WiFiManager.h
#pragma once
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

class WiFiManager {
public:
    WiFiManager(const char *ssid, const char *password);

    [[nodiscard]] esp_err_t init() const;

private:
    const char *ssid_;
    const char *password_;
};
