//（1）从文件中读取数据，生成模拟迷宫地图，30行30列。
//（2）给出任意入口和出口，显示输出迷宫路线。
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

using namespace std;

int G[33][33];//迷宫
int visited[33][33] = { 0 };//标记数组
int row = 1;//迷宫行数
int col = 1;//迷宫列数
int dir[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };//走法

typedef struct item
{
    int x;
    int y;
    item(int mx,int my):x(mx),y(my) {}
};//存储坐标信息

stack<item> path;//路径
vector<item>answer;//最短路径

//将起点终点反过来，方便输出路径
void SearchPath(int endx, int endy, int startx,int starty)
{
    //终点
    if (startx == endx && starty == endy)
    {
        vector<item>temppath;
        while (!path.empty())
        {
            item temp = path.top();
            path.pop();
            temppath.push_back(temp);
        }
        for (int i = 0; i < temppath.size(); i++)
            path.push(temppath[temppath.size() - 1 - i]);
        if (temppath.size() < answer.size() || answer.size() == 0)
            answer = temppath;
        return;
    }
    else 
    {
        for (int i = 0; i < 4; i++)
        {
            int x = startx + dir[i][0];
            int y = starty + dir[i][1];
            if (x > 0 && x <= col && y > 0 && y <= row && visited[y][x] == 0&&G[y][x]==1)
            {
                path.push(item(x, y));
                visited[y][x] = 1;
                SearchPath(endx, endy, x, y);
                visited[y][x] = 0;
                path.pop();
            }
        }
    }
}

int main()
{
    FILE* fp;
    fp = fopen("text.txt", "r");
    if (!fp)
        return 0;
    while (!feof(fp))
    {
        char ch;
        ch=fgetc(fp);

        if (ch == '1' || ch == '0')
            G[row][col++] = ch - '0';

        if (ch == '\n')
        {
            row++;
            col = 1;
        }
    }
    fclose(fp);
    col--;

    cout << "该迷宫为：" << endl;
    for (int i = 1; i <= row; i++)
    {
        for (int j = 1; j <= col; j++)
            cout << G[i][j] << " ";
        cout << endl;
    }

    cout << "请输入起点x 起点y 终点x 终点y" << endl;
    int startx,starty, endx,endy;
    cin >> startx >>starty>> endx>>endy;
    while (G[starty][startx] == 0||(starty==endy&&startx==endx))
    {
        cout << "起点终点有误，请重新输入" << endl;
        cout << "请输入起点x 起点y 终点x 终点y" << endl;
        cin >> startx >> starty >> endx >> endy;
    }

    //将终点入栈
    item temp1(endx, endy);
    path.push(temp1);
    visited[endy][endx] = 1;//标记

    SearchPath(startx,starty, endx,endy);

    cout << "最短路径为：" << endl;
    for(int it=0;it<answer.size(); it++)
    {
        item temp = answer[it];
        cout << "(" << temp.x << "," << temp.y << ")" << endl;
    }

    return 0;
}