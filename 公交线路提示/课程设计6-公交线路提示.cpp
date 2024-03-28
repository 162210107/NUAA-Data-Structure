#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
using namespace std;
//公交线路信息
typedef struct Bus
{
	int No;                         //该公交车的编号
	int StationCount;               //该公交车经过的站点数量
	int BusRoute[70];               //该公交车依次经过的站点的数组下标 
}Bus;
//站点信息
typedef struct Station
{
	int StationName;                //站点名称
	int BusCount;                   //经过该站点的公交车数量
	int bus[34];                   //经过该站点的公交车的编号
}Station;
//大的公交路线图
typedef struct BusMap
{
	Bus bus[730];
	Station station[6600];
	int BusCount;                  //公交车数量
	int StationCount;              //站点数量
}BusMap;
//获取站台对应的编号
int GetBusStation(BusMap& M, int ID)            
{
	for (int i = 1; i <= M.StationCount; i++)
	{
		if (M.station[i].StationName==ID)
			return i;
	}
	return 0;
}
//更新图信息
void IncreaseBusStation(BusMap& M, int ID)
{
	int num = GetBusStation(M, ID);
	//该站台未创建
	if (num == 0)
	{
		//纳入新的station，并将count++
		M.station[++M.StationCount].StationName= ID;
		num = M.StationCount;//现在ID在站点的编号
	}
	//给当前公交线加入新的站点，并把该公交线的站点数量++
	M.bus[M.BusCount].BusRoute[M.bus[M.BusCount].StationCount++] = num;
	//更新该站点经过的公交线路，以及经过公交数量
	M.station[num].bus[++M.station[num].BusCount] = M.bus[M.BusCount].No;
}
//创建图 
int CreateMap(BusMap& M)                                 
{
	M.BusCount = 0;//公交车数量
	M.StationCount = 0;//站点总数
	//初始化
	for (int i = 0; i < 730; i++)
		M.bus[i].StationCount = 1;
	for (int i = 0; i < 6600; i++)
		M.station[i].BusCount = 0;

	int ID=0;//站点ID
	char ch;//一个一个获取字符
	FILE* fp;
	if ((fp = fopen("test.txt", "r")) == NULL)
	{
		cout<<"文件打开失败"<<endl;
		exit(0);
	}
	//获取第一个公交车ID
	int busid = 0;
	ch = fgetc(fp);
	while (ch != ' ')
	{
		busid = busid * 10 + ch - '0';
		ch = fgetc(fp);
	}
	M.bus[++M.BusCount].No=busid;//纳入

	while (1)
	{
		ch = fgetc(fp);

		if (feof(fp))
		{
			IncreaseBusStation(M, ID);
			break;//文件结束
		}
		if (ch == ' ')//只找最后的站点ID
		{
			ID = 0;
			continue;
		}
		if (ch == '\n')
		{
			IncreaseBusStation(M, ID);
			ID = 0;
			//获取公交ID
			int busid = 0;
			ch = fgetc(fp);
			while (ch != ' ')
			{
				busid = busid * 10 + ch - '0';
				ch = fgetc(fp);
			}
			//如果和先前的不一样，则开始新的线路创建
			if (busid != M.bus[M.BusCount].No)
			{
				M.bus[++M.BusCount].No = busid;
				continue;
			}
			else
				continue;
		}
		ID = ID * 10 + ch - '0';
	}
	fclose(fp);
}
//获取公交线路对应的编号 
int GetBus(BusMap& M, int No)                             
{
	for (int i = 1; i <= M.BusCount; i++)
	{
		if (M.bus[i].No == No)
			return i;
	}
	return 0;
}
//队列内部节点
typedef struct LNode
{
	int data;
	struct LNode* next;
}LNode, * QueuePtr;
//队列
typedef struct LinkQueue
{
	QueuePtr front;
	QueuePtr rear;
}LinkQueue;
//队列初始化
void InitQueue(LinkQueue& Q)
{
	Q.front = Q.rear = (LNode*)malloc(sizeof(LNode));
	if (Q.front == NULL) exit(0);
	Q.front->next = NULL;
}
//1是空，0是有
int QueueEmpty(LinkQueue Q)
{
	if (Q.front != Q.rear)
		return 0;
	else
		return 1;
}
//队列入队
void EnQueue(LinkQueue& Q, int e)
{
	LNode* s;
	s = (LNode*)malloc(sizeof(LNode));
	if (s == NULL) exit(0);
	s->data = e;
	s->next = NULL;
	Q.rear->next = s;
	Q.rear = s;
}
//队列出队
int DeQueue(LinkQueue& Q)
{
	LNode* p;
	int e;
	if (Q.front == Q.rear) return -1;//队列为空
	p = Q.front->next;
	e = p->data;
	Q.front->next = p->next;
	//当前节点为尾节点
	if (Q.rear == p)
		Q.rear = Q.front;
	free(p);
	return e;
}
//NextStation记录下一站点，busno记录经过该站所乘坐的公交车 
typedef struct info                           
{
	int NextStation;
	int busno;
	int visited;
}info;

