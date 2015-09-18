/**************************
*功能：连连看
*制作人：Ryoko
*编译环境： VC++ 6.0
*程序语言： C&C++
*库：EaxyX
**************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <graphics.h>//EasyX图形库

//引用Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

/****************************以下是宏定义参数*****************/
#define WIN_WIDTH 1000 //窗口宽度
#define WIN_HEIGHT 625 //窗口高度
#define ROW 10		//行 
#define COL 12		//列
#define BLANKTOP 150 //图片生成布局离窗口上部的距离
#define BLANK 150    //图片生成布局离窗口左部的距离
#define LEN 48       //方块图片的长度  
#define PM 10		 //图片横向数量
#define PN 8         //图片纵向数量
#define HPCOLOR		RGB(135,206,235)	//HP条颜色 
#define PINLAN		RGB(65,105,225)		//品蓝
#define TIANLAN		RGB(135,206,235)	//天蓝
#define SENLINLV	RGB(34,139,34)		//森林绿
#define GULV		RGB(61,145,64)		//钴绿色
#define TRUE		1
#define FAlSE		0
#define TIME_MAX	100

/****************************以下是数据结构定义***************/
struct picture //图片的结构
{
	bool IsClick;
	bool IsDis;
};

struct array_point//数组下标
{
	int i;
	int j;
};

struct point //点结构
{
	int x;
	int y;
};

struct Rect //整个图片组成的矩形
{
	int x;
	int y;
	int w;
	int h;
};

/**************************以下是全局变量*******************/
int chessman[ROW][COL];//棋盘
IMAGE chess[ROW][COL];
int mousea_x;
int mousea_y; //鼠标A点坐标
int mouseb_x;
int mouseb_y; //鼠标B点坐标
bool isMouseDown; //鼠标按下
int time_now;//当前剩余时间
int time_leave;//走过的时间
int time_lea;
int round;//回合
int initstart;	//开始变量
int start,end;
int grates;


/****************************函数声明**************************/
//消除方式
void Clear_A(int a,int b,int a1,int b1);//消除图片
void Clear_B(int a,int b,int a1,int b1);//消除图片
void Clear_C(int a,int b,int a1,int b1);//消除图片
void Clear_D(int a,int b,int a1,int b1);//消除图片

//消除法则
int Clear_Up(int a,int b,int a1,int b1);
int Clear_Row(int a,int b,int a1,int b1);
int Clear_Col(int a,int b,int a1,int b1);

//菜单生成函数
void Begin_Play_Manu();
void PLAY_GAME_Menu();
void HELP_GAME_Menu();
void EXIT_GAME_Menu();

//游戏初始化
void Init(); 

//分数
void Grate();

//游戏中控件
void RETURN_Menu();
void Calc_Time();
void Update_HP_For_Game();

void Draw_Refresh_Button();
void Is_Mouse_In_Refresh(int x,int y);
void IsRefresh(int x,int y);
void Refresh();

void Draw_Return_Button();
void Is_Mouse_In_Return(int x,int y);
void IsReturn(int x,int y);

//判断鼠标位置是否在图片内
bool Is_In_Rect(int x,int y,Rect r);

//图片选中与否
void Draw_Frame(int x,int y);
void Clear_Frame(int x,int y);

//是否死锁的判定
void Search();

//屏幕坐标与数组下标的转换
array_point Change_To_Array(int x,int y);

//游戏重启
void Restart();

//判断完成游戏标志
int Finish_Game(); 

//音效
void Play_ok_Sound();
void Play_great_Sound();
void Play_excellent_Sound();
void Play_amazing_Sound();
void Play_unbelivable_Sound();
void Play_refresh_Sound();


