#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "gpio_ctrl.h"
#include "timer_app.h"
#include "mqtt_app.h"
#include "uart485.h"
#include "wifi_sta.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3

uint8_t flag_mqtt_test = 0;
static const char *TAG = "GPIO_CTRL";
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
    uint8_t buf_state = 0;
    int cnt = 0;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            buf_state = gpio_get_level(io_num);
            ESP_LOGI(TAG,"GPIO[%d] intr, val: %d", io_num, buf_state);
            //if(buf_state ==1)
            sw_key_read(io_num,buf_state); //judge button press once twice or long
        }
    }
}


#ifdef DEVICE_TYPE_BRUSH
void centralizer_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 1);
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        #ifdef GPIOTEST   
        gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        #endif
        parameter_write_centralizer(1);
        ESP_LOGI(TAG, "brush_para.centralizer = 1");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 1);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 1);
        #endif
        parameter_write_centralizer(2);
        ESP_LOGI(TAG, "brush_para.centralizer = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        #endif
        parameter_write_centralizer(0);
        ESP_LOGI(TAG, "brush_para.centralizer = 0");}
}
void rotation_io_out(uint8_t value)
{
    if(value==1){
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 1);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        #endif
        parameter_write_rotation(1);
        ESP_LOGI(TAG, "brush_para.rotation = 1");}
    else if(value==2){
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 1);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 1);
        #endif
        parameter_write_rotation(2);
        ESP_LOGI(TAG, "brush_para.rotation = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        #endif
        parameter_write_rotation(0);
        ESP_LOGI(TAG, "brush_para.rotation = 0");} 
}
void nozzle_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 1);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        #endif
        parameter_write_nozzle(1);
        ESP_LOGI(TAG, "brush_para.nozzle = 1");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 1);
        #endif
        parameter_write_nozzle(2);
        ESP_LOGI(TAG, "brush_para.nozzle = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        #endif
        parameter_write_nozzle(0);
        ESP_LOGI(TAG, "brush_para.nozzle = 0");}
}

void brush_stop_io_out(uint8_t value,uint8_t state) //state 0 :from mqtt don't change the parameter 
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
    #ifdef GPIOTEST
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
    #endif  
        if(state){
            parameter_write_emergency_stop(1);
            gpio_set_level(GPIO_OUTPUT_LED_1, 1);    
            gpio_set_level(GPIO_SYS_LED, 1);
            ESP_LOGI(TAG, "brush_para.emergency_stop = 1");
            }
        parameter_write_centralizer(0);
        parameter_write_rotation(0);
        parameter_write_nozzle(0);
        }
    else{
        if(state){
            parameter_write_emergency_stop(0); 
            gpio_set_level(GPIO_SYS_LED, 0);
            ESP_LOGI(TAG, "brush_para.emergency_stop = 0");
            #ifndef GPIOTEST
            gpio_set_level(GPIO_OUTPUT_LED_1, 0);
            #endif 
            }
        }
}

