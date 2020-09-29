#include "wiringPi.h"
#include "touch.h"
#include <stdio.h>




//SCL，29
int SCL_Flag=0;//0：未初始化；1：输出；3：输入
void SCL_Set(_Bool SetBit)
{
	if(SCL_Flag!=1)
	{SCL_Flag=1;
		pinMode(29, OUTPUT);
	}				

	
	if(SetBit) digitalWrite(29, HIGH);				
	else  digitalWrite(29, LOW);
}

//SDA，28
int SDA_Flag=0;//0：未初始化；1：输出；3：输入
void SDA_Set(_Bool SetBit)
{
	if(SDA_Flag!=1)
	{SDA_Flag=1;
		pinMode(28, OUTPUT);
	}
	if(SetBit) digitalWrite(28, HIGH);				
	else  digitalWrite(28, LOW);
}
_Bool/*ReadBit*/ SDA_Read(void)
{
	if(SDA_Flag != 2)
	{SDA_Flag=2;	
		pinMode(28, INPUT);
	}
	 
	return digitalRead(28);	/* 读SDA口线状态 */
}

//RST,25
int RST_Flag=0;//0：未初始化；1：输出；3：输入
void RST_Set(_Bool SetBit)
{
	if(RST_Flag!=1)
	{RST_Flag=1;
		pinMode(25, OUTPUT);
	}
	if(SetBit) digitalWrite(25, HIGH);				
	else  digitalWrite(25, LOW);
}

//INT,24
int INT_Flag=0;//0：未初始化；1：输出；3：输入
void INT_Set(_Bool SetBit)
{
	if(INT_Flag!=1)
	{INT_Flag=1;
		pinMode(24, OUTPUT);
	}
	if(SetBit) digitalWrite(24, HIGH);				
	else  digitalWrite(24, LOW);
}
_Bool/*ReadBit*/ INT_Read(void)
{
	if(INT_Flag != 2)
	{INT_Flag=2;
		
		pinMode(24, INPUT);
	}
	 
	return digitalRead(24);	/* 读SDA口线状态 */
}
//电容触摸芯片IIC接口初始化
void I2C_GPIO_Config(void)
{	
 
}

	
//控制I2C速度的延时
void I2C_Delay(void)
{
	delayMicroseconds(20);
	/*
      delayMicroseconds() #高精度微秒延时
      delay() #毫秒延时。
    */
}
									
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


/*
***********************************
*	函 数 名: I2C_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
***********************************
*/
void I2C_Start(void)
{
	
/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */	
	SDA_Set(1);	  	  
	SCL_Set(1);
	I2C_Delay();
	
	
 	SDA_Set(0);//START:when CLK is high,DATA change form high to low
	SCL_Set(1);	
	I2C_Delay();

	SDA_Set(0);		
	SCL_Set(0);
	I2C_Delay();

	SDA_Read();//主机释放总线，让等下从机去改变SDA电平
	SCL_Set(0);	
	I2C_Delay();
}	



/*
***********************************
*	函 数 名: I2C_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
***********************************
*/
void I2C_Stop(void)
{
/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	
	SDA_Read(); //主机释放总线，让等下从机去改变SDA电平	
	SCL_Set(1);
	I2C_Delay();	
	
	SDA_Set(0);
	SCL_Set(1);
	I2C_Delay();
	
	SDA_Set(1);
	SDA_Set(1);
	I2C_Delay();
}

void I2C_SendBit(_Bool SendBit)
{
	
//这是SDA数据切换的中间步骤
		SDA_Read(); //主机释放总线，让自己等下去改变SDA电平	
		SCL_Set(0);
		I2C_Delay();
//写SDA	

		SDA_Set(SendBit);
		
		SCL_Set(0);
		
		I2C_Delay();
//保持SDA	
		SDA_Set(SendBit);
		
		SCL_Set(1);
		I2C_Delay();		
//保持SDA
		SDA_Set(SendBit);
		
		SCL_Set(0);
		
		I2C_Delay();
		
//这是SDA数据切换的中间步骤
		SDA_Read(); //主机释放总线，让等下主机或者从机去改变SDA电平	
		SCL_Set(0);
		I2C_Delay();		
		
}





