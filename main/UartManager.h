// UARTManager.h
#pragma once
#include "driver/uart.h"

class UARTManager {
public:
    UARTManager(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate);

    [[nodiscard]] esp_err_t init() const;

    esp_err_t write(const char *data, size_t length) const;

    esp_err_t read(uint8_t *buffer, size_t length, size_t *bytes_read) const;

private:
    uart_port_t uart_num_;
    int tx_pin_;
    int rx_pin_;
    int baud_rate_;
};
