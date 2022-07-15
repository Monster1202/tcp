#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"

#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/unistd.h"
#include "para_list.h"


// void delay_us(int cnt)
// {
//   //vTaskDelay(1 / portTICK_RATE_MS);
//   //usleep(cnt);
//   cnt = cnt*2;
//   ets_delay_us(cnt);
// }

// void DS_DIR_IN(void)  //让PB9为浮空输入模式
// {
// 	gpio_pad_select_gpio(GPIO_IO_DS18B20);
// 	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_INPUT);
// }
 
//   //  #define GPIO_IO_DS18B20    8
//   // #define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)

// void DS_DIR_OUT(void) //让PB9为推挽输出模式
// {
// 	gpio_pad_select_gpio(GPIO_IO_DS18B20);
// 	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_OUTPUT);
// }

// void Init_DS18B20(void) 
// {
// 	unsigned char t = 0;
// 	DS_DIR_OUT();  //让GPIO口为推挽输出模式
// 	// gpio_set_level(GPIO_IO_DS18B20, 1);		//拉高数据线 
// 	// delay_us(1); 
// 	gpio_set_level(GPIO_IO_DS18B20, 0);  //发送复位脉冲 ds18b20 DQ管脚接到单片机的PB9
// 	delay_us(300); 		//延时（>480us) 
// 	gpio_set_level(GPIO_IO_DS18B20, 1);		//拉高数据线 
// 	delay_us(15); 				//等待（15~60us)	
// 	DS_DIR_IN(); //配置GPIO口为浮空输入模式
// 	while(gpio_get_level(GPIO_IO_DS18B20) == 1) //等待拉低
// 	{
// 		delay_us(1);
// 		t++;
// 		if(t >= 240)//如果超过240us还是高电平 代表ds18b20没发数据回来 丢失连接了
// 			return;   
// 	}
// 	t = 0;
// 	while(gpio_get_level(GPIO_IO_DS18B20) == 0) //等待拉高
// 	{
// 		delay_us(1);
// 		t++;
// 		if(t >= 240)//如果超过240us还是低电平 代表ds18b20没发数据回来 丢失连接了
// 			return; 
// 	}
// 	//printf("温度初始化完成！"); //我的printf函数是跟串口结合了，用于看ds18b20是否初始化完成
// }
// //单片机向DS18B20写一位  0  对应上图里面 左上方的图
// void Write_Bit_0(void)
// {
// 	// gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
// 	// delay_us(1); 
// 	gpio_set_level(GPIO_IO_DS18B20, 0); //拉低
// 	delay_us(60);  //在60us --- 120us之间
// 	gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
// 	delay_us(10);  //拉高大于1us 这里选用10us
// }
// //单片机向DS18B20写一位 1
// void Write_Bit_1(void)
// {
// 	// gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
// 	// delay_us(1); 
// 	gpio_set_level(GPIO_IO_DS18B20, 0); //拉低 
// 	delay_us(10);  //拉低大于1us
// 	gpio_set_level(GPIO_IO_DS18B20, 1);  //拉高
// 	delay_us(60); //我的理解是 拉高大于60us就是写1 所以选用90us
// }
 
// //单片机向DS18B20读一位 
// unsigned char Read_Bit(void)
// {
// 	DS_DIR_OUT(); //PB9为推挽输出模式
// 	gpio_set_level(GPIO_IO_DS18B20, 1); 
// 	delay_us(1); 
// 	gpio_set_level(GPIO_IO_DS18B20, 0);  //拉低
// 	delay_us(2); //大于1us 选用10us
// 	DS_DIR_IN(); //PB9为浮空输入模式
// 	delay_us(10); //等待一会 用于后面判断该管脚返回的是高电平还是低电平 太大不好试过80us返回的数据就奇怪了
// 	if(gpio_get_level(GPIO_IO_DS18B20) == 1)
// 	{
// 		return 1; //高电平返回1 
// 	}
// 	else
// 	{
// 		return 0; //低电平返回0
// 	}
// 	delay_us(47);
// }
// //读一个字节
// unsigned char ReadOneChar(void)  			
// {
// 	unsigned char i=0; 		
// 	unsigned char dat=0; 
// 	for (i=0;i<8;i++) 		
// 	{
// 		dat = dat | (Read_Bit() << i); //DS18B20经手册查的是低位开始传输回来的，要获取一个完整的字节，这样子就可以。
// 	} 
// 	return(dat);
// }
 
