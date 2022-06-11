#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "para_list.h"


//
PARAMETER_BRUSH bursh_para;

esp_err_t test_app_1(void)
{
    printf("para_init\n");
    return ESP_OK;
}


