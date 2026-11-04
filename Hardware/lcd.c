#include <lcd.h>
#include <font.h>
#include <sys.h>

_lcd_dev lcddev;

u16 POINT_COLOR = 0x0000;		//画笔颜色
u16 BACK_COLOR  = 0xFFFF;  		//背景色（白色）

void LCD_WR_REG(u16 regval)
{
	LCD->LCD_REG = regval;
}

void LCD_WR_DATA(u16 data)
{
	LCD->LCD_RAM=data;
}

u16 LCD_RD_DATA(void)
{
	u16 ram;			//防止被优化
	ram=LCD->LCD_RAM;	
	return ram;	 
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
 if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }
    else     //7796/5310/7789等设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }

    LCD->LCD_REG=lcddev.wramcmd; 
		
		if(lcddev.id == 0X7789){LCD->LCD_RAM=color>>8;	
		LCD->LCD_RAM=color;}
		else 
			
    LCD->LCD_RAM=color; 
}

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}


//设置LCD的自动扫描方向
//dir:0~7,代表8个方向(具体定义见lcd.h)
//7796/5310/5510/7789等IC已经实际测试
//注意:其他函数可能会受到此函数设置的影响,
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.  
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
    //横屏时，IC改变扫描方向！
    if (lcddev.dir == 1 )
    {
        switch (dir)   //方向转换
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }

    switch (dir)
    {
        case L2R_U2D://从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U://从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D://从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U://从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R://从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L://从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R://从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L://从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    if (lcddev.id == 0X5510)dirreg = 0X3600;
    else dirreg = 0X36;

    if (lcddev.id == 0X7796)   //7796 & 7789 要设置BGR位
    {
        regval |= 0X08;
    }

    LCD_WriteReg(dirreg, regval);

    
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    

    //设置显示区域(开窗)大小
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    lcddev.dir = dir;       //竖屏/横屏

    if (dir == 0)           //竖屏
    {
		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;
        if (lcddev.id == 0X5310||lcddev.id == 0X7796)    //如果是5310/7796 则表示是 320*480分辨率
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }  
    }
    else     //横屏
    {
		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;
        if (lcddev.id == 0X5310||lcddev.id == 0X7796)    //如果是5310/7796 则表示是 320*480分辨率
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR);     //默认扫描方向
}


//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if (lcddev.id == 0X5510)     //5510设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else     //7796/5310/7789设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
}



void LCD_Init(void)
{
	LCD_RES_Set();
	HAL_Delay(10);
	LCD_RES_Clr();
	HAL_Delay(20);
	LCD_RES_Set();
	HAL_Delay(50);
	LCD_WR_REG(0XD4);				   
	lcddev.id=LCD_RD_DATA();//dummy read  
	lcddev.id=LCD_RD_DATA();//????0X01	 
	lcddev.id=LCD_RD_DATA();//????0X53	
	lcddev.id<<=8;	 //你好
	lcddev.id|=LCD_RD_DATA();	//???????0X10	
	printf(" LCD ID:%x\r\n",lcddev.id); //???LCD ID
	

LCD_WR_REG(0xED);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0xFE);

LCD_WR_REG(0xEE);
LCD_WR_DATA(0xDE);
LCD_WR_DATA(0x21);

LCD_WR_REG(0xF1);
LCD_WR_DATA(0x01);
LCD_WR_REG(0xDF);
LCD_WR_DATA(0x10);

//VCOMvoltage//
LCD_WR_REG(0xC4);
LCD_WR_DATA(0x8F);	  //5f

LCD_WR_REG(0xC6);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xE2);
LCD_WR_DATA(0xE2);
LCD_WR_DATA(0xE2);
LCD_WR_REG(0xBF);
LCD_WR_DATA(0xAA);

