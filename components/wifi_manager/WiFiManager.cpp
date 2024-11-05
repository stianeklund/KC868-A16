// WiFiManager.cpp
#include "include/WiFiManager.h"
#include <cstring>

WiFiManager::WiFiManager(const char *ssid, const char *password)
    : ssid_(ssid), password_(password) {
}

esp_err_t WiFiManager::init() const {
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) return ret;

    ret = esp_netif_init();
    if (ret != ESP_OK) return ret;

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) return ret;

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) return ret;

    wifi_config_t wifi_config = {};
    strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid_, sizeof(wifi_config.sta.ssid));
    strncpy(reinterpret_cast<char *>(wifi_config.sta.password), password_, sizeof(wifi_config.sta.password));

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) return ret;

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) return ret;

    return esp_wifi_start();
}
