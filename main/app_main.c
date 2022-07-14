/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "para_list.h"
#include "ds18b20.h"
#include "pressure_i2c.h"
#include "wifi_sta.h"
#include "ota_app.h"
#include "mqtt_app.h"
#include "gpio_ctrl.h"
#include "led_strip.h"
#include "timer_app.h"
//#include "uart485.h"

#define BLINK_GPIO 48
#define CONFIG_BLINK_LED_RMT_CHANNEL 0
static led_strip_t *pStrip_a;
static void blink_led(uint8_t s_led_state);
static void configure_led(void);
void beep_out(void);
static const char *TAG = "led_strip";

void app_main(void)
{
//parameter_init
    para_init();
//gpio task in/out     PRIO 10 
    gpio_init();
//DS18B20 task
   xTaskCreate(ds18b20_read, "ds18b20_read", 4096, NULL, 30, NULL);
//pressure_read
    xTaskCreate(pressure_read, "pressure_read", 2048, NULL, 13, NULL);
//wifi connect STA    configMAX_PRIORITIES -5                  ( 5 )
    wifi_connect();     
//MQTT enable     MQTT task priority, default is 5,
    //mqtt_app_start();  
 //   mqtt_init();
//OTA enable
    //vTaskDelay(10000 / portTICK_RATE_MS);
    native_ota_app();
//uart read/write example without event queue;
    //xTaskCreate(uart485_task, "uart485_task", 2048, NULL, 12, NULL);
    //int cnt = 0;
    //xTaskCreate(beep_out, "beep_out", 2048, NULL, 9, NULL);
//    configure_led();
    timer_periodic();
//wifi_scan
   xTaskCreate(wifi_scan, "wifi_scan", 4096, NULL, 3, NULL);
    //uint8_t s_led_state = 0;
    while(1) {
        // printf("cnt: %d\n", cnt++);
        // ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        //blink_led(s_led_state);
        // /* Toggle the LED state */
        //s_led_state = !s_led_state;
        vTaskDelay(60000 / portTICK_RATE_MS);
        //gpio_set_level(GPIO_SYS_LED, s_led_state);
        //gpio_set_level(GPIO_BEEP, s_led_state);
        //get_conf();
        //get_rssi();
        //timer_app();
    }
}


 
static void blink_led(uint8_t s_led_state)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        pStrip_a->refresh(pStrip_a, 100);
    } else {
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}


// void beep_out(void)
// {
//     uint8_t beep_state = 0;
//     int cnt = 0;
//     uint8_t beep_enable = 1;
//     uint16_t k = 1;
//     for(;;)
//     {
//         vTaskDelay(9 / portTICK_RATE_MS);
//         if(beep_enable)
//         {
//             beep_state = !beep_state;
//             gpio_set_level(GPIO_BEEP, beep_state);    
//             //vTaskDelay(1 / portTICK_RATE_MS);  
//         }
//         if(cnt == 300*k)
//         {
//             beep_enable = 0;
//         }
//         else if(cnt == 600*k)
//         {
//             beep_enable = 1;
//         }    
//         else if(cnt == 900*k)
//         {
//             beep_enable = 0;
//         }
//         else if(cnt == 1200*k)
//         {
//             beep_enable = 1;
//         } 
//         else if(cnt == 1500*k)
//         {
//             beep_enable = 0;
//         } 
//         if(cnt<2000*k)    
//             //cnt++;
//             printf("cnt: %d\n", cnt++);   
//     }
// }

