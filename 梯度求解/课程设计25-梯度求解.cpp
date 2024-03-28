#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include<sstream>
#include <stack>

using namespace std;

long long mod = 1e9 + 7;
vector<long long>val;//存储求导时的变量值

struct item
{
    long long k;//常系数
    map<int, int>mp;//存储每一项
    item(long long coe, map<int, int>mp) :k(coe), mp(mp) {}//结构体构造函数
};

struct fomula
{
    vector<item> value;//每一项
    fomula(vector<item>vec) :value(vec) {}//有构造函数才能返回
};

stack<fomula>st;//栈S

long long convert(string str)//字符串转换成数字
{
    long long num = 0;
    for (int i = (str[0] == '-') ? 1 : 0; i < str.length(); i++)
    {
        num *= 10;
        num += str[i] - '0';
    }
    return (str[0] == '-') ? -1 * num : num;
}

item item_mul(item A, item B)//乘积项乘法函数
{
    long long k = A.k * B.k;
    map<int, int>mp_c;
    map<int, int>::iterator it;//创建迭代器
    //更新A项中每个变量的指数
    for (it = A.mp.begin(); it != A.mp.end(); it++)
    {
        mp_c[it->first] = A.mp[it->first] + B.mp[it->first];
        B.mp.erase(it->first);//同时删除，便于后续添加B项中指数
    }
    //添加B中的项
    for (it = B.mp.begin(); it != B.mp.end(); it++)
        mp_c[it->first] = B.mp[it->first];

    return item(k, mp_c);
}

fomula fomula_mul(fomula A, fomula B)//多项式乘法
{
    vector<item>vec;
    for (int i = 0; i < A.value.size(); i++)
        for (int j = 0; j < B.value.size(); j++)
            vec.push_back(item_mul(A.value[i], B.value[j]));//A每一项都会×B中每一项

    return fomula(vec);
}

fomula fomula_add(fomula A, fomula B)//多项式加法
{
    for (int i = 0; i < B.value.size(); i++) {
        A.value.push_back(B.value[i]);
    }
    return A;
}

fomula fomula_sub(fomula A, fomula B)//多项式减法
{
    //每一项带符号再相加
    for (int i = 0; i < A.value.size(); i++) {
        A.value[i].k *= -1;
    }
    return fomula_add(B, A);
}

long long function(fomula A, int goal)//求导函数 对最终的fomula求导
{
    long long sum = 0, mul;
    for (int i = 0; i < A.value.size(); i++)
    {
        item now = A.value[i];
        mul = 1;
        if (now.mp.find(goal) != now.mp.end())
        {//此乘积项含有要求导的变量才拥有计算价值
            mul = (now.k * now.mp[goal]) % mod;
            now.mp[goal]--;//求导过程

            //计算该项求导结果
            for (map<int, int>::iterator it = now.mp.begin(); it != now.mp.end(); it++)
                for (int k = 0; k < it->second; k++)
                    mul = (mul * val[it->first]) % mod;

            sum = (sum + mul) % mod;
        }
    }
    return sum;
}

int main()
{
    int n, m;
    cin >> n >> m;//求解函数中所含自变量的个数和要求解的偏导数的个数
    getchar();//清空缓存区

    string s, temp;
    getline(cin, s);

    stringstream sin(s);
    while (sin >> temp)
    {
        if (temp == "+" || temp == "-" || temp == "*")
        {//运算符
            fomula A = st.top(); st.pop();//从栈中依次弹出两个formula
            fomula B = st.top(); st.pop();
            if (temp == "*")
                st.push(fomula_mul(B, A));
            else if (temp == "+")
                st.push(fomula_add(B, A));
            else
                st.push(fomula_sub(A, B));//A B的顺序很重要
        }
        else
        {
            map<int, int>mp;//下标 指数
            vector<item>vec;
            if (temp[0] == 'x')
            {//自变量
                mp[convert(temp.substr(1, temp.length() - 1))] = 1;
                vec.push_back(item(1, mp));//把乘积项包装成多项式
            }
            else
                vec.push_back(item(convert(temp), mp));//把乘积项包装成多项式
            st.push(fomula(vec));
        }
    }
    for (int i = 0; i < m; i++)
    {
        long long value;
        for (int j = 0; j < n + 1; j++)
        {
            cin >> value;
            val.push_back(value);
        }
        long long ans = function(st.top(), val[0]);
        cout << ((ans < 0) ? ans + mod : ans) << endl;//当计算整数n对M的模时，若n为负数,需要注意将结果调整至区间[0,M)内
        val.clear();
    }
}