uint8_t brush_input(uint8_t io_num,uint8_t state)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    // uint8_t register_emergency_stop = 0;
    // register_emergency_stop = parameter_read_emergency_stop();
    if(state == 0 && io_num == GPIO_INPUT_IO_6)  //pressure 0/1 input
    {
        parameter_write_water(0);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_6:0");
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_6)
    {
        parameter_write_water(1);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_6:1");
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
    }
    else if(state == 0 && io_num == GPIO_INPUT_IO_7)  //pressure 0/1 input
    {
        parameter_write_pressure_alarm(1);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_7:0");
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_7)
    {
        parameter_write_pressure_alarm(0);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_7:1");
        gpio_set_level(GPIO_OUTPUT_LED_6, 1);
    }
    else if(io_num == GPIO_INPUT_IO_STOP)
    {
        ESP_LOGI(TAG, "GPIO_INPUT_IO_STOP");
        register_value = parameter_read_emergency_stop();
        register_afterpress = UI_press_output(register_value,1);
        brush_stop_io_out(register_afterpress,1);       
    }
    else
    {
        return 0;
    }
    device_states_publish(0);
    return 1;
}
void brush_press_output(uint8_t io_num)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    uint8_t register_emergency_stop = 0;
    register_emergency_stop = parameter_read_emergency_stop();
    if(register_emergency_stop)
    {
        if(io_num == GPIO_INPUT_IO_STOP)
        {
            ESP_LOGI(TAG, "back to normal mode");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            brush_stop_io_out(register_afterpress,1);
        }
        else{
            ESP_LOGI(TAG, "emergency_stop_error_press");
            timer_periodic();
        }
    }
    else
    {
        switch(io_num)  //BRUSH
        {
            case GPIO_INPUT_IO_1:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,1);
            centralizer_io_out(register_afterpress);            
            break;
            case GPIO_INPUT_IO_2:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,2);
            centralizer_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_3:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,1);
            rotation_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_4:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_4");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,2);
            rotation_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_5:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_5");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,1);
            nozzle_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_6:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_6");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,2);
            nozzle_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_7:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_7");
            break;
            case GPIO_INPUT_IO_STOP:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_STOP");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            brush_stop_io_out(register_afterpress,1);
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
    }
    device_states_publish(0);
}
#endif


uint8_t UI_press_output(uint8_t value,uint8_t button)
{
    if(value==button)
        value = 0;
    else
        value = button;
    return value;
}

#ifdef DEVICE_TYPE_BLISTER

void blister_stop_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_HEATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);

        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);

        parameter_write_emergency_stop(1);
        gpio_set_level(GPIO_SYS_LED, 1);
        parameter_write_heater(0);
        parameter_write_mode(0);
        ESP_LOGI(TAG, "blister_para.emergency_stop = 1");}
    else{
        parameter_write_emergency_stop(0);
        gpio_set_level(GPIO_SYS_LED, 0);
        ESP_LOGI(TAG, "blister_para.emergency_stop = 0");}
}
void heater_io_out(uint8_t value)
{
    if(value == 1){
        heater_water_module_test(6);
        gpio_set_level(GPIO_OUTPUT_IO_HEATER, 1);
        gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        parameter_write_heater(1);
        ESP_LOGI(TAG, "blister.heater = 1");}
    else{
        heater_water_module_test(7);
        gpio_set_level(GPIO_OUTPUT_IO_HEATER, 0);
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        parameter_write_heater(0);
        ESP_LOGI(TAG, "blister.heater = 0");}
}
void blister_mode_io_out(uint8_t value)
{
    if(value == 1){
        heater_water_module_test(4);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 1);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 1);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        parameter_write_mode(1);
        ESP_LOGI(TAG, "blister_para.mode = 1");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        parameter_write_mode(2);
        ESP_LOGI(TAG, "blister_para.mode = 2");}
    else{
        heater_water_module_test(5);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        parameter_write_mode(0);
        ESP_LOGI(TAG, "blister_para.mode = 0");}
}
static uint8_t quantity = 0;
void blister_quantity_io_out(uint8_t value)
{
    if(value == 1){
        heater_water_module_test(1);
        // gpio_set_level(GPIO_OUTPUT_IO_WATER, 1);
        // gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 1);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        //parameter_write_mode(1);
        quantity = 1;
        ESP_LOGI(TAG, "blister_para.mode = 1");}
    else if(value == 2){
        heater_water_module_test(2);
        // gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        // gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        //parameter_write_mode(2);
        quantity = 2;
        ESP_LOGI(TAG, "blister_para.mode = 2");}
    else{
        heater_water_module_test(3);
        // gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        // gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        //parameter_write_mode(0);
        quantity = 3;
        ESP_LOGI(TAG, "blister_para.mode = 0");}
}
void blister_press_output(uint8_t io_num)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    uint8_t register_emergency_stop = 0;
    register_emergency_stop = parameter_read_emergency_stop();
    
    if(register_emergency_stop)
    {
        // if(io_num == GPIO_INPUT_IO_STOP)
        // {
        //     ESP_LOGI(TAG, "back to normal mode");
        //     register_value = parameter_read_emergency_stop();
        //     register_afterpress = UI_press_output(register_value,1);
        //     blister_stop_io_out(register_afterpress);
        // }
        // else{
        ESP_LOGI(TAG, "emergency_stop_error_press");
        timer_periodic();
        //}
    }
    else
    {
        switch(io_num)   //BLISTER
        {
            case GPIO_INPUT_IO_1:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
            register_value = parameter_read_heater();
            register_afterpress = UI_press_output(register_value,1);
            heater_io_out(register_afterpress);            
            break;
            case GPIO_INPUT_IO_2:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
            register_value = parameter_read_mode();
            register_afterpress = UI_press_output(register_value,1);
            blister_mode_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_3:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
            register_value = parameter_read_mode();
            register_afterpress = UI_press_output(register_value,2);
            blister_mode_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_4:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_4");
            register_afterpress = UI_press_output(quantity,1);
            blister_quantity_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_5:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_5");
            register_afterpress = UI_press_output(quantity,2);
            blister_quantity_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_6:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_6");
            parameter_write_FTC533(1);
            break;
            case GPIO_INPUT_IO_7:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_7");
            parameter_write_FTC533(3);
            break;
            case GPIO_INPUT_IO_STOP:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_STOP");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            blister_stop_io_out(register_afterpress);
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
        if(io_num == GPIO_INPUT_IO_1 || io_num == GPIO_INPUT_IO_2 || io_num == GPIO_INPUT_IO_3 )
        {
            device_states_publish(0);
        }
    }
    //device_states_publish(0);
}