/************************************************************************/
/*								主函数                                  */
/************************************************************************/
int main ()
{
	int arow,acolumn;						//A点
	int brow,bcolumn;						//B点.
	MOUSEMSG m,n;							//鼠标事件
	IMAGE pass,fail;						//成功、 失败图片
	array_point p,q;
	
	start=clock()/1000;
	
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	setbkmode(TRANSPARENT);//开启背景透明模式
	
    //打开音乐
	mciSendString("open bkmusic.mp3 alias mymusic",NULL,0,NULL);
	//播放音乐
	mciSendString("play mymusic repeat",NULL,0,NULL);

	Begin_Play_Manu();
	
	
	for(;round<=4;round++)					//从第几回合开始
	{	
		Init();								//游戏初始化
		grates=0;
		Draw_Refresh_Button();	//画重列按钮	
		Draw_Return_Button();	//画返回按钮
		end=clock()/1000;
		do
		{
			Update_HP_For_Game();			//画HP条

			do 
			{
				Search();						//查找是否死锁

				do
				{
					Update_HP_For_Game();
					FlushMouseMsgBuffer();		//清除鼠标缓冲区
					MOUSEMSG GetMouseMsg();		//获取一个鼠标消息。如果没有，就等待
					m = GetMouseMsg(); 
					Is_Mouse_In_Refresh(m.x,m.y);	//画重列按钮	
					Is_Mouse_In_Return(m.x,m.y);	//画返回按钮
					Calc_Time();
				
				}while(m.uMsg!=WM_LBUTTONDOWN); 

				IsRefresh(m.x,m.y);	//实现重列
				IsReturn(m.x,m.y);	//实现返回
				p=Change_To_Array(m.x,m.y);	//得到数组下标

			} while (!chessman[p.i][p.j]);

			
			Draw_Frame(m.x,m.y);	
			arow=p.i;			
			acolumn=p.j; 	
 
			do 
			{
				
				do
				{
					Update_HP_For_Game();
					FlushMouseMsgBuffer();
					MOUSEMSG GetMouseMsg();		//获取一个鼠标消息。如果没有，就等待
					n = GetMouseMsg();
					Is_Mouse_In_Refresh(n.x,n.y);	//画重列按钮	
					Is_Mouse_In_Return(n.x,n.y);	//画返回按钮
					Calc_Time();
								
				}while(n.uMsg!=WM_LBUTTONDOWN);

				IsRefresh(n.x,n.y);
				IsReturn(n.x,n.y);	//实现返回
				q=Change_To_Array(n.x,n.y);	//得到图片的屏幕坐标
				
			} while (!chessman[q.i][q.j]);
			
			
			Draw_Frame(n.x,n.y);
			brow=q.i;			
			bcolumn=q.j; 
			
			
			Calc_Time();
			Update_HP_For_Game();

			//消除方框
			Clear_Frame(m.x,m.y);
			Clear_Frame(n.x,n.y);

			if(arow==brow&&acolumn==bcolumn) //如果A点与B点相同，则不处理
			{
				Clear_Frame(m.x,m.y);
				continue;
			}
			
			if(chessman[arow][acolumn]==chessman[brow][bcolumn])
			{
				//使用函数处理，使得A点一定在B点的上方
				if(arow<=brow)    
				{
					if(Clear_Up(arow,acolumn,brow,bcolumn))
					{
						switch (round)
						{
						case 1:Clear_A(arow,acolumn,brow,bcolumn);break;	//第一种消除方式	
						case 2:Clear_B(arow,acolumn,brow,bcolumn);break;	//第二种消除方式
						case 3:												//第三种消除方式
							if (acolumn<bcolumn)							
							{											 //////////////////////////////////
								Clear_C(arow,acolumn,brow,bcolumn);		 //由于向左缩进的消除方式有些特殊
							}											
							else										 //故使用函数处理，使得在左边的点成为A点
							{
								Clear_C(brow,bcolumn,arow,acolumn);		 /////////////////////////////////
							}
							break;										
						case 4:Clear_D(arow,acolumn,brow,bcolumn);break;	//第四种消除方式				
						}
					}
				} 
				else
				{
					if(Clear_Up(brow,bcolumn,arow,acolumn))
					{				
						switch (round)
						{
						case 1:Clear_A(brow,bcolumn,arow,acolumn);break;
						case 2:Clear_B(brow,bcolumn,arow,acolumn);break;
						case 3:
							if (acolumn<bcolumn)
							{
								Clear_C(arow,acolumn,brow,bcolumn);
							}
							else
							{
								Clear_C(brow,bcolumn,arow,acolumn);
							}
							break;
						case 4:Clear_D(brow,bcolumn,arow,acolumn);break;
						}
					}
				}
				
			}		
			

		Grate();
			
		}while(Finish_Game());

		if (round==1)
			Play_great_Sound();
		if(round==2)
			Play_excellent_Sound();
		if (round==3)
			Play_amazing_Sound();
		if (round==4)
			Play_unbelivable_Sound();
	
	}
	loadimage(&pass, _T("IMAGE"),_T("pass"));	// 读取图片 pass
	putimage(0, 0, &pass);	// 在坐标 (0, 0) 位置显示 pass 
	Restart();
	mciSendString("stop mymusic",NULL,0,NULL);
	mciSendString("close mymusic",NULL,0,NULL);
	closegraph();
	return 0;
}


/************************************************************************/
/*								消除规则                                */
/************************************************************************/
int Clear_Row(int a,int b,int a1,int b1)
{
	int n;                       //用于计数
	int cmin,cmax;				
	cmin=b<b1?b:b1;
	cmax=b<b1?b1:b;
	if(cmax==cmin+1)
	{
		if (chessman[a][b]==0||chessman[a1][b1]==0||chessman[a][b]==chessman[a1][b1])
		{
			return 1;
		}
	}
	else 
	{
		for(n=0;cmin<cmax-1;cmin++)
		{
			if(chessman[a][cmin+1]==0)n++;
			else break;
		}
		if(n==(b-b1-1)&&n>0)
		{
			
			return 1;
		}
		if(n==(b1-b-1)&&n>0)
		{
			return 1;
		}
	}
	
	return 0;
	
}

int Clear_Col(int a,int b,int a1,int b1)
{
	int n;                       //用于计数
	int rmin,rmax;
	rmin=a;
	rmax=a1;
	if(a1==a+1)
	{
		if(chessman[a][b]==0||chessman[a1][b1]==0||chessman[a][b]==chessman[a1][b1])
		{
			return 1;
		}
	}
	else 
	{
		for(n=0;rmin<rmax-1;rmin++)
		{			
			if(chessman[rmin+1][b]==0)n++;
			else break;
		}
		
		if(n==(a1-a-1)&&n>0)
		{
			
			return 1;
		}
	}
	return 0;
}

void Clear_A(int a,int b,int a1,int b1)			//第一种消除方式
{
	IMAGE imgk;	
	loadimage(&imgk, _T("IMAGE"),_T("imgk"));
	chessman[a][b]=chessman[a1][b1]=0;
	putimage(BLANK+b*48,BLANKTOP+(a-1)*48,&imgk);
	putimage(BLANK+b1*48,BLANKTOP+(a1-1)*48,&imgk);
	grates+=1;
	Play_ok_Sound();
}

void Clear_B(int a,int b,int a1,int b1)			//第二种消除方式（向上缩进）
{
	IMAGE imgk;
	IMAGE img1,img2;
	int x,y;
	int i,j,n=0;
	loadimage(&imgk, _T("IMAGE"),_T("imgk"));
	chessman[a][b]=chessman[a1][b1]=0;
	putimage(BLANK+b*48,BLANKTOP+(a-1)*48,&imgk);
	putimage(BLANK+b1*48,BLANKTOP+(a1-1)*48,&imgk);
	grates+=2;
	Play_ok_Sound();
	
	if (b==b1)									
	{
		for (i=a,j=b;i<8;)
		{
			if(chessman[i][j]==0)
			{
				n++;
				for (x=j,y=i;y<=8;y++)
				{
					getimage(&img1,BLANK+x*48,BLANKTOP+y*48,48,48);
					putimage(BLANK+x*48,BLANKTOP+y*48,&imgk);
					putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img1);
					chessman[y][x]=chessman[y+1][x];	
				}
				if(n==2)
				{
					return;
				}
			}
			
			else
			{
				i++;
			}					
		}
	} 
	
	
	else
	{
		for(x=b,y=a;y<9;y++)
		{
			getimage(&img1,BLANK+x*48,BLANKTOP+y*48,48,48);
			putimage(BLANK+x*48,BLANKTOP+y*48,&imgk);
			putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img1);
			chessman[y][x]=chessman[y+1][x];
		}
		for (x=b1,y=a1;y<9;y++)
		{
			getimage(&img2,BLANK+x*48,BLANKTOP+y*48,48,48);
			putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img2);
			chessman[y][x]=chessman[y+1][x];
		}				
	}
}

