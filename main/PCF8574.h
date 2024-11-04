// PCF8574.h
#pragma once
#include "I2CManager.h"

class PCF8574 {
public:
    PCF8574(I2CManager &i2c_manager, uint8_t device_addr);

    esp_err_t write(uint8_t data);

    esp_err_t read(uint8_t &data) const;

    esp_err_t setPin(uint8_t pin, bool level);

    esp_err_t getPin(uint8_t pin, bool &level) const;

private:
    I2CManager &i2c_manager_;
    uint8_t device_addr_;
    uint8_t state_; // Keep track of output state
};