// //写一个字节
// void WriteOneChar(unsigned char dat) //dat要发送的数据
// { 
// 	unsigned char i=0; 		
// 	DS_DIR_OUT(); //推挽输出
// 	for(i=8;i>0;i--) 		//在15~60us之间对数据线进行采样，如果是高电平就写1，低写0发生。 
// 	{
// 		if((dat & 0x01) == 1)
// 		{
// 			Write_Bit_1();
// 		}
// 		else
// 		{
// 			Write_Bit_0();
// 		}
// 		dat >>= 1;
// 	} 
// }

// //读温度值（低位放tempL;高位放tempH;）
// double ReadTemperature(void) 
// { 
// 	unsigned char tempL=0;         //设全局变量
// 	unsigned char tempH=0; 
// 	unsigned int sdata;            //温度的部分
// 	unsigned char fg=1;                    //温度正负标志
// 	double temp = 0;
// 	Init_DS18B20(); 					//初始化
// 	WriteOneChar(0xcc); 				//跳过读序列号的操作
// 	WriteOneChar(0x44); 				//启动温度转换
// 	delay_us(2000);	//delay_us(1000);					    //转换需要一点时间，延时 
// 	Init_DS18B20(); 					//初始化
// 	WriteOneChar(0xcc); 				//跳过读序列号的操作 
// 	WriteOneChar(0xbe); 				//读温度寄存器（头两个值分别为温度的低位和高位）	
// 	tempL=ReadOneChar(); 				//读出温度的低位LSB
// 	tempH=ReadOneChar(); 				//读出温度的高位MSB	
// 	// printf("tempL:%d  \r\n",tempL);
// 	// printf("tempH:%d  \r\n",tempH);
// 	if(tempH>0x7f)      				//最高位为1时温度是负
// 	{
// 		tempL=~tempL;					//补码转换，取反加一
// 		tempH=~tempH+1;       
// 		fg=0;      						//读取温度为负时fg=0
// 	}
// 	sdata = (tempH << 8) + tempL;
// 	temp = (double)sdata * 0.0625;  //这里×100 用于保留两位小数了，因为我是unsigned int类型不是float。
// 	printf("temp=%f\r\n",temp);
// 	return temp;
// }

// int Compare_double(const void* a, const void* b)
// {
// 	double arg1 = *(const double*)a;
// 	double arg2 = *(const double*)b;
// 	double arg3 = arg1 - arg2;
// 	double eps = 1e-6;
// 	if (-eps <= arg3 && arg3 <= eps)
// 	{
// 		return 0;
// 	}
// 	if (eps <= arg3 )
// 	{
// 		return 1;
// 	}
// 	if ( arg3 <= -eps)
// 	{
// 		return -1;
// 	}
//     return 0;
// }
//  #define length_temp_sort 11  //5  
// void ds18b20_read(void* arg)
// {
//     static double temp[length_temp_sort]={0};
//     double temp_sorted[length_temp_sort]={0};
//     //int temp_int[5]={0};
//     double temp_mid = 0;
	
// 	BaseType_t iRet;
//     for(;;)
//     {
//         vTaskDelay(500 / portTICK_RATE_MS);
//         temp[length_temp_sort-1]=ReadTemperature();
//         for(uint8_t i=0;i<length_temp_sort;i++)
//             temp_sorted[i] = temp[i];
//         qsort(temp_sorted, length_temp_sort, sizeof(temp_sorted[0]), Compare_double); //increase
//         temp_mid = temp_sorted[length_temp_sort/2];
//         //bursh_para.temperature = temp_mid;
// 		parameter_write_temperature(temp_mid);
// //        printf("qsort:%f,%f,%f,%f,%f;temp_mid:%f\n",temp_sorted[0],temp_sorted[1],temp_sorted[2],temp_sorted[3],temp_sorted[4],temp_mid);
//         printf("qsort-temp_mid:%f\n",temp_mid);
//         for(uint8_t i=1;i<length_temp_sort;i++)
//             temp[i-1] = temp[i];
//     }
// }
// void ds18b20_read(void* arg)
// {
//     static double temp[5]={0};
//     double temp_sorted[5]={0};
//     //int temp_int[5]={0};
//     double temp_mid = 0;
	
