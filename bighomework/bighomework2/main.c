/*
 * main.c
 *
 * Created: 2024/5/11 22:56:30
 *  Author: crk
 */ 

#include<stdlib.h>
#include<time.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <stdlib.h>

#include "my_oled.h" 
#include "dp.h"

#define ROW 128
#define COL 48
#define AIR 0
#define WALL 1 
#define FOOD 2 
#define HEAD 3 
#define BODY 4 

#define UP 1
#define DOWN 3
#define LEFT 2
#define RIGHT 0
// 定义蛇身体节点结构体
struct Body {
	unsigned char x;    
	unsigned char y;    
}body[10];

// 定义蛇结构体
struct Snake {
	unsigned char x;  // 蛇头的横坐标
	unsigned char y;  // 蛇头的纵坐标
	unsigned char length; // 蛇的长度
};

struct Snake snake;
unsigned char count=0;//触摸开关的触摸次数
unsigned char grade=0;//初始得分
unsigned char foodx;
unsigned char foody;
unsigned char GRAM[128][6];
//初始化界面

void InitInterface()
{
	for(unsigned char i=0;i<24;i++){
		OLED_DRAW_BIG(1,i,GRAM);
		OLED_DRAW_BIG(63,i,GRAM);
	}
	for(unsigned char j=1;j<63;j++){
		OLED_DRAW_BIG(j,0,GRAM);
		OLED_DRAW_BIG(j,23,GRAM);
	}
}
void InitSnake()
{
	snake.length = 3; 
	snake.x = 32; 
	snake.y = 12;
	body[0].x = 31;
	body[0].y =12;
	body[1].x =30;
	body[1].y =12;
}


unsigned char judge(unsigned char x,unsigned char y){
	if(x==0||x==63||y==0||y==23) return WALL;
	for(unsigned char i=0;i<snake.length;i++){
		if(body[i].x==x&&body[i].y==y) return BODY;
	}
	return AIR;
}


unsigned char ranNum(unsigned char x,unsigned char y){
	return rand()%(y-x)+x;
}

void RandFood()
{
	unsigned char i, j;
	do
	{
		i = ranNum(1,62);
		j = ranNum(1,22);
	} while (judge(i,j)!=AIR); 
	foodx=i;
	foody=j;
	OLED_DRAW_BIG(foodx,foody,GRAM);
	OLED_Refresh(GRAM);
}
//打印蛇与覆盖蛇
void DrawSnake(unsigned char flag)
{
	if (flag == 1) //打印蛇
	{
		OLED_DRAW_BIG(snake.x,snake.y,GRAM);
		for (unsigned char i = 0; i < snake.length; i++){
			OLED_DRAW_BIG(body[i].x, body[i].y,GRAM);
		}
		OLED_Refresh(GRAM);
	}
	else //覆盖蛇
	{
		OLED_DEL_BIG(snake.x,snake.y,GRAM);
		for (unsigned char i = 0; i < snake.length; i++){
			OLED_DEL_BIG(body[i].x, body[i].y,GRAM);
		}
		OLED_Refresh(GRAM);
	}
}
//移动蛇,将第i段蛇身的坐标信息更新为第i-1段蛇身的坐标信息，而第0段，即第一段蛇身的坐标信息需要更新为当前蛇头的坐标信息
void MoveSnake(unsigned char x, unsigned char y)
{
	DrawSnake(0);
	for (unsigned char i = snake.length - 1; i > 0; i--){
		body[i].x = body[i - 1].x;
		body[i].y = body[i - 1].y;
	}
	body[0].x = snake.x;
	body[0].y = snake.y;
	snake.x = snake.x + x;
	snake.y = snake.y + y;
	DrawSnake(1); //打印
}
//判断得分与结束
void JudgeFunc(unsigned char x, unsigned char y)
{
	//食物
	if (snake.y==foody && snake.x==foodx)
	{
		RandFood();
		snake.length++; 
		grade += 1; 
		OLED_Refresh(GRAM);
	}
	//墙或者蛇身
	else if (judge(snake.x + x,snake.y + y) == WALL || judge(snake.x + x,snake.y + y) ==  BODY || snake.length>10)
	{
		_delay_ms(2000); 
		OLED_Clear();//game over!
		exit(0);
	}
}
//运行游戏
void run(unsigned char x,unsigned char y,unsigned char n)
{
	while (1)
	{
		dp_show(grade);
		JudgeFunc(x, y); 
		MoveSnake(x, y); 
//		_delay_ms(300); 
		if (n!=count) //若触发开关，回到main函数
		{
			break;
		}
	}
}

int main(void) {
	// 初始化 OLED
	TWI_Init(); 
	OLED_Init();
	OLED_Clear();
	DDRD &= ~(1<<DDRD1);//开中断
	MCUCR |=((1<<ISC11)|(1<<ISC10));
	GICR |= (1<<INT1);
	sei();
	InitInterface();
	InitSnake();
	srand((unsigned)time(NULL)); 
	RandFood();
	DrawSnake(1); //打印蛇
	while(1) {
		OLED_Refresh(GRAM);
		unsigned char n=count;
		switch(count){
			case UP:
				run(0, -1, n); //向上移动（横坐标偏移为0，纵坐标偏移为-1）
				break;
			case DOWN:
				run(0, 1, n);
				break;
			case LEFT:
				run(-1, 0, n);
				break;
			case RIGHT:
				run(1, 0, n);
				break;
		}
		n=count;
	}

	return 0;
}
//中断处理函数
ISR(INT1_vect)
{
	if(count < 3) count++;
	else count = 0;
}