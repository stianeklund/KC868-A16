#pragma once
#include "PCF8574.h"
#include "I2CManager.h"

class KC868A16 {
public:
    KC868A16(I2CManager& i2c_manager);
    
    // Initialize the board
    esp_err_t init();

    // Reference to the I2C manager for direct access
    I2CManager& getI2CManager() { return i2c_manager_; }
    
    // Set single output (1-16)
    esp_err_t setOutput(uint8_t output, bool state);
    
    // Set all outputs at once
    esp_err_t setAllOutputs(uint16_t states);
    
    // Get the state of a single output (1-16)
    esp_err_t getOutput(uint8_t output, bool& state);

private:
    // PCF8574 7-bit I2C addresses for relay control
    static constexpr uint8_t PCF8574_R1_ADDR = 0x24; // First relay PCF8574 (outputs 1-8)
    static constexpr uint8_t PCF8574_R2_ADDR = 0x25; // Second relay PCF8574 (outputs 9-16)
    
    // Input PCF8574s (not used for relay control but keeping for reference)
    static constexpr uint8_t PCF8574_I1_ADDR = 0x26; // First input PCF8574
    static constexpr uint8_t PCF8574_I2_ADDR = 0x27; // Second input PCF8574
    
    PCF8574 pcf8574_1_;
    PCF8574 pcf8574_2_;
};