// 	BaseType_t iRet;
//     for(;;)
//     {
//         vTaskDelay(500 / portTICK_RATE_MS);
// 		//portDISABLE_INTERRUPTS();
// 		//taskENTER_CRITICAL();
// 		//vTaskSuspendAll();
// 		iRet = xSemaphoreTake(mutexHandle,1000);
//         temp[4]=ReadTemperature();
// 		xSemaphoreGive(mutexHandle);
// 		//xTaskResumeAll();
// 		//taskEXIT_CRITICAL();
// 		//portENABLE_INTERRUPTS();
//         for(uint8_t i=0;i<5;i++)
//             temp_sorted[i] = temp[i];
//         qsort(temp_sorted, 5, sizeof(temp_sorted[0]), Compare_double); //increase
//         temp_mid = temp_sorted[2];
//         //bursh_para.temperature = temp_mid;
// 		parameter_write_temperature(temp_mid);
// //        printf("qsort:%f,%f,%f,%f,%f;temp_mid:%f\n",temp_sorted[0],temp_sorted[1],temp_sorted[2],temp_sorted[3],temp_sorted[4],temp_mid);
//         printf("qsort-temp_mid:%f\n",temp_mid);
//         for(uint8_t i=1;i<5;i++)
//             temp[i-1] = temp[i];

//          //taskENTER_CRITICAL();
//         // vPortEnterCritical();      
//         // 
//         // taskENTER_CRITICAL_FROM_ISR();//taskENTER_CRITICAL(); //vPortEnterCritical();//  
         
//         // taskENTER_CRITICAL_FROM_ISR();//taskEXIT_CRITICAL();//vPortExitCritical();//
//     }
// }
/*******************************************************************************
  * 函数名：DS18B20_set
  * 功  能：DS18B20复位
  * 参  数：无
  * 返回值：无
  * 说  明：复位
*******************************************************************************/
void delay_us(int cnt)
{
  //vTaskDelay(1 / portTICK_RATE_MS);
  //usleep(cnt);
  //cnt = cnt*2;
  ets_delay_us(cnt);
}
void DS18B20_Reset(void)
{
	DS18B20_DQModeOutput();//设置为输出
	DS18B20_DQReset();//低电平
	delay_us(750);//750us
	DS18B20_DQSet();//高电平
	delay_us(15);//15us	
}
/*******************************************************************************
  * 函数名：DS18B20_Check
  * 功  能：检测DS18B20是否存在
  * 参  数：无
  * 返回值：1不存在，0存在
  * 说  明：无
*******************************************************************************/
uint8_t DS18B20_Check(void)
{
	uint8_t u8Retry = 0;
	DS18B20_DQModeInput();//设置为输入
	while ((DS18B20_DIORead() == 1)&&(u8Retry < 200))
	{
		u8Retry++;
		delay_us(1);
	}
	
	if (u8Retry >= 200)
	{
		return 1;
	}else
	{
		u8Retry = 0;
	}
	while ((DS18B20_DIORead() == 0) && (u8Retry < 240))
	{
		u8Retry++;
		delay_us(1);
	}
	if(u8Retry >= 120)
	{
		return 1;
	}
	return 0;	
}
/*******************************************************************************
  * 函数名：DS18B20_WriteByte
  * 功  能：向DS18B20写入一个字节
  * 参  数：u8Data:要写入的数据
  * 返回值：无
  * 说  明：
*******************************************************************************/
void DS18B20_WriteByte(uint8_t u8Data)
{
	uint8_t tempIndex,tempData;
	DS18B20_DQModeOutput();//设置为输出
	for (tempIndex = 1; tempIndex <= 8; tempIndex++)
	{
		tempData = (u8Data & 0x01);
		u8Data >>= 1;
		if (tempData == 1)
		{
			DS18B20_DQReset();//低电平
			delay_us(2);
			DS18B20_DQSet();//高电平
			delay_us(60);//延时60us
		}else
		{
			DS18B20_DQReset();//低电平
			delay_us(60);//延时60us
			DS18B20_DQSet();//高电平
			delay_us(2);
		}		
	}
}
/*******************************************************************************
  * 函数名：DS18B20_ReadBit
  * 功  能：从DS18B20读取一个位
  * 参  数：无
  * 返回值：1或0
  * 说  明：无
*******************************************************************************/
uint8_t DS18B20_ReadBit(void)
{
	uint8_t u8Data = 0;
	DS18B20_DQModeOutput();//设置为输出
	DS18B20_DQReset();//低电平
	delay_us(2);
	DS18B20_DQSet();//高电平
	DS18B20_DQModeInput();//设置为输入
	delay_us(12);
	u8Data = ((DS18B20_DIORead() == 1) ? 1 : 0);
	delay_us(50);
	return u8Data;
}
/*******************************************************************************
  * 函数名：DS18B20_ReadByte
  * 功  能：从DS18B20读取一个字节
  * 参  数：无
  * 返回值：u8Data读出的数据
  * 说  明：无
*******************************************************************************/
uint8_t DS18B20_ReadByte(void)
{
	uint8_t i,j, u8Ddata = 0;
	
	for (i = 1; i <= 8; i++)
	{		
		j = DS18B20_ReadBit();
		u8Ddata = (j << 7) | (u8Ddata >> 1);
	}	
	return u8Ddata;
}
/*******************************************************************************
  * 函数名：DS18B20_Start
  * 功  能：开始温度转换
  * 参  数：无
  * 返回值：无
  * 说  明：无
*******************************************************************************/

