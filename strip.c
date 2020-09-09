#include <fastmath.h>

#include "strip.h"

// static const char *TAG = "[strip]";

void hsv2rgb(uint8_t *hsv, uint8_t *rgb) {
  float h = hsv[0]/255.0;
  float s = hsv[1]/255.0;
  float v = hsv[2]/255.0;
  float r = 0; float g = 0; float b = 0;

  uint32_t i = (uint32_t)(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch(i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
  }
  rgb[0] = (uint8_t)(r*255);
  rgb[1] = (uint8_t)(g*255);
  rgb[2] = (uint8_t)(b*255);
}

static void strip_write(StripData_t *strip) {
  uint8_t rgb[3];
  if (strip->type == STRIP_RGB) {
    rgb[0] = strip->color[0];
    rgb[1] = strip->color[1];
    rgb[2] = strip->color[2];
  } else {
    hsv2rgb(strip->color, rgb);
  }

  float brightness = (float)(strip->brightness) / 255.0;
  uint8_t r = (strip->power == 0) ? 0 : (uint8_t)round(rgb[0] * brightness);
  uint8_t g = (strip->power == 0) ? 0 : (uint8_t)round(rgb[1] * brightness);
  uint8_t b = (strip->power == 0) ? 0 : (uint8_t)round(rgb[2] * brightness);

  for (uint32_t i = 0; i < strip->size; i++) {
    strip->led_strip->set_pixel(strip->led_strip, i, r, g, b);
  }
  strip->led_strip->refresh(strip->led_strip, 100);
}

void strip_color(StripData_t *strip, uint8_t color[3]) {
  strip->color[0] = color[0];
  strip->color[1] = color[1];
  strip->color[2] = color[2];

  strip_write(strip);
}

void strip_colors(StripData_t *strip, uint8_t *colors) {
  for (uint32_t i = 0; i < strip->size; i++) {
    uint8_t *pixel = &(colors[3*i]);
    strip->led_strip->set_pixel(strip->led_strip, i, pixel[0], pixel[1], pixel[2]);
  }
  strip->led_strip->refresh(strip->led_strip, 100);
}

void strip_power(StripData_t *strip, uint8_t power) {
  strip->power = power;
  strip_write(strip);
}

void strip_brightness(StripData_t *strip, uint8_t brightness) {
  strip->brightness = brightness;
  strip_write(strip);
}

StripData_t *strip_init(StripConfig_t cfg) {
  rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(cfg.pin, RMT_CHANNEL_0);
  rmt_cfg.clk_div = 2;
  ESP_ERROR_CHECK(rmt_config(&rmt_cfg));
  ESP_ERROR_CHECK(rmt_driver_install(rmt_cfg.channel, 0, 0));

  led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(cfg.size, (led_strip_dev_t)rmt_cfg.channel);
  led_strip_t *led_strip = led_strip_new_rmt_ws2812(&strip_config);
  if (!led_strip) {
    return NULL;
  }

  StripData_t *strip = (StripData_t *)malloc(sizeof(StripData_t));
  strip->brightness = cfg.brightness;
  strip->color[0] = cfg.color[0];
  strip->color[1] = cfg.color[1];
  strip->color[2] = cfg.color[2];
  strip->power = cfg.power;

  strip->type = cfg.type;
  strip->size = cfg.size;
  strip->led_strip = led_strip;

  strip_write(strip);

  return strip;
}
