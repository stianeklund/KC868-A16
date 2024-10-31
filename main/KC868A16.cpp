#include "KC868A16.h"
#include "esp_log.h"

static const char* TAG = "KC868A16";

KC868A16::KC868A16(I2CManager& i2c_manager)
    : pcf8574_1_(i2c_manager, PCF8574_R1_ADDR)
    , pcf8574_2_(i2c_manager, PCF8574_R2_ADDR)
{}

esp_err_t KC868A16::init() {
    // Initialize with all outputs OFF
    esp_err_t ret = pcf8574_1_.write(0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize first PCF8574");
        return ret;
    }
    
    ret = pcf8574_2_.write(0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize second PCF8574");
        return ret;
    }
    
    return ESP_OK;
}

esp_err_t KC868A16::setOutput(uint8_t output, bool state) {
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

esp_err_t KC868A16::setAllOutputs(uint16_t states) {
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
