idf_component_register(
    SRCS "EthernetManager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        driver
        esp_eth
        esp_netif
        esp_event
        nvs_flash
        lwip
)
