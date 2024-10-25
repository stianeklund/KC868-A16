// EthernetManager.cpp
#include "EthernetManager.h"
#include "driver/gpio.h"

EthernetManager::EthernetManager() : eth_handle_(nullptr) {}

esp_err_t EthernetManager::init() {
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t* eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 0;
    phy_config.reset_gpio_num = -1;

    // Configure GPIOs
    gpio_pad_select_gpio(GPIO_NUM_23); // MDC
    gpio_pad_select_gpio(GPIO_NUM_18); // MDIO
    gpio_pad_select_gpio(GPIO_NUM_17); // CLK_OUT

    // RMII Clock Output to GPIO17
    gpio_matrix_out(GPIO_NUM_17, EMAC_CLK_OUT, false, false);
    phy_config.clock_mode = ETH_CLOCK_GPIO17_OUT;

    esp_eth_mac_t* mac = esp_eth_mac_new_esp32(&mac_config);
    esp_eth_phy_t* phy = esp_eth_phy_new_lan8720(&phy_config);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    esp_err_t ret = esp_eth_driver_install(&config, &eth_handle_);
    if (ret != ESP_OK) return ret;

    return esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle_));
}
