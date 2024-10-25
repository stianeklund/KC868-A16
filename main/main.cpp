// main.cpp
#include "WiFiManager.h"
#include "EthernetManager.h"
#include "UARTManager.h"
#include "I2CManager.h"
#include "PCF8574.h"
#include "driver/gpio.h"
#include "esp_log.h"

extern "C" {
void app_main();
}

void app_main() {
    ESP_LOGI("KC868_A16", "Starting application...");

    // Initialize Wi-Fi
    WiFiManager wifiManager("your_ssid", "your_password");
    if (wifiManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize Wi-Fi");
    } else {
        ESP_LOGI("KC868_A16", "Wi-Fi initialized");
    }

    // Initialize Ethernet
    EthernetManager ethManager;
    if (ethManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize Ethernet");
    } else {
        ESP_LOGI("KC868_A16", "Ethernet initialized");
    }

    // Initialize UART
    UARTManager uartManager(UART_NUM_1, GPIO_NUM_13, GPIO_NUM_16, 9600);
    if (uartManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize UART");
    } else {
        ESP_LOGI("KC868_A16", "UART initialized");
    }

    // Initialize I2C and PCF8574 devices
    I2CManager i2cManager(I2C_NUM_0, GPIO_NUM_4, GPIO_NUM_5);
    if (i2cManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize I2C");
    } else {
        ESP_LOGI("KC868_A16", "I2C initialized");
    }

    PCF8574 inputs1_8(i2cManager, 0x22);
    PCF8574 inputs9_16(i2cManager, 0x21);
    PCF8574 outputs1_8(i2cManager, 0x24);
    PCF8574 outputs9_16(i2cManager, 0x25);

    // Configure GPIO inputs (HT1, HT2, HT3)
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_32) | (1ULL << GPIO_NUM_33) | (1ULL << GPIO_NUM_14);
    gpio_config(&io_conf);

    while (true) {
        // Read inputs from PCF8574
        uint8_t in1_8_data = 0;
        uint8_t in9_16_data = 0;
        inputs1_8.read(in1_8_data);
        inputs9_16.read(in9_16_data);

        // Process inputs and control outputs
        // Example: Mirror input X01 to output Y01
        bool x01_status = in1_8_data & (1 << 0);
        outputs1_8.setPin(0, x01_status); // Set Y01

        // Delay
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
