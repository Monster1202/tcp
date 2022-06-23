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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "para_list.h"
#include "ds18b20.h"
#include "uart485.h"
//#include "pid_ctrl.h"

//gpio
#include "driver/gpio.h"
#include "led_strip.h"
#define BLINK_GPIO 48
#define CONFIG_BLINK_LED_RMT_CHANNEL 0
//static uint8_t s_led_state = 0;
static led_strip_t *pStrip_a;
#define GPIO_OUTPUT_IO_STRETCH    39
#define GPIO_OUTPUT_IO_DRAW    40
#define GPIO_OUTPUT_IO_ROTATEX    41
#define GPIO_OUTPUT_IO_ROTATEY    42
#define GPIO_OUTPUT_IO_WATER    19
#define GPIO_OUTPUT_IO_BUBBLE    20
#define GPIO_OUTPUT_IO_STOP    21
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_STRETCH) | (1ULL<<GPIO_OUTPUT_IO_DRAW)| (1ULL<<GPIO_OUTPUT_IO_ROTATEX)| (1ULL<<GPIO_OUTPUT_IO_ROTATEY)| (1ULL<<GPIO_OUTPUT_IO_WATER)| (1ULL<<GPIO_OUTPUT_IO_BUBBLE)| (1ULL<<GPIO_OUTPUT_IO_STOP))
// #define GPIO_OUTPUT_IO_0 17
// #define GPIO_OUTPUT_IO_1 18
// #define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0     6
#define GPIO_INPUT_IO_1     7
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0
// #define GPIO_IO_DS18B20    9
// #define GPIO_INOUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)

#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
//#define KEY_PRESS gpio_get_level(GPIO_INPUT_IO_0)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3


extern PARAMETER_BRUSH bursh_para;