double DS18B20_Start(void)
{ 
	unsigned char tempL=0;         //设全局变量
	unsigned char tempH=0; 
	unsigned int sdata;            //温度的部分
	unsigned char fg=1;                    //温度正负标志
  	uint8_t check;
	double temp = 0;
	gpio_pad_select_gpio(GPIO_IO_DS18B20);
	DS18B20_Reset();
	check = DS18B20_Check();
  	//printf("DS18B20_Check:%d\n",check);
	DS18B20_WriteByte(0xCC);//跳过ROM
	DS18B20_WriteByte(0x44);//温度转换	
	//delay_us(1000);
	DS18B20_Reset();
	check = DS18B20_Check();
	DS18B20_WriteByte(0xCC);//跳过ROM
	DS18B20_WriteByte(0xBE);//温度转换	
	
	tempL = DS18B20_ReadByte();
	tempH = DS18B20_ReadByte();
	// printf("tempL:%d  \r\n",tempL);
	// printf("tempH:%d  \r\n",tempH);
	if(tempH>0x7f)      				//最高位为1时温度是负
	{
		tempL=~tempL;					//补码转换，取反加一
		tempH=~tempH+1;       
		fg=0;      						//读取温度为负时fg=0
	}
	// sdata = (tempH << 8) + tempL;
	// sdata = (sdata * 0.0625) * 100;  //这里×100 用于保留两位小数了，因为我是unsigned int类型不是float。
	// printf("%d  \r\n",sdata);
 	sdata = (tempH << 8) + tempL;
	temp = (double)sdata * 0.0625;  //这里×100 用于保留两位小数了，因为我是unsigned int类型不是float。
	printf("DS18B20_temp=%f\r\n",temp);
	return temp;
	//return sdata/100;
}

void ds18b20_read(void* arg)
{
    double temp_mid = 0;
	
    for(;;)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
		temp_mid = DS18B20_Start();
		parameter_write_temperature(temp_mid);
	}	
}
// // OneWire commands
// #define GETTEMP			0x44  // Tells device to take a temperature reading and put it on the scratchpad
// #define SKIPROM			0xCC  // Command to address all devices on the bus
// #define SELECTDEVICE	0x55  // Command to address all devices on the bus
// #define COPYSCRATCH     0x48  // Copy scratchpad to EEPROM
// #define READSCRATCH     0xBE  // Read from scratchpad
// #define WRITESCRATCH    0x4E  // Write to scratchpad
// #define RECALLSCRATCH   0xB8  // Recall from EEPROM to scratchpad
// #define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
// #define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition
// // Scratchpad locations
// #define TEMP_LSB        0
// #define TEMP_MSB        1
// #define HIGH_ALARM_TEMP 2
// #define LOW_ALARM_TEMP  3
// #define CONFIGURATION   4
// #define INTERNAL_BYTE   5
// #define COUNT_REMAIN    6
// #define COUNT_PER_C     7
// #define SCRATCHPAD_CRC  8
// // DSROM FIELDS
// #define DSROM_FAMILY    0
// #define DSROM_CRC       7
// // Device resolution
// #define TEMP_9_BIT  0x1F //  9 bit
// #define TEMP_10_BIT 0x3F // 10 bit
// #define TEMP_11_BIT 0x5F // 11 bit
// #define TEMP_12_BIT 0x7F // 12 bit

