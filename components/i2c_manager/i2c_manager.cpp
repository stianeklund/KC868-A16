// I2CManager.cpp
#include "i2c_manager.h"
#include <esp_log.h>

namespace kc868 {

I2CManager::I2CManager(const uint8_t i2c_num, const gpio_num_t sda_pin, const gpio_num_t scl_pin,
                      const uint32_t freq_hz)
    : i2c_num_(static_cast<i2c_port_t>(i2c_num))
    , sda_pin_(sda_pin)
    , scl_pin_(scl_pin)
    , freq_hz_(freq_hz) {
}

I2CManager::~I2CManager() {
    i2c_driver_delete(i2c_num_);
}

esp_err_t I2CManager::init() const {
    const i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin_,
        .scl_io_num = scl_pin_,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = freq_hz_
        }
    };

    if (const esp_err_t ret = i2c_param_config(i2c_num_, &conf); ret != ESP_OK) return ret;

    return i2c_driver_install(i2c_num_, conf.mode, 0, 0, 0);
}

esp_err_t I2CManager::write(const uint8_t device_addr, const uint8_t *data, const size_t length) const {
    const auto cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, length, true);
    i2c_master_stop(cmd);

    const esp_err_t ret = i2c_master_cmd_begin(i2c_num_, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t I2CManager::scan() const {
    ESP_LOGI("I2C", "Scanning I2C bus...");
    bool found = false;

    for (uint8_t address = 1; address < 127; address++) {
        const auto cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        const esp_err_t ret = i2c_master_cmd_begin(i2c_num_, cmd, pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            ESP_LOGI("I2C", "Found device at address 0x%02X", address);
            found = true;
        }
    }

    if (!found) {
        ESP_LOGW("I2C", "No devices found");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t I2CManager::read(const uint8_t device_addr, uint8_t *data, const size_t length) const {
    const auto cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    if (length > 1) {
        i2c_master_read(cmd, data, length - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + length - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    const esp_err_t ret = i2c_master_cmd_begin(i2c_num_, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

} // namespace kc868