void sw_key_read(uint8_t io_num);
uint8_t KEY_READ(uint8_t io_num);
void data_process(char *data);
void data_publish(char *data,uint8_t case_pub);
esp_err_t get_chip_id(uint32_t* chip_id);
void para_init(void);
void gpio_init(void);
static void blink_led(uint8_t s_led_state);
static void configure_led(void);
static const char *TAG = "MQTT_EXAMPLE";
void ds18b20_read(void* arg);

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    //char topic_sub_1[] ="/topic/qos0";
    char topic_sub_2[] ="/pneumatic-brush-device/switch-control"; 
    char topic_sub_3[] ="/emergency-control"; 
    char topic_sub_4[] ="/timestamp";
    
    //char topic_pub_1[] ="/topic/qos1";
    char topic_pub_2[] ="/pneumatic-brush-device/states"; 
    char topic_pub_3[] ="/device-register";
    char data_pub_1[300] = "init";
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        data_publish(data_pub_1,0);   //data
        msg_id = esp_mqtt_client_publish(client, topic_pub_3, data_pub_1, 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, topic_sub_2, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, topic_sub_3, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, topic_sub_4, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        // msg_id = esp_mqtt_client_subscribe(client, topic_pub_1, 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        // msg_id = esp_mqtt_client_unsubscribe(client, topic_pub_1);
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, topic_sub_1, "SUBSCRIBED", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        data_process(event->data);
        data_publish(data_pub_1,1); 
        msg_id = esp_mqtt_client_publish(client, topic_pub_2, data_pub_1, 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}


void app_main(void)
{
    para_init();
    gpio_init();
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

//DS18B20 task
    xTaskCreate(ds18b20_read, "ds18b20_read", 2048, NULL, 11, NULL);
//uart read/write example without event queue;
    xTaskCreate(uart485_task, "uart_echo_task", 2048, NULL, 12, NULL);
//wifi connect
    ESP_ERROR_CHECK(example_connect());   
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //   ESP_ERROR_CHECK(nvs_flash_erase());
    //   ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);
    // ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");7
//MQTT enable    
    mqtt_app_start();
    //test_app();
    //cJSON_init();   //test cjson
    //get_conf();
    // int cnt = 0;
    uint8_t s_led_state = 0;
    while(1) {
        // printf("cnt: %d\n", cnt++);
        // ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
         blink_led(s_led_state);
        // /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(2000 / portTICK_RATE_MS);
        //bursh_para.temperature = ReadTemperature();
        // gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        // gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
        //get_conf();
    }
}


void data_process(char *data)
{
    cJSON *json_str_xy = cJSON_Parse(data);
    if(json_str_xy == NULL) {
        cJSON_Delete(json_str_xy);
        return 0;
    }
    cJSON *json_emergency_stop = cJSON_GetObjectItem(json_str_xy, "emergency_stop");
    if(json_emergency_stop != NULL && json_emergency_stop->type == cJSON_Number) {
        bursh_para.timestamp = json_emergency_stop->valueint;
        printf("emergency_stop = %d\n", json_emergency_stop->valueint);
        if(json_emergency_stop->valueint){
            //gpio_set_level(GPIO_OUTPUT_IO_STOP, 1);
            bursh_para.emergency_stop = 1;
            printf("bursh_para.emergency_stop = 1\n");
            gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
            gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
            bursh_para.nozzle = 0;
            printf("bursh_para.nozzle = 0\n");
            gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
            gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
            bursh_para.centralizer = 0;
            printf("bursh_para.centralizer = 0\n");
            gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
            gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
            bursh_para.rotation = 0;
            printf("bursh_para.rotation = 0\n");}            
        else{
            //gpio_set_level(GPIO_OUTPUT_IO_STOP, 0);
            bursh_para.emergency_stop = 0;
            printf("bursh_para.emergency_stop = 0\n");}
    }
    cJSON *json_switch_name = cJSON_GetObjectItem(json_str_xy, "switch_name");
    if(json_switch_name != NULL && json_switch_name->type == cJSON_String) {
        printf("switch_name = %s\n", json_switch_name->valuestring);
        cJSON *json_value = cJSON_GetObjectItem(json_str_xy, "value");
        if(json_value != NULL && json_value->type == cJSON_Number) {
            printf("value = %d\n", json_value->valueint);
            if(strcmp(json_switch_name->valuestring,"nozzle")==0){
                if(json_value->valueint == 1){
                    gpio_set_level(GPIO_OUTPUT_IO_WATER, 1);
                    gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
                    bursh_para.nozzle = 1;
                    printf("bursh_para.nozzle = 1\n");}
                else if(json_value->valueint == 2){
                    gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
                    bursh_para.nozzle = 2;
                    printf("bursh_para.nozzle = 2\n");}
                else{
                    gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
                    bursh_para.nozzle = 0;
                    printf("bursh_para.nozzle = 0\n");}
            }
            else if(strcmp(json_switch_name->valuestring,"centralizer")==0){
                if(json_value->valueint == 1){
                    gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 1);
                    gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
                    bursh_para.centralizer = 1;
                    printf("bursh_para.centralizer = 1\n");}
                else if(json_value->valueint == 2){
                    gpio_set_level(GPIO_OUTPUT_IO_DRAW, 1);
                    gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
                    bursh_para.centralizer = 2;
                    printf("bursh_para.centralizer = 0\n");}
                else{
                    gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
                    bursh_para.centralizer = 0;
                    printf("bursh_para.centralizer = 0\n");}
            }
            else if(strcmp(json_switch_name->valuestring,"rotation")==0){
                if(json_value->valueint==1){
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 1);
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
                    bursh_para.rotation = 1;
                    printf("bursh_para.rotation = 1\n");}
                else if(json_value->valueint==2){
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 1);
                    bursh_para.rotation = 2;
                    printf("bursh_para.rotation = 2\n");}
                else{
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
                    bursh_para.rotation = 0;
                    printf("bursh_para.rotation = 0\n");}    
            }
        }
    }
    
    cJSON *json_timestamp = cJSON_GetObjectItem(json_str_xy, "timestamp");
    if(json_timestamp != NULL && json_timestamp->type == cJSON_Number) {
        bursh_para.timestamp = json_timestamp->valuedouble;
        printf("timestamp = %f\n", json_timestamp->valuedouble);
    }
    cJSON *json_msg_id = cJSON_GetObjectItem(json_str_xy, "msg_id");
    if(json_msg_id != NULL && json_msg_id->type == cJSON_String) {
        strcpy(bursh_para.msg_id,json_msg_id->valuestring);
        printf("msg_id = %s\n", json_msg_id->valuestring);
    }
    //cJSON_Delete(json_str_xy);
}

int Compare_double(const void* a, const void* b)
{
	double arg1 = *(const double*)a;
	double arg2 = *(const double*)b;
	double arg3 = arg1 - arg2;
	double eps = 1e-6;
	if (-eps <= arg3 && arg3 <= eps)
	{
		return 0;
	}
	if (eps <= arg3 )
	{
		return 1;
	}
	if ( arg3 <= -eps)
	{
		return -1;
	}
    return 0;
}