uint8_t blister_input(uint8_t io_num,uint8_t state)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    if(state == 0 && io_num == GPIO_INPUT_IO_6)  //pressure 0/1 input
    {
        parameter_write_water(0);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_6:0");
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_IO_PUMP, 1);       
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_6)
    {
        parameter_write_water(1);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_6:1");
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
    }
    else if(state == 0 && io_num == GPIO_INPUT_IO_7)  //pressure 0/1 input
    {
        parameter_write_pressure_alarm(0);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_7:0");
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        gpio_set_level(GPIO_OUTPUT_IO_PUMP, 0); 
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_7)
    {
        parameter_write_pressure_alarm(1);
        ESP_LOGI(TAG, "GPIO_INPUT_IO_7:1");
        gpio_set_level(GPIO_OUTPUT_LED_6, 1); 
    }
    else
    {
        return 0;
    }
    device_states_publish(0);
    return 1;
}

#endif

#ifdef DEVICE_TYPE_REMOTE
void centralizer_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        parameter_write_centralizer(1);
        ESP_LOGI(TAG, "remote_para.centralizer = 1");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 1);
        parameter_write_centralizer(2);
        ESP_LOGI(TAG, "remote_para.centralizer = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        parameter_write_centralizer(0);
        ESP_LOGI(TAG, "remote_para.centralizer = 0");}
}
void rotation_io_out(uint8_t value)
{
    if(value==1){
        gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        parameter_write_rotation(1);
        ESP_LOGI(TAG, "remote_para.rotation = 1");}
    else if(value==2){
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 1);
        parameter_write_rotation(2);
        ESP_LOGI(TAG, "remote_para.rotation = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        parameter_write_rotation(0);
        ESP_LOGI(TAG, "remote_para.rotation = 0");} 
}
void nozzle_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_nozzle(1);
        ESP_LOGI(TAG, "remote_para.nozzle = 1");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 1);
        parameter_write_nozzle(2);
        ESP_LOGI(TAG, "remote_para.nozzle = 2");}
    else{
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_nozzle(0);
        ESP_LOGI(TAG, "remote_para.nozzle = 0");}
}

