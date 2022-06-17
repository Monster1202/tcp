#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
//#include "pid_ctrl.h"
#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/unistd.h"
// #include <stdarg.h>
// #include "sdkconfig.h"
// #include "esp_flash.h"
// #include "esp_attr.h"
esp_err_t test_app(void)
{
    printf("KEY_ONCE\n");
    //delay_us(750);
    vTaskDelay(1000 / portTICK_RATE_MS);
    usleep(1000);
    gpio_set_level(GPIO_IO_DS18B20, 1);	
    return ESP_OK;
    
}
void delay_us(int cnt)
{
  //vTaskDelay(1 / portTICK_RATE_MS);
  usleep(cnt);
}
void DS_DIR_IN(void)  //让PB9为浮空输入模式
{
  //GPIO_IO_DS18B20    GPIO_INOUT_PIN_SEL
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<GPIO_IO_DS18B20);//GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
	
    // io_conf.pull_up_en = 0;
	// io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
	gpio_set_pull_mode(GPIO_IO_DS18B20, GPIO_FLOATING);
	// GPIO_InitTypeDef GPIO_InitStruct;
	
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	// GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	// GPIO_Init(GPIOB,&GPIO_InitStruct);
}
 
  //  #define GPIO_IO_DS18B20    8
  // #define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)

void DS_DIR_OUT(void) //让PB9为推挽输出模式
{
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<GPIO_IO_DS18B20);//GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 1;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
	gpio_set_pull_mode(GPIO_IO_DS18B20, GPIO_PULLUP_PULLDOWN);
	// GPIO_InitTypeDef GPIO_InitStruct;
	
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	// GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	// GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void Init_DS18B20(void) 
{
	unsigned char t = 0;
	DS_DIR_OUT();  //让GPIO口为推挽输出模式
	gpio_set_level(GPIO_IO_DS18B20, 0);  //发送复位脉冲 ds18b20 DQ管脚接到单片机的PB9
	delay_us(600); 		//延时（>480us) 
	gpio_set_level(GPIO_IO_DS18B20, 1);		//拉高数据线 
	delay_us(55); 				//等待（15~60us)	
	DS_DIR_IN(); //配置GPIO口为浮空输入模式
	while(gpio_get_level(GPIO_IO_DS18B20) == 1) //等待拉低
	{
		delay_us(1);
		t++;
		if(t >= 240)//如果超过240us还是高电平 代表ds18b20没发数据回来 丢失连接了
			return;   
	}
	t = 0;
	while(gpio_get_level(GPIO_IO_DS18B20) == 0) //等待拉高
	{
		delay_us(1);
		t++;
		if(t >= 240)//如果超过240us还是低电平 代表ds18b20没发数据回来 丢失连接了
			return; 
	}
	printf("温度初始化完成！"); //我的printf函数是跟串口结合了，用于看ds18b20是否初始化完成
}

  // #define GPIO_IO_DS18B20    8
  // #define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)


//单片机向DS18B20写一位  0  对应上图里面 左上方的图
void Write_Bit_0(void)
{
	gpio_set_level(GPIO_IO_DS18B20, 0); //拉低
	delay_us(90);  //在60us --- 120us之间
	gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
	delay_us(10);  //拉高大于1us 这里选用10us
}
//单片机向DS18B20写一位 1
void Write_Bit_1(void)
{
	gpio_set_level(GPIO_IO_DS18B20, 0); //拉低 
	delay_us(10);  //拉低大于1us
	gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
	delay_us(90); //我的理解是 拉高大于60us就是写1 所以选用90us
}
 
//单片机向DS18B20读一位 
unsigned char Read_Bit(void)
{
	DS_DIR_OUT(); //PB9为推挽输出模式
	gpio_set_level(GPIO_IO_DS18B20, 0);  //拉低
	delay_us(10); //大于1us 选用10us
	DS_DIR_IN(); //PB9为浮空输入模式
	delay_us(10); //等待一会 用于后面判断该管脚返回的是高电平还是低电平 太大不好试过80us返回的数据就奇怪了
	if(gpio_get_level(GPIO_IO_DS18B20) == 1)
	{
		return 1; //高电平返回1 
	}
	else
	{
		return 0; //低电平返回0
	}
}
//读一个字节
unsigned char ReadOneChar(void)  			
{
	unsigned char i=0; 		
	unsigned char dat=0; 
	for (i=0;i<8;i++) 		
	{
		dat = dat | (Read_Bit() << i); //DS18B20经手册查的是低位开始传输回来的，要获取一个完整的字节，这样子就可以。
	} 
	return(dat);
}
 
//写一个字节
void WriteOneChar(unsigned char dat) //dat要发送的数据
{ 
	unsigned char i=0; 		
	DS_DIR_OUT(); //推挽输出
	for(i=8;i>0;i--) 		//在15~60us之间对数据线进行采样，如果是高电平就写1，低写0发生。 
	{
		if((dat & 0x01) == 1)
		{
			Write_Bit_1();
		}
		else
		{
			Write_Bit_0();
		}
		dat >>= 1;
	} 
}