void ds18b20_read(void* arg)
{
    double temp[5]={0};
    //int temp_int[5]={0};
    double temp_mid = 0;
    for(;;)
    {
        vTaskDelay(2000 / portTICK_RATE_MS);
        temp[0]=ReadTemperature();
        vTaskDelay(2000 / portTICK_RATE_MS);
        temp[1]=ReadTemperature();
        vTaskDelay(2000 / portTICK_RATE_MS);
        temp[2]=ReadTemperature();
        vTaskDelay(2000 / portTICK_RATE_MS);
        temp[3]=ReadTemperature();
        vTaskDelay(2000 / portTICK_RATE_MS);
        temp[4]=ReadTemperature();
        qsort(temp, 5, sizeof(temp[0]), Compare_double); 
        temp_mid = temp[2];
        bursh_para.temperature = temp_mid;//ReadTemperature();
        printf("qsort:%f,%f,%f,%f,%f;temp_mid:%f\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp_mid);

    }
}


void data_publish(char *data,uint8_t case_pub)
{
    cJSON*root = cJSON_CreateObject();
    if(case_pub){
        cJSON_AddNumberToObject(root, "status",bursh_para.status);
        cJSON_AddNumberToObject(root, "water",bursh_para.water);
        cJSON_AddNumberToObject(root, "pressure_alarm",bursh_para.pressure_alarm);
        cJSON_AddNumberToObject(root, "nozzle",bursh_para.nozzle);
        cJSON_AddNumberToObject(root, "centralizer",bursh_para.centralizer);
        cJSON_AddNumberToObject(root, "rotation",bursh_para.rotation);
        cJSON_AddNumberToObject(root, "emergency_stop",bursh_para.emergency_stop);  
        cJSON_AddNumberToObject(root, "temperature",bursh_para.temperature);           
        cJSON_AddNumberToObject(root, "timestamp",bursh_para.timestamp);
        cJSON_AddItemToObject(root, "msg_id",cJSON_CreateString(bursh_para.msg_id)); //
        }
    else{
        cJSON_AddNumberToObject(root, "device_sn",bursh_para.uuid);
        cJSON_AddNumberToObject(root, "timestamp",bursh_para.timestamp);
        cJSON_AddItemToObject(root, "device_type",cJSON_CreateString("PNEUMATIC_BRUSH"));
        }

    char *msg = cJSON_Print(root);
    printf("%s\n",msg); 
    strcpy(data,msg);
    cJSON_Delete(root);
}

esp_err_t get_chip_id(uint32_t* chip_id){
    esp_err_t status = ESP_OK;
    *chip_id = (REG_READ(0x3FF00050) & 0xFF000000) |
                         (REG_READ(0x3ff0005C) & 0xFFFFFF);
    return status;
}


void para_init(void)
{
    uint32_t id;
    get_chip_id(&id);
    printf("SDK version:%s,chip id:%u\n", esp_get_idf_version(),id);
    bursh_para.uuid = id;
    bursh_para.nozzle = 0;
    bursh_para.centralizer = 0;
    bursh_para.rotation = 0;
    bursh_para.status = 1;
    bursh_para.water = 0;
    bursh_para.pressure_alarm = 0;
    bursh_para.emergency_stop = 0;
    bursh_para.timestamp = 1654585625000;
    strcpy(bursh_para.msg_id,"msg_id");
    bursh_para.temperature = 0;
    bursh_para.counter_1s = 0;
}

void cJSON_init(void)
{
    cJSON*root = cJSON_CreateObject();
    cJSON *item = cJSON_CreateObject();
    //CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
    cJSON_AddNumberToObject(root, "nozzle",1);
    cJSON_AddNumberToObject(root, "status",1);
    char *msg = cJSON_Print(root);
    printf("%s\n",msg);

    cJSON_Delete(root);

    return 0;
}
//gpio
static xQueueHandle gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
            sw_key_read(io_num); //judge button press once twice or long
        }
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

uint8_t KEY_READ(uint8_t io_num)
{
    uint16_t b,c;
    if(!gpio_get_level(io_num))  //button press  gpio_get_level(io_num)
    {
        c = 0;
        vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);  //delay debounce
        if(!gpio_get_level(io_num))  //check again
        {
            while((!gpio_get_level(io_num)) && c<KEY_SPEED_LONG) //long press time counting
            {
                c++;
                vTaskDelay(10 / portTICK_RATE_MS);//delay_us(10000); //10ms
            }
            if(c>=KEY_SPEED_LONG)
            {
                while(!gpio_get_level(io_num))
                    return KEY_LONG;                   
            }
            else{   
                for(b=0;b<KEY_SPEED_DOUBLE;b++)  //double press check
                {
                    vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);
                    if(!gpio_get_level(io_num))
                    {
                        while(!gpio_get_level(io_num))
                            return KEY_TWICE;
                    }
                }
            }
            //printf("CCCCCCC:\n",%d);
            return KEY_ONCE; //single press
        }
    }
    return 0; //no press
}

