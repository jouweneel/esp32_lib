#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "driver/i2c.h"
#include "gpio.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct ThSensor_t {
  uint8_t scl_pin;
  uint8_t sda_pin;
  void (*temperature_cb)(int8_t temperature);
  void (*humidity_cb)(uint8_t humidity);
} ThSensor_t;

#ifdef __cplusplus
extern "C" {
#endif

void th_sensor(void *params);

#ifdef __cplusplus
}
#endif

#endif
