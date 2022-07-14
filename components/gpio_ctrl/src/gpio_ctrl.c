#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "gpio_ctrl.h"
#include "timer_app.h"
#include "mqtt_app.h"
#include "uart485.h"

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
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            buf_state = gpio_get_level(io_num);
            printf("GPIO[%d] intr, val: %d\n", io_num, buf_state);
            //if(buf_state ==1)
            sw_key_read(io_num,buf_state); //judge button press once twice or long
        }
    }
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

#ifdef DEVICE_TYPE_BRUSH
void centralizer_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 1);
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        //bursh_para.centralizer = 1;
        parameter_write_centralizer(1);
        printf("bursh_para.centralizer = 1\n");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 1);
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 1);
        //bursh_para.centralizer = 2;
        parameter_write_centralizer(2);
        printf("bursh_para.centralizer = 2\n");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        //bursh_para.centralizer = 0;
        parameter_write_centralizer(0);
        printf("bursh_para.centralizer = 0\n");}
}
void rotation_io_out(uint8_t value)
{
    if(value==1){
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 1);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        //bursh_para.rotation = 1;
        parameter_write_rotation(1);
        printf("bursh_para.rotation = 1\n");}
    else if(value==2){
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 1);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 1);
        //bursh_para.rotation = 2;
        parameter_write_rotation(2);
        printf("bursh_para.rotation = 2\n");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        //bursh_para.rotation = 0;
        parameter_write_rotation(0);
        printf("bursh_para.rotation = 0\n");} 
}
void nozzle_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 1);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        //bursh_para.nozzle = 1;
        parameter_write_nozzle(1);
        printf("bursh_para.nozzle = 1\n");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 1);
        //bursh_para.nozzle = 2;
        parameter_write_nozzle(2);
        printf("bursh_para.nozzle = 2\n");}
    else{
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        //bursh_para.nozzle = 0;
        parameter_write_nozzle(0);
        printf("bursh_para.nozzle = 0\n");}
}

void emergency_stop_io_out(uint8_t value)
{
    if(value == 1){
        gpio_set_level(GPIO_OUTPUT_IO_DRAW, 0);
        gpio_set_level(GPIO_OUTPUT_IO_STRETCH, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEY, 0);
        gpio_set_level(GPIO_OUTPUT_IO_ROTATEX, 0);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_emergency_stop(1);
        gpio_set_level(GPIO_SYS_LED, 1);
        parameter_write_centralizer(0);
        parameter_write_rotation(0);
        parameter_write_nozzle(0);
        printf("bursh_para.emergency_stop = 1\n");}
    else{
        parameter_write_emergency_stop(0);
        gpio_set_level(GPIO_SYS_LED, 0);
        printf("bursh_para.emergency_stop = 0\n");}
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
            printf("back to normal mode\n");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            emergency_stop_io_out(register_afterpress);
        }
        else{
            printf("emergency_stop_error_press\n");
            timer_periodic();
        }
    }
    else
    {
        switch(io_num)  //BRUSH
        {
            case GPIO_INPUT_IO_1:
            printf("GPIO_INPUT_IO_1\n");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,1);
            centralizer_io_out(register_afterpress);            
            break;
            case GPIO_INPUT_IO_2:
            printf("GPIO_INPUT_IO_2\n");
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,2);
            centralizer_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_3:
            printf("GPIO_INPUT_IO_3\n");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,1);
            rotation_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_4:
            printf("GPIO_INPUT_IO_4\n");
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,2);
            rotation_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_5:
            printf("GPIO_INPUT_IO_5\n");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,1);
            nozzle_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_6:
            printf("GPIO_INPUT_IO_6\n");
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,2);
            nozzle_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_7:
            printf("GPIO_INPUT_IO_7\n");
            break;
            case GPIO_INPUT_IO_STOP:
            printf("GPIO_INPUT_IO_STOP\n");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            emergency_stop_io_out(register_afterpress);
            break;
            default:
            //printf("KEY_default\n");
            break;
        }
    }
    mqtt_publish();
}

