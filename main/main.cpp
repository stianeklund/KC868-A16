// main.cpp
#include "WiFiManager.h"
#include "EthernetManager.h"
#include "UARTManager.h"
#include "I2CManager.h"
#include "PCF8574.h"
#include "KC868A16.h"
#include "driver/gpio.h"
#include "esp_log.h"

// cycles through the outputs 1-16 for testing purposes
void cycle_outputs(KC868A16& board) {
    static uint8_t current_output = 1;
    static uint8_t previous_output = 16;
    
    // Turn off the previous output
    board.setOutput(previous_output, false);
    ESP_LOGI("STROBE", "Turning off output %d", previous_output);
    
    // Turn on the current output
    board.setOutput(current_output, true);
    ESP_LOGI("STROBE", "Turning on output %d", current_output);
    
    // Store current output as previous for next iteration
    previous_output = current_output;
    
    // Move to next output, wrap around to 1 if we reach 17
    current_output = (current_output % 16) + 1;
}

extern "C" {
[[noreturn]] int app_main();
}

[[noreturn]] int app_main() {
    ESP_LOGI("KC868_A16", "Starting application...");

    // Initialize Wi-Fi
    const WiFiManager wifiManager("your_ssid", "your_password");
    ESP_ERROR_CHECK(wifiManager.init());
    ESP_LOGI("KC868_A16", "Wi-Fi initialized");

    // We can't initialize ethernet and wifi at the same time
    // Initialize Ethernet
    // EthernetManager ethManager;
    // ESP_ERROR_CHECK(ethManager.init());
    // ESP_LOGI("KC868_A16", "Ethernet initialized");

    // Initialize UART
    const UARTManager uartManager(UART_NUM_2, GPIO_NUM_13, GPIO_NUM_16, 9600);
    ESP_ERROR_CHECK(uartManager.init());
    ESP_LOGI("KC868_A16", "UART initialized");

    // Initialize I2C
    I2CManager i2cManager(I2C_NUM_0, GPIO_NUM_4, GPIO_NUM_5);
    ESP_ERROR_CHECK(i2cManager.init());
    ESP_LOGI("KC868_A16", "I2C initialized");

    // Initialize KC868-A16 board
    KC868A16 board(i2cManager);
    ESP_ERROR_CHECK(board.init());
    ESP_LOGI("KC868_A16", "KC868-A16 initialized");

    while (true) {
        cycle_outputs(board);
        // Adjust delay to control strobe speed (currently 100ms)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
