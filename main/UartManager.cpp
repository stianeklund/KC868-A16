// UARTManager.cpp
#include "UARTManager.h"

UARTManager::UARTManager(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate)
        : uart_num_(uart_num), tx_pin_(tx_pin), rx_pin_(rx_pin), baud_rate_(baud_rate) {}

esp_err_t UARTManager::init() {
    uart_config_t uart_config = {
            .baud_rate = baud_rate_,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
    };

    esp_err_t ret = uart_param_config(uart_num_, &uart_config);
    if (ret != ESP_OK) return ret;

    ret = uart_set_pin(uart_num_, tx_pin_, rx_pin_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) return ret;

    return uart_driver_install(uart_num_, 2048, 0, 0, NULL, 0);
}

esp_err_t UARTManager::write(const char* data, size_t length) {
    int txBytes = uart_write_bytes(uart_num_, data, length);
    return txBytes == length ? ESP_OK : ESP_FAIL;
}

esp_err_t UARTManager::read(uint8_t* buffer, size_t length, size_t* bytes_read) {
    int rxBytes = uart_read_bytes(uart_num_, buffer, length, pdMS_TO_TICKS(1000));
    if (rxBytes >= 0) {
        *bytes_read = rxBytes;
        return ESP_OK;
    }
    return ESP_FAIL;
}