uint8_t brush_input(uint8_t io_num,uint8_t state)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    // uint8_t register_emergency_stop = 0;
    // register_emergency_stop = parameter_read_emergency_stop();
    if(state == 0 && io_num == GPIO_INPUT_IO_7)  //pressure 0/1 input
    {
        parameter_write_water(0);
        printf("GPIO_INPUT_IO_7:0\n");
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_7)
    {
        parameter_write_water(1);
        printf("GPIO_INPUT_IO_7:1\n");
    }
    else if(state == 0 && io_num == GPIO_INPUT_IO_6)  //pressure 0/1 input
    {
        parameter_write_pressure_alarm(0);
        printf("GPIO_INPUT_IO_7:0\n");
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_6)
    {
        parameter_write_pressure_alarm(1);
        printf("GPIO_INPUT_IO_7:1\n");
    }
    else if(state == 1 && io_num == GPIO_INPUT_IO_STOP)
    {
        printf("GPIO_INPUT_IO_STOP\n");
        register_value = parameter_read_emergency_stop();
        register_afterpress = UI_press_output(register_value,1);
        emergency_stop_io_out(register_afterpress);
    }
    else
    {
        return 0;
    }
    mqtt_publish();
    return 1;
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
        printf("blister_para.emergency_stop = 1\n");}
    else{
        parameter_write_emergency_stop(0);
        gpio_set_level(GPIO_SYS_LED, 0);
        printf("blister_para.emergency_stop = 0\n");}
}
void heater_io_out(uint8_t value)
{
    if(value == 1){
        heater_water_module_test(6);
        gpio_set_level(GPIO_OUTPUT_IO_HEATER, 1);
        gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        parameter_write_heater(1);
        printf("blister.heater = 1\n");}
    else{
        heater_water_module_test(7);
        gpio_set_level(GPIO_OUTPUT_IO_HEATER, 0);
        gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        parameter_write_heater(0);
        printf("blister.heater = 0\n");}
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
        printf("blister_para.mode = 1\n");}
    else if(value == 2){
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        parameter_write_mode(2);
        printf("blister_para.mode = 2\n");}
    else{
        heater_water_module_test(5);
        gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        parameter_write_mode(0);
        printf("blister_para.mode = 0\n");}
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
        printf("blister_para.mode = 1\n");}
    else if(value == 2){
        heater_water_module_test(2);
        // gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        // gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        //parameter_write_mode(2);
        quantity = 2;
        printf("blister_para.mode = 2\n");}
    else{
        heater_water_module_test(3);
        // gpio_set_level(GPIO_OUTPUT_IO_WATER, 0);
        // gpio_set_level(GPIO_OUTPUT_IO_BUBBLE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        //parameter_write_mode(0);
        quantity = 3;
        printf("blister_para.mode = 0\n");}
}
void blister_press_output(uint8_t io_num)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    uint8_t register_emergency_stop = 0;
    register_emergency_stop = parameter_read_emergency_stop();
    
    if(register_emergency_stop)
    {
        if(io_num == GPIO_INPUT_IO_STOP)
        {
            printf("back to normal mode\n");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            blister_stop_io_out(register_afterpress);
        }
        else{
            printf("emergency_stop_error_press\n");
            timer_periodic();
        }
    }
    else
    {
        switch(io_num)   //BLISTER
        {
            case GPIO_INPUT_IO_1:
            printf("GPIO_INPUT_IO_1\n");
            register_value = parameter_read_heater();
            register_afterpress = UI_press_output(register_value,1);
            heater_io_out(register_afterpress);            
            break;
            case GPIO_INPUT_IO_2:
            printf("GPIO_INPUT_IO_2\n");
            register_value = parameter_read_mode();
            register_afterpress = UI_press_output(register_value,1);
            blister_mode_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_3:
            printf("GPIO_INPUT_IO_3\n");
            register_value = parameter_read_mode();
            register_afterpress = UI_press_output(register_value,2);
            blister_mode_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_4:
            printf("GPIO_INPUT_IO_4\n");
            register_afterpress = UI_press_output(quantity,1);
            blister_quantity_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_5:
            printf("GPIO_INPUT_IO_5\n");
            register_afterpress = UI_press_output(quantity,2);
            blister_quantity_io_out(register_afterpress);
            break;
            case GPIO_INPUT_IO_6:
            printf("GPIO_INPUT_IO_6\n");
            break;
            case GPIO_INPUT_IO_7:
            printf("GPIO_INPUT_IO_7\n");
            break;
            case GPIO_INPUT_IO_STOP:
            printf("GPIO_INPUT_IO_STOP\n");
            register_value = parameter_read_emergency_stop();
            register_afterpress = UI_press_output(register_value,1);
            blister_stop_io_out(register_afterpress);
            break;
            default:
            //printf("KEY_default\n");
            break;
        }
    }
    mqtt_publish();
}
#endif



void sw_key_read(uint8_t io_num,uint8_t state)
{
    // uint8_t key_status = 0;
    // key_status=KEY_READ(io_num);
    //printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));

    //brush_input(io_num,state);
    if(state == 1)
    {
    #ifdef DEVICE_TYPE_BRUSH
    brush_press_output(io_num);
    #endif
    #ifdef DEVICE_TYPE_BLISTER
    blister_press_output(io_num);
    #endif
    }


    // switch(key_status)
    // {
    //     case KEY_ONCE:
    //     led_gpio_output(io_num);
    //     printf("KEY_ONCE\n");
    //     break;
    //     case KEY_TWICE:
    //     printf("KEY_TWICE\n");
    //     break;
    //     case KEY_LONG:
    //     printf("KEY_LONG\n");
    //     break;
    //     default:
    //     //printf("KEY_default\n");
    //     break;
    // }
    vTaskDelay(10 / portTICK_RATE_MS);
}
void gpio_init(void)
{
    //GPIO
    
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

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
}