// uint8_t DS_GPIO;
// uint8_t init=0;
// uint8_t bitResolution=12;
// uint8_t devices=0;

// DeviceAddress ROM_NO;
// uint8_t LastDiscrepancy;
// uint8_t LastFamilyDiscrepancy;
// bool LastDeviceFlag;

// /// Sends one bit to bus
// void ds18b20_write(char bit){
// 	if (bit & 1) {
// 		gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
// 		noInterrupts();
// 		gpio_set_level(DS_GPIO,0);
// 		ets_delay_us(6);
// 		gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);	// release bus
// 		ets_delay_us(64);
// 		interrupts();
// 	} else {
// 		gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
// 		noInterrupts();
// 		gpio_set_level(DS_GPIO,0);
// 		ets_delay_us(60);
// 		gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);	// release bus
// 		ets_delay_us(10);
// 		interrupts();
// 	}
// }

// // Reads one bit from bus
// unsigned char ds18b20_read(void){
// 	unsigned char value = 0;
// 	gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
// 	noInterrupts();
// 	gpio_set_level(DS_GPIO, 0);
// 	ets_delay_us(6);
// 	gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
// 	ets_delay_us(9);
// 	value = gpio_get_level(DS_GPIO);
// 	ets_delay_us(55);
// 	interrupts();
// 	return (value);
// }
// // Sends one byte to bus
// void ds18b20_write_byte(char data){
//   unsigned char i;
//   unsigned char x;
//   for(i=0;i<8;i++){
//     x = data>>i;
//     x &= 0x01;
//     ds18b20_write(x);
//   }
//   ets_delay_us(100);
// }
// // Reads one byte from bus
// unsigned char ds18b20_read_byte(void){
//   unsigned char i;
//   unsigned char data = 0;
//   for (i=0;i<8;i++)
//   {
//     if(ds18b20_read()) data|=0x01<<i;
//     ets_delay_us(15);
//   }
//   return(data);
// }
// // Sends reset pulse
// unsigned char ds18b20_reset(void){
// 	unsigned char presence;
// 	gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
// 	noInterrupts();
// 	gpio_set_level(DS_GPIO, 0);
// 	ets_delay_us(480);
// 	gpio_set_level(DS_GPIO, 1);
// 	gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
// 	ets_delay_us(70);
// 	presence = (gpio_get_level(DS_GPIO) == 0);
// 	ets_delay_us(410);
// 	interrupts();
// 	return presence;
// }

// bool ds18b20_setResolution(const DeviceAddress tempSensorAddresses[], int numAddresses, uint8_t newResolution) {
// 	bool success = false;
// 	// handle the sensors with configuration register
// 	newResolution = constrain(newResolution, 9, 12);
// 	uint8_t newValue = 0;
// 	ScratchPad scratchPad;
// 	// loop through each address
// 	for (int i = 0; i < numAddresses; i++){
// 		// we can only update the sensor if it is connected
// 		if (ds18b20_isConnected((DeviceAddress*) tempSensorAddresses[i], scratchPad)) {
// 			switch (newResolution) {
// 			case 12:
// 				newValue = TEMP_12_BIT;
// 				break;
// 			case 11:
// 				newValue = TEMP_11_BIT;
// 				break;
// 			case 10:
// 				newValue = TEMP_10_BIT;
// 				break;
// 			case 9:
// 			default:
// 				newValue = TEMP_9_BIT;
// 				break;
// 			}
// 			// if it needs to be updated we write the new value
// 			if (scratchPad[CONFIGURATION] != newValue) {
// 				scratchPad[CONFIGURATION] = newValue;
// 				ds18b20_writeScratchPad((DeviceAddress*) tempSensorAddresses[i], scratchPad);
// 			}
// 			// done
// 			success = true;
// 		}
// 	}
// 	return success;
// }

