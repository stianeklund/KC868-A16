#pragma once

#include "driver/i2c.h"
#include "esp_err.h"
#include <cstdint>

namespace kc868 {

class I2CManager {
public:
    /**
     * @brief Construct a new I2C Manager
     * 
     * @param i2c_num I2C port number
     * @param sda_pin SDA pin number
     * @param scl_pin SCL pin number
     * @param freq_hz Clock frequency in Hz
     */
    I2CManager(uint8_t i2c_num, gpio_num_t sda_pin, gpio_num_t scl_pin,
               uint32_t freq_hz = 100000);
    
    ~I2CManager();

    // Prevent copying
    I2CManager(const I2CManager&) = delete;
    I2CManager& operator=(const I2CManager&) = delete;

    /**
     * @brief Initialize the I2C interface
     * 
     * @return esp_err_t ESP_OK on success
     */
    [[nodiscard]] esp_err_t init() const;

    /**
     * @brief Write data to an I2C device
     * 
     * @param device_addr Device address
     * @param data Data buffer
     * @param length Data length
     * @return esp_err_t ESP_OK on success
     */
    [[nodiscard]] esp_err_t write(uint8_t device_addr, const uint8_t *data, size_t length) const;

    /**
     * @brief Read data from an I2C device
     * 
     * @param device_addr Device address
     * @param data Data buffer
     * @param length Data length
     * @return esp_err_t ESP_OK on success
     */
    [[nodiscard]] esp_err_t read(uint8_t device_addr, uint8_t *data, size_t length) const;

    /**
     * @brief Scan the I2C bus for devices
     * 
     * @return esp_err_t ESP_OK if any devices found
     */
    [[nodiscard]] esp_err_t scan() const;

private:
    i2c_port_t i2c_num_;
    gpio_num_t sda_pin_;
    gpio_num_t scl_pin_;
    uint32_t freq_hz_;
};

} // namespace kc868
