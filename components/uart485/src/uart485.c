#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
//#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "para_list.h"

#define TAG "RS485_ECHO_APP"
// Note: Some pins on target chip cannot be assigned for UART communication.
// Please refer to documentation for selected board and target to configure pins using Kconfig.


// RTS for RS485 Half-Duplex Mode manages DE/~RE
#define ECHO_TEST_RTS   (19)

// CTS is not used in RS485 Half-Duplex Mode
#define ECHO_TEST_CTS   (-1)

#define BUF_SIZE        (127)
#define BAUD_RATE       (115200)

// Read packet timeout
#define PACKET_READ_TICS        (100 / 10)
// #define ECHO_TASK_STACK_SIZE    (2048)
// #define ECHO_TASK_PRIO          (12)
#define ECHO_UART_PORT          (1)
#define ECHO_TEST_TXD   (2)//(48)//
#define ECHO_TEST_RXD   (1)//(45)//
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define ECHO_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks

#define ECHO_232_PORT          (2)
#define ECHO_232_TXD   (48)
#define ECHO_232_RXD   (45)

const int uart_num = ECHO_UART_PORT;
const int uart232_num = ECHO_232_PORT;

uart_config_t uart_config = {
    .baud_rate = BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
    .source_clk = UART_SCLK_APB,
};

static void echo_send(const int port, const char* str, uint8_t length)
{
    if (uart_write_bytes(port, str, length) != length) {
        ESP_LOGE(TAG, "Send data critical failure.");
        // add your code to handle sending failure here
        abort();
    }
}
void uart232_task(void *arg)
{
    ESP_LOGI(TAG, "Start RS232 application test and configure UART.");
    // Install UART driver (we don't need an event queue here)
    // In this example we don't even use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(uart232_num, BUF_SIZE * 2, 0, 0, NULL, 0));
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart232_num, &uart_config));
    ESP_LOGI(TAG, "UART set pins, mode and install driver.");
    // Set UART pins as per KConfig settings
    ESP_ERROR_CHECK(uart_set_pin(uart232_num, ECHO_232_TXD, ECHO_232_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    // Set RS485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart232_num, UART_MODE_RS485_HALF_DUPLEX));
    // Set read timeout of UART TOUT feature
    ESP_ERROR_CHECK(uart_set_rx_timeout(uart232_num, ECHO_READ_TOUT));
    // Allocate buffers for UART
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    ESP_LOGI(TAG, "UART start recieve loop.\r\n");
    echo_send(uart232_num, "Start RS232 UART test.\r\n", 24);
    while(1) {
        //Read data from UART
        int len = uart_read_bytes(uart232_num, data, BUF_SIZE, PACKET_READ_TICS);

        //Write data back to UART
        if (len > 0) {
            //echo_send(uart_num, "\r\n", 2);
            char prefix[] = "RS232 Received: [";
            echo_send(uart232_num, prefix, (sizeof(prefix) - 1));
            ESP_LOGI(TAG, "Received %u bytes:", len);
            ESP_LOGI(TAG, "[ ");
            for (int i = 0; i < len; i++) {
                printf("0x%.2X ", (uint8_t)data[i]);
                echo_send(uart232_num, (const char*)&data[i], 1);
                // Add a Newline character if you get a return charater from paste (Paste tests multibyte receipt/buffer)
                if (data[i] == '\r') {
                    echo_send(uart232_num, "\n", 1);
                }
            }
            printf("] \n");
            echo_send(uart232_num, "]\r\n", 3);
        } 
        else {
            // Echo a "." to show we are alive while we wait for input
            //echo_send(uart_num, ".", 1);
            ESP_ERROR_CHECK(uart_wait_tx_done(uart232_num, 10));
        }
    }
    vTaskDelete(NULL);
}
// An example of echo test with hardware flow control on UART
void uart485_task(void *arg)
{
    // Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "Start RS485 application test and configure UART.");

    // Install UART driver (we don't need an event queue here)
    // In this example we don't even use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    ESP_LOGI(TAG, "UART set pins, mode and install driver.");

    // Set UART pins as per KConfig settings
    ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Set RS485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));

    // Set read timeout of UART TOUT feature
    ESP_ERROR_CHECK(uart_set_rx_timeout(uart_num, ECHO_READ_TOUT));

    // Allocate buffers for UART
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

    ESP_LOGI(TAG, "UART start recieve loop.\r\n");
    echo_send(uart_num, "Start RS485 UART test.\r\n", 24);

    while(1) {
        //Read data from UART
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, PACKET_READ_TICS);

        //Write data back to UART
        if (len > 0) {
            //echo_send(uart_num, "\r\n", 2);
            char prefix[] = "RS485 Received: [";
            echo_send(uart_num, prefix, (sizeof(prefix) - 1));
            ESP_LOGI(TAG, "Received %u bytes:", len);
            ESP_LOGI(TAG, "[ ");
            for (int i = 0; i < len; i++) {
                printf("0x%.2X ", (uint8_t)data[i]);
                echo_send(uart_num, (const char*)&data[i], 1);
                // Add a Newline character if you get a return charater from paste (Paste tests multibyte receipt/buffer)
                if (data[i] == '\r') {
                    echo_send(uart_num, "\n", 1);
                }
            }
            printf("] \n");
            echo_send(uart_num, "]\r\n", 3);
        } 
        else {
            // Echo a "." to show we are alive while we wait for input
            //echo_send(uart_num, ".", 1);
            ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 10));
        }
    }
    vTaskDelete(NULL);
}