//读温度值（低位放tempL;高位放tempH;）
void ReadTemperature(void) 
{ 
	unsigned char tempL=0;         //设全局变量
	unsigned char tempH=0; 
	unsigned int sdata;            //温度的部分
	unsigned char fg=1;                    //温度正负标志

	Init_DS18B20(); 					//初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作
	WriteOneChar(0x44); 				//启动温度转换
	delay_us(1000);					    //转换需要一点时间，延时 
	Init_DS18B20(); 					//初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作 
	WriteOneChar(0xbe); 				//读温度寄存器（头两个值分别为温度的低位和高位）	
	tempL=ReadOneChar(); 				//读出温度的低位LSB
	tempH=ReadOneChar(); 				//读出温度的高位MSB	
	printf("tempL:%d  \r\n",tempL);
	printf("tempH:%d  \r\n",tempH);
	if(tempH>0x7f)      				//最高位为1时温度是负
	{
		tempL=~tempL;					//补码转换，取反加一
		tempH=~tempH+1;       
		fg=0;      						//读取温度为负时fg=0
	}
	sdata = (tempH << 8) + tempL;
	sdata = (sdata * 0.0625) * 100;  //这里×100 用于保留两位小数了，因为我是unsigned int类型不是float。
	printf("%d  \r\n",sdata);
}

/*******************************************************************************
  * 函数名：DS18B20_set
  * 功  能：DS18B20复位
  * 参  数：无
  * 返回值：无
  * 说  明：复位
*******************************************************************************/
// void DS18B20_Reset(void)
// {
// 	DS18B20_DQModeOutput();//设置为输出
// 	DS18B20_DQReset();//低电平
// 	delay_us(750);//750us
// 	DS18B20_DQSet();//高电平
// 	delay_us(15);//15us	
// }
// /*******************************************************************************
//   * 函数名：DS18B20_Check
//   * 功  能：检测DS18B20是否存在
//   * 参  数：无
//   * 返回值：1不存在，0存在
//   * 说  明：无
// *******************************************************************************/
// uint8_t DS18B20_Check(void)
// {
// 	uint8_t u8Retry = 0;
// 	DS18B20_DQModeInput();//设置为输入
// 	while ((DS18B20_DIORead() == 1)&&(u8Retry < 200))
// 	{
// 		u8Retry++;
// 		delay_us(1);
// 	}
	
// 	if (u8Retry >= 200)
// 	{
// 		return 1;
// 	}else
// 	{
// 		u8Retry = 0;
// 	}
// 	while ((DS18B20_DIORead() == 0) && (u8Retry < 240))
// 	{
// 		u8Retry++;
// 		delay_us(1);
// 	}
// 	if(u8Retry >= 120)
// 	{
// 		return 1;
// 	}
// 	return 0;	
// }
// /*******************************************************************************
//   * 函数名：DS18B20_WriteByte
//   * 功  能：向DS18B20写入一个字节
//   * 参  数：u8Data:要写入的数据
//   * 返回值：无
//   * 说  明：
// *******************************************************************************/
// void DS18B20_WriteByte(uint8_t u8Data)
// {
// 	uint8_t tempIndex,tempData;
// 	DS18B20_DQModeOutput();//设置为输出
// 	for (tempIndex = 1; tempIndex <= 8; tempIndex++)
// 	{
// 		tempData = (u8Data & 0x01);
// 		u8Data >>= 1;
// 		if (tempData == 1)
// 		{
// 			DS18B20_DQReset();//低电平
// 			delay_us(2);
// 			DS18B20_DQSet();//高电平
// 			delay_us(60);//延时60us
// 		}else
// 		{
// 			DS18B20_DQReset();//低电平
// 			delay_us(60);//延时60us
// 			DS18B20_DQSet();//高电平
// 			delay_us(2);
// 		}		
// 	}
// }
// /*******************************************************************************
//   * 函数名：DS18B20_ReadBit
//   * 功  能：从DS18B20读取一个位
//   * 参  数：无
//   * 返回值：1或0
//   * 说  明：无
// *******************************************************************************/
// uint8_t DS18B20_ReadBit(void)
// {
// 	uint8_t u8Data = 0;
// 	DS18B20_DQModeOutput();//设置为输出
// 	DS18B20_DQReset();//低电平
// 	delay_us(2);
// 	DS18B20_DQSet();//高电平
// 	DS18B20_DQModeInput();//设置为输入
// 	delay_us(12);
// 	u8Data = ((DS18B20_DIORead() == 1) ? 1 : 0);
// 	delay_us(50);
// 	return u8Data;
// }
// /*******************************************************************************
//   * 函数名：DS18B20_ReadByte
//   * 功  能：从DS18B20读取一个字节
//   * 参  数：无
//   * 返回值：u8Data读出的数据
//   * 说  明：无
// *******************************************************************************/
// uint8_t DS18B20_ReadByte(void)
// {
// 	uint8_t i,j, u8Ddata = 0;
	
// 	for (i = 1; i <= 8; i++)
// 	{		
// 		j = DS18B20_ReadBit();
// 		u8Ddata = (j << 7) | (u8Ddata >> 1);
// 	}	
// 	return u8Ddata;
// }
// /*******************************************************************************
//   * 函数名：DS18B20_Start
//   * 功  能：开始温度转换
//   * 参  数：无
//   * 返回值：无
//   * 说  明：无
// *******************************************************************************/
// void DS18B20_Start(void)
// { 
//   uint8_t check;
// 	DS18B20_Reset();
// 	check = DS18B20_Check();
//   printf("DS18B20_Check:%d\n",check);
// 	DS18B20_WriteByte(0xCC);//跳过ROM
// 	DS18B20_WriteByte(0x44);//温度转换	
// }

