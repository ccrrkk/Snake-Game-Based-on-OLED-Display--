/*
 * my_oled.h
 *
 * Created: 2024/5/11 22:56:49
 *  Author: crk
 */ 


#ifndef MY_OLED_H_
#define MY_OLED_H_

#define OLED_SCL_PIN     PC5
#define OLED_SDA_PIN     PC4
#define OLED_SCL_PORT    PORTC
#define OLED_SDA_PORT    PORTC
#define OLED_SCL_DDR     DDRC
#define OLED_SDA_DDR     DDRC
#define OLED_SCL_HIGH()   (OLED_SCL_PORT |= (1 << OLED_SCL_PIN))
#define OLED_SCL_LOW()    (OLED_SCL_PORT &= ~(1 << OLED_SCL_PIN))
#define OLED_SDA_HIGH()   (OLED_SDA_PORT |= (1 << OLED_SDA_PIN))
#define OLED_SDA_LOW()    (OLED_SDA_PORT &= ~(1 << OLED_SDA_PIN))
#define OLED_CMD         0
#define OLED_DATA        1
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define Brightness	0xFF
#define X_WIDTH 	128
#define OLED_ADDRESS 0x78 // OLED I2C地址
#define OLED_BACKLIGHT_ON 1



void TWI_Init(void) //twi 接口的初始化
{ //设置SCL的频率：1MHz cpu-50KHz scl,2M-100K,8M-400K
	TWSR = 0x00; //最低2位为预分频设置(00-1,01-4,10-16,11-64)
	TWBR = 0x02; //位率设置，fscl=cpu频率/(16+2*TWBR*预分频值)
	TWCR = (1<<TWEN); //开启TWI
}
void TWI_Start(void) //发送Start信号，开始本次TWI通信
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//发送Start信号
	while(!(TWCR &(1<<TWINT))); //等待Start信号发出
}
void TWI_Stop(void) //发送Stop信号，结束本次TWI通信
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//发送Stop信号
}
void TWI_Write(unsigned char uc_data) //向TWI接口发送8位数据
{
	TWDR = uc_data; //8位数据存放在TWDR
	TWCR = (1<<TWINT)|(1<<TWEN); //发送TWDR中的数据
	while(!(TWCR &(1<<TWINT))); //等待数据发出
}
unsigned char TWI_Read_With_ACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);//准备接收数据，并ACK
	while(!(TWCR &(1<<TWINT))); //等待接收数据
	return TWDR; //返回接收到的数据
}
unsigned char TWI_Read_With_NACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN); //准备接收数据，并NACK
	while(!(TWCR &(1<<TWINT))); //等待接收数据
	return TWDR; //返回接收到的数据
}
unsigned char TWI_Get_State_Info(void)
{
	unsigned char uc_status;
	uc_status = TWSR & 0xf8; //取状态寄存器高5位，低3位为0
	return uc_status; //返回状态寄存器高5位
}
// OLED写入函数，根据cmd参数判断发送命令还是数据
void OLED_WR_Byte(unsigned char dat, unsigned char cmd) {
	TWI_Start(); // 发送Start信号
	TWI_Write(OLED_ADDRESS); // 写入OLED地址，写入模式
	if (cmd == 0x00) {
		TWI_Write(0x00); // 数据模式
		} else {
		TWI_Write(0x40); // 命令模式
	}
	TWI_Write(dat); // 发送数据或命令
	TWI_Stop(); // 发送Stop信号
}

//开启OLED显示
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

void OLED_Set_Pos(unsigned char x, unsigned char y) {
	OLED_WR_Byte(0xb0 + (y & 0x07), OLED_CMD); // 设置页地址(0-7)
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD); // 设置列地址的高4位
	OLED_WR_Byte((x & 0x0f), OLED_CMD); // 设置列地址的低4位
}
void OLED_Clear(void)
{
	unsigned char i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA);
	} //更新显示
}
// OLED初始化函数
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	OLED_Set_Pos(0,0);
}



void OLED_Refresh(unsigned char GRAM[128][6])
{
	unsigned char i,n;
	for(i=1;i<7;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		for(n=0;n<128;n++)
		OLED_WR_Byte(GRAM[n][i],OLED_DATA);
	}
}

void OLED_DRAW(unsigned char x,unsigned char y,unsigned char GRAM[128][6]){//x:0-127;y:8-56
	unsigned char n,m;
	n=y/8+1;
	m=y%8;
	GRAM[x][n]|=0x01<<m;
}

void OLED_DEL(unsigned char x,unsigned char y,unsigned char GRAM[128][6]){
	unsigned char n,m;
	n=y/8+1;
	m=y%8;
	GRAM[x][n] &= ~(0x01<<m);
}

void OLED_DRAW_BIG(unsigned char big_x,unsigned char big_y,unsigned char GRAM[128][6]){
	for(unsigned char i=0;i<2;i++){
		for(unsigned char j=0;j<2;j++){
			OLED_DRAW(big_x*2+i,big_y*2+j,GRAM);
		}
	}
}

void OLED_DEL_BIG(unsigned char big_x,unsigned char big_y,unsigned char GRAM[128][6]){
	for(unsigned char i=0;i<2;i++){
		for(unsigned char j=0;j<2;j++){
			OLED_DEL(big_x*2+i,big_y*2+j,GRAM);
		}
	}
}


#endif /* MY_OLED_H_ */