#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "uart.h"

#define PIN_TX 25
#define PIN_RX 26
#define PIN_RTS (UART_PIN_NO_CHANGE)
#define PIN_CTS (UART_PIN_NO_CHANGE)

#define UART_PORT UART_NUM_2
#define UART_BAUD_RATE 115200

#define BUF_SIZE (1024)

static const char *TAG = "UART";

void uart_task() {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, PIN_TX, PIN_RX, PIN_RTS, PIN_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    ESP_LOGI(TAG, "starting UART task");

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len > 0) {
            ESP_LOGI(TAG, "Received %s", (char *)data);
        }
        // Write data back to the UART
        // uart_write_bytes(UART_PORT, (const char *) data, len);
    }
}
