/*************************************************************************
	> File Name: snake.c
	> Author: huangwei
	>qq Mail: 1026856341@qq.com 
	> Created Time: Sun 12 Aug 2018 02:04:48 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<ncurses.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>

#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_UP 'w'
#define MOVE_DOWN 's'
struct listnode
{
	int pos_x;
	int pos_y;
	struct listnode *pre;
	struct listnode *next;
};
struct List
{
	struct listnode *first;
	struct listnode *last;
	int number;
};
int p_x = 0;
int p_y = 0;
//sem_t mutex;

struct List *add_list(struct List *list);
void *derection(void *arg);
void print_list(struct List *list);
void init_wall();
void randomm();
struct List *move_add_del(struct List *list);
void myfree(struct List *list);

void *put_things(void *arg);
int x = 0,y = 0;
struct List *create_list()
{
	struct List *list = malloc(sizeof(*list));//创建链表
	list -> first = NULL;
	list -> last = NULL;
	list -> number = 0;
	struct listnode *pnew = malloc(sizeof(*pnew));//初始化节点
	pnew -> pos_x = 30;
	pnew -> pos_y = 10;
	pnew -> pre = NULL;
	pnew -> next = NULL;
	list -> first = list -> last = pnew;
	list -> first -> pre = list -> last;
	list -> last -> next = list -> first;
	//list -> number++;
	return list;
}
struct List *add_list(struct List *list)
{
	struct listnode *pnew = malloc(sizeof(*pnew));
	pnew -> pos_x = p_x ;
	pnew -> pos_y = p_y;
	pnew -> next =NULL;
	pnew -> pre = NULL;
	list -> number++;
	list -> first -> pre = pnew;
	pnew -> next = list -> first;
	list -> first = pnew;
	list -> first -> pre = list -> last;
	list -> last -> next = list -> first;
	randomm();
	return list;
}
struct List *move_add_del(struct List *list)
{
	if(list -> first -> pos_x +x >= 60 || list -> first -> pos_y +y >=20||
	list -> first -> pos_x +x <= 0 ||list -> first -> pos_y +y <= 0)
	{
		myfree(list);
		endwin();
		printf("您已撞墙,游戏结束\n");
		exit(1);
	}
	struct listnode *pp = list -> first;
	while(list -> number !=0 && pp)
	{
		if(list -> first -> pos_x +x== pp -> pos_x &&
		list -> first -> pos_y + y== pp -> pos_y)
		{
			myfree(list);
			endwin();
			printf("你已撞自己，游戏结束\n");
			exit(1);
		}
		pp = pp -> next;
		if(pp == list -> first)
		{
			break;
		}
		
		
	}
	
	//移动 头插，删除尾节点
	struct listnode *pnew = malloc(sizeof(*pnew));
	struct listnode *p = NULL;
	pnew -> pos_x = list -> first ->pos_x + x;
	pnew -> pos_y = list -> first ->pos_y + y;
	pnew -> next = NULL;
	pnew -> pre = NULL;
	list -> first -> pre =pnew;
	pnew -> next = list -> first;
	list -> first = pnew;
	p = list -> last;
	list -> last = list -> last -> pre;
	p -> next =NULL;
	p -> pre = NULL;
	list -> first -> pre = list -> last;
	list -> last -> next = list -> first;
	free(p);
	return list;
}
void *derection(void *arg)
{
	while(1)
	{
		char ch = getch();//获取键盘输入单个字符
		switch(ch)
		{
			case MOVE_LEFT:
				x = -1;y = 0;break;
			case MOVE_RIGHT:
				x = 1;y = 0;break;
			case MOVE_UP:
				x = 0;y = -1;break;
			case MOVE_DOWN:
				x = 0;y = 1;break;
			default:break;
		}
	}

}
void print_list(struct List *list)
{
	clear();//清空屏幕
	struct listnode *p = list -> first;
	while(p)
	{
		move(p->pos_y,p->pos_x);
		addch('*');
		p = p->next;
		if(p == list->first)
		{
			break;
		}
	}
	move(p_y,p_x);
	addch('*');
	move(25,30);
	char str[20];
	sprintf(str,"%d",list -> number);
	addstr(str);
	init_wall();
	refresh();
	usleep(100000);
}

//初始化地图
void init_wall()
{
	int x1,y1;
	for(y1 = 0;y1 < 21 ;y1++)
	{
		for(x1 = 0;x1 < 61;x1++)
		{
			move(y1,x1);
			if(x1 == 0 || x1 == 60 || y1 == 0 || y1 == 20)//画墙
			{
				addch('*');
			}
		}
	}
	//refresh();
}

//获取食物得随机位置并放置
void randomm()
{
	srand(time(NULL));
	p_x = rand()%59+1;
	p_y = rand()%19+1;
	move(p_y,p_x);
	addch('*');
	refresh();
}
void *put_things(void *arg)
{
	struct List *list = create_list();//创建贪吃蛇链表
	randomm();//初始化获取食物得随机位置并放置
	while(1)
	{
		if(list -> first ->pos_x == p_x &&list ->first -> pos_y == p_y)//吃到食物，则增加节点
		{
			list = add_list(list);
			print_list(list);
		}	
		else//未吃到则移动
		{	
			list = move_add_del(list);
			print_list(list);
		}
	}
}
void myfree(struct List *list)
{
	struct listnode *pn = NULL;
	list -> last -> next =NULL;
	while(list -> first)
	{
		pn = list -> first;
		list -> first = pn -> next;
		pn -> next = NULL;
		pn -> pre = NULL;
		free(pn);
	}
	free(list);
}
int main()
{	
	initscr();//打开curses模式
	refresh();////刷新缓存  更新屏幕 基于标准屏幕stdscr
	init_wall();
	
	pthread_t tid_get, tid_put;
	//sem_init(&mutex , 0, 1 );
	
	//按方向线程
	pthread_create(&tid_get, 
					NULL,
					derection, 
					NULL );
	//主要处理
	pthread_create(&tid_put, 
					NULL,
					put_things, 
					NULL );
	pthread_join(tid_get,  NULL);
	pthread_join(tid_put,	NULL);
	//endwin();
}

