#include "include/KC868A16.h"
#include "esp_log.h"

static auto TAG = "KC868A16";

KC868A16::KC868A16(kc868::I2CManager &i2c_manager)
    : i2c_manager_(i2c_manager)
      , pcf8574_1_(i2c_manager, PCF8574_R1_ADDR)
      , pcf8574_2_(i2c_manager, PCF8574_R2_ADDR) {
}

esp_err_t KC868A16::init() {
    ESP_LOGI(TAG, "Initializing KC868A16 with PCF8574 addresses: 0x%02X, 0x%02X",
             PCF8574_R1_ADDR, PCF8574_R2_ADDR);

    // Scan the I2C bus first
    ESP_LOGI(TAG, "Starting I2C scan...");
    if (const esp_err_t scan_ret = i2c_manager_.scan(); scan_ret != ESP_OK) {
        ESP_LOGE(TAG, "No I2C devices found during scan");
        return ESP_FAIL;
    }

    // Verify PCF8574 devices are present
    uint8_t dummy;
    if (i2c_manager_.read(PCF8574_R1_ADDR, &dummy, 1) != ESP_OK) {
        ESP_LOGE(TAG, "PCF8574_R1 not found at address 0x%02X", PCF8574_R1_ADDR);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Found PCF8574_R1 at address 0x%02X", PCF8574_R1_ADDR);

    if (i2c_manager_.read(PCF8574_R2_ADDR, &dummy, 1) != ESP_OK) {
        ESP_LOGE(TAG, "PCF8574_R2 not found at address 0x%02X", PCF8574_R2_ADDR);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Found PCF8574_R2 at address 0x%02X", PCF8574_R2_ADDR);

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

    // Outputs are 1-16, decrement here to get a 0-based number
    output--;

    // Determine which PCF8574 to use
    PCF8574 &pcf = (output < 8) ? pcf8574_1_ : pcf8574_2_;
    // make sure our value is within 0-7 (same pin number for each of these, just different addresses)
    const uint8_t pin = output % 8;

    // Note: PCF8574 is active LOW, so we invert the state
    return pcf.setPin(pin, !state);
}

esp_err_t KC868A16::setAllOutputs(const uint16_t states) {
    // We invert here because PCF8574 is active LOW
    // States is split into high & low bytes as each PC8574 can handle 8 IO pins
    const uint8_t states1 = ~(states & 0xFF);
    const uint8_t states2 = ~((states >> 8) & 0xFF);

    if (const esp_err_t ret = pcf8574_1_.write(states1); ret != ESP_OK) return ret;

    return pcf8574_2_.write(states2);
}

esp_err_t KC868A16::getOutput(uint8_t output, bool &state) const {
    if (output < 1 || output > 16) {
        ESP_LOGE(TAG, "Invalid output number: %d", output);
        return ESP_ERR_INVALID_ARG;
    }

    // Outputs are 1-16, decrement here to get a 0-based number
    output--;

    // Determine which PCF8574 to use (either our output or inputs..)
    const PCF8574 &pcf = (output < 8) ? pcf8574_1_ : pcf8574_2_;
    const uint8_t pin = output % 8;

    bool rawState;

    // Get the state and invert it (PCF8574 is active LOW)
    const esp_err_t ret = pcf.getPin(pin, rawState);

    if (ret == ESP_OK) {
        state = !rawState;
    }
    return ret;
}

// cycles through the outputs 1-16 for testing purposes
void KC868A16::cycle_outputs() {
    static uint8_t current_output = 1;
    static uint8_t previous_output = 16;

    // Turn off the previous output
    setOutput(previous_output, false);
    ESP_LOGI("STROBE", "Turning off output %d", previous_output);

    // Turn on the current output
    setOutput(current_output, true);
    ESP_LOGI("STROBE", "Turning on output %d", current_output);

    // Store current output as previous for next iteration
    previous_output = current_output;

    // Move to next output, wrap around to 1 if we reach 17
    current_output = (current_output % 16) + 1;
}