void remote_stop_io_out(uint8_t value , uint8_t state) //state 0 from press dom't change parameter
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_heater(0);
        parameter_write_mode(0);
        parameter_write_centralizer(0);
        parameter_write_rotation(0);
        parameter_write_nozzle(0);
        if(state){
            parameter_write_emergency_stop(1);
            gpio_set_level(GPIO_SYS_LED, 1);
            ESP_LOGI(TAG, "remote_para.emergency_stop = 1");}}
    else{
        if(state){
            parameter_write_emergency_stop(0);
            gpio_set_level(GPIO_SYS_LED, 0);
            ESP_LOGI(TAG, "remote_para.emergency_stop = 0");}}
}

void remote_press_output(uint8_t io_num)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    uint8_t register_emergency_stop = 0;
    register_emergency_stop = parameter_read_emergency_stop();
    
    if(register_emergency_stop)
    {
        // if(io_num == GPIO_INPUT_IO_STOP)
        // {
        //     ESP_LOGI(TAG, "back to normal mode");
        //     register_value = parameter_read_emergency_stop();
        //     register_afterpress = UI_press_output(register_value,1);
        //     remote_stop_io_out(register_afterpress,0);
        //     device_states_publish(4);
        // }
        // else{
        ESP_LOGI(TAG, "emergency_stop_error_press");
        timer_periodic();
        //}
    }
    else
    {
        switch(io_num)   //BLISTER
        {
            case GPIO_INPUT_IO_1:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,1);
            centralizer_io_out(register_afterpress);
            device_states_publish(1);            
            break;
            case GPIO_INPUT_IO_2:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,2);
            centralizer_io_out(register_afterpress);
            device_states_publish(1); 
            break;
            case GPIO_INPUT_IO_3:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,1);
            rotation_io_out(register_afterpress);
            device_states_publish(2); 
            break;
            case GPIO_INPUT_IO_4:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_4");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,2);
            rotation_io_out(register_afterpress);
            device_states_publish(2); 
            break;
            case GPIO_INPUT_IO_5:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_5");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,1);
            nozzle_io_out(register_afterpress);
            device_states_publish(3); 
            break;
            case GPIO_INPUT_IO_6:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_6");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,2);
            nozzle_io_out(register_afterpress);
            device_states_publish(3); 
            break;
            case GPIO_INPUT_IO_7:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_7");
            break;
            case GPIO_INPUT_IO_STOP:
            ESP_LOGI(TAG, "GPIO_INPUT_IO_STOP");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            remote_stop_io_out(register_afterpress,0);
            device_states_publish(4);
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
    }
}
#endif

