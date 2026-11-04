#ifndef __HX711__H__
#define __HX711__H__

#include "main.h"

#define MEDIAN_LEN  11
#define MEDIAN      5


typedef struct {
    // 硬件引脚（每片HX711独立配置）
    GPIO_TypeDef *sck_port;
    uint16_t      sck_pin;
    GPIO_TypeDef *data_port;
    uint16_t      data_pin;
    
    // 单通道私有状态（完全隔离，不跨通道干扰）
    u32  buffer[MEDIAN_LEN];  // 中值滤波缓存
    u8   medleng;              // 当前缓存采样计数
    float xishu;               // 单通道独立校准系数
	u32 convert;			  //最终结果换算为g重
    float  pi_weight;            // 单通道皮重
    float  weight;               // 单通道最终重量值
} HX711_ChannelTypeDef;

extern HX711_ChannelTypeDef hx_ch1;
extern HX711_ChannelTypeDef hx_ch2;
extern u8 calibrate_num;
extern u8 CalWgtFlag;


extern u32 Read_hx711(HX711_ChannelTypeDef*ch);
extern void Get_Tare(HX711_ChannelTypeDef*ch);//获取皮重
extern void Get_Weight(HX711_ChannelTypeDef*ch);
extern void CalibraWeight(HX711_ChannelTypeDef*ch, u8 calibrate_num);

#endif





