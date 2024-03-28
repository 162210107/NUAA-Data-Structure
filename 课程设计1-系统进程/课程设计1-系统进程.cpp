#undef UNICODE
#undef _UNICODE
#include<iostream>
#include<iomanip>
#include<cstdio>
#include<windows.h>
#include<TLHELP32.h>
#include<iomanip>
#include<string>
#include"Psapi.h"
#include<ctime>
#pragma comment(lib,"Psapi.lib")
using namespace std;

typedef struct DLNode 
{
	char name[100];//进程名
	int duration;//持续时间
	int endTime;//结束时间
	DLNode* prior, * next;
}DLNode, * DLinkList;//双向链表

typedef struct SLNode 
{
	char name[100];//进程名
	int duration;//持续时间
	int memory;//内存使用情况
	SLNode* next;
}SLNode, * SLinkList;//单向链表

//获取内存
int GetMemoryInfo(DWORD processID)
{
	//API
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL)
		return 0;

	//GetProcessMemoryInfo()用于获取内存的使用情况
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		CloseHandle(hProcess);

		//pmc.WorkingSetSize就是程序在当前代码处的内存使用量
		return pmc.WorkingSetSize;
	}
}

//创建当前进程链表
int CreateList(SLinkList& S) 
{
	//头结点
	S = (SLinkList)malloc(sizeof(SLNode));
	S->duration = 0;
	S->memory = 0;
	S->next = NULL;

	SLinkList Fakehead = S, newnode;

	//存放进程信息的结构体
	PROCESSENTRY32 temp;
	temp.dwSize = sizeof(temp);

	//获取系统内的所有进程
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnapshot)
	{
		printf("未获得进程！\n");
		return 0;
	}

	//获取进程
	BOOL bMore = Process32First(hProcessSnapshot, &temp);
	while (bMore) 
	{
		//获得当前进程的内存量
		int judge = GetMemoryInfo(temp.th32ProcessID);
		if (judge) 
		{
			//新进程的内容赋给新结点，接在双向链表最后
			newnode = (SLinkList)malloc(sizeof(SLNode));
			newnode->memory = judge / 1024;
			strcpy_s(newnode->name, temp.szExeFile);
			newnode->duration = 0;
			Fakehead->next = newnode;
			newnode->next = NULL;
			Fakehead = newnode;
		}
		bMore = Process32Next(hProcessSnapshot, &temp);
	}
	//清除hProcess句柄
	CloseHandle(hProcessSnapshot);
	return 1;
}

//根据PID，在储存结束时间的链表中查找该进程
void LocateOverList(DLinkList& D, char* key, DLinkList& p)
{
	//在已结束进程中查找
	p = D->next;
	while (p != D)
	{
		if (strcmp(p->name, key)==0)
			return;
		p = p->next;
	}
	return ;
}

//创建已结束进程链表
int CreateEndedList(DLinkList& Finished, SLinkList Total)
{
	//把总进程链表所有内容赋给记录已结束进程链表
	Finished = (DLinkList)malloc(sizeof(DLNode));
	Finished->duration = 0;//总时间
	Finished->endTime = 0;//结束时间
	Finished->next = Finished;
	Finished->prior = Finished;
	SLinkList  cur_S = Total->next;
	DLinkList cur_D = Finished, newnode;
	while (cur_S) 
	{
		newnode = (DLinkList)malloc(sizeof(DLNode));
		strcpy_s(newnode->name, strlen(cur_S->name) + 1, cur_S->name);
		newnode->duration = 0;
		newnode->endTime = 0;

		newnode->next = cur_D->next;
		cur_D->next->prior = newnode;
		cur_D->next = newnode;
		newnode->prior = cur_D;
		
		cur_D = cur_D->next;
		cur_S = cur_S->next;
	}
	return 1;
}

//按持续时间给进程排序
int SortDList(DLinkList& D) 
{
	//统计结点数目
	int statisticNum = 0;
	DLinkList statistic = D->next;
	while (statistic!=D) 
	{
		statisticNum++;
		statistic = statistic->next;
	}

	//冒泡排序
	DLinkList q = D->next;
	DLinkList t = (DLinkList)malloc(sizeof(DLNode));
	//标记排序是否发生交换
	//若不发生交换
	//则排序完成
	int flag = 1;

	while (flag == 1 && statisticNum-1> 0)
	{
		q = D->next;
		//若没有发生交换
		//flag为0，则不会发生下一趟排序
		flag = 0;
		for (int i = 1; i < statisticNum; i++)
		{
			if (q->duration > q->next->duration)
			{
				flag = 1;
				strcpy_s(t->name, strlen(q->name) + 1, q->name);
				t->endTime = q->endTime;
				t->duration = q->duration;
				strcpy_s(q->name, strlen(q->next->name) + 1, q->next->name);
				q->endTime = q->next->endTime;
				q->duration = q->next->duration;
				strcpy_s(q->next->name, strlen(t->name) + 1, t->name);
				q->next->endTime = t->endTime;
				q->next->duration = t->duration;
			}
			q = q->next;
		}
		statisticNum--;
	}
	free(t);
	return 1;
}

