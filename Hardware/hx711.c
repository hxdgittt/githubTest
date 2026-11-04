#include <hx711.h>
#include "sys.h"
#include "tim.h"

int   medleng = 0;
u32   xd,xd1;
u8 calibrate_num = 0;
u8 CalWgtFlag = 0;

HX711_ChannelTypeDef hx_ch1 = {
	.sck_port	= HX711_SCK_1_GPIO_Port,		.sck_pin  = HX711_SCK_1_Pin,
	.data_port	= HX711_DATA_1_GPIO_Port,	.data_pin = HX711_DATA_1_Pin,
	.buffer = {0}, .medleng = 0,
	.xishu = 0.12494,  .convert = 1660,		
};

HX711_ChannelTypeDef hx_ch2 = {
	.sck_port	= HX711_SCK_2_GPIO_Port,		.sck_pin  = HX711_SCK_2_Pin,
	.data_port	= HX711_DATA_2_GPIO_Port,	.data_pin = HX711_DATA_2_Pin,
	.buffer = {0}, .medleng = 0,
	.xishu = 0.13145, .convert = 1000,			
};


u32 Read_hx711(HX711_ChannelTypeDef*ch)
{
	volatile u32 data = 0;
	u8 i = 0;	
	HAL_GPIO_WritePin(ch->sck_port,ch->sck_pin,GPIO_PIN_RESET);
	while(HAL_GPIO_ReadPin(ch->data_port,ch->data_pin) == GPIO_PIN_SET)
	{}
	delay_us(1);
	for(i = 0; i < 24; ++i)
	{
		HAL_GPIO_WritePin(ch->sck_port,ch->sck_pin,GPIO_PIN_SET);
		data <<= 1;
		
		delay_us(1);
		HAL_GPIO_WritePin(ch->sck_port,ch->sck_pin,GPIO_PIN_RESET);
		data |= HAL_GPIO_ReadPin(ch->data_port,ch->data_pin);
		delay_us(1);
	}
	HAL_GPIO_WritePin(ch->sck_port,ch->sck_pin,GPIO_PIN_SET);
	data = data^0x800000;
	delay_us(1);
	HAL_GPIO_WritePin(ch->sck_port,ch->sck_pin,GPIO_PIN_RESET);
	
	
	return data;
}

void Get_Tare(HX711_ChannelTypeDef*ch)//获取皮重
{
	u32 hx711_dat;
	u8 i,j;
	for(i=0;i<MEDIAN_LEN;i++)
	{
		hx711_dat=Read_hx711(ch);	        	//HX711AD转换数据处理
		if(ch->medleng == 0)                    //缓存的第1个元素,直接放入,不需要排序
		{ 
			ch->buffer[0] = hx711_dat; ch->medleng = 1; 
		}
		else                            	//插入排序算法,按从小到大的顺序排列 
		{  
			for(j = 0; j < ch->medleng; j ++)  
			{
				if( ch->buffer[j] > hx711_dat) 	// 轮询到的当前元素>AD值,则交换它们的值，xd为中间变量存放位置
				{ 
					xd = hx711_dat; hx711_dat = ch->buffer[j]; ch->buffer[j] = xd;
				}
			}
			ch->buffer[ch->medleng] = hx711_dat; 	//把轮询出较大的数放入缓存的后面.
			ch->medleng++;
		}		
		if(ch->medleng >= MEDIAN_LEN) 		    //ADC采样的数据个数达到中值滤波要求的数据个数
		{
			hx711_dat = ch->buffer[MEDIAN];	    //最终重量取中值滤波数组的中间值
			ch->medleng = 0; 
		}
	}
	ch->pi_weight=hx711_dat;
}


void Get_Weight(HX711_ChannelTypeDef*ch)	  //获取被测物体重量
{
	u32 a;
	float aa;	
	a=Read_hx711(ch);		   //HX711数据采集函数
	if(a > ch->pi_weight)
	{	
		aa=a-ch->pi_weight;			     //测得的重量值减去皮重
		ch->weight = ((float)aa*ch->xishu) / ch->convert;
	}
	else{	
		aa=ch->pi_weight-a;			     //测得的重量值减去皮重
		ch->weight = -(((float)aa*ch->xishu) / ch->convert);
	}
}

void CalibraWeight(HX711_ChannelTypeDef*ch, u8 calibrate_num)
{
	u32 a;
	float aa;	
	a=Read_hx711(ch);		   //HX711数据采集函数
	aa=a-ch->pi_weight;
	ch->convert = ((float)aa*ch->xishu) / calibrate_num;
}