_Bool/*ReceiveBit*/ I2C_ReceiveBit(void)
{
	
//这是SDA数据切换的中间步骤
		SDA_Read(); //主机释放总线，让等下从机去改变SDA电平	
		SCL_Set(0);
		I2C_Delay();
//让从机写SDA	

		SDA_Read(); //主机释放总线，让从机去改变SDA电平	
		
		SCL_Set(0);
		
		I2C_Delay();
//让从机写SDA	

		SDA_Read();//主机释放总线，让从机去改变SDA电平	
		SCL_Set(1);
		I2C_Delay();	
	
	
//读SDA
		_Bool ReceiveBit=SDA_Read();
		SCL_Set(1);
		I2C_Delay();	
		
		
//让从机写SDA	
		SDA_Read(); //主机释放总线，让从机去改变SDA电平	
		
		SCL_Set(0);
		
		I2C_Delay();
		
//这是SDA数据切换的中间步骤
		SDA_Read(); //主机释放总线，让等下主机或者从机，去改变SDA电平	
		SCL_Set(0);
		I2C_Delay();


																							return ReceiveBit;
		
}

/*-----------------------------------------------------------------------------*/
/*
****************************************
*	函 数 名: I2C_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
****************************************
*/
void I2C_SendByte(uint8_t SendByte)
{

	
	/* 先发送字节的高位bit7 */
	for (uint8_t i = 0; i < 8; i++)
	{	
		
		I2C_SendBit( SendByte & (0x80>>i)  );//0x1000 0000//MSB，高位先行
		//本来应该是如下的
		//I2C_SendBit( (SendByte & (0x80>>i) ) >> (7-i)  );
		//但为什么上面又行了呢？因为我们后面用到的if强制转换会把  非0->1,0->0
	}
	
}

/*
***************************************************
*	函 数 名: I2C_ReceiveAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
***************************************************
*/
				#define Ack_Effective 0
				#define Ack_Invalid   1
_Bool/*ReceiveAck*/ I2C_ReceiveAck(void)
{
	_Bool  ReceiveAck= I2C_ReceiveBit();
	if(ReceiveAck==Ack_Invalid) printf("无效\r\n");
	return  ReceiveAck;
}

_Bool/*ReceiveAck*/ I2C_SendByte_and_ReceiveAck(uint8_t SendByte)
{	
	I2C_SendByte(SendByte );	
	 
	_Bool  ReceiveAck= I2C_ReceiveAck();
	

	return ReceiveAck;
}
/*-----------------------------------------------------------------------------*/

/*
**********************************************
*	函 数 名: I2C_ReceiveByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
**********************************************
*/
uint8_t/*ReceiveByte*/ I2C_ReceiveByte(void)
{uint8_t ReceiveByte=0;
	
	
	/* 读到第1个bit为数据的bit7 */
	for (uint8_t i = 0; i < 8; i++)
	{	
		
		ReceiveByte <<= 1;ReceiveByte|=I2C_ReceiveBit();//MSB，高位先行		
	}
	
	return ReceiveByte;
}

/*
********************************************************
*	函 数 名: I2C_SendAck
*	功能说明: CPU产生一个ACK信号,发送0表示有效应答，发送1为无效
*	形    参：无
*	返 回 值: 无
********************************************************
*/
				#define Ack_Effective 0
				#define Ack_Invalid   1
void I2C_SendAck(_Bool SendAck)
{
	I2C_SendBit(SendAck);
}

