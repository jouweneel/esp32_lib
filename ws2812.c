#include "driver/rmt.h"
#include "esp_log.h"

#include "strip.h"

#define BITS_PER_LED_CMD 24

#define TS 14
#define TL 36

static const char *TAG = "ws2812";

static uint8_t rmt_channel = 0;
static const rmt_item32_t RMT_1 = {{{ TL, 1, TL, 0 }}};
static const rmt_item32_t RMT_0 = {{{ TS, 1, TL, 0 }}};

void *ws2812_init(StripConfig_t *cfg) {
  rmt_config_t config = {
    .rmt_mode = RMT_MODE_TX,
    .channel = rmt_channel,
    .gpio_num = cfg->pin,
    .mem_block_num = 1,
    .tx_config.loop_en = false,
    .tx_config.carrier_en = false,
    .tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW,
    .tx_config.idle_output_en = true,
    .tx_config.idle_level = RMT_IDLE_LEVEL_LOW,
    .clk_div = 2,
  };

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(rmt_channel, 0, 0));

  rmt_item32_t *rmtbuf = (rmt_item32_t *)malloc((BITS_PER_LED_CMD * cfg->size) * sizeof(rmt_item32_t));

  rmt_channel++;
  
  return (void *)rmtbuf;
}

static void ws2812_write_pixel(
  uint8_t r, uint8_t g, uint8_t b, uint32_t offset, rmt_item32_t *rmtbuf
) {
  rmtbuf[offset]     = (0b10000000 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 1] = (0b01000000 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 2] = (0b00100000 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 3] = (0b00010000 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 4] = (0b00001000 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 5] = (0b00000100 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 6] = (0b00000010 & r) ? RMT_1 : RMT_0;
  rmtbuf[offset + 7] = (0b00000001 & r) ? RMT_1 : RMT_0;

  rmtbuf[offset + 8]  = (0b10000000 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 9]  = (0b01000000 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 10] = (0b00100000 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 11] = (0b00010000 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 12] = (0b00001000 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 13] = (0b00000100 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 14] = (0b00000010 & g) ? RMT_1 : RMT_0;
  rmtbuf[offset + 15] = (0b00000001 & g) ? RMT_1 : RMT_0;

  rmtbuf[offset + 16] = (0b10000000 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 17] = (0b01000000 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 18] = (0b00100000 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 19] = (0b00010000 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 20] = (0b00001000 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 21] = (0b00000100 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 22] = (0b00000010 & b) ? RMT_1 : RMT_0;
  rmtbuf[offset + 23] = (0b00000001 & b) ? RMT_1 : RMT_0;
}

void ws2812_write_color(StripData_t *strip, uint8_t g, uint8_t r, uint8_t b) {
  rmt_item32_t *rmtbuf = (rmt_item32_t *)(strip->ctx);

  for (uint32_t i = 0; i < strip->size; i++) {
    uint32_t offset = i * BITS_PER_LED_CMD;
    ws2812_write_pixel(r, g, b, offset, rmtbuf);
  }

  ESP_ERROR_CHECK(rmt_write_items(0, rmtbuf, strip->size * BITS_PER_LED_CMD, true));
}

void ws2812_write(StripData_t *strip, uint8_t *buf) {
  rmt_item32_t *rmtbuf = (rmt_item32_t *)(strip->ctx);

  for (uint32_t i = 0; i < strip->size; i++) {
    uint32_t offset = i * BITS_PER_LED_CMD;
    uint32_t idx = 3 * i;

    uint8_t g = buf[idx];
    uint8_t r = buf[idx + 1];
    uint8_t b = buf[idx + 2];

    ws2812_write_pixel(r, g, b, offset, rmtbuf);
  }

  ESP_ERROR_CHECK(rmt_write_items(0, rmtbuf, strip->size * BITS_PER_LED_CMD, true));
}
