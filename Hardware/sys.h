#ifndef __SYS__H__
#define __SYS__H__

#include "main.h"
#include "stdio.h"

#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_SET)//RES


#define LCD_BL_Clr()  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_RESET)//RES
#define LCD_BL_Set()  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET)//RES


#define HX711_SCK_Clr()  HAL_GPIO_WritePin(HX711_SCK_1_GPIO_Port,HX711_SCK_1_Pin,GPIO_PIN_RESET)//RES
#define HX711_SCK_Set()  HAL_GPIO_WritePin(HX711_SCK_1_GPIO_Port,HX711_SCK_1_Pin,GPIO_PIN_SET)//RES

#define HX711_DATA_1_Red() HAL_GPIO_ReadPin(HX711_DATA_1_GPIO_Port,HX711_DATA_1_Pin)



#endif

