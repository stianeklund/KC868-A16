// EthernetManager.cpp
#include "EthernetManager.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"
#include "lwip/ip_addr.h"
#include "esp_netif_sntp.h"

static auto TAG = "EthernetManager";

#define STATIC_IP               1

#if STATIC_IP
#define S_IP        "192.168.1.5"
#define GATEWAY     "192.168.1.1"
#define NETMASK     "255.255.255.0"
#endif /* STATIC_IP */

EthernetManager::EthernetManager(const bool use_static_ip, const char *static_ip,
                                 const char *gateway, const char *netmask)
    : eth_handle_(nullptr)
      , eth_netif_(nullptr)
      , event_group_(nullptr)
      , use_static_ip_(use_static_ip)
      , static_ip_(static_ip ? static_ip : "")
      , gateway_(gateway ? gateway : "")
      , netmask_(netmask ? netmask : "") {
    event_group_ = xEventGroupCreate();
}

EthernetManager::~EthernetManager() {
    if (event_group_) {
        vEventGroupDelete(event_group_);
    }
}

esp_err_t EthernetManager::initInternal() {
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = ETH_PHY_ADDR;
    phy_config.reset_gpio_num = ETH_PHY_RST_GPIO;

    eth_esp32_emac_config_t esp32_emac_config = {
        .smi_mdc_gpio_num = ETH_MDC_GPIO,
        .smi_mdio_gpio_num = ETH_MDIO_GPIO,
        .interface = EMAC_DATA_INTERFACE_RMII,
        .clock_config = {
            .rmii = {
                .clock_mode = EMAC_CLK_OUT,
                .clock_gpio = EMAC_CLK_OUT_180_GPIO
            }
        },
        .dma_burst_len = ETH_DMA_BURST_LEN_32
    };

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    if (!mac) return ESP_FAIL;

    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
    if (!phy) {
        mac->del(mac);
        return ESP_FAIL;
    }

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_err_t ret = esp_eth_driver_install(&config, &eth_handle_);
    if (ret != ESP_OK) {
        mac->del(mac);
        phy->del(phy);
        return ret;
    }

    return ESP_OK;
}

esp_err_t EthernetManager::configureStaticIP() const {
    if (!use_static_ip_) return ESP_OK;

    esp_err_t ret = esp_netif_dhcpc_stop(eth_netif_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop DHCP client");
        return ret;
    }

#if STATIC_IP
    esp_netif_ip_info_t ip_info = {};
    memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
    ip_info.ip.addr = esp_ip4addr_aton(S_IP);
    ip_info.gw.addr = esp_ip4addr_aton(GATEWAY);
    ip_info.netmask.addr = esp_ip4addr_aton(NETMASK);

    if (esp_netif_set_ip_info(eth_netif_, &ip_info) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ip info");
        return ESP_FAIL;
    }
    return ESP_OK;
#endif
}

esp_err_t EthernetManager::init() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    const esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif_ = esp_netif_new(&cfg);

    ESP_ERROR_CHECK(initInternal());
    ESP_ERROR_CHECK(configureStaticIP());

    ESP_ERROR_CHECK(esp_netif_attach(eth_netif_, esp_eth_new_netif_glue(eth_handle_)));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler, this));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler, this));

    return ESP_OK;
}

esp_err_t EthernetManager::start() const {
    return esp_eth_start(eth_handle_);
}

esp_err_t EthernetManager::stop() const {
    return esp_eth_stop(eth_handle_);
}

bool EthernetManager::waitForConnection(const TickType_t timeout) const {
    const EventBits_t bits = xEventGroupWaitBits(event_group_,
                                                 ETHERNET_CONNECTED_BIT | ETHERNET_FAIL_BIT,
                                                 pdFALSE,
                                                 pdFALSE,
                                                 timeout);

    if (bits & ETHERNET_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Ethernet Connection established");
        return true;
    } else if (bits & ETHERNET_FAIL_BIT) {
        ESP_LOGE(TAG, "Ethernet Connection Failed");
    } else {
        ESP_LOGE(TAG, "Connection timeout");
    }
    return false;
}

void EthernetManager::eth_event_handler(void *arg, esp_event_base_t event_base,
                                        const int32_t event_id, void *event_data) {
    const auto *eth_manager = static_cast<EthernetManager *>(arg);
    uint8_t mac_addr[6] = {};
    const auto eth_handle = *static_cast<esp_eth_handle_t *>(event_data);

    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            ESP_LOGI(TAG, "Ethernet Link Up");
            ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet Link Down");
            xEventGroupSetBits(eth_manager->event_group_, ETHERNET_FAIL_BIT);
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        default:
            break;
    }
}

void EthernetManager::got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                           int32_t event_id, void *event_data) {
    const auto *eth_manager = static_cast<EthernetManager *>(arg);
    const auto event = static_cast<ip_event_got_ip_t *>(event_data);

    ESP_LOGI(TAG, "Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
    ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&event->ip_info.gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    xEventGroupSetBits(eth_manager->event_group_, ETHERNET_CONNECTED_BIT);
}