void heater_water_module_test(uint8_t send_case)
{
    char buf_send1[] = {0xFA,0x03,0x01,0x00,0x96,0x94}; //low quantity
    char buf_send2[] = {0xFA,0x03,0x01,0x01,0xF4,0xF3}; //mid quantity
    char buf_send3[] = {0xFA,0x03,0x01,0x03,0xE8,0xE9}; //high quantity

    char buf_send6[] = {0xFA,0x02,0x02,0x32,0x30};  //50 heater temp 19~64 step 5
    char buf_send7[] = {0xFA,0x02,0x02,0x19,0x17};  //25
    //char buf_send7[] = {0xFA,0x02,0x02,0x50,0x4E}; 
    char buf_send4[] = {0xFA,0x02,0x03,0x01,0x00}; //start
    char buf_send5[] = {0xFA,0x02,0x03,0x02,0x01}; //stop

    char buf_string[6] = {0};
    // strcpy(buf_string,)
    // echo_send(uart_num, buf_string, (sizeof(buf_string) - 1));
    switch(send_case)   //BLISTER
    {
        case 1:
            echo_send(uart_num, buf_send1, sizeof(buf_send1));
            ESP_LOGI(TAG, "buf_send1");
        break;
        case 2:
            echo_send(uart_num, buf_send2, sizeof(buf_send2));
            ESP_LOGI(TAG, "buf_send2");
        break;
        case 3:
            echo_send(uart_num, buf_send3, sizeof(buf_send3));
            ESP_LOGI(TAG, "buf_send3");
        break;
        case 4:
            echo_send(uart_num, buf_send4, sizeof(buf_send4));
            ESP_LOGI(TAG, "buf_send4");
        break;
        case 5:
            echo_send(uart_num, buf_send5, sizeof(buf_send5));
            ESP_LOGI(TAG, "buf_send5");
        break;
        case 6:
            echo_send(uart_num, buf_send6, sizeof(buf_send6));
            ESP_LOGI(TAG, "buf_send6");
        break;
        case 7:
            echo_send(uart_num, buf_send7, sizeof(buf_send7));
            ESP_LOGI(TAG, "buf_send7");
        break;
        default:
        break;
    }
}

        // if (len > 0) {
        //     echo_send(uart_num, "\r\n", 2);
        //     char prefix[] = "RS485 Received: [";
        //     echo_send(uart_num, prefix, (sizeof(prefix) - 1));
        //     ESP_LOGI(TAG, "Received %u bytes:", len);
        //     ESP_LOGI(TAG, "[ ");
        //     for (int i = 0; i < len; i++) {
        //         ESP_LOGI(TAG, "0x%.2X ", (uint8_t)data[i]);
        //         echo_send(uart_num, (const char*)&data[i], 1);
        //         // Add a Newline character if you get a return charater from paste (Paste tests multibyte receipt/buffer)
        //         if (data[i] == '\r') {
        //             echo_send(uart_num, "\n", 1);
        //         }
        //     }
        //     ESP_LOGI(TAG, "] \n");
        //     echo_send(uart_num, "]\r\n", 3);
        // } else {
        //     // Echo a "." to show we are alive while we wait for input
        //     echo_send(uart_num, ".", 1);
        //     ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 10));
        // }

// esp_err_t test_app(void)
// {
//     ESP_LOGI(TAG, "KEY_ONCE\n");
//     return ESP_OK;
// }