info k[6600];//记录行走路线的信息
//寻找经过站点最少的方案 
void FindLeastStationNum(BusMap& M, int StartStation, int EndStation)         
{
	//记录站点在数组中的下标
	int start = GetBusStation(M, StartStation);
	int end = GetBusStation(M, EndStation);

	//初始化
	for (int i = 1; i < 6600; i++)
	{
		k[i].visited = 0;//标记该结点是否访问过
		k[i].NextStation = i;
		k[i].busno = 0;
	}

	LinkQueue Q;
	InitQueue(Q);
	EnQueue(Q, end);          
	//倒序查找 
	k[end].visited = 1;

	while (Q.front != Q.rear)//队列不为空
	{
		end = DeQueue(Q);
		//遍历所有经过此站的线路 
		for (int i = 1; i <= M.station[end].BusCount; i++)
		{
			//获取该编号公交车对应大Map中bus数组的下标
			int b = GetBus(M, M.station[end].bus[i]);
			//遍历该路公交车，找到该站点对应的该公交线路的第几站
			for (int j = 1; j < M.bus[b].StationCount; j++)
			{
				if (M.bus[b].BusRoute[j] == end)
				{
					//下一站或上一站为起始站 
					if (M.bus[b].BusRoute[j + 1] == start || M.bus[b].BusRoute[j - 1] == start)
					{
						int count = 0;

						int bus = k[end].busno;//start后一个站点的公交
						
						cout << M.station[start].StationName<<"(乘坐"<<M.bus[b].No<<"路)->";
						while (end != GetBusStation(M, EndStation))
						{
							cout << M.station[end].StationName;
							cout << "(乘坐 " << bus << "路)->";

							end = k[end].NextStation;
							bus = k[end].busno;
							count++;
						}
						cout << M.station[end].StationName << "(到达终点，下车)" << endl;
						cout << "最少经过" << count+1 << "个站点" << endl;
						return;
					}
					//当前一站存在且未被访问 
					if (j - 1 > 0)
					{
						if (k[M.bus[b].BusRoute[j - 1]].visited == 0)
						{
							k[M.bus[b].BusRoute[j - 1]].busno = M.bus[b].No;
							k[M.bus[b].BusRoute[j - 1]].NextStation = end;
							EnQueue(Q, M.bus[b].BusRoute[j - 1]);
							k[M.bus[b].BusRoute[j - 1]].visited = 1;
						}
					}
					//当后一站存在且未被访问 
					if (j + 1 < M.bus[b].StationCount)
					{
						if (k[M.bus[b].BusRoute[j + 1]].visited == 0)
						{
							k[M.bus[b].BusRoute[j + 1]].busno = M.bus[b].No;
							k[M.bus[b].BusRoute[j + 1]].NextStation = end;
							EnQueue(Q, M.bus[b].BusRoute[j + 1]);
							k[M.bus[b].BusRoute[j + 1]].visited = 1;
						}
					}
					break;//找到第几站就不用继续for了
				}
			}
		}
	}
}
//寻找转车最少的方案
void FindLeastTransfer(BusMap& M, int StartStation, int EndStation)        
{
	int start = GetBusStation(M, StartStation);
	int end = GetBusStation(M, EndStation);
	for (int i = 1; i < 300; i++)
	{
		k[i].visited = 0;
		k[i].NextStation = i;
		k[i].busno = 0;
	}

	LinkQueue Q;
	InitQueue(Q);
	EnQueue(Q, end);                          
	k[end].visited = 1;

	//队列不为空
	while (Q.front != Q.rear)
	{
		end = DeQueue(Q);
		//遍历所有经过该站点的公交车路线
		for (int i = 1; i <= M.station[end].BusCount; i++)
		{
			int b = GetBus(M, M.station[end].bus[i]);
			//遍历该路公交车的所有站点
			for (int j = 1; j < M.bus[b].StationCount; j++)
			{
				//如果该路公交车有start结点，就直接走
				if (M.bus[b].BusRoute[j] == start)
				{
					int bus = k[end].busno;
					int count = 1;
					cout << M.station[start].StationName<<"(乘坐"<< M.bus[b].No <<"路公交车)->";
					while (end!= GetBusStation(M, EndStation))
					{
						
						cout << M.station[end].StationName<<"(换乘"<<bus<<"路车)->";
						count++;
						
						end = k[end].NextStation;
						bus = k[end].busno;
					}
					cout << M.station[end].StationName << endl;
					cout << "至少需要乘坐" << count << "班公交车" << endl;
					return;
				}
				else if (k[M.bus[b].BusRoute[j]].visited == 0)
				{
					k[M.bus[b].BusRoute[j]].visited = 1;
					k[M.bus[b].BusRoute[j]].busno = M.bus[b].No;
					k[M.bus[b].BusRoute[j]].NextStation = end;//保证该条线路都是从end发出
					EnQueue(Q, M.bus[b].BusRoute[j]);
				}
			}
		}
	}
	//先把经过该站点的所有公交线路上的站点入队，再一个一个搜索每个站点，直到找到一条线包含start就是最少转车
}

int main()
{
	BusMap M;
	CreateMap(M);
	int start, end;
	cout<<"请输入起始站点,和最终站点ID："<<endl;
	cin >> start >> end;
	while (GetBusStation(M, start) == 0 || GetBusStation(M, end) == 0)
	{
		cout << "站点输入有误！" << endl;
		cout << "请重新输入起始站点,和最终站点ID：";
		cin >> start >> end;
	}
		
	FindLeastStationNum(M, start, end);
	FindLeastTransfer(M, start, end);

	return 0;
}