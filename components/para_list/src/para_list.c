#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "para_list.h"


//
PARAMETER_BRUSH bursh_para;

void parameter_write_pressure(uint16_t pressure)
{   
    bursh_para.pressure = pressure;
}

uint16_t parameter_read_pressure(void)
{
    return bursh_para.pressure;
}

void parameter_write_temperature(uint16_t temperature)
{   
    bursh_para.temperature = temperature;
}

uint16_t parameter_read_temperature(void)
{
    return bursh_para.temperature;
}

// esp_err_t test_app_1(void)
// {
//     printf("para_init\n");
//     return ESP_OK;
// }


