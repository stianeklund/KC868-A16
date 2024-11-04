idf_component_register(SRCS "main.cpp" "KC868A16.cpp" "EthernetManager.cpp" "I2CManager.cpp" "PCF8574.cpp" "UartManager.cpp"
        "WiFiManager.cpp"
        INCLUDE_DIRS "."
        REQUIRES freertos esp_wifi nvs_flash esp_http_server driver esp_netif esp_timer json esp_eth driver)