void Clear_C(int a,int b,int a1,int b1)			//第三种消除方式（向左缩进）
{
	IMAGE imgk;
	IMAGE img1,img2;
	int x,y;
	int i,j,n=0;
	loadimage(&imgk, _T("IMAGE"),_T("imgk"));
	chessman[a][b]=chessman[a1][b1]=0;
	putimage(BLANK+b*48,BLANKTOP+(a-1)*48,&imgk);
	putimage(BLANK+b1*48,BLANKTOP+(a1-1)*48,&imgk);
	grates+=3;
	Play_ok_Sound();

	if (a==a1)
	{
		for (i=a,j=b;j<=10;)
		{
			if(chessman[i][j]==0)
			{
				n++;
				for (x=j,y=i;x<11;x++)
				{
					getimage(&img1,BLANK+(x+1)*48,BLANKTOP+(y-1)*48,48,48);
					putimage(BLANK+(x+1)*48,BLANKTOP+(y-1)*48,&imgk);
					putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img1);
					chessman[y][x]=chessman[y][x+1];	
				}
				if(n==2)
				{
					return;
				}
				
			}
			
			else
			{
				j++;
			}					
		}
	} 
	
	
	else
	{
		for(x=b,y=a;x<11;x++)
		{
			getimage(&img1,BLANK+(x+1)*48,BLANKTOP+(y-1)*48,48,48);
			putimage(BLANK+(x+1)*48,BLANKTOP+(y-1)*48,&imgk);
			putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img1);
			chessman[y][x]=chessman[y][x+1];	
		}
		for (x=b1,y=a1;x<11;x++)
		{
			getimage(&img1,BLANK+(x+1)*48,BLANKTOP+(y-1)*48,48,48);
			putimage(BLANK+(x+1)*48,BLANKTOP+(y-1)*48,&imgk);
			putimage(BLANK+x*48,BLANKTOP+(y-1)*48,&img1);
			chessman[y][x]=chessman[y][x+1];	
		}	
	}

}

void Clear_D(int a,int b,int a1,int b1)			//第四种消除方式（无限重列）
{
	IMAGE imgk;	
	loadimage(&imgk, _T("IMAGE"),_T("imgk"));
	chessman[a][b]=chessman[a1][b1]=0;
	putimage(BLANK+b*48,BLANKTOP+(a-1)*48,&imgk);
	putimage(BLANK+b1*48,BLANKTOP+(a1-1)*48,&imgk);
	grates+=4;
	Play_ok_Sound();
	if(Finish_Game())
	{
		Refresh();
	}
}


int Clear_Up(int a,int b,int a1,int b1)//消除算法
{
	
	/**********************连连看A、B两点分同行，同列，不同行不同列三种情况****************/
	
	
	int i,j,k;                       //用于循环
	
	
	/*************************************一线（相邻）********************************************/
	
	
	if(a==a1)                //同行相邻
	{	
		if(Clear_Row(a,b,a1,b1))
		{
			return 1;	
		}
	}
	
	
	if(b==b1)               //同列相邻
	{
		if(Clear_Col(a,b,a1,b1))
		{
			return 1;
		}
	}
	
	/**************************************************************************
	*以A点为中心，寻找到达B点的路，据连连看规则，最多两个折点，即3次方向的转换
	*而且，每次转方向后，则只需进行同行的判断或同列的判断
	**************************************************************************/
	
	/*********************************从A点左边走***************************/
	
	
	for(i=a,j=b-1;chessman[i][j]==0&&j>=0;j--)//0折点
	{
		
		for(k=i;chessman[k][j]==0&&k<a1;k++)//1折点
		{
			if(j==b1&&Clear_Col(k,j,a1,b1))
			{
				return 1;
			}
		}

		if(k==a1&&Clear_Row(k,j,a1,b1)&&chessman[k][j]==0)
		{
			return 1;
		}
		
		
	}
	
	
	
	/************************************右******************************/
	

	for(i=a,j=b+1;chessman[i][j]==0&&j<COL;j++)//0折点
	{
		
		for(k=i;chessman[k][j]==0&&k<a1;k++)//1折点
		{
			if(j==b1&&Clear_Col(k,j,a1,b1))
			{
				return 1;
			}
		}

		if(k==a1&&Clear_Row(k,j,a1,b1)&&chessman[k][j]==0)
		{
			return 1;
		}
		
	}
	
	/*******************************A点上方向*************************/
	
	for(i=a-1,j=b;chessman[i][j]==0&&i>=0;i--)//0折点
	{
		if(b<b1)
		{
			for(k=j;chessman[i][k]==0&&k<b1;k++)//1折点
			{
				if(i==a1&&Clear_Row(i,k,a1,b1))
				{
					return 1;
				}
			}
			if(Clear_Col(i,k,a1,b1)&&k==b1)
			{
				return 1;
			}			
		}
		
		
		if(b>b1)
		{
			for(k=j;chessman[i][k]==0&&k>b1;k--)//1折点
			{
				if(i==a1&&Clear_Row(i,k,a1,b1))
				{
					return 1;
				}
			}
			
			
			if(Clear_Col(i,k,a1,b1)&&k==b1)
			{
				return 1;
			}	
		}
	}
	
	
	/*************************A点下方向*******************************/
	
	for(i=a+1,j=b;chessman[i][j]==0&&i<10;i++)//0折点
	{
		if(b<b1)
		{
			for(k=j;chessman[i][k]==0&&k<b1;k++)//1折点
			{ 
				if(i==a1&&Clear_Row(i,k,a1,b1))
				{
					return 1;
				}
			}
			if(a1<i&&chessman[i][k]==0&&k==b1)
			{
				if(Clear_Col(a1,b1,i,k))
				{
					return 1;
				}
			}
			if(a1>i&&chessman[i][k]==0&&k==b1)
			{
				if(Clear_Col(i,k,a1,b1))
				{
					return 1;
				}
			}
			
			
		}
		
		
		else
		{
			for(k=j;chessman[i][k]==0&&k>b1;k--)//1折点
			{
				if(i==a1&&Clear_Row(i,k,a1,b1))
				{
					return 1;
				}
			}
			if(a1<i&&chessman[i][k]==0&&k==b1)
			{
				if(Clear_Col(a1,b1,i,k))
				{
					return 1;
				}
			}
			if(a1>i&&chessman[i][k]==0&&k==b1)
			{
				if(Clear_Col(i,k,a1,b1))
				{
					return 1;
				}
			}
		}
	}
	return 0;
	/**********************完***************************/
}