uint8_t/*ReceiveByte*/ I2C_ReceiveByte_and_SendAck(_Bool SendAck)
{
	uint8_t ReceiveByte= I2C_ReceiveByte();
	I2C_SendAck( SendAck);
	
	return  ReceiveByte;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
										
/////////////////////////////////////////////////////////////////////////////////////////////////////
//初始化GT9147触摸屏
//返回值:0,初始化成功;1,初始化失败 
void GT9147_Init(void)
{
	I2C_GPIO_Config();      	//初始化电容屏的I2C总线  
	
	//下面就当没看见
	INT_Set(1);//先推挽输出，说是用来设置地址的
		
	RST_Set(0);				//复位
	delay(10);
 	RST_Set(1);				//释放复位 
	delay(10);
 
	INT_Read();//原子写的是下拉输入，我们改为浮空输入，不知道有没有问题，反正以后貌似就再也没用过这个输入引脚
	
	delay(100);	
	

}




//这里我们本应该写一个通用的函数
//但是我们写不出来好吗，所以放弃
/*
void complete_one_process(void)
{

	void I2C_Start(void);

	_Bool I2C_SendByte_and_ReceiveAck(uint8_t SendByte)
	uint8_t I2C_ReceiveByte_and_SendAck(_Bool SendAck)

	void I2C_Stop(void);
	
	
//////////////////////////////////////////////////////////////////	
	
	void I2C_Start(void);

	_Bool I2C_SendByte_and_ReceiveAck(uint8_t SendByte)
	uint8_t I2C_ReceiveByte_and_SendAck(_Bool SendAck)

	void I2C_Stop(void);
	
	
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//下面就当一个枚举类型
		//I2C读写命令	
		#define GT_CMD_WR 		0X28     	//写命令
		#define GT_CMD_RD 		0X29		//读命令
			
		//GT9147 部分寄存器定义 
		#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
		#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
		#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
		#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

		#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
		#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
		#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
		#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
		#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
		#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址 


//向GT9147写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
void GT9147_WR_Reg(u16 reg,u8 *buf,u8 len)
{

	I2C_Start();
	
	
	I2C_SendByte_and_ReceiveAck(GT_CMD_WR);//发送写命令
	
	
	I2C_SendByte_and_ReceiveAck(reg>>8);//发送高8位地址

	I2C_SendByte_and_ReceiveAck(reg&0XFF);//发送低8位地址
 
	for(int i=0;i<len;i++)
	{	   
    I2C_SendByte_and_ReceiveAck(buf[i]);  	//发数据
	}
	
	
  I2C_Stop();					//产生一个停止条件	    
 
}
//从GT9147读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len)
{

 	I2C_Start();

	
 	I2C_SendByte_and_ReceiveAck(GT_CMD_WR);   //发送写命令 	 

	
 	I2C_SendByte_and_ReceiveAck(reg>>8);   	//发送高8位地址

	
 	I2C_SendByte_and_ReceiveAck(reg&0XFF);   	//发送低8位地址
 

	I2C_Stop();//产生一个停止条件
///////////////////////////////////////////////////////////////////	
	
 	I2C_Start();
	
	I2C_SendByte_and_ReceiveAck(GT_CMD_RD);   //发送读命令		   	 
  
	for(int i=0;i<len;i++)
	{	   
    	buf[i]=I2C_ReceiveByte_and_SendAck(!(i==(len-1)?0:1)); //发数据	  
	} 
    I2C_Stop();//产生一个停止条件    
}
////////////////////////////////////////////////////////////////////////////////////


					//发送GT9147配置参数，
					//mode:0,参数不保存到flash
					//     1,参数保存到flash

											//宏定义
											//GT9147配置参数表
											//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
											//flash原有版本号,才会更新配置.
											const u8 GT9147_CFG_TBL[]=
											{ 
												0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
												0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
												0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
												0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
												0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
												0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
												0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
												0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
												0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
												0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
												0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
												0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
												0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
												0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
												0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
												0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
												0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
												0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
												0XFF,0XFF,0XFF,0XFF,
											}; 
					u8 GT9147_Send_Cfg(u8 mode)
					{

								
						GT9147_WR_Reg(GT_CFGS_REG,(u8*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//发送寄存器配置
						
												u8 buf[2];

												//计算校验和
												buf[0]=0;
												
												for(int i=0;i<sizeof(GT9147_CFG_TBL);i++)
												{
													buf[0]+=GT9147_CFG_TBL[i];//计算校验和
												}
												
												 buf[0]=(~buf[0])+1;
												
												//是否写入到GT9147 FLASH?  即是否掉电保存
												buf[1]=mode;	
												
						GT9147_WR_Reg(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
						
						return 0;
					}
																	
//更新GT9147flash版本版本,但我测试，完全没有必要
void GT9147_Config(void)
{
		{
			u8 temp[5];
			temp[4]='\0';
			GT9147_RD_Reg(GT_PID_REG,temp,4);	//读取产品ID

			//printf("CTP ID:%s\r\n",temp);		//打印ID,ID==9147
		}
		
		
		u8 temp6[1];
		temp6[0]=0X02;			
		GT9147_WR_Reg(GT_CTRL_REG,temp6,1);//软复位GT9147
		
		
		
		u8 temp9[1];
 		GT9147_RD_Reg(GT_CFGS_REG,temp9,1);//读取GT_CFGS_REG寄存器
		//printf("Default Ver:%d\r\n",temp9[0]);
		
		if(temp9[0]<0X60)//默认版本比较低,需要更新flash配置
		{
			GT9147_Send_Cfg(1);//更新并保存配置
			delay(10);
		}
		
		GT9147_RD_Reg(GT_CFGS_REG,temp9,1);//读取GT_CFGS_REG寄存器
		//printf("After Renew Ver:%d\r\n",temp9[0]);
		
		u8 temp3[1];
		temp3[0]=0X00;	 
		GT9147_WR_Reg(GT_CTRL_REG,temp3,1);	//结束复位  
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//这个其实相当于一个返回值，并不是所谓的对象


_m_tp_dev tp_dev=
{
	0	 
};




/*别去读了，读不懂,知道如下即可

			for(u8 t=0;t<CT_MAX_TOUCH;t++)
			{
				if((tp_dev.sta)&(1<<t))//有触屏点按下了
				{
						ILI9341_SetPointPixel(tp_dev.x[t],tp_dev.y[t],RED);					
				}
			}	

      tp_dev.x[t],tp_dev.y[t] 在没有按下的时候=-1=0xffff,所以一般连tp_dev.sta都可以不管
*/

//扫描触摸屏(采用查询方式)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸


		//一些宏定义
					const u16 GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};

					#define TP_PRES_DOWN 0x80  		//触屏被按下	  
					#define TP_CATH_PRES 0x40  		//有按键按下了

					
					//#include "Parameter_TypeDef.h"
					#define  DIR  1  //0：竖屏  1：横屏
					#define  WIDTH  800
					#define  HEIGHT 480
					
