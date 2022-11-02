#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "gpio_ctrl.h"
#include "timer_app.h"
#include "para_list.h"

#define FTC533_data_1()    gpio_set_level(GPIO_IO_FTC533, 0);
#define FTC533_data_0()    gpio_set_level(GPIO_IO_FTC533, 1);
#define idle_state 0
#define start_state 1
#define key0_state 2
#define key1_state 3
#define key3_state 4
#define key4_state 5
uint8_t LED5_state = 0;

static void periodic_timer_callback(void* arg);
//static void oneshot_timer_callback(void* arg);
static void FTC533_timer_callback(void* arg);

static const char* TAG = "timer_example";



void timer_periodic(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());

    usleep(100000);

    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));

    ESP_LOGI(TAG, "Stopped and deleted timers");
}

uint8_t beep_state = 0;
static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    //ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
    beep_state = !beep_state;
    gpio_set_level(GPIO_BEEP, beep_state);  
}



void timer_FTC533(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &FTC533_timer_callback,
            //.dispatch_method = 1,
            /* name is optional, but may help identify the timer when debugging */
            .name = "timer_FTC533"
    };

    esp_timer_handle_t FTC533_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &FTC533_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(FTC533_timer, 375));
    //ESP_ERROR_CHECK(esp_timer_start_periodic(FTC533_timer, 1000));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());

    // usleep(100000);

    // ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    // ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));

    // ESP_LOGI(TAG, "Stopped and deleted timers");
}


void data_0_process(uint8_t state)
{
    if(state%2==0){
        FTC533_data_1();}
    else{
        FTC533_data_0();}
}
void data_1_process(uint8_t state)
{
    if(state%4==0){
        FTC533_data_1();}
    else{
        FTC533_data_0();}
}

static void FTC533_timer_callback(void* arg)
{
    static uint8_t cur_state = 0;
    static uint8_t next_state = 0;
    static uint8_t cnt_state = 0;
    static uint8_t cnt_data = 0;
    static uint8_t cnt_time = 0;
    uint8_t key = 0;
    cur_state = next_state;
    //printf("FTC533_timer_callback");
    //int64_t time_since_boot = esp_timer_get_time();
    //ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
    switch(cur_state)
    {
        case idle_state:FTC533_data_1();
        cnt_state++;
        if(cnt_state == 8)
        {
            next_state = start_state;
            cnt_state = 0;
            //printf("FTC533_timer_callback:idle_state");
        }
        break;
        case start_state:FTC533_data_0();
        cnt_state++;
        if(cnt_state == 2)
        {
            if(cnt_time >= 4)
            {
                cnt_time = 0;
                next_state = key0_state;
                parameter_write_FTC533(0);
            }
            //printf("FTC533_timer_callback:start_state");
            key = parameter_read_FTC533();
            if(key == 0)
                next_state = key0_state;
            else if(key == 1)
                next_state = key1_state;
            else if(key == 3)
                next_state = key3_state;
                
            cnt_state = 0;
        }            
        break;
        case key0_state:
        if(cnt_data < 8)   //0000
            data_0_process(cnt_data);        
        else if(cnt_data >= 8 && cnt_data < 24) //1111
            data_1_process(cnt_data);
        else if(cnt_data >= 24 && cnt_data < 32) //0000
            data_0_process(cnt_data);
        else if(cnt_data >= 32 && cnt_data < 48) //1111
            data_1_process(cnt_data);      
        cnt_data++; 
        if(cnt_data >= 48){
            next_state = idle_state;
            cnt_data = 0;
            //printf("FTC533_timer_callback:key0_state");
        }
        break; 
        case key1_state:
        if(cnt_data < 8)   //0000
            data_0_process(cnt_data);        
        else if(cnt_data >= 8 && cnt_data < 24) //1111
            data_1_process(cnt_data);
        else if(cnt_data >= 24 && cnt_data < 28) //1
            data_1_process(cnt_data);
        else if(cnt_data >= 28 && cnt_data < 36) //0000
            data_0_process(cnt_data);  
        else if(cnt_data >= 36 && cnt_data < 48) //111
            data_1_process(cnt_data);  
        cnt_data++;
        if(cnt_data >= 48){
            next_state = idle_state;
            cnt_data = 0;
            cnt_time++;
            // LED5_state = !LED5_state;
            // gpio_set_level(GPIO_OUTPUT_LED_5, LED5_state);  
            //printf("FTC533_timer_callback:key1_state");
        }
        break;    
        case key3_state:
        if(cnt_data < 4)   //1
            data_1_process(cnt_data);        
        else if(cnt_data >= 4 && cnt_data < 12) //0000
            data_0_process(cnt_data-2);
        else if(cnt_data >= 12 && cnt_data < 24) //111
            data_1_process(cnt_data);
        else if(cnt_data >= 24 && cnt_data < 32) //0000
            data_0_process(cnt_data-2);  
        else if(cnt_data >= 32 && cnt_data < 48) //1111
            data_1_process(cnt_data);  
        cnt_data++;
        if(cnt_data >= 48){
            next_state = idle_state;
            cnt_data = 0;
            cnt_time++;
            // LED5_state = !LED5_state;
            // gpio_set_level(GPIO_OUTPUT_LED_5, LED5_state);  
            //printf("FTC533_timer_callback:key3_state");
        }       
        break;
        case key4_state:
        if(cnt_data < 2)   //0
            data_0_process(cnt_data);        
        else if(cnt_data >= 2 && cnt_data < 6) //1
            data_1_process(cnt_data-2);
        else if(cnt_data >= 6 && cnt_data < 10) //00
            data_0_process(cnt_data);
        else if(cnt_data >= 10 && cnt_data < 14) //1
            data_1_process(cnt_data-2);  
        else if(cnt_data >= 14 && cnt_data < 16) //0
            data_0_process(cnt_data);  
        else if(cnt_data >= 16 && cnt_data < 24) //11
            data_1_process(cnt_data);  
        else if(cnt_data >= 24 && cnt_data < 32) //0000
            data_0_process(cnt_data);       
        else if(cnt_data >= 32 && cnt_data < 48) //1111
            data_1_process(cnt_data);    
        cnt_data++;
        if(cnt_data >= 48){
            next_state = idle_state;
            cnt_data = 0;
            cnt_time++;
            //printf("FTC533_timer_callback:key3_state");
        }
        break; 
        default:
        break;   
    }
}




