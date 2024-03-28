#include<cstdio>
#include<iostream>
#include<algorithm>
#include <fstream>
using namespace std;

const int N = 1002;//点数
const int M = 100002;//边
const int INF = 100000;
int n, m;//输入点数和边数
int index[N];//存储生成树的边

int a[N][N];
bool visit[N] = {};//节点是否已经被访问 
int dist[N];//顶点与集合的距离
int knot[N]={0};//顶点与集合中哪一点距离最短

struct edge
{
	int x, y;//起点和终点
	float cost;
}Edges[M];//边 

int father[N];//并查集

int findfather(int x)
{
	int a = x;
	int z;

	//到这条边最初的起点
	while (x != father[x])
		x = father[x];

	//路径压缩
	while (a != father[a])
	{
		z = a;
		a = father[a];
		father[z] = x;
	}

	return x;
}

//比较权值
bool compare(edge a, edge b) { return a.cost < b.cost; }

float kruskal(int n, int m)
{
	float sum = 0;
	int edgenum = 0;//权和点数
	
	for (int i = 1; i <= n; i++)
		father[i] = i;
	//初始化并查集

	sort(Edges, Edges + m, compare);//从小到大排序

	for (int i = 0; i < m; i++)
	{
		//判断该条边起点和终点在该边未加入时是否连通
		int fax = findfather(Edges[i].x);
		int fay = findfather(Edges[i].y);

		//不连通则可以加入
		if (fax != fay)
		{
			father[fax] = fay;
			sum += Edges[i].cost;
			index[edgenum++]=i;
			if (edgenum == n - 1)break;//找到最小生成树
		}
	}
	return sum;
}

void prim()
{
	fill(dist, dist + N, INF);//赋初值
	dist[1] = 0;//将1顶点纳入生成树
	
	float sum = 0;//总费用

	cout << "边：" << endl;
	//找最小距离
	for (int i = 1; i <= n; i++)
	{
		int minx = -1;//记录顶点
		int mini = INF;//记录距离

		for (int j = 1; j <= n; j++)
		{
			if (visit[j] == false && dist[j] <= mini)
			{
				minx = j;
				mini = dist[j];
			}
		}//找到与当前集合距离最近的节点 

		if (minx != -1)//找到符合条件的点
		{
			visit[minx] = true;//第minx号点已经访问过了
			sum += dist[minx];//加入最小生成树,该点加入集合 
			if (i != 1)
				cout << minx << "----" << knot[minx] << endl;
		}

		//更新剩余的点到集合的最短距离
		for (int y = 1; y <= n; y++)
		{
			if (visit[y] == false && a[minx][y] != INF && a[minx][y] < dist[y])
			{
				dist[y] = a[minx][y];
				knot[y] = minx;
			}
		}
	}
	cout<<"Prim的最小权值:"<<sum;
}

int main()
{
	fstream fp;
	fp.open("test.txt", ios::in);

	fp >> n >> m;//初始化输入 	
	fill(a[0], a[0] + N * N, INF);//初始化
	for (int i = 0; i < m; i++)
	{
		fp >> Edges[i].x >> Edges[i].y >> Edges[i].cost; //初始化输入边	
		a[Edges[i].x][Edges[i].y] = a[Edges[i].y][Edges[i].x] = Edges[i].cost;
	}
	fp.close();

	float quanzhi1=kruskal(n,m);
	cout << "边:" << endl;
	for (int i = 0; i < n - 1; i++)
		cout << Edges[index[i]].x << "----" << Edges[index[i]].y << endl;
	cout << "KrusKal的最小权值:" << quanzhi1 << endl;

	prim();

	return 0;
}