// void ds18b20_writeScratchPad(const DeviceAddress *deviceAddress, const uint8_t *scratchPad) {
// 	ds18b20_reset();
// 	ds18b20_select(deviceAddress);
// 	ds18b20_write_byte(WRITESCRATCH);
// 	ds18b20_write_byte(scratchPad[HIGH_ALARM_TEMP]); // high alarm temp
// 	ds18b20_write_byte(scratchPad[LOW_ALARM_TEMP]); // low alarm temp
// 	ds18b20_write_byte(scratchPad[CONFIGURATION]);
// 	ds18b20_reset();
// }

// bool ds18b20_readScratchPad(const DeviceAddress *deviceAddress, uint8_t* scratchPad) {
// 	// send the reset command and fail fast
// 	int b = ds18b20_reset();
// 	if (b == 0) return false;
// 	ds18b20_select(deviceAddress);
// 	ds18b20_write_byte(READSCRATCH);
// 	// Read all registers in a simple loop
// 	// byte 0: temperature LSB
// 	// byte 1: temperature MSB
// 	// byte 2: high alarm temp
// 	// byte 3: low alarm temp
// 	// byte 4: DS18B20 & DS1822: configuration register
// 	// byte 5: internal use & crc
// 	// byte 6: DS18B20 & DS1822: store for crc
// 	// byte 7: DS18B20 & DS1822: store for crc
// 	// byte 8: SCRATCHPAD_CRC
// 	for (uint8_t i = 0; i < 9; i++) {
// 		scratchPad[i] = ds18b20_read_byte();
// 	}
// 	b = ds18b20_reset();
// 	return (b == 1);
// }

// void ds18b20_select(const DeviceAddress *address){
//     uint8_t i;
//     ds18b20_write_byte(SELECTDEVICE);           // Choose ROM
//     for (i = 0; i < 8; i++) ds18b20_write_byte(((uint8_t *)address)[i]);
// }

// void ds18b20_requestTemperatures(){
// 	ds18b20_reset();
// 	ds18b20_write_byte(SKIPROM);
// 	ds18b20_write_byte(GETTEMP);
//     unsigned long start = esp_timer_get_time() / 1000ULL;
//     while (!isConversionComplete() && ((esp_timer_get_time() / 1000ULL) - start < millisToWaitForConversion())) vPortYield();
// }

// bool isConversionComplete() {
// 	uint8_t b = ds18b20_read();
// 	return (b == 1);
// }

// uint16_t millisToWaitForConversion() {
// 	switch (bitResolution) {
// 	case 9:
// 		return 94;
// 	case 10:
// 		return 188;
// 	case 11:
// 		return 375;
// 	default:
// 		return 750;
// 	}
// }

// bool ds18b20_isConnected(const DeviceAddress *deviceAddress, uint8_t *scratchPad) {
// 	bool b = ds18b20_readScratchPad(deviceAddress, scratchPad);
// 	return b && !ds18b20_isAllZeros(scratchPad) && (ds18b20_crc8(scratchPad, 8) == scratchPad[SCRATCHPAD_CRC]);
// }

// uint8_t ds18b20_crc8(const uint8_t *addr, uint8_t len){
// 	uint8_t crc = 0;
// 	while (len--) {
// 		crc = *addr++ ^ crc;  // just re-using crc as intermediate
// 		crc = pgm_read_byte(dscrc2x16_table + (crc & 0x0f)) ^
// 		pgm_read_byte(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f));
// 	}
// 	return crc;
// }

// bool ds18b20_isAllZeros(const uint8_t * const scratchPad) {
// 	for (size_t i = 0; i < 9; i++) {
// 		if (scratchPad[i] != 0) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

// float ds18b20_getTempF(const DeviceAddress *deviceAddress) {
// 	ScratchPad scratchPad;
// 	if (ds18b20_isConnected(deviceAddress, scratchPad)){
// 		int16_t rawTemp = calculateTemperature(deviceAddress, scratchPad);
// 		if (rawTemp <= DEVICE_DISCONNECTED_RAW)
// 			return DEVICE_DISCONNECTED_F;
// 		// C = RAW/128
// 		// F = (C*1.8)+32 = (RAW/128*1.8)+32 = (RAW*0.0140625)+32
// 		return ((float) rawTemp * 0.0140625f) + 32.0f;
// 	}
// 	return DEVICE_DISCONNECTED_F;
// }

