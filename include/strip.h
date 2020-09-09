#ifndef _STRIP_H_
#define _STRIP_H_

#include "driver/rmt.h"
#include "rmt_ws2812.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum strip_type_t { STRIP_RGB, STRIP_HSV } strip_type_t;

typedef struct StripConfig_t {
  uint8_t brightness;
  uint8_t color[3];
  uint8_t power;
  uint32_t size;
  strip_type_t type;
  uint8_t pin;
} StripConfig_t;

typedef struct StripData_t {
  uint8_t brightness;
  uint8_t color[3];
  uint8_t power;
  uint32_t size;
  strip_type_t type;

  led_strip_t *led_strip;
} StripData_t;

void hsv2rgb(uint8_t *hsv, uint8_t *rgb);

StripData_t *strip_init(StripConfig_t cfg);
void strip_color(StripData_t *strip, uint8_t color[3]);
void strip_colors(StripData_t *strip, uint8_t *colors);
void strip_power(StripData_t *strip, uint8_t power);
void strip_brightness(StripData_t *strip, uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif
