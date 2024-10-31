#pragma once
#include "PCF8574.h"
#include "I2CManager.h"

class KC868A16 {
public:
    KC868A16(I2CManager& i2c_manager);
    
    // Initialize the board
    esp_err_t init();
    
    // Set single output (1-16)
    esp_err_t setOutput(uint8_t output, bool state);
    
    // Set all outputs at once
    esp_err_t setAllOutputs(uint16_t states);
    
    // Get the state of a single output (1-16)
    esp_err_t getOutput(uint8_t output, bool& state);

private:
    // Convert 8-bit Arduino addresses (0x3C, 0x3D) to 7-bit addresses by shifting right
    static constexpr uint8_t PCF8574_R1_ADDR = 0x3C >> 1; // First relay PCF8574 (0x1E)
    static constexpr uint8_t PCF8574_R2_ADDR = 0x3D >> 1; // Second relay PCF8574 (0x1F)
    
    // Input PCF8574s (not used for relay control but keeping for reference)
    static constexpr uint8_t PCF8574_I1_ADDR = 0x3A >> 1; // First input PCF8574 (0x1D)
    static constexpr uint8_t PCF8574_I2_ADDR = 0x39 >> 1; // Second input PCF8574 (0x1C)
    
    PCF8574 pcf8574_1_;
    PCF8574 pcf8574_2_;
};
