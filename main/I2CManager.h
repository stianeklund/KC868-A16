// I2CManager.h
#pragma once
#include "driver/i2c.h"

class I2CManager {
public:
    I2CManager(i2c_port_t i2c_num, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t freq_hz = 100000);
    esp_err_t init();
    esp_err_t write(uint8_t device_addr, uint8_t* data, size_t length);
    esp_err_t read(uint8_t device_addr, uint8_t* data, size_t length);
private:
    i2c_port_t i2c_num_;
    gpio_num_t sda_pin_;
    gpio_num_t scl_pin_;
    uint32_t freq_hz_;
};
