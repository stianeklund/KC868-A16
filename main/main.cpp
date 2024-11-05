// main.cpp
#include "driver/gpio.h"
#include "esp_log.h"
#include "i2c_manager.h"
#include "include/KC868A16.h"
#include "include/WiFiManager.h"
#include "include/UartManager.h"


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
    kc868::I2CManager i2cManager(I2C_NUM_0, GPIO_NUM_4, GPIO_NUM_5);
    ESP_ERROR_CHECK(i2cManager.init());
    ESP_LOGI("KC868_A16", "I2C initialized");

    // Initialize KC868-A16 board
    KC868A16 board(i2cManager);
    ESP_ERROR_CHECK(board.init());
    ESP_LOGI("KC868_A16", "KC868-A16 initialized");

    while (true) {
        board.cycle_outputs();
        // Adjust delay to control strobe speed (currently 100ms)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