void sw_key_read(uint8_t io_num)
{
    uint8_t key_status = 0;
    key_status=KEY_READ(io_num);
    switch(key_status)
    {
        case KEY_ONCE:
        printf("KEY_ONCE\n");//LED0=0;
        break;
        case KEY_TWICE:
        printf("KEY_TWICE\n");//LED0=0;
        break;
        case KEY_LONG:
        printf("KEY_LONG\n");//LED0=0;
        break;
        default:
        //printf("KEY_default\n");
        break;
    }
    vTaskDelay(10 / portTICK_RATE_MS);
    //delay_us(10000);
}
void gpio_init(void)
{
    //GPIO
    configure_led();
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
        //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    //remove isr handler for gpio number.
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    //GPIO_IO_DS18B20    GPIO_INOUT_PIN_SEL
    // io_conf.intr_type = GPIO_INTR_DISABLE;
    // io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    // io_conf.pin_bit_mask = GPIO_INOUT_PIN_SEL;
    // io_conf.pull_down_en = 0;
    // io_conf.pull_up_en = 0;
    // gpio_config(&io_conf);
}

void mqtt_active_pub(void)
{
        // int msg_id;
    // char topic_pub_2[] ="/pneumatic-brush-device/states"; 
    // char data_pub_1[300] = "init";

            // bursh_para.counter_1s++;
        // if(bursh_para.counter_1s%10==1){
        //     data_publish(data_pub_1,1); 
        //     msg_id = esp_mqtt_client_publish(client, topic_pub_2, data_pub_1, 0, 1, 0);
        //     ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        // }
}
void test(void)
{
        // printf("ds18b20_init\n");
    //ds18b20_init(TEMP_BUS);
    // printf("ds18b20_init1\n");
	// getTempAddresses(tempSensors);
    // printf("ds18b20_init2\n");
	// ds18b20_setResolution(tempSensors,2,10);

	// printf("Address 0: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", tempSensors[0][0],tempSensors[0][1],tempSensors[0][2],tempSensors[0][3],tempSensors[0][4],tempSensors[0][5],tempSensors[0][6],tempSensors[0][7]);
	// printf("Address 1: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", tempSensors[1][0],tempSensors[1][1],tempSensors[1][2],tempSensors[1][3],tempSensors[1][4],tempSensors[1][5],tempSensors[1][6],tempSensors[1][7]);
    
            // ds18b20_requestTemperatures();
		// float temp1 = ds18b20_getTempF((DeviceAddress *)tempSensors[0]);
		// float temp2 = ds18b20_getTempF((DeviceAddress *)tempSensors[1]);
		// float temp3 = ds18b20_getTempC((DeviceAddress *)tempSensors[0]);
		// float temp4 = ds18b20_getTempC((DeviceAddress *)tempSensors[1]);
		// printf("Temperatures: %0.1fF %0.1fF\n", temp1,temp2);
		// printf("Temperatures: %0.1fC %0.1fC\n", temp3,temp4);

		// float cTemp = ds18b20_get_temp();
		// printf("Temperature: %0.1fC\n", cTemp);
        
        // DS18B20_Start();    
}

// #define TEMP_BUS 14
// #define LED 2
// #define HIGH 1
// #define LOW 0
// #define digitalWrite gpio_set_level

// DeviceAddress tempSensors[2];

// void getTempAddresses(DeviceAddress *tempSensorAddresses) {
// 	unsigned int numberFound = 0;
// 	reset_search();
// 	// search for 2 addresses on the oneWire protocol
// 	while (search(tempSensorAddresses[numberFound],true)) {
// 		numberFound++;
// 		if (numberFound == 2) break;
// 	}
    
// 	// if 2 addresses aren't found then flash the LED rapidly
// 	while (numberFound != 2) {
// 		numberFound = 0;
// 		blink_led(1);//digitalWrite(LED, HIGH);
// 		vTaskDelay(100 / portTICK_PERIOD_MS);
// 		blink_led(0);//digitalWrite(LED, LOW);
// 		vTaskDelay(100 / portTICK_PERIOD_MS);
// 		// search in the loop for the temp sensors as they may hook them up
// 		reset_search();
//         //printf("getTempAddresses\n");
// 		while (search(tempSensorAddresses[numberFound],true)) {
// 			numberFound++;
// 			if (numberFound == 2) break;
// 		}
// 	}
//     printf("getTempAddresses2\n");
// 	return;
// }