#ifndef __IPS_H
#define __IPS_H 			   

#define uint8_t char
#define uint16_t short int


//初始化
void Lcd_Init(void);




//1.1指令
//向液晶屏写一个8位指令
void Lcd_WriteIndex(uint8_t Index);
//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data);
//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data);





//1.1.1
#define X_MAX_PIXEL	        160
#define Y_MAX_PIXEL	        80


#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t fc);
//1.1.1.1
void Lcd_Clear(uint16_t bc)   ;
//1.1.1.2
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color) ;

#endif		

