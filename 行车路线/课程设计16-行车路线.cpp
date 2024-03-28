#include <iostream>
#include <cstring>
#include <algorithm>
#include <queue>
#include <fstream>

using namespace std;

const int N = 510, M = 200010, INF = 0x3f3f3f3f;
int h[N],                    e[M],          w[M], tag[M], ne[M],        idx;
//以该顶点为起点的边的编号，该编号边的终点，权重，类型，与该边关联的边，编号
int dist[N][1010];//连续小道的长度之和一定不会超过1000。
//dist[i][j]——表示从1号点走到第i号点，路径包含最后一段是连续小道的总长度为j的最短路径
bool st[N][1010];//点的去重标记也跟随拆点
int path[N][1010];//记录连续长度为j时的前驱节点
int continuelength[N][1010];//记录连续长度为j的前驱节点的最短路径的连续长度
int n, m;//点数和边数

//邻接表
void add(int t, int a, int b, int c) 
{
    e[idx] = b, w[idx] = c, tag[idx] = t, ne[idx] = h[a], h[a] = idx++;
}

struct Node 
{//拆点，由于y的不同，x号点被拆成若干点
    int x, y, d;//y是小道的连续长度，d为最短疲惫
    bool operator<(const Node& p)const 
    {
        return d > p.d;
    }
};

//堆优化版dijkstra
void dijkstra() 
{
    priority_queue<Node> heap;//priority_queue默认大根堆，由于Node内置排序为降序，因此实际意义还是小根堆
    heap.push({ 1,0,0 });
    memset(dist, 0x3f, sizeof dist);
    dist[1][0] = 0;
    while (heap.size()) 
    {
        Node t = heap.top();
        heap.pop();

        //已经访问
        if (st[t.x][t.y])continue;

        st[t.x][t.y] = true;//标记，将该点拆开来

        //访问与t.x相邻的点
        for (int i = h[t.x]; i != -1; i = ne[i]) 
        {
            int k = e[i], weight = w[i];
            if (tag[i]) 
            {//小路
                if (t.y + weight <= 1000)//连续小道的长度之和一定不会超过1000
                    if (dist[k][t.y + weight] > t.d - t.y * t.y + (t.y + weight) * (t.y + weight)) 
                    {
                        dist[k][t.y + weight] = t.d - t.y * t.y + (t.y + weight) * (t.y + weight);
                        if (dist[k][t.y + weight] <= INF) heap.push({ k,t.y + weight,dist[k][t.y + weight] });
                        path[k][t.y + weight] = t.x;
                        continuelength[k][t.y+weight] = t.y;
                    }
            }
            else 
            {//大路
                if (dist[k][0] > t.d + weight) 
                {
                    dist[k][0] = t.d + weight;
                    if (dist[k][0] <= INF) heap.push({ k,0,dist[k][0] });
                    path[k][0] = t.x;
                    continuelength[k][0] = t.y;
                }
            }
        }
    }
}

int main() 
{
    memset(h, -1, sizeof h);

    fstream fp;
    fp.open("test.txt", ios::in);

    fp >> n >> m;

    int t, a, b, c;
    while (m--) 
    {
        fp >> t >> a >> b >> c;
        //无向图
        add(t, a, b, c);
        add(t, b, a, c);
    }
    fp.close();

    dijkstra();

    vector<int>ans;
    ans.push_back(0);
    int nlianxu;
    for (int j = 2; j <= n; j++)
    {
        int res = INF;
        nlianxu = 0;
        for (int i = 0; i <= 1000; i++)
        {
            if (res > dist[j][i])
                nlianxu = i;
            res = min(res, dist[j][i]);
        }
        ans.push_back(res);
    }

    for (int i = 0; i < n; i++)
        cout << "到" << i + 1 << "最小疲劳值" << ans[i] << endl;

    cout << "最短路径为:" << endl;
    int endknot = n;
    while (path[endknot][nlianxu])
    {
        cout << endknot << " ";
        int k = endknot;
        endknot = path[k][nlianxu];
        nlianxu = continuelength[k][nlianxu];
    }
    cout << 1;
}