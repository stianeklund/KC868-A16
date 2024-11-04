// EthernetManager.h
#pragma once
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/event_groups.h"
#include <string>

class EthernetManager {
public:
    // LAN8720 configuration matching ESPHome
    static constexpr int ETH_PHY_ADDR = 0;      // phy_addr: 0
    static constexpr int ETH_PHY_RST_GPIO = -1; // not connected
    static constexpr int ETH_MDC_GPIO = 23;     // mdc_pin: GPIO23
    static constexpr int ETH_MDIO_GPIO = 18;    // mdio_pin: GPIO18
    static constexpr int ETH_CLK_GPIO = 17;     // clk_mode: GPIO17_OUT


    // Event group bits
    static constexpr int ETHERNET_CONNECTED_BIT = BIT0;
    static constexpr int ETHERNET_FAIL_BIT = BIT1;

    explicit EthernetManager(bool use_static_ip = false,
                             const char *static_ip = nullptr,
                             const char *gateway = nullptr,
                             const char *netmask = nullptr);

    ~EthernetManager();

    esp_err_t init();

    [[nodiscard]] esp_err_t start() const;

    esp_err_t start();

    [[nodiscard]] esp_err_t stop() const;

    [[nodiscard]] bool waitForConnection(TickType_t timeout = portMAX_DELAY) const;

private:
    static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data);

    static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data);

    esp_err_t initInternal();

    [[nodiscard]] esp_err_t configureStaticIP() const;

    esp_eth_handle_t eth_handle_;
    esp_netif_t *eth_netif_;
    EventGroupHandle_t event_group_;
    bool use_static_ip_;
    std::string static_ip_;
    std::string gateway_;
    std::string netmask_;
};