// float ds18b20_getTempC(const DeviceAddress *deviceAddress) {
// 	ScratchPad scratchPad;
// 	if (ds18b20_isConnected(deviceAddress, scratchPad)){
// 		int16_t rawTemp = calculateTemperature(deviceAddress, scratchPad);
// 		if (rawTemp <= DEVICE_DISCONNECTED_RAW)
// 			return DEVICE_DISCONNECTED_F;
// 		// C = RAW/128
// 		// F = (C*1.8)+32 = (RAW/128*1.8)+32 = (RAW*0.0140625)+32
// 		return (float) rawTemp/128.0f;
// 	}
// 	return DEVICE_DISCONNECTED_F;
// }

// // reads scratchpad and returns fixed-point temperature, scaling factor 2^-7
// int16_t calculateTemperature(const DeviceAddress *deviceAddress, uint8_t* scratchPad) {
// 	int16_t fpTemperature = (((int16_t) scratchPad[TEMP_MSB]) << 11) | (((int16_t) scratchPad[TEMP_LSB]) << 3);
// 	return fpTemperature;
// }

// // Returns temperature from sensor
// float ds18b20_get_temp(void) {
//   if(init==1){
//     unsigned char check;
//     char temp1=0, temp2=0;
//       check=ds18b20_RST_PULSE();
//       if(check==1)
//       {
//         ds18b20_send_byte(0xCC);
//         ds18b20_send_byte(0x44);
//         vTaskDelay(750 / portTICK_RATE_MS);
//         check=ds18b20_RST_PULSE();
//         ds18b20_send_byte(0xCC);
//         ds18b20_send_byte(0xBE);
//         temp1=ds18b20_read_byte();
//         temp2=ds18b20_read_byte();
//         check=ds18b20_RST_PULSE();
//         float temp=0;
//         temp=(float)(temp1+(temp2*256))/16;
//         return temp;
//       }
//       else{return 0;}

//   }
//   else{return 0;}
// }

// void ds18b20_init(int GPIO) {
// 	DS_GPIO = GPIO;
// 	gpio_pad_select_gpio(DS_GPIO);
// 	init = 1;
// }

// //
// // You need to use this function to start a search again from the beginning.
// // You do not need to do it for the first search, though you could.
// //
// void reset_search() {
// 	devices=0;
// 	// reset the search state
// 	LastDiscrepancy = 0;
// 	LastDeviceFlag = false;
// 	LastFamilyDiscrepancy = 0;
// 	for (int i = 7; i >= 0; i--) {
// 		ROM_NO[i] = 0;
// 	}
// }
// // --- Replaced by the one from the Dallas Semiconductor web site ---
// //--------------------------------------------------------------------------
// // Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// // search state.
// // Return TRUE  : device found, ROM number in ROM_NO buffer
// //        FALSE : device not found, end of search

// bool search(uint8_t *newAddr, bool search_mode) {
// 	uint8_t id_bit_number;
// 	uint8_t last_zero, rom_byte_number;
// 	bool search_result;
// 	uint8_t id_bit, cmp_id_bit;

// 	unsigned char rom_byte_mask, search_direction;

// 	// initialize for search
// 	id_bit_number = 1;
// 	last_zero = 0;
// 	rom_byte_number = 0;
// 	rom_byte_mask = 1;
// 	search_result = false;

// 	// if the last call was not the last one
// 	if (!LastDeviceFlag) {
// 		// 1-Wire reset
// 		if (!ds18b20_reset()) {
// 			// reset the search
// 			LastDiscrepancy = 0;
// 			LastDeviceFlag = false;
// 			LastFamilyDiscrepancy = 0;
// 			return false;
// 		}

// 		// issue the search command
// 		if (search_mode == true) {
// 			ds18b20_write_byte(0xF0);   // NORMAL SEARCH
// 		} else {
// 			ds18b20_write_byte(0xEC);   // CONDITIONAL SEARCH
// 		}

// 		// loop to do the search
// 		do {
// 			// read a bit and its complement
// 			id_bit = ds18b20_read();
// 			cmp_id_bit = ds18b20_read();

