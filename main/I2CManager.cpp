// I2CManager.cpp
#include "I2CManager.h"

I2CManager::I2CManager(const i2c_port_t i2c_num, const gpio_num_t sda_pin, const gpio_num_t scl_pin,
                       const uint32_t freq_hz)
    : i2c_num_(i2c_num), sda_pin_(sda_pin), scl_pin_(scl_pin), freq_hz_(freq_hz) {
}

esp_err_t I2CManager::init() const {
    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin_,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin_,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = freq_hz_,
    };

    if (const esp_err_t ret = i2c_param_config(i2c_num_, &i2c_conf); ret != ESP_OK) return ret;

    return i2c_driver_install(i2c_num_, i2c_conf.mode, 0, 0, 0);
}

esp_err_t I2CManager::write(const uint8_t device_addr, const uint8_t *data, const size_t length) const {
    const auto cmd = i2c_cmd_link_create();
    esp_err_t ret = ESP_OK;

    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    ret |= i2c_master_write(cmd, data, length, true);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(i2c_num_, cmd, pdMS_TO_TICKS(1000));

    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t I2CManager::read(const uint8_t device_addr, uint8_t *data, const size_t length) const {
    const auto cmd = i2c_cmd_link_create();
    esp_err_t ret = ESP_OK;

    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    ret |= i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(i2c_num_, cmd, pdMS_TO_TICKS(1000));

    i2c_cmd_link_delete(cmd);
    return ret;
}
