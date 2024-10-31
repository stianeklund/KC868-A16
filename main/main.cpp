// main.cpp
#include "WiFiManager.h"
#include "EthernetManager.h"
#include "UARTManager.h"
#include "I2CManager.h"
#include "PCF8574.h"
#include "KC868A16.h"
#include "driver/gpio.h"
#include "esp_log.h"

extern "C" {
[[noreturn]] int app_main();
}

[[noreturn]] int app_main() {
    esp_err_t ret = ESP_OK;
    ESP_LOGI("KC868_A16", "Starting application...");

    // Initialize Wi-Fi
    WiFiManager wifiManager("your_ssid", "your_password");
    if (wifiManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize Wi-Fi");
    } else {
        ESP_LOGI("KC868_A16", "Wi-Fi initialized");
    }

    // Initialize Ethernet
    if (EthernetManager ethManager; ethManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize Ethernet");
    } else {
        ESP_LOGI("KC868_A16", "Ethernet initialized");
    }

    // Initialize UART
    if (const UARTManager uartManager(UART_NUM_2, GPIO_NUM_13, GPIO_NUM_16, 9600); uartManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize UART");
    } else {
        ESP_LOGI("KC868_A16", "UART initialized");
    }

    // Initialize I2C
    I2CManager i2cManager(I2C_NUM_0, GPIO_NUM_4, GPIO_NUM_5);
    if (i2cManager.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize I2C");
        return;
    }
    ESP_LOGI("KC868_A16", "I2C initialized");

    // Initialize KC868-A16 board
    KC868A16 board(i2cManager);
    if (board.init() != ESP_OK) {
        ESP_LOGE("KC868_A16", "Failed to initialize KC868-A16");
    }
    ESP_LOGI("KC868_A16", "KC868-A16 initialized");

    // Example: Control some outputs
    board.setOutput(1, true); // Turn ON output 1
    board.setOutput(2, false); // Turn OFF output 2

    // Turn ON outputs 1, 3, and 5 using setAllOutputs
    board.setAllOutputs(0b0000000000010101);

    bool output_state;

    while (true) {
        // Read and log the state of output 1
        if (board.getOutput(1, output_state) == ESP_OK) {
            ESP_LOGI("KC868_A16", "Output 1 is %s", output_state ? "ON" : "OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