u8 GT9147_Scan(void)
{
	
	//mode:0,正常扫描.
	u8 mode=0;
	u8 buf[4];
	int i=0;
	u8 res=0;
	u8 temp;
	u8 tempsta;
	
	
 	static u8 t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{

		GT9147_RD_Reg(GT_GSTID_REG,&mode,1);	//读取触摸点的状态 
		
 		if(mode&0X80&&((mode&0XF)<6))
		{
			u8 temp=0;
			GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//清标志 		
		}		
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta=tp_dev.sta;			//保存当前的tp_dev.sta值
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//保存触点0的数据
			tp_dev.y[4]=tp_dev.y[0];
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//触摸有效?
				{
					GT9147_RD_Reg(GT9147_TPX_TBL[i],buf,4);	//读取XY坐标值
					tp_dev.touchtype=DIR;//横屏还是竖屏
					if(tp_dev.touchtype&0X01)//横屏
					{
						tp_dev.y[i]=((u16)buf[1]<<8)+buf[0];
						tp_dev.x[i]=800-(((u16)buf[3]<<8)+buf[2]);
					}else
					{
						tp_dev.x[i]=((u16)buf[1]<<8)+buf[0];
						tp_dev.y[i]=((u16)buf[3]<<8)+buf[2];
					}  
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 
			res=1;
			if(tp_dev.x[0]>WIDTH||tp_dev.y[0]>HEIGHT)//非法数据(坐标超出了)
			{ 
				if((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}else					//非法数据,则忽略此次数据(还原原来的)  
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					mode=0X80;		
					tp_dev.sta=tempsta;	//恢复tp_dev.sta
				}
			}else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}	 
	} 	
	if(t>240)t=10;//重新从10开始计数
	return res;
}
//////////////////////////////////////////////////
