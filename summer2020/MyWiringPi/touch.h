#ifndef _TOUCH_H
#define _TOUCH_H


#define uint8_t char
#define uint16_t short int
#define uint32_t  int
#define u8 char
#define u16 short int

void GT9147_Init(void);
void GT9147_Config(void);	
u8 GT9147_Scan(void);

#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点
typedef struct 
{
	
	u16 x[CT_MAX_TOUCH]; 		//当前坐标
	u16 y[CT_MAX_TOUCH];		//电容屏有5组坐标，用x[i],y[i]存储第一次按下时的坐标. 													
	u8  sta;			//笔的状态 
								//b7:按下1/松开0; 
	              //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏支持的点数,固定为5点，每一点是否按下(0,表示未按下,1表示按下)
	u8 touchtype;
								//0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
								//1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
	
}_m_tp_dev;

extern _m_tp_dev tp_dev;
				
				


#endif