LCD_WR_REG(0xB0);
LCD_WR_DATA(0x0D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x0D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x11);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x19);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x21);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x5D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x5D);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB1);
LCD_WR_DATA(0x80);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x8B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x96);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x03);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB4);
LCD_WR_DATA(0x8B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x96);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA1);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB5);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x03);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x04);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB6);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3F);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x5E);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x64);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x8C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xAC);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDC);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x70);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x90);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xEB);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDC);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xB8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xBA);
LCD_WR_DATA(0x24);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC1);
LCD_WR_DATA(0x20);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x54);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xFF);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC2);
LCD_WR_DATA(0x0A);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x04);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC3);
LCD_WR_DATA(0x3C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3A);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x39);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x37);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x36);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x32);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2F);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x29);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x26);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x24);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x24);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x23);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x36);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x32);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2F);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x29);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x26);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x24);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x24);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x23);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC4);
LCD_WR_DATA(0x62);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x05);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x84);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x18);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA4);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x18);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x50);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x0C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x17);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x95);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xE6);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC5);
LCD_WR_DATA(0x32);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x65);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x76);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC6);
LCD_WR_DATA(0x20);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x17);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xC9);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE0);
LCD_WR_DATA(0x16);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x1C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x21);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x36);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x46);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x52);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x64);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x7A);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x8B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB9);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC4);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xCA);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD9);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xE0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE1);
LCD_WR_DATA(0x16);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x1C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x22);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x36);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x45);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x52);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x64);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x7A);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x8B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB9);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC4);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xCA);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xE0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE2);
LCD_WR_DATA(0x05);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x0B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x1B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x34);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x4F);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x61);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x79);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x97);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA6);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD1);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD6);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDD);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);
LCD_WR_REG(0xE3);
LCD_WR_DATA(0x05);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x1C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x33);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x50);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x62);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x78);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x97);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA6);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC7);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD1);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD5);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDD);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE4);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x2A);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x4B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x5D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x74);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x84);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x93);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xBE);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC4);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xCD);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDD);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);
LCD_WR_REG(0xE5);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x29);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x3C);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x4B);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x5D);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x74);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x84);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x93);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xA2);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xB3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xBE);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xC4);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xCD);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xD3);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xDC);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xF3);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE6);
LCD_WR_DATA(0x11);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x34);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x56);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x76);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x77);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x66);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xBB);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x66);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x45);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x43);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE7);
LCD_WR_DATA(0x32);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x76);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x66);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x67);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x67);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x87);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xBB);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x77);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x56);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x23);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x33);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x45);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE8);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x87);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x77);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x66);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xAA);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0xBB);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x99);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x66);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x55);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xE9);
LCD_WR_DATA(0xAA);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0x00);
LCD_WR_DATA(0xAA);

LCD_WR_REG(0xCF);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xF0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x50);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xF3);
LCD_WR_DATA(0x00);

LCD_WR_REG(0xF9);
LCD_WR_DATA(0x06);
LCD_WR_DATA(0x10);
LCD_WR_DATA(0x29);
LCD_WR_DATA(0x00);

LCD_WR_REG(0x3A);
LCD_WR_DATA(0x55);	//66

LCD_WR_REG(0x11);
 HAL_Delay(100);
LCD_WR_REG(0x29);
LCD_WR_REG(0x35);
LCD_WR_DATA(0x00);

LCD_WR_REG(0x51);
LCD_WR_DATA(0xFF);
LCD_WR_REG(0x53);
LCD_WR_DATA(0x2C);
LCD_WR_REG(0x55);
LCD_WR_DATA(0x82);
LCD_WR_REG(0x2c);

LCD_Display_Dir(0);		//默认为竖屏
LCD_BL_Set();
LCD_Clear(WHITE);

}

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width; 
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);			//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		if(lcddev.id == 0X7789){		LCD->LCD_RAM=color>>8;	
		LCD->LCD_RAM=color;	}
		else
		LCD->LCD_RAM=color;	
	} 
}


//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else if(size==32)temp=asc2_3216[num][t];	//调用3216字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
}



//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }  
}


