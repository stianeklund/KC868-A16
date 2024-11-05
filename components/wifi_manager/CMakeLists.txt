idf_component_register(
    SRCS "WiFiManager.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        esp_wifi
        nvs_flash
        esp_netif
        esp_event
)
