#include "wiringPi.h"
#include "ips.h"





#define	LCD_SCK_INIT  	pinMode(15, OUTPUT);
#define	LCD_SCK_SET  	digitalWrite(15, HIGH);delayMicroseconds(1);
#define	LCD_SCK_CLR  	digitalWrite(15, LOW);delayMicroseconds(1);


#define	LCD_MOSI_INIT  	pinMode(16, OUTPUT);
#define	LCD_MOSI_SET  	digitalWrite(16, HIGH);delayMicroseconds(1);
#define	LCD_MOSI_CLR  	digitalWrite(16, LOW);delayMicroseconds(1);

#define	LCD_RST_INIT  	pinMode(1, OUTPUT);
#define	LCD_RST_SET  	digitalWrite(1, HIGH);delayMicroseconds(1);
#define	LCD_RST_CLR  	digitalWrite(1, LOW);delayMicroseconds(1);

 
#define	LCD_DC_INIT  	pinMode(4, OUTPUT);
#define	LCD_DC_SET  	digitalWrite(4, HIGH);delayMicroseconds(1);
#define	LCD_DC_CLR  	digitalWrite(4, LOW);delayMicroseconds(1);

#define	LCD_NSS_INIT  	pinMode(5, OUTPUT);
#define	LCD_NSS_SET  	digitalWrite(5, HIGH);delayMicroseconds(1);
#define	LCD_NSS_CLR  	digitalWrite(5, LOW);delayMicroseconds(1);


void Lcd_Reset(void)
{
    LCD_RST_CLR;
    delay(100);
    LCD_RST_SET;
    delay(50);
}

//LCD Init For 1.44Inch LCD Panel with ST7735R.
void Lcd_Init(void)
{	
LCD_SCK_INIT;
LCD_MOSI_INIT;
LCD_RST_INIT;
LCD_DC_INIT;
LCD_NSS_INIT;

Lcd_Reset(); //Reset before LCD Init.

//LCD Init For 1.44Inch LCD Panel with ST7735R.
Lcd_WriteIndex(0x11);//Sleep exit 
delay (120);
Lcd_WriteIndex(0x21); 
Lcd_WriteIndex(0x21); 

Lcd_WriteIndex(0xB1); 
Lcd_WriteData(0x05);
Lcd_WriteData(0x3A);
Lcd_WriteData(0x3A);

Lcd_WriteIndex(0xB2);
Lcd_WriteData(0x05);
Lcd_WriteData(0x3A);
Lcd_WriteData(0x3A);

Lcd_WriteIndex(0xB3); 
Lcd_WriteData(0x05);  
Lcd_WriteData(0x3A);
Lcd_WriteData(0x3A);
Lcd_WriteData(0x05);
Lcd_WriteData(0x3A);
Lcd_WriteData(0x3A);

Lcd_WriteIndex(0xB4);
Lcd_WriteData(0x03);

Lcd_WriteIndex(0xC0);
Lcd_WriteData(0x62);
Lcd_WriteData(0x02);
Lcd_WriteData(0x04);

Lcd_WriteIndex(0xC1);
Lcd_WriteData(0xC0);

Lcd_WriteIndex(0xC2);
Lcd_WriteData(0x0D);
Lcd_WriteData(0x00);

Lcd_WriteIndex(0xC3);
Lcd_WriteData(0x8D);
Lcd_WriteData(0x6A);   

Lcd_WriteIndex(0xC4);
Lcd_WriteData(0x8D); 
Lcd_WriteData(0xEE); 

Lcd_WriteIndex(0xC5);  /*VCOM*/
Lcd_WriteData(0x0E);    

Lcd_WriteIndex(0xE0);
Lcd_WriteData(0x10);
Lcd_WriteData(0x0E);
Lcd_WriteData(0x02);
Lcd_WriteData(0x03);
Lcd_WriteData(0x0E);
Lcd_WriteData(0x07);
Lcd_WriteData(0x02);
Lcd_WriteData(0x07);
Lcd_WriteData(0x0A);
Lcd_WriteData(0x12);
Lcd_WriteData(0x27);
Lcd_WriteData(0x37);
Lcd_WriteData(0x00);
Lcd_WriteData(0x0D);
Lcd_WriteData(0x0E);
Lcd_WriteData(0x10);

Lcd_WriteIndex(0xE1);
Lcd_WriteData(0x10);
Lcd_WriteData(0x0E);
Lcd_WriteData(0x03);
Lcd_WriteData(0x03);
Lcd_WriteData(0x0F);
Lcd_WriteData(0x06);
Lcd_WriteData(0x02);
Lcd_WriteData(0x08);
Lcd_WriteData(0x0A);
Lcd_WriteData(0x13);
Lcd_WriteData(0x26);
Lcd_WriteData(0x36);
Lcd_WriteData(0x00);
Lcd_WriteData(0x0D);
Lcd_WriteData(0x0E);
Lcd_WriteData(0x10);

Lcd_WriteIndex(0x3A); 
Lcd_WriteData(0x05);

Lcd_WriteIndex(0x36);
Lcd_WriteData(0xA8);//

Lcd_WriteIndex(0x29); 


LCD_NSS_CLR ;	 
}