//按照内存大小排序
int SortSList(SLinkList& S) 
{
	//记录结点数目
	int statisticNum = 0;
	SLinkList statistic = S->next;
	while (statistic) 
	{
		statisticNum++;
		statistic = statistic->next;
	}

	//冒泡排序
	SLinkList q = S->next;
	SLinkList t = (SLinkList)malloc(sizeof(SLNode));
	//标记排序是否发生交换
	//若不发生交换
	//则排序完成
	int flag = 1;

	while (flag == 1 && statisticNum - 1 > 0)
	{
		q = S->next;
		//若没有发生交换
		//flag为0，则不会发生下一趟排序
		flag = 0;
		for (int i = 1; i < statisticNum; i++)
		{
			if (q->memory < q->next->memory)
			{
				flag = 1;
				strcpy_s(t->name, strlen(q->name) + 1, q->name);
				t->memory = q->memory;
				t->duration = q->duration;
				strcpy_s(q->name, strlen(q->next->name) + 1, q->next->name);
				q->memory = q->next->memory;
				q->duration = q->next->duration;
				strcpy_s(q->next->name, strlen(t->name) + 1, t->name);
				q->next->memory = t->memory;
				q->next->duration = t->duration;
			}
			q = q->next;
		}
		statisticNum--;
	}
	free(t);
	return 1;
}

//统计已结束进程
int UpdateTime(SLinkList& Total, SLinkList& Current, DLinkList& Finished)
{
	//判断有无找到未结束进程
	int judge;

	DLinkList p_Finished = Finished->next;
	SLinkList p_Total = Total->next, p_Current;

	//头结点用来记录调试时间
	Finished->duration += 1;

	while (p_Total)
	{
		//重置为1
		judge = 1;

		//寻找有无未结束进程
		p_Current = Current->next;
		while (p_Current)
		{
			//该进程未结束
			if (strcmp(p_Current->name,p_Total->name)==0)
			{
				//找到，置为0
				judge = 0;
				break;
			}
			p_Current = p_Current->next;
		}

		//进程已结束
		if (judge)
		{
			//获取该结束进程在已结束进程链表的位置
			//更新结束时间
			LocateOverList(Finished, p_Total->name,p_Finished);

			//如果结束时间为0，更新结束时间
			//如果结束时间不为0，更新结束的持续时间
			if (p_Finished->endTime == 0)
				p_Finished->endTime = Finished->duration;
			else
				p_Finished->duration += 1;
		}

		//进程未结束
		//更新当前进程的运行的持续时间
		else 
		{
			p_Total->duration += 1;
			p_Current->duration = p_Total->duration;
		}

		p_Total = p_Total->next;
	}
	return 1;
}

//刷新后更新进程
int UpdateProgarm(SLinkList& Total, SLinkList Current, DLinkList& Finished)
{
	//判断有无找到新进程
	int judge;

	DLinkList p_Finished = Finished->next, q_Finished = Finished->next, temp_finished ;

	SLinkList p_Total = Total->next, p_Current = Current->next, q_Total = Total->next, temp_Total;

	//遍历当前进程表
	while (p_Current) 
	{
		judge = 1;

		p_Total = Total->next;
		while (p_Total) 
		{
			//该进程已在进程表中
			if (strcmp(p_Current->name,p_Total->name)==0)
			{
				judge = 0;
				break;
			}
			p_Total = p_Total->next;
		}

		//进程不在total中,将其添加
		if (judge)
		{
			while (q_Total->next)
			{
				q_Total = q_Total->next;
			}
			temp_Total = (SLinkList)malloc(sizeof(SLNode));
			temp_Total->memory = p_Current->memory;
			temp_Total->duration = 0;
			strcpy_s(temp_Total->name, p_Current->name);
			q_Total->next = temp_Total;
			temp_Total->next = NULL;

			while (q_Finished->next != Finished)
			{
				q_Finished = q_Finished->next;
			}
			temp_finished = (DLinkList)malloc(sizeof(DLNode));
			temp_finished->duration = 0;
			temp_finished->endTime = 0;
			strcpy_s(temp_finished->name, p_Current->name);
			q_Finished->next->prior = temp_finished;
			temp_finished->next = q_Finished->next;
			q_Finished->next = temp_finished;
			temp_finished->prior = q_Finished;
		}
		else 
		{
			p_Finished = Finished->next;
			while (p_Finished!=Finished)
			{
				if (strcmp(p_Current->name ,p_Finished->name)==0 && p_Finished->endTime != 0)
				{
					//重新启用进程
					p_Finished->endTime = 0;

					p_Current->duration = p_Total->duration = 0;
					break;
				}
				p_Finished = p_Finished->next;
			}
		}
		p_Current = p_Current->next;
	}
	return 1;
}

