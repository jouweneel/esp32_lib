#include "driver/rmt.h"

#include "strip.h"

#define BITS_PER_LED_CMD 24

#define T0H 7
#define T0L 18
#define T1H 18
#define T1L 7

static uint8_t rmt_channel = 0;

static rmt_item32_t RMT_1 = {{{ T1H, 1, T1L, 0 }}};
static rmt_item32_t RMT_0 = {{{ T0H, 1, T0L, 0 }}};

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
    .clk_div = 4,
  };

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(rmt_channel, 0, 0));

  rmt_item32_t *rmtbuf = (rmt_item32_t *)malloc(BITS_PER_LED_CMD * cfg->size * sizeof(rmt_item32_t));

  rmt_channel++;
  
  return (void *)rmtbuf;
}

void ws2812_write(StripData_t *data, uint8_t *buf) {
  rmt_item32_t *rmtbuf = (rmt_item32_t *)(data->ctx);

  for (uint32_t i = 0; i < data->size; i++) {
    uint32_t offset = i * BITS_PER_LED_CMD;
    uint8_t g = buf[3 * i];
    uint8_t r = buf[3 * i + 1];
    uint8_t b = buf[3 * i + 2];

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

  ESP_ERROR_CHECK(rmt_write_items(0, rmtbuf, data->size * BITS_PER_LED_CMD, true));
}