// void timer_app(void)
// {
//     /* Create two timers:
//      * 1. a periodic timer which will run every 0.5s, and print a message
//      * 2. a one-shot timer which will fire after 5s, and re-start periodic
//      *    timer with period of 1s.
//      */

//     const esp_timer_create_args_t periodic_timer_args = {
//             .callback = &periodic_timer_callback,
//             /* name is optional, but may help identify the timer when debugging */
//             .name = "periodic"
//     };

//     esp_timer_handle_t periodic_timer;
//     ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
//     /* The timer has been created but is not running yet */

//     const esp_timer_create_args_t oneshot_timer_args = {
//             .callback = &oneshot_timer_callback,
//             /* argument specified here will be passed to timer callback function */
//             .arg = (void*) periodic_timer,
//             .name = "one-shot"
//     };
//     esp_timer_handle_t oneshot_timer;
//     ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));

//     /* Start the timers */
//     ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
//     //ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, 5000000));
//     ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());

//     /* Print debugging information about timers to console every 2 seconds */
//     // for (int i = 0; i < 5; ++i) {
//     //     ESP_ERROR_CHECK(esp_timer_dump(stdout));
//     //     usleep(2000000);
//     // }

//     /* Timekeeping continues in light sleep, and timers are scheduled
//      * correctly after light sleep.
//      */
//     // ESP_LOGI(TAG, "Entering light sleep for 0.5s, time since boot: %lld us",
//     //         esp_timer_get_time());

//     // ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(500000));
//     // esp_light_sleep_start();

//     // ESP_LOGI(TAG, "Woke up from light sleep, time since boot: %lld us",
//     //             esp_timer_get_time());

//     // /* Let the timer run for a little bit more */
//     // usleep(2000000);

//     // /* Clean up and finish the example */
//      ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
//      ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
//     // ESP_ERROR_CHECK(esp_timer_delete(oneshot_timer));
//      ESP_LOGI(TAG, "Stopped and deleted timers");
// }

// static void oneshot_timer_callback(void* arg)
// {
//     int64_t time_since_boot = esp_timer_get_time();
//     ESP_LOGI(TAG, "One-shot timer called, time since boot: %lld us", time_since_boot);
//     esp_timer_handle_t periodic_timer_handle = (esp_timer_handle_t) arg;
//     /* To start the timer which is running, need to stop it first */
//     ESP_ERROR_CHECK(esp_timer_stop(periodic_timer_handle));
//     ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_handle, 1000000));
//     time_since_boot = esp_timer_get_time();
//     ESP_LOGI(TAG, "Restarted periodic timer with 1s period, time since boot: %lld us",
//             time_since_boot);
// }