//显示当前进程
void ShowArray_D(SLinkList SL, DLinkList DL) 
{
	cout.setf(ios::left);
	cout << setw(20) << "当前系统进程名" << '	';
	cout.setf(ios::right, ios::left);
	cout << '	' << setw(17) << "内存使用情况" << '	' << "持续时间" << '	' << setw(20) << endl;
	cout << "----------------------------------------------------------------" << endl;

	DLinkList p = DL->next;
	SLinkList s = SL->next;

	//打印当前进程链表的内容
	while (s) 
	{
		char* nameSave = s->name;
		for (int i = 0; i < 20; i++) 
		{
			cout << nameSave[i];
			if (nameSave[i] == '\0') 
			{
				for (int j = i; j < 20; j++) 
					cout << " ";
				break;
			}
		}

		cout.setf(ios::left);
		cout << '	';

		cout.setf(ios::right, ios::left);
		cout << '	' << setw(15) << s->memory << "KB";
		if (s->duration < 60)
			cout << '	' << s->duration << "s" << endl;
		if (s->duration >= 60)
			cout << '	' << s->duration / 60 << "m  " << s->duration % 60 << "s" << endl;

		s = s->next;
	}
	cout << endl;

	if (p!=DL) 
	{
		cout.setf(ios::left);
		cout << setw(20) << "已结束进程名" << '	';
		cout.setf(ios::right, ios::left);
		cout << '	' << setw(17) << "持续时间" << '	' << "结束时间" << '	' << setw(20) << endl;
		cout << "----------------------------------------------------------------" << endl;
	}

	//当链表未结束
	//已结束进程链表的结点的endTime值不为0，打印
	while (p!=DL)
	{
		if (p->endTime != 0)
		{
			char* nameSave1 = p->name;
			for (int i = 0; i < 20; i++)
			{
				cout << nameSave1[i];
				if (nameSave1[i] == '\0')
				{
					for (int j = i; j < 20; j++)
						cout << " ";
					break;
				}
			}

			cout.setf(ios::left);
			cout << '\t';

			cout.setf(ios::right, ios::left);
			if (p->duration < 60)
				cout << '	' << setw(15) << p->duration << "s";
			if (p->duration >= 60)
				cout << '	' << setw(12) << p->duration / 60 << "m  " << p->duration % 60 << "s";


			cout.setf(ios::left, ios::right);

			if (p->endTime < 60)
				cout << '	' << p->endTime << "s" << endl;
			if (p->endTime >= 60)
				cout << '	' << p->endTime / 60 << "m  " << p->endTime % 60 << "s" << endl;
		}

		p = p->next;
	}
}

int main() 
{
	//Total为总进程表，cuttent为当前进程表
	SLinkList Total;
	SLinkList Current;
	//Finished中存储已结束进程
	DLinkList Finished;
	//创建该程序执行时得到的最初的进程表，用于与当前进程比对，来获取已结束进程
	CreateList(Total);
	//创建结束进程表
	CreateEndedList(Finished, Total);
	//排序
	SortSList(Total);
	//打印
	ShowArray_D(Total, Finished);

	int times = 5;//次数

	while (times) 
	{
		Sleep(500);
		//清屏
		system("cls");
		//获取当前进程表
		CreateList(Current);
		//排序
		SortSList(Current);
		//调整结束时间及持续时间
		UpdateTime(Total, Current, Finished);
		//调整重新调用的程序及新调用的程序
		UpdateProgarm(Total, Current, Finished);
		//排序
		SortDList(Finished);
		//打印
		ShowArray_D(Current, Finished);
		times--;
		//暂停一下
		system("pause");
	}
	system("pause");
	return 0;
}