//菜单

/************************************************************************/
/*                                主菜单                                */
/************************************************************************/				
void Begin_Play_Manu()
{
	IMAGE imgbk;	// 定义 IMAGE 对象 背景图片
	Rect a,b,c;
	MOUSEMSG m;
	int flag=1;
	
	loadimage(&imgbk, _T("IMAGE"),_T("imgbk"));	//读取开始菜单图片	
	putimage(0, 0, &imgbk);						//显示图片
	
	settextstyle(62, 30, _T("方正喵呜体"));		//设置字体
	
	//版本
	settextstyle(22, 10, _T("Verdana"));
	settextcolor(PINLAN);
	outtextxy(600, 213, _T("v2.0 ---By:Ryoko"));
	
	//说明
	settextstyle(22, 10, _T("Verdana"));
	settextcolor(PINLAN);
	outtextxy(223, 500, _T("Programing By VC++ 6.0 & EasyX     Date:2015.4.13"));
	
	//主菜单按钮+flash实现
	setlinecolor(TIANLAN);
	settextcolor(PINLAN);
	settextstyle(25, 10, _T("Verdana"));
	
	//开始按钮
	a.x = 340;
	a.y = 280;
	a.w = a.x + 250;
	a.h = a.y + 35;
	rectangle(a.x,a.y,a.w,a.h);
	outtextxy(a.x + 60, a.y + 5, _T("PLAY GAME"));
	
	//游戏帮助按钮
	b.x = 340;
	b.y = 330;
	b.w = b.x + 250;
	b.h = b.y + 35;
	rectangle(b.x,b.y,b.w,b.h);
	outtextxy(b.x + 60, b.y + 5, _T("HELP GAME"));
	
	////退出游戏按钮
	c.x = 340;
	c.y = 380;
	c.w = c.x + 250;
	c.h = c.y + 35;
	rectangle(c.x,c.y,c.w,c.h);
	outtextxy(c.x + 60, c.y + 5, _T("EXIT GAME"));
	
	while (flag)
	{
		//动画效果
		do
		{
			
			FlushMouseMsgBuffer();
			m=GetMouseMsg();
			
			//是否选中PLAY GAME按钮
			if(Is_In_Rect(m.x,m.y,a))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(a.x,a.y,a.w,a.h);
				settextcolor(RGB(34,139,34));
				outtextxy(a.x + 60, a.y + 5, _T("PLAY GAME"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(a.x,a.y,a.w,a.h);
				settextcolor(RGB(65,105,225));
				outtextxy(a.x + 60, a.y + 5, _T("PLAY GAME"));
			}
			
			//是否选中HELP GAME按钮
			if(Is_In_Rect(m.x,m.y,b))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(b.x,b.y,b.w,b.h);
				settextcolor(RGB(34,139,34));
				outtextxy(b.x + 60, b.y + 5, _T("HELP GAME"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(b.x,b.y,b.w,b.h);
				settextcolor(RGB(65,105,225));
				outtextxy(b.x + 60, b.y + 5, _T("HELP GAME"));
			}
			
			//是否选中EXIT GAME按钮
			if(Is_In_Rect(m.x,m.y,c))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(c.x,c.y,c.w,c.h);
				settextcolor(RGB(34,139,34));
				outtextxy(c.x + 60, c.y + 5, _T("EXIT GAME"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(c.x,c.y,c.w,c.h);
				settextcolor(RGB(65,105,225));
				outtextxy(c.x + 60, c.y + 5, _T("EXIT GAME"));
			}
			
		}while(m.uMsg!=WM_LBUTTONDOWN);
		
		//PLAY GAME
		if(Is_In_Rect(m.x,m.y,a))
		{
			flag=0;
			PLAY_GAME_Menu();
		}
		
		//HELP GAME
		if(Is_In_Rect(m.x,m.y,b))
		{
			flag=0;
			HELP_GAME_Menu();
		}
		
		//EXIT GAME
		if(Is_In_Rect(m.x,m.y,c))
		{			
			flag=0;	
			EXIT_GAME_Menu();
		}
	}
	
}

/************************************************************************/
/*								二级菜单                                */
/************************************************************************/

void PLAY_GAME_Menu()
{
	Rect x,y,z,w,v;
	MOUSEMSG n;
	int out=1;
	IMAGE imggk;								// 定义 IMAGE 对象 背景图片
	loadimage(&imggk, _T("IMAGE"),_T("imggk"));	// 读取该菜单图片
	putimage(0, 0, &imggk);						// 显示图片
	
	setlinecolor(TIANLAN);
	settextstyle(25, 10, _T("Verdana"));
	
	//第一关按钮
	x.x = 360;
	x.y = 280;
	x.w = x.x + 250;
	x.h = x.y + 35;
	rectangle(x.x,x.y,x.w,x.h);
	outtextxy(x.x + 80, x.y + 5, _T("LEVEL 1"));
	
	//第二关按钮
	y.x = 360;
	y.y = 330;
	y.w = y.x + 250;
	y.h = y.y + 35;
	rectangle(y.x,y.y,y.w,y.h);
	outtextxy(y.x + 80, y.y + 5, _T("LEVEL 2"));
	
	//第三关按钮
	z.x = 360;
	z.y = 380;
	z.w = z.x + 250;
	z.h = z.y + 35;
	rectangle(z.x,z.y,z.w,z.h);
	outtextxy(z.x + 80, z.y + 5, _T("LEVEL 3"));
	
	//第四关按钮
	w.x = 360;
	w.y = 430;
	w.w = w.x + 250;
	w.h = w.y + 35;
	rectangle(w.x,w.y,w.w,w.h);
	outtextxy(w.x + 80, w.y + 5, _T("LEVEL 4"));
	
	//返回按钮
	v.x = 360;
	v.y = 480;
	v.w = v.x + 250;
	v.h = v.y + 35;
	rectangle(v.x,v.y,v.w,v.h);
	outtextxy(v.x + 80, v.y + 5, _T("RETURN"));
	
	while (out)
	{
		//动画效果
		do
		{					
			FlushMouseMsgBuffer();
			n=GetMouseMsg();
			
			//是否选中第一关
			if(Is_In_Rect(n.x,n.y,x))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(x.x,x.y,x.w,x.h);
				settextcolor(RGB(34,139,34));
				outtextxy(x.x + 80, x.y + 5, _T("LEVEL 1"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(x.x,x.y,x.w,x.h);
				settextcolor(RGB(65,105,225));
				outtextxy(x.x + 80, x.y + 5, _T("LEVEL 1"));
			}
			
			//是否选中第二关
			if(Is_In_Rect(n.x,n.y,y))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(y.x,y.y,y.w,y.h);
				settextcolor(RGB(34,139,34));
				outtextxy(y.x + 80, y.y + 5, _T("LEVEL 2"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(y.x,y.y,y.w,y.h);
				settextcolor(RGB(65,105,225));
				outtextxy(y.x + 80, y.y + 5, _T("LEVEL 2"));
			}
			
			
			//是否选中第三关
			if(Is_In_Rect(n.x,n.y,z))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(z.x,z.y,z.w,z.h);
				settextcolor(RGB(34,139,34));
				outtextxy(z.x + 80, z.y + 5, _T("LEVEL 3"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(z.x,z.y,z.w,z.h);
				settextcolor(RGB(65,105,225));
				outtextxy(z.x + 80, z.y + 5, _T("LEVEL 3"));
			}
			
			//是否选中第四关
			if(Is_In_Rect(n.x,n.y,w))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(w.x,w.y,w.w,w.h);
				settextcolor(RGB(34,139,34));
				outtextxy(w.x + 80, w.y + 5, _T("LEVEL 4"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(w.x,w.y,w.w,w.h);
				settextcolor(RGB(65,105,225));
				outtextxy(w.x + 80, w.y + 5, _T("LEVEL 4"));
			}
			
			//是否选择退出
			if(Is_In_Rect(n.x,n.y,v))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(v.x,v.y,v.w,v.h);
				settextcolor(RGB(34,139,34));
				outtextxy(v.x + 80, v.y + 5, _T("RETURN"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(v.x,v.y,v.w,v.h);
				settextcolor(RGB(65,105,225));
				outtextxy(v.x + 80, v.y + 5, _T("RETURN"));
			}
			
		}while(n.uMsg!=WM_LBUTTONDOWN);
		
		//从第一关开始
		if(Is_In_Rect(n.x,n.y,x))
		{
			
			out=0;
			round=1;
		}
		
		//从第二关开始
		if(Is_In_Rect(n.x,n.y,y))
		{
			out=0;
			round=2;		
		}
		
		//第三关开始
		if(Is_In_Rect(n.x,n.y,z))
		{
			out=0;
			round=3;
		}
		
		//从第四关开始
		if(Is_In_Rect(n.x,n.y,w))
		{
			out=0;
			round=4;
		}
		
		//返回
		if(Is_In_Rect(n.x,n.y,v))
		{
			out=0;
			RETURN_Menu();
		}
		
	}
	
}

void HELP_GAME_Menu()
{
	IMAGE help;
	Rect d;
	MOUSEMSG m;
	int flag=1;
	loadimage(&help, _T("IMAGE"),_T("help"));	// 读取图片到 help 对象中
	putimage(0,0,&help);
	
	d.x=350;
	d.y=550;
	d.w=d.x+250;
	d.h=d.y+35;
	setlinecolor(RGB(135,206,235));
	rectangle(d.x,d.y,d.w,d.h);
	settextcolor(RGB(65,105,225));
	outtextxy(d.x + 75, d.y + 5, _T("RETURN"));
	
	while (flag)
	{
		do
		{
			
			FlushMouseMsgBuffer();
			m=GetMouseMsg();
			
			//是否选中返回按钮
			if(Is_In_Rect(m.x,m.y,d))
			{
				setlinecolor(RGB(34,139,34));
				fillrectangle(d.x,d.y,d.w,d.h);
				settextcolor(RGB(34,139,34));
				outtextxy(d.x + 75, d.y + 5, _T("RETURN"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(d.x,d.y,d.w,d.h);
				settextcolor(RGB(65,105,225));
				outtextxy(d.x + 75, d.y + 5, _T("RETURN"));
			}
			
		}while(m.uMsg!=WM_LBUTTONDOWN);
		if(Is_In_Rect(m.x,m.y,d))
		{
			flag=0;
			RETURN_Menu();
		}
		
	}
}

void EXIT_GAME_Menu()
{
	closegraph();
	exit(0);
}

void RETURN_Menu()
{
	time_lea=clock()/1000;
	main();	
}

//游戏中功能

/************************************************************************/
/*								初始化布局                              */
/************************************************************************/

int Create_Array()
{
	int i,j;
	int c[10]={0};                //用于计数 记录哪些数字出现次数不是偶数
	int flag=0;
	for(i=1;i<ROW-1;i++) 			
	{
		for(j=1;j<COL-1;j++)
		{
			if (chessman[i][j]==0&&initstart)
			{
				continue;
			}
			else
			{
				chessman[i][j]=1+rand()%10;         //棋盘内容为大写字母1到10
				
				if(chessman[i][j]==1) c[0]++;        //记录1的个数
				if(chessman[i][j]==2) c[1]++;        //记录2的个数
				if(chessman[i][j]==3) c[2]++;        //记录3的个数
				if(chessman[i][j]==4) c[3]++;        //记录4的个数
				if(chessman[i][j]==5) c[4]++;        //记录5的个数
				if(chessman[i][j]==6) c[5]++;        //记录6的个数
				if(chessman[i][j]==7) c[6]++;        //记录7的个数
				if(chessman[i][j]==8) c[7]++;        //记录8的个数
				if(chessman[i][j]==9) c[8]++;        //记录9的个数
				if(chessman[i][j]==10) c[9]++;       //记录10的个数
			}
		}
	}
	
	for (i=0;i<10;i++)
	{
		if (c[i]%2!=0)
		{
			flag=1;
			break;
		}
	}
	
	return flag;
}

void Init()
{
    int i,j;
    int m,n;//记录屏幕坐标位 m记录y n记录x
	int result;
	//	initgraph(WIN_WIDTH, WIN_HEIGHT); //进入图形模式
	
	///////////////////////////////定义背景图片
	
	IMAGE img;	// 定义 IMAGE 对象 背景图片
	loadimage(&img, _T("IMAGE"),_T("img"));	// 读取图片到 img 对象中
	putimage(0, 0, &img);	// 在坐标 (0, 0) 位置显示 IMAGE 对象
	
	///////////////////////////////定义连连看的图片种子	
    IMAGE imga;	
	loadimage(&imga, _T("IMAGE"),_T("imga"));	
	
	IMAGE imgb;	
	loadimage(&imgb, _T("IMAGE"),_T("imgb"));	
	
	IMAGE imgc;	
	loadimage(&imgc, _T("IMAGE"),_T("imgc"));	
	
	IMAGE imgd;	
	loadimage(&imgd, _T("IMAGE"),_T("imgd"));	
	
	IMAGE imge;	
	loadimage(&imge, _T("IMAGE"),_T("imge"));
	
	IMAGE imgf;	
	loadimage(&imgf, _T("IMAGE"),_T("imgf"));
	
	IMAGE imgg;	
	loadimage(&imgg, _T("IMAGE"),_T("imgg"));
	
	IMAGE imgh;	
	loadimage(&imgh, _T("IMAGE"),_T("imgh"));
	
	IMAGE imgi;	
	loadimage(&imgi, _T("IMAGE"),_T("imgi"));	
	
	IMAGE imgj;	
	loadimage(&imgj, _T("IMAGE"),_T("imgj"));
	
    
	IMAGE a[10]={imga,imgb,imgc,imgd,imge,imgf,imgg,imgh,imgi,imgj}; //用数组a[11]存储这些图片
	
	//////////////////////////////////////////////沿用整形二维数组存储数字 1~10代表不同的图片 0代表通路 
	
    int c[10]={0};                //用于计数 记录哪些数字出现次数不是偶数
	
	srand(time(NULL));
	
	initstart=0;
	
	do 
	{
		
		result=Create_Array();
		
	} while (result);
	
	//////////////////////////////////////生成图片布局
	
	for(i=1,n=BLANKTOP;i<ROW-1;i++)
	{
		m=BLANK;
		for(j=1;j<COL-1;j++)
		{
			if(chessman[i][j]==1)chess[i][j]=a[0];
			if(chessman[i][j]==2)chess[i][j]=a[1];
			if(chessman[i][j]==3)chess[i][j]=a[2];
			if(chessman[i][j]==4)chess[i][j]=a[3];
			if(chessman[i][j]==5)chess[i][j]=a[4];
			if(chessman[i][j]==6)chess[i][j]=a[5];
			if(chessman[i][j]==7)chess[i][j]=a[6];
			if(chessman[i][j]==8)chess[i][j]=a[7];
			if(chessman[i][j]==9)chess[i][j]=a[8];
			if(chessman[i][j]==10)chess[i][j]=a[9];
			
			m=m+48;
			
			putimage(m,n,&chess[i][j]);	
			
		}
		n=n+48;
	}
	
	settextstyle(25, 10, _T("Verdana"));
	settextcolor(PINLAN);
	outtextxy(200, 82, "HP:");
	settextstyle(30, 15, _T("Verdana"));
	settextcolor(PINLAN);
	outtextxy(750, 280, "がんばって！！！");

	Rect g;
	g.x=0;
	g.y=0;
	g.w=g.x+100;
	g.h=g.y+25;
	setfillcolor(WHITE);
	solidrectangle(g.x, g.y,g.w,g.h);	// 画填充矩形(无边框)
	settextstyle(25, 10, _T("Verdana"));
	settextcolor(PINLAN);	
	switch(round)
	{
	case 1:outtextxy(10, 0, "第一关");break;
	case 2:outtextxy(10, 0, "第二关");break;
	case 3:outtextxy(10, 0, "第三关");break;
	case 4:outtextxy(10, 0, "第四关");break;
	}
}

/************************************************************************/
/*								计时函数                                */
/************************************************************************/

void Calc_Time()
{
	IMAGE fail;						//成功、 失败图片
	time_leave=clock()/1000-time_lea;
	time_now=TIME_MAX-time_leave+(end-start);
	if(time_now<=0)
	{	
		loadimage(&fail, _T("IMAGE"),_T("fail"));	// 读取图片到 fail 对象中
		putimage(0, 0, &fail);	// 在坐标 (0, 0) 位置显示 fail 对象
		Restart();
		Sleep(300); 
		closegraph();
	}
}

/************************************************************************/
/*							  更新血量条                                */
/************************************************************************/

void Update_HP_For_Game()
{
	BeginBatchDraw();
	int x = 250;
	int y = 90;
	int HPBARW = LEN * PM-150 ;
	setlinecolor(PINLAN);
	setfillcolor(WHITE);
	fillrectangle(x, y, x + HPBARW, y + 15);
	setfillcolor(HPCOLOR);
	fillrectangle(x, y, x + (int)(HPBARW * (1.0 * time_now / TIME_MAX)), y + 15);
	EndBatchDraw();
}

/************************************************************************/
/*						重列按钮及重列功能函数                          */
/************************************************************************/

void Draw_Refresh_Button() //重列按钮
{
	Rect f;
	setlinecolor(RGB(135,206,235));
	settextcolor(RGB(65,105,225));
	settextstyle(25, 10, _T("Verdana"));
	//重列按钮
	f.x = 900;
	f.y = 0;
	f.w = f.x + 100;
	f.h = f.y + 25;
	rectangle(f.x,f.y,f.w,f.h);
	outtextxy(f.x + 30, f.y, _T("重列"));
	
}

void Is_Mouse_In_Refresh(int x,int y)
{

	Rect f;
	//重列按钮位置
	f.x = 900;
	f.y = 0;
	f.w = f.x + 100;
	f.h = f.y + 25;
	if(Is_In_Rect(x,y,f))
	{
		setlinecolor(SENLINLV);
		rectangle(f.x,f.y,f.w,f.h);
		settextcolor(SENLINLV);
		outtextxy(f.x + 30, f.y , _T("重列"));
	}
	else
	{
		setlinecolor(PINLAN);
		rectangle(f.x,f.y,f.w,f.h);
		settextcolor(PINLAN);
		outtextxy(f.x + 30, f.y , _T("重列"));
	}	

}

void IsRefresh(int x,int y)
{
	
	Rect f;
	f.x = 900;
	f.y = 0;
	f.w = f.x + 100;
	f.h = f.y + 25;
	
	if(Is_In_Rect(x,y,f))
	{
		Refresh();				
	}
	
}

void Refresh()
{
	int i,j;
	int m,n;//记录屏幕坐标位 m记录y n记录x
	int result;
	
	///////////////////////////////定义连连看的图片种子	
	IMAGE imga;	
	loadimage(&imga, _T("IMAGE"),_T("imga"));	
	
	IMAGE imgb;	
	loadimage(&imgb, _T("IMAGE"),_T("imgb"));	
	
	IMAGE imgc;	
	loadimage(&imgc, _T("IMAGE"),_T("imgc"));	
	
	IMAGE imgd;	
	loadimage(&imgd, _T("IMAGE"),_T("imgd"));	
	
	IMAGE imge;	
	loadimage(&imge, _T("IMAGE"),_T("imge"));
	
	IMAGE imgf;	
	loadimage(&imgf, _T("IMAGE"),_T("imgf"));
	
	IMAGE imgg;	
	loadimage(&imgg, _T("IMAGE"),_T("imgg"));
	
	IMAGE imgh;	
	loadimage(&imgh, _T("IMAGE"),_T("imgh"));
	
	IMAGE imgi;	
	loadimage(&imgi, _T("IMAGE"),_T("imgi"));	
	
	IMAGE imgj;	
	loadimage(&imgj, _T("IMAGE"),_T("imgj"));
	IMAGE imgk;	
	loadimage(&imgk, _T("IMAGE"),_T("imgk"));
	
	
	IMAGE a[10]={imga,imgb,imgc,imgd,imge,imgf,imgg,imgh,imgi,imgj}; //用数组a[11]存储这些图片
	
	//////////////////////////////////////////////沿用整形二维数组存储数字 1~10代表不同的图片 0代表通路 
	
	int c[10]={0};                //用于计数 记录哪些数字出现次数不是偶数
	
	srand(time(NULL));			  //刷新时间 重新随机种子
	
	initstart=1;
	
	do 
	{
		
		result=Create_Array();
		
	} while (result);
	
	
	//////////////////////////////////////生成图片布局
	
	for(i=1,n=BLANKTOP;i<ROW-1;i++)
	{
		m=BLANK;
		for(j=1;j<COL-1;j++)
		{
			
			if(chessman[i][j]==0)chess[i][j]=imgk;
			if(chessman[i][j]==1)chess[i][j]=a[0];
			if(chessman[i][j]==2)chess[i][j]=a[1];
			if(chessman[i][j]==3)chess[i][j]=a[2];
			if(chessman[i][j]==4)chess[i][j]=a[3];
			if(chessman[i][j]==5)chess[i][j]=a[4];
			if(chessman[i][j]==6)chess[i][j]=a[5];
			if(chessman[i][j]==7)chess[i][j]=a[6];
			if(chessman[i][j]==8)chess[i][j]=a[7];
			if(chessman[i][j]==9)chess[i][j]=a[8];
			if(chessman[i][j]==10)chess[i][j]=a[9];
			
			m=m+48;
			
			putimage(m,n,&chess[i][j]);				//摆放图片
			
		}
		n=n+48;
	}
	
}


/************************************************************************/
/*						返回按钮及返回功能函数                          */
/************************************************************************/

void Draw_Return_Button()		//画返回按钮
{
	Rect g;
	setlinecolor(RGB(135,206,235));
	settextcolor(RGB(65,105,225));
	settextstyle(25, 10, _T("Verdana"));
	//返回按钮
	g.x = 900;
	g.y = 30;
	g.w = g.x + 100;
	g.h = g.y + 25;
	rectangle(g.x,g.y,g.w,g.h);
	outtextxy(g.x + 30, g.y, _T("返回"));
}

void Is_Mouse_In_Return(int x,int y)
{
	Rect g;
	//返回按钮位置
	g.x = 900;
	g.y = 30;
	g.w = g.x + 100;
	g.h = g.y + 25;
	if(Is_In_Rect(x,y,g))
	{
		setlinecolor(SENLINLV);
		rectangle(g.x,g.y,g.w,g.h);
		settextcolor(SENLINLV);
		outtextxy(g.x + 30, g.y , _T("返回"));
	}
	else
	{
		setlinecolor(PINLAN);
		rectangle(g.x,g.y,g.w,g.h);
		settextcolor(PINLAN);
		outtextxy(g.x + 30, g.y , _T("返回"));
	}		
}

void IsReturn(int x,int y)
{
	Rect f;
	f.x = 900;
	f.y = 30;
	f.w = f.x + 100;
	f.h = f.y + 25;
	
	if(Is_In_Rect(x,y,f))
	{
		RETURN_Menu();			
	}
}

/************************************************************************/
/*								等级及积分                              */
/************************************************************************/

void Grate()
{
	Rect a;
	a.x=100;
	a.y=0;
	a.w=a.x+150;
	a.h=a.y+25;
	setlinestyle(PS_SOLID,1);
	setfillcolor(WHITE);
	solidrectangle(a.x, a.y,a.w,a.h);	// 画填充矩形(无边框)	
	settextcolor(PINLAN);
	settextstyle(25, 10, _T("Verdana"));
	TCHAR b[10];
		_stprintf(b,"%d",grates);
	outtextxy(100,0, "Score:");	
	outtextxy(180,0, b);	
}                                 

/************************************************************************/
/*						判断鼠标位置是否在图片内                        */
/************************************************************************/

bool Is_In_Rect(int x,int y,Rect r)//判断鼠标位置是否在图片内
{
	return((x>=r.x&&x<=r.w)&&(y>=r.y&&y<=r.h));
}

/************************************************************************/
/*	                        图片的选中及反选	                        */
/************************************************************************/

void Draw_Frame(int x,int y)
{
	Rect win;
	win.x=BLANK;
	win.y=BLANKTOP;
	win.w=BLANK+COL*48;
	win.h=BLANKTOP+ROW*48;

	if (Is_In_Rect(x,y,win))
	{
		setlinestyle(PS_SOLID,3);
		setcolor(RGB(255,99,71));
		x=x-((x-BLANK)%48);
		y=y-((y-BLANKTOP)%48);
		rectangle(x,y,x+48,y+48);
	}	
}


void Clear_Frame(int x,int y)				//消除方框
{
	Rect win;
	win.x=BLANK+48;
	win.y=BLANKTOP;
	win.w=BLANK+(11*48);
	win.h=BLANKTOP+(8*48);
	
	if (Is_In_Rect(x,y,win))
	{
		setlinestyle(PS_SOLID,3);
		setcolor(WHITE);
		x=x-((x-BLANK)%48);
		y=y-((y-BLANKTOP)%48);
		rectangle(x,y,x+48,y+48);
	}	
}

/************************************************************************/
/*							屏幕坐标转数组下标                          */
/************************************************************************/

array_point Change_To_Array(int x,int y)
{

		array_point a; 
		a.i=(y-BLANKTOP)/LEN+1;
		a.j=(x-BLANK)/LEN;
		return a;

}

/************************************************************************/
/*							判断是否死锁                                */
/************************************************************************/

void Search()
{
	int ai,aj,bi,bj;
	int flag=1;
	for(ai=1;ai<ROW-1;ai++)
		for(aj=1;aj<COL-1;aj++)
			for(bi=1;bi<ROW-1;bi++)
				for(bj=1;bj<COL-1;bj++)
				{
					if(chessman[ai][aj]==0||chessman[bi][bj]==0)
						continue;
					if(ai==bi&&aj==bj)
						continue;
					if(chessman[ai][aj]==chessman[bi][bj]&&(ai!=bi||aj!=bj))
					{						
						if(Clear_Up(ai,aj,bi,bj)||Clear_Up(bi,bj,ai,aj))
						{
							flag=0;					//只要flag不为0，则死锁
						}
					}
				}
				
				
				if (flag&&Finish_Game())				//flag为0，且游戏还没结束
				{
					Refresh();
					Play_refresh_Sound();
				}
				
				
}

/************************************************************************/
/*								重新开始                                */
/************************************************************************/

void Restart() //重新开始按钮
{
	Rect res,end;
	int flag=1;
	MOUSEMSG n;
	setlinecolor(RGB(135,206,235));
	settextcolor(RGB(65,105,225));
	settextstyle(25, 10, _T("Verdana"));
	
	//重新开始按钮
	res.x = 100;
	res.y = 200;
	res.w = res.x + 150;
	res.h = res.y + 25;
	rectangle(res.x,res.y,res.w,res.h);
	outtextxy(res.x + 30, res.y, _T("重新开始"));
	
	//结束游戏按钮
	end.x = 100;
	end.y = 250;
	end.w = end.x + 150;
	end.h = end.y + 25;
	rectangle(end.x,end.y,end.w,end.h);
	outtextxy(end.x + 30, end.y, _T("结束游戏"));
	
	
	while (flag)
	{
		//动画效果
		do
		{					
			FlushMouseMsgBuffer();
			n=GetMouseMsg();
			if(Is_In_Rect(n.x,n.y,res))
			{
				setlinecolor(RGB(61,89,171));
				rectangle(res.x,res.y,res.w,res.h);
				settextcolor(RGB(8,46,84));
				outtextxy(res.x + 30, res.y , _T("重新开始"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(res.x,res.y,res.w,res.h);
				settextcolor(RGB(65,105,225));
				outtextxy(res.x + 30, res.y , _T("重新开始"));
			}	
			if(Is_In_Rect(n.x,n.y,end))
			{
				setlinecolor(RGB(61,89,171));
				rectangle(end.x,end.y,end.w,end.h);
				settextcolor(RGB(8,46,84));
				outtextxy(end.x + 30, end.y , _T("结束游戏"));
			}
			else
			{
				setlinecolor(RGB(135,206,235));
				rectangle(end.x,end.y,end.w,end.h);
				settextcolor(RGB(65,105,225));
				outtextxy(end.x + 30, end.y , _T("结束游戏"));
			}	
			
			
		}while(n.uMsg!=WM_LBUTTONDOWN);
		
		
		if(Is_In_Rect(n.x,n.y,res))
		{
			flag=0;
			RETURN_Menu();
		}
		
		if(Is_In_Rect(n.x,n.y,end))
		{
			flag=0;	
			exit(0);
		}		
		
	}
	
}


/************************************************************************/
/*							判断游戏是否完成                            */
/************************************************************************/

int Finish_Game() //判断完成游戏标志
{
	int i,j; //用于循环
	for(i=1;i<=8;i++)
	{
		for(j=1;j<=10;j++)
		{
			if(chessman[i][j]!=0)        
			{
				return 1;        //只要有1个棋子不为0，就返回1
			}
		}
	}
	return 0;
}

/************************************************************************/
/*							音效			                            */
/************************************************************************/

void Play_ok_Sound()
{
	//打开音乐
	mciSendString("close ok.mp3 ",NULL,0,NULL);
	mciSendString("open ok.mp3 ",NULL,0,NULL);

	//播放音乐
	mciSendString("play ok.mp3 from 0 ",NULL,0,NULL);
}

void Play_great_Sound()
{
	//打开音乐
	mciSendString("open great.mp3 ",NULL,0,NULL);
	//播放音乐
	mciSendString("play great.mp3 from 0 ",NULL,0,NULL);
}

void Play_excellent_Sound()
{
	//打开音乐
	mciSendString("open excellent.mp3 ",NULL,0,NULL);
	//播放音乐
	mciSendString("play excellent.mp3 from 0 ",NULL,0,NULL);
}

void Play_amazing_Sound()
{
	//打开音乐
	mciSendString("open amazing.mp3 ",NULL,0,NULL);
	//播放音乐
	mciSendString("play amazing.mp3 from 0 ",NULL,0,NULL);
}

void Play_unbelivable_Sound()
{
	//打开音乐
	mciSendString("open unbelivable.mp3 ",NULL,0,NULL);
	//播放音乐
	mciSendString("play unbelivable.mp3 from 0 ",NULL,0,NULL);
}

void Play_refresh_Sound()
{
	//打开音乐
	mciSendString("open refresh.mp3 ",NULL,0,NULL);
	//播放音乐
	mciSendString("play refresh.mp3 from 0 ",NULL,0,NULL);
}
