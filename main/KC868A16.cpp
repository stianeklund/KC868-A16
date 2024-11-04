#include "KC868A16.h"
#include "esp_log.h"

static const char* TAG = "KC868A16";

KC868A16::KC868A16(I2CManager& i2c_manager)
    : pcf8574_1_(i2c_manager, PCF8574_R1_ADDR)
    , pcf8574_2_(i2c_manager, PCF8574_R2_ADDR)
{}

esp_err_t KC868A16::init() {
    ESP_LOGI(TAG, "Initializing KC868A16 with PCF8574 addresses: 0x%02X, 0x%02X", 
             PCF8574_R1_ADDR, PCF8574_R2_ADDR);

    // Scan the I2C bus first
    esp_err_t scan_ret = i2c_manager_.scan();
    if (scan_ret != ESP_OK) {
        ESP_LOGE(TAG, "No I2C devices found during scan");
        return ESP_FAIL;
    }

    // Initialize with all outputs OFF (HIGH due to active LOW)
    esp_err_t ret = pcf8574_1_.write(0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize first PCF8574 at address 0x%02X", PCF8574_R1_ADDR);
        return ret;
    }
    ESP_LOGI(TAG, "First PCF8574 initialized successfully");
    
    ret = pcf8574_2_.write(0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize second PCF8574 at address 0x%02X", PCF8574_R2_ADDR);
        return ret;
    }
    ESP_LOGI(TAG, "Second PCF8574 initialized successfully");
    
    return ESP_OK;
}

esp_err_t KC868A16::setOutput(uint8_t output, const bool state) {
    if (output < 1 || output > 16) {
        ESP_LOGE(TAG, "Invalid output number: %d", output);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Convert 1-based output number to 0-based
    output--;
    
    // Determine which PCF8574 to use
    PCF8574& pcf = (output < 8) ? pcf8574_1_ : pcf8574_2_;
    uint8_t pin = output % 8;
    
    // Note: PCF8574 is active LOW, so we invert the state
    return pcf.setPin(pin, !state);
}

esp_err_t KC868A16::setAllOutputs(const uint16_t states) {
    // Split the 16-bit state into two 8-bit states
    // Note: We invert the states because PCF8574 is active LOW
    uint8_t states1 = ~(states & 0xFF);
    uint8_t states2 = ~((states >> 8) & 0xFF);
    
    esp_err_t ret = pcf8574_1_.write(states1);
    if (ret != ESP_OK) return ret;
    
    return pcf8574_2_.write(states2);
}

esp_err_t KC868A16::getOutput(uint8_t output, bool& state) {
    if (output < 1 || output > 16) {
        ESP_LOGE(TAG, "Invalid output number: %d", output);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Convert 1-based output number to 0-based
    output--;
    
    // Determine which PCF8574 to use
    PCF8574& pcf = (output < 8) ? pcf8574_1_ : pcf8574_2_;
    uint8_t pin = output % 8;
    
    // Get the state and invert it (PCF8574 is active LOW)
    bool rawState;
    esp_err_t ret = pcf.getPin(pin, rawState);
    if (ret == ESP_OK) {
        state = !rawState;
    }
    return ret;
}
