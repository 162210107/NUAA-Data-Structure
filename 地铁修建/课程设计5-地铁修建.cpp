#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
int n, m;

struct road 
{
    int start, end, day;
    road(int q, int w, int e) :start(q), end(w), day(e) {};
};

bool cmp(road g, road h) 
{
    return g.day < h.day;
}

int father[100001];//并查集

int findfather(int nownode) 
{
    int u = nownode;
    
    //找到nownode的最跟结点
    while (father[nownode] != nownode) 
    {
        nownode = father[nownode];
    }

    //目前结点的根节点也是nownode
    father[u] = nownode;

    return nownode;//返回当前节点的父亲
}

int main() 
{
    int j, k, l;
    cin >> n >> m;//枢纽数量，隧道数量

    vector<road>roadlist;//隧道参数

    while (m--) 
    {
        cin >> j >> k >> l;
        roadlist.push_back(road(j, k, l));
    }

    //并查集初始化
    for (int i = 0; i < n; i++) 
        father[i] = i;

    //由小到大排序，保证1和n连接时的路径的天数是所有前面路径的最大值
    sort(roadlist.begin(), roadlist.end(), cmp);

    for (auto y : roadlist) 
    {
        //这条边将两边根节点连接连接，相当于把y.start所有的点的父亲都变为y.end的父亲
        father[findfather(y.start)] = findfather(y.end);

        //1和n连接，则结束
        if (findfather(1) == findfather(n))
        {
            cout << y.day;//目前该条路径的天数，就是前面所有的最大值
            return 0;
        }
    }
}