// 			// check for no devices on 1-wire
// 			if ((id_bit == 1) && (cmp_id_bit == 1)) {
// 				break;
// 			} else {
// 				// all devices coupled have 0 or 1
// 				if (id_bit != cmp_id_bit) {
// 					search_direction = id_bit;  // bit write value for search
// 				} else {
// 					// if this discrepancy if before the Last Discrepancy
// 					// on a previous next then pick the same as last time
// 					if (id_bit_number < LastDiscrepancy) {
// 						search_direction = ((ROM_NO[rom_byte_number]
// 								& rom_byte_mask) > 0);
// 					} else {
// 						// if equal to last pick 1, if not then pick 0
// 						search_direction = (id_bit_number == LastDiscrepancy);
// 					}
// 					// if 0 was picked then record its position in LastZero
// 					if (search_direction == 0) {
// 						last_zero = id_bit_number;

// 						// check for Last discrepancy in family
// 						if (last_zero < 9)
// 							LastFamilyDiscrepancy = last_zero;
// 					}
// 				}

// 				// set or clear the bit in the ROM byte rom_byte_number
// 				// with mask rom_byte_mask
// 				if (search_direction == 1)
// 					ROM_NO[rom_byte_number] |= rom_byte_mask;
// 				else
// 					ROM_NO[rom_byte_number] &= ~rom_byte_mask;

// 				// serial number search direction write bit
// 				ds18b20_write(search_direction);

// 				// increment the byte counter id_bit_number
// 				// and shift the mask rom_byte_mask
// 				id_bit_number++;
// 				rom_byte_mask <<= 1;

// 				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
// 				if (rom_byte_mask == 0) {
// 					rom_byte_number++;
// 					rom_byte_mask = 1;
// 				}
// 			}
// 		} while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

// 		// if the search was successful then
// 		if (!(id_bit_number < 65)) {
// 			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
// 			LastDiscrepancy = last_zero;

// 			// check for last device
// 			if (LastDiscrepancy == 0) {
// 				LastDeviceFlag = true;
// 			}
// 			search_result = true;
// 		}
// 	}

// 	// if no device found then reset counters so next 'search' will be like a first
// 	if (!search_result || !ROM_NO[0]) {
// 		devices=0;
// 		LastDiscrepancy = 0;
// 		LastDeviceFlag = false;
// 		LastFamilyDiscrepancy = 0;
// 		search_result = false;
// 	} else {
// 		for (int i = 0; i < 8; i++){
// 			newAddr[i] = ROM_NO[i];
// 		}
// 		devices++;
// 	}
// 	return search_result;
// }
//TEST2 

// void test(void)
// {
//         printf("ds18b20_init\n");
//     ds18b20_init(TEMP_BUS);
//     printf("ds18b20_init1\n");
// 	getTempAddresses(tempSensors);
//     printf("ds18b20_init2\n");
// 	ds18b20_setResolution(tempSensors,2,10);

// 	printf("Address 0: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", tempSensors[0][0],tempSensors[0][1],tempSensors[0][2],tempSensors[0][3],tempSensors[0][4],tempSensors[0][5],tempSensors[0][6],tempSensors[0][7]);
// 	printf("Address 1: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", tempSensors[1][0],tempSensors[1][1],tempSensors[1][2],tempSensors[1][3],tempSensors[1][4],tempSensors[1][5],tempSensors[1][6],tempSensors[1][7]);
    
//             ds18b20_requestTemperatures();
// 		float temp1 = ds18b20_getTempF((DeviceAddress *)tempSensors[0]);
// 		float temp2 = ds18b20_getTempF((DeviceAddress *)tempSensors[1]);
// 		float temp3 = ds18b20_getTempC((DeviceAddress *)tempSensors[0]);
// 		float temp4 = ds18b20_getTempC((DeviceAddress *)tempSensors[1]);
// 		printf("Temperatures: %0.1fF %0.1fF\n", temp1,temp2);
// 		printf("Temperatures: %0.1fC %0.1fC\n", temp3,temp4);

// 		float cTemp = ds18b20_get_temp();
// 		printf("Temperature: %0.1fC\n", cTemp);
        
//         DS18B20_Start();    
// }



