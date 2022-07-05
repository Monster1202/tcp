#include <stdio.h>
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
#include "mqtt_app.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "para_list.h"
#include "gpio_ctrl.h"
// #define GPIO_OUTPUT_IO_STRETCH    39
// #define GPIO_OUTPUT_IO_DRAW    40
// #define GPIO_OUTPUT_IO_ROTATEX    41
// #define GPIO_OUTPUT_IO_ROTATEY    42
// #define GPIO_OUTPUT_IO_WATER    19
// #define GPIO_OUTPUT_IO_BUBBLE    20
// #define GPIO_OUTPUT_IO_STOP    21

#define MQTT_BROKER_URL "mqtt://172.16.161.171"
#define TOPIC_TIMESTAMP "/timestamp"
#define TOPIC_EMERGENCY_CONTROL "/emergency-control"
#define TOPIC_DEVICE_REGISTER "/device-register"

#define TOPIC_BRUSH_CONTROL "/pneumatic-brush-device/switch-control"
#define TOPIC_BRUSH_STATES "/pneumatic-brush-device/states"
#define TOPIC_BLISTER_CONTROL "/blister-device/switch-control"
#define TOPIC_BLISTER_STATES "/blister-device/states"
#define TOPIC_REMOTE_CONTROL "/remote-control-device/switch-control"
//#define TOPIC_BRUSH_STATES "/pneumatic-brush-device/states"
extern PARAMETER_BRUSH bursh_para;


static const char *TAG = "MQTT_EXAMPLE";


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    //char topic_sub_2[] ="/pneumatic-brush-device/switch-control"; 
    //char topic_sub_3[] ="/emergency-control"; 
    //char topic_sub_4[] ="/timestamp";
    
    //char topic_pub_1[] ="/topic/qos1";
    //char topic_pub_2[] ="/pneumatic-brush-device/states"; 
    //char topic_pub_3[] ="/device-register";
    char data_pub_1[300] = "init";
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        data_publish(data_pub_1,0);   //device_register
        msg_id = esp_mqtt_client_publish(client, TOPIC_DEVICE_REGISTER, data_pub_1, 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, TOPIC_EMERGENCY_CONTROL, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, TOPIC_TIMESTAMP, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        
    #ifdef DEVICE_TYPE_BRUSH
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_BRUSH_CONTROL, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        data_publish(data_pub_1,1); 
        msg_id = esp_mqtt_client_publish(client, TOPIC_BRUSH_STATES, data_pub_1, 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
    #else
        #ifdef DEVICE_TYPE_BLISTER
            msg_id = esp_mqtt_client_subscribe(client, TOPIC_BLISTER_CONTROL, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            data_publish(data_pub_1,1); 
            msg_id = esp_mqtt_client_publish(client, TOPIC_BLISTER_STATES, data_pub_1, 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        #else
            msg_id = esp_mqtt_client_subscribe(client, TOPIC_BRUSH_STATES, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_subscribe(client, TOPIC_BLISTER_STATES, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        #endif
    #endif
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
        msg_id = esp_mqtt_client_publish(client, TOPIC_BRUSH_STATES, data_pub_1, 0, 1, 0);
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
        .uri = MQTT_BROKER_URL,//CONFIG_BROKER_URL,
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


void mqtt_init(void)
{
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

    mqtt_app_start();
}

void data_process(char *data)
{
    cJSON *json_str_xy = cJSON_Parse(data);
    if(json_str_xy == NULL) {
        cJSON_Delete(json_str_xy);
        return 0;
    }
#ifdef DEVICE_TYPE_BRUSH
    cJSON *json_emergency_stop = cJSON_GetObjectItem(json_str_xy, "emergency_stop");
    if(json_emergency_stop != NULL && json_emergency_stop->type == cJSON_Number) {
        //bursh_para.emergency_stop = json_emergency_stop->valueint;
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
                    printf("bursh_para.centralizer = 2\n");}
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
#else
    #ifdef DEVICE_TYPE_BLISTER   
    cJSON *json_emergency_stop = cJSON_GetObjectItem(json_str_xy, "emergency_stop");
    if(json_emergency_stop != NULL && json_emergency_stop->type == cJSON_Number) {
        printf("emergency_stop = %d\n", json_emergency_stop->valueint);
        if(json_emergency_stop->valueint){
            //gpio_set_level(GPIO_OUTPUT_IO_STOP, 1);
            blister_para.emergency_stop = 1;
            printf("blister_para.emergency_stop = 1\n");
            gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
            gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
            blister_para.mode = 0;
            printf("blister_para.mode = 0\n");
            gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
            gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
            blister_para.centralizer = 0;
            printf("blister_para.centralizer = 0\n");}       
        else{
            //gpio_set_level(GPIO_OUTPUT_IO_STOP, 0);
            blister_para.emergency_stop = 0;
            printf("blister_para.emergency_stop = 0\n");}
    }
    //#else
    #endif
#endif 
    //cJSON_Delete(json_str_xy);
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


// void cJSON_init(void)
// {
//     cJSON*root = cJSON_CreateObject();
//     cJSON *item = cJSON_CreateObject();
//     //CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
//     cJSON_AddNumberToObject(root, "nozzle",1);
//     cJSON_AddNumberToObject(root, "status",1);
//     char *msg = cJSON_Print(root);
//     printf("%s\n",msg);

//     cJSON_Delete(root);

//     return 0;
// }