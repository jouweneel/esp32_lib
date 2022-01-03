#include "esp_log.h"

#include "driver/i2c.h"
#include "th_sensor.h"

#define I2C_PORT I2C_NUM_0
#define I2C_ADDRESS 0x70 // 0x38 << 1
#define I2C_READ 0x01
#define I2C_ACK 0x1
#define I2C_FREQ 200000

#define CMD_INIT 0xE1
#define CMD_MEASUREMENT 0xAC
#define CMD_RESET 0xBA

#define ARG_CALIBRATE 0x08
#define ARG_MEASURE 0x33

#define RX_BUF_SIZE 6

static const char *TAG = "AHT10";
static uint8_t rx_buf[RX_BUF_SIZE];

static esp_err_t th_write(
  i2c_port_t port, uint8_t data, uint8_t arg
) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  esp_err_t err = ESP_OK;

  if ((err = i2c_master_start(cmd) != ESP_OK)) return err;
  i2c_master_write_byte(cmd, I2C_ADDRESS, 0);
  i2c_master_write_byte(cmd, data, 0);
  i2c_master_write_byte(cmd, arg, 0);
  i2c_master_write_byte(cmd, 0, I2C_ACK);
  
  i2c_master_stop(cmd);
  err = i2c_master_cmd_begin(port, cmd, 10);
  i2c_cmd_link_delete(cmd);

  vTaskDelay(8);

  if (err < 0) {
    return err;
  } else {
    return ESP_OK;
  }
}

static esp_err_t th_read(
  i2c_port_t port, uint8_t *buf, uint8_t size
) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  esp_err_t err = ESP_OK;

  if ((err = i2c_master_start(cmd) != ESP_OK)) return err;

  i2c_master_write_byte(cmd, I2C_ADDRESS | I2C_READ, I2C_ACK);
  i2c_master_read(cmd, buf, size, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  err = i2c_master_cmd_begin(port, cmd, 100);
  i2c_cmd_link_delete(cmd);
  
  return err;
}

void th_sensor(void *params) {
  ThSensor_t *s = (ThSensor_t *)params;

  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = s->sda_pin,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = s->scl_pin,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_FREQ
  };

  esp_err_t err = ESP_OK;
  if ((err = i2c_param_config(I2C_PORT, &conf)) != ESP_OK) {
    ESP_LOGE(TAG, "i2c config: %d", err);
  }
  if ((err = i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0)) != ESP_OK) {
    ESP_LOGE(TAG, "i2c install: %d", err);
  }
  if ((err = th_write(I2C_PORT, CMD_INIT, ARG_CALIBRATE)) != ESP_OK) {
    ESP_LOGE(TAG, "i2c init: %i", err);
  }

  uint8_t humidity = 0;
  int8_t temperature = 0;

  while(true) {
    if ((err = th_write(I2C_PORT, CMD_MEASUREMENT, ARG_MEASURE)) != ESP_OK) {
      ESP_LOGE(TAG, "SEND CMD: %d", err);
    } else {
      for (int i = 0; i < RX_BUF_SIZE; i++) {
        rx_buf[i] = 0;
      }
      if ((err = th_read(I2C_PORT, rx_buf, RX_BUF_SIZE)) != ESP_OK) {
        ESP_LOGE(TAG, "READ: %d", err);
      } else {
        uint32_t raw = ((uint32_t)rx_buf[1] << 12) | ((uint32_t)rx_buf[2] << 4) | (rx_buf[3] >> 4);
        uint8_t _humidity = (uint8_t)((float)raw * 100 / 0x100000);

        raw = ((uint32_t)(rx_buf[3] & 0x0f) << 16) | ((uint32_t)rx_buf[4] << 8) | rx_buf[5];
        int8_t _temperature = (uint8_t)((float)raw * 200 / 0x100000 - 50);

        if (temperature != _temperature) {
          temperature = _temperature;
          s->temperature_cb(temperature);
        }

        if (humidity != _humidity) {
          humidity = _humidity;
          s->humidity_cb(humidity);
        }
      }
    }

    vTaskDelay(100);
  }
}