#ifdef mqtt_test
void mqtt_gpio_test(void* arg)
{
    int cnt = 0;
    for(;;) {
        if(flag_mqtt_test)
        {
            //device_states_publish(cnt%4+1);  
            remote_press_output(cnt%6+35);  
            ESP_LOGI(TAG,"mqtt_gpio_test_cnt: %d\n", cnt++);
            //printf("mqtt_gpio_test_cnt: %d\n", cnt++);
        }
        
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
#endif

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
            //ESP_LOGI(TAG, "CCCCCCC:",%d);
            return KEY_ONCE; //single press
        }
    }
    return 0; //no press
}
void sw_key_read(uint8_t io_num,uint8_t state)
{
    uint8_t key_status = 0;

    #ifdef GPIOWORKING
    #ifdef DEVICE_TYPE_BRUSH
        brush_input(io_num,state);
    #endif
    #endif
    // #ifdef DEVICE_TYPE_BLISTER
    //     blister_input(io_num,state);
    // #endif
    if(state == 1)
    {
    #ifdef GPIOTEST
    brush_press_output(io_num);
    #endif
    #ifdef DEVICE_TYPE_BLISTER
    blister_press_output(io_num);
    #endif
    #ifdef DEVICE_TYPE_REMOTE
    remote_press_output(io_num);
    #endif
    }
    #ifdef DEVICE_TYPE_REMOTE
    if(io_num == GPIO_INPUT_IO_STOP)
    {
        key_status=KEY_READ(io_num);
        //ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
        switch(key_status)
        {
            case KEY_ONCE:
            ESP_LOGI(TAG, "KEY_ONCE");
            break;
            case KEY_TWICE:
            ESP_LOGI(TAG, "KEY_TWICE");
            wifi_url_inital_set_para();
            vTaskDelay(1000 / portTICK_RATE_MS);
            wifi_reset();
            vTaskDelay(1000 / portTICK_RATE_MS);
            mqtt_reset();
            break;
            case KEY_LONG:
            ESP_LOGI(TAG, "KEY_LONG");
            #ifdef mqtt_test
                register_afterpress = UI_press_output(flag_mqtt_test,1);
                flag_mqtt_test = register_afterpress;
            #endif 
            // wifi1_url_inital_set_para();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // wifi_reset();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // mqtt_reset();
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
    }
    #endif
    #ifdef DEVICE_TYPE_BRUSH
    if(io_num == GPIO_INPUT_IO_1)
    {
        key_status=KEY_READ(io_num);
        //ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
        switch(key_status)
        {
            case KEY_ONCE:
            ESP_LOGI(TAG, "KEY_ONCE");
            break;
            case KEY_TWICE:
            ESP_LOGI(TAG, "KEY_TWICE");
            wifi_url_inital_set_para();
            vTaskDelay(1000 / portTICK_RATE_MS);
            wifi_reset();
            vTaskDelay(1000 / portTICK_RATE_MS);
            mqtt_reset();
            break;
            case KEY_LONG:
            ESP_LOGI(TAG, "KEY_LONG");
            // wifi1_url_inital_set_para();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // wifi_reset();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // mqtt_reset();
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
    }
    #endif
    vTaskDelay(10 / portTICK_RATE_MS);
}
void gpio_init(void)
{
    //GPIO
    //esp_log_level_set("GPIO_CTRL", ESP_LOG_DEBUG);  //ESP_LOG_DEBUG ESP_LOG_INFO ESP_LOG_WARN
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
    io_conf.intr_type = GPIO_INTR_ANYEDGE;//GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_STOP, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 4096, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_STOP, gpio_isr_handler, (void*) GPIO_INPUT_IO_STOP);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void*) GPIO_INPUT_IO_2);
    gpio_isr_handler_add(GPIO_INPUT_IO_3, gpio_isr_handler, (void*) GPIO_INPUT_IO_3);
    gpio_isr_handler_add(GPIO_INPUT_IO_4, gpio_isr_handler, (void*) GPIO_INPUT_IO_4);
    gpio_isr_handler_add(GPIO_INPUT_IO_5, gpio_isr_handler, (void*) GPIO_INPUT_IO_5);
    gpio_isr_handler_add(GPIO_INPUT_IO_6, gpio_isr_handler, (void*) GPIO_INPUT_IO_6);
    gpio_isr_handler_add(GPIO_INPUT_IO_7, gpio_isr_handler, (void*) GPIO_INPUT_IO_7);
    // //remove isr handler for gpio number.
    // gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    // //hook isr handler for specific gpio pin again
    // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    ESP_LOGI(TAG, "Minimum free heap size: %d bytes", esp_get_minimum_free_heap_size());

    // gpio_set_level(GPIO_OUTPUT_LED_1, 0);
    // gpio_set_level(GPIO_OUTPUT_LED_2, 0);
    // gpio_set_level(GPIO_OUTPUT_LED_3, 0);
    // gpio_set_level(GPIO_OUTPUT_LED_4, 0);
    // gpio_set_level(GPIO_OUTPUT_LED_5, 0);
    // gpio_set_level(GPIO_OUTPUT_LED_6, 0);

}