//1.1
//向SPI总线传输一个8位数据
void  SPI_WriteData(uint8_t Data)
{
    unsigned char i=0;
    for(i=8;i>0;i--)
    {
        if(Data&0x80)
		{	
            LCD_MOSI_SET; //输出数据
		}
        else 
		{
            LCD_MOSI_CLR;
		}
         
        LCD_SCK_CLR;       
        LCD_SCK_SET;
        
        
    Data<<=1; 
    }
}
//向液晶屏写一个8位指令
void Lcd_WriteIndex(uint8_t Index)
{
   //SPI 写命令时序开始
   LCD_DC_CLR;
	 SPI_WriteData(Index);
}

//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data)
{
   LCD_DC_SET;
   SPI_WriteData(Data); 
}

//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{

   LCD_DC_SET;
	 SPI_WriteData(Data>>8); 	//写入高8位数据
	 SPI_WriteData(Data); 			//写入低8位数据

}







//1.1.1
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
用例：
		Lcd_Init();

		Lcd_Clear(GRAY0);
		
		Gui_DrawPoint(0,0,RED);
		Gui_DrawPoint(X_MAX_PIXEL-1,Y_MAX_PIXEL-1,RED);

		for(int x=0;i<50;i+=1)//试试i+=2；
		for(int y=0;j<50;j+=1)
		{
			Gui_DrawPoint(x  +50,y  +20,RED);
		}														
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t fc)
//fc：front color
//bc：back color
{

				uint16_t x_start=x;
				uint16_t y_start=y;
				uint16_t x_end=x;
				uint16_t y_end=y;
	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+1);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+1);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+0x1A);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+0x1A);	
	
	Lcd_WriteIndex(0x2C);
	LCD_WriteData_16Bit(fc);
}   






//1.1.1.1

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
用例：		
		Lcd_Init();
		Lcd_Clear(GRAY0);
*************************************************/
void Lcd_Clear(uint16_t bc)   
//fc：front color
//bc：back color	
{	

				uint16_t x_start=0;
				uint16_t y_start=0;
				uint16_t x_end=X_MAX_PIXEL-1;
				uint16_t y_end=Y_MAX_PIXEL-1;
	
		Lcd_WriteIndex(0x2a);
		Lcd_WriteData(0x00);
		Lcd_WriteData(x_start+1);
		Lcd_WriteData(0x00);
		Lcd_WriteData(x_end+1);

		Lcd_WriteIndex(0x2b);
		Lcd_WriteData(0x00);
		Lcd_WriteData(y_start+0x1A);
		Lcd_WriteData(0x00);
		Lcd_WriteData(y_end+0x1A);	

		Lcd_WriteIndex(0x2C);
		for(unsigned int i=0;i<X_MAX_PIXEL;i++)
			for(unsigned int m=0;m<Y_MAX_PIXEL;m++)
			{	
				LCD_WriteData_16Bit(bc);
			}   
}																								

//1.1.1.2
/*
画线函数，使用Bresenham 画线算法
用例：
		Lcd_Init();
		Lcd_Clear(GRAY0);

		Gui_DrawLine(0,0,X_MAX_PIXEL,Y_MAX_PIXEL-50,RED);
		
*/
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	dx = x1-x0;//计算x距离
	dy = y1-y0;//计算y距离

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;//*2
	dy2 = dy << 1;//*2

	if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{//且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//要画的点数不会超过x距离
		{
			//画点
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //是否需要增加y坐标值
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{//以y轴为递增画点
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}

