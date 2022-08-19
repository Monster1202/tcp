#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "pid_ctrl.h"

esp_err_t test_app(void)
{
    printf("KEY_ONCE\n");
    return ESP_OK;
}


// void FTC533_T_delay(uint8_t cnt)
// {
// 	for(uint8_t i=0;i<cnt;i++)
// 	{
// 		delay_us(375);
// 	}
// }
// void FTC533_init(void)
// {
// 	gpio_set_direction(GPIO_IO_FTC533,GPIO_MODE_DEF_OUTPUT);//设置为输出
// }

// void FTC533_idle(void)
// {
// 	gpio_set_level(GPIO_IO_FTC533, 1);
// 	FTC533_T_delay(7);
// }
// void FTC533_start(void)
// { 
// 	gpio_set_level(GPIO_IO_FTC533, 0);
// 	FTC533_T_delay(2);
// }
// void FTC533_data0(void)
// {
// 	gpio_set_level(GPIO_IO_FTC533, 1);
// 	FTC533_T_delay(1);
// 	gpio_set_level(GPIO_IO_FTC533, 0);
// 	FTC533_T_delay(1);
// }
// void FTC533_data1(void)
// {
// 	gpio_set_level(GPIO_IO_FTC533, 1);
// 	FTC533_T_delay(1);
// 	gpio_set_level(GPIO_IO_FTC533, 0);
// 	FTC533_T_delay(3);
// }

// void FTC533_cycle(void)
// {
// 	static uint8_t cnt = 0;
// 	uint8_t key = 0;
// 	FTC533_idle();
// 	FTC533_start();
// 	key = parameter_read_FTC533();
// 	switch(key)
// 	{
// 		case 0:	FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data1();FTC533_data1();FTC533_data1();
// 		FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data1();FTC533_data1();FTC533_data1();
// 		break;
// 		case 1:	FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data1();FTC533_data1();FTC533_data1();
// 		FTC533_data0();FTC533_data1();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data0();FTC533_data1();FTC533_data1();
// 		break;
// 		case 3:	FTC533_data1();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data1();FTC533_data1();
// 		FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data0();FTC533_data1();FTC533_data1();FTC533_data1();FTC533_data1();
// 		break;
// 		default:
// 		break;
// 	}
// 	if(key != 0)
// 	{
// 		cnt++;
// 		if(cnt == 16)
// 		{
// 			parameter_write_FTC533(0);
// 			cnt = 0;
// 		}
// 	}
// }

// void FTC533_process(void)
// {
// 	//FTC533_init();
//     timer_FTC533();//start timer
//     for(;;)
//     {
//         //vTaskDelay(1 / portTICK_RATE_MS);
// 		FTC533_cycle();
// 	}
// }