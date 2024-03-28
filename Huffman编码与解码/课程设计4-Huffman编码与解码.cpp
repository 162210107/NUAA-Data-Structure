//给一篇英文文章（text），统计各字符出现(仅需包括英文大小写字母)次数。
//（1）	输出每个字符出现的次数，并进行Huffman树构造，将每个字符的编码存入到文件code1.txt。
//（2）	输出字符串”Data Structure”的编码。
//（3） 将英文文章前4段的Huffman编码保存到文件code2.txt。
//（4） 实现解码功能, 对文章的前2段进行解码。
#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include <iostream>

int Index[60]={0};//记录文件中英文字母的个数
//前26个为小写，后26个为大写
char HC[53][53];//字符编码结果

using namespace std;

//哈夫曼树
typedef struct
{
    int weight;
    int parent;
    int lchild;
    int rchild;
}HTNode;

//反转字符串
void reverseS(char* temp, int end)
{
    //双指针
    int left = 0;
    int right = end - 2;
    while (left < right)
    {
        char ret = temp[left];
        temp[left] = temp[right];
        temp[right] = ret;

        left++;
        right--;
    }
}

//选择两个最小的数,且没有父亲结点（没有被建）
void select_two_small(HTNode* HT, int end, int& s1, int& s2)
{
    for (int i = 1; i <= end; i++)
    {
        if (HT[i].weight < HT[s1].weight && HT[i].parent == 0)
            s1 = i;
    }

    for (int i = 1; i <= end; i++)
    {
        if (HT[i].weight < HT[s2].weight && HT[i].parent == 0 && i != s1)
            s2 = i;
    }
}

//初始化树
void initHT(HTNode* HT)
{
    HT[0].weight = 360000;
    HT[0].parent = 0;
    HT[0].lchild = 0;
    HT[0].rchild = 0;
    //初始化0，便于后续找最小

    for (int i = 1; i <= 52; i++)
    {
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
        HT[i].weight = Index[i];
    }
    //初始化叶子结点

    for (int i = 53; i < 104; i++)
    {
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
        HT[i].weight = 0;
    }
    //初始化结点
}

//将temp存入HC中
void strcpys(int index, char temp[], int end)
{
    for (int i = 0; i < end; i++)
    {
        HC[index][i] = temp[i];
    }
}

//哈夫曼树
void HufCode()
{
    //52个叶结点+1个最大结点（便于找最小值)
    HTNode HT[104];

    //初始化结点
    initHT(HT);

    //创建哈夫曼树
    for (int i = 53; i < 2 * 52; i++)
    {
        int s1 = 0;
        int s2 = 0;
        //在1-i-1区间里面找两个parent=0等权值最小的s1，s2
        select_two_small(HT, i - 1, s1, s2);

        //建树
        HT[s1].parent = HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }

    //从叶节点到根节点获取
    char temp[53]={0};
    //分配临时空间存储编码
    for (int i = 1; i <= 52; i++)
    {
        int start = 0;
        //一个孩子c和一个父亲f，一直往上遍历
        for (int c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent)
        {
            if (HT[f].lchild == c)
                temp[start++] = '1';
            else if (HT[f].rchild == c)
                temp[start++] = '0';
        }
        temp[start++] = '\0';
        reverseS(temp,start);//翻转才是要的编码
        strcpys(i, temp,start);
    }
}

int SearchHC(char temp[])
{
    for (int i = 1; i <= 52; i++)
    {
        if (strcmp(temp, HC[i])==0)
            return i;
    }
}

int main()
{
    fstream fp;
    fp.open("text.txt", ios::in);
    if (!fp.is_open())
    {
        cout << "文件打开失败" << endl;
        return 0;
    }
    //读取文件，统计单词个数到数组
    while (!fp.eof())
    {
        char temp;
        fp >> temp;

        if (temp >= 'a' && temp <= 'z')
            Index[temp - 'a'+1]++;
        else if (temp >= 'A' && temp <= 'Z')
            Index[temp - 'A' + 1 + 26]++;
    }
    fp.close();

    cout << "统计结果" << endl;
    for (int i = 1; i <= 52; i++)
        cout << Index[i]<<" ";
    cout << endl;

    HufCode();

    fstream fp1;
    fp1.open("code1.txt", ios::out);
    for (int i = 1; i <= 52; i++)
        fp1 << HC[i] << endl;
    fp1.close();

    cout << "转译Data Structure" << endl;
    char temp[20] = "Data Structure";
    string answer="";
    for (int i = 0; i <= 13; i++)
    {
        if (temp[i] >= 'a' && temp[i] <= 'z')
            answer += HC[temp[i] - 'a' + 1];
        else
            answer += HC[temp[i] - 'A' + 1 + 26];
        answer += ' ';
    }
    cout << answer << endl;

    FILE*fp2=fopen("text.txt","r");
    fstream fp3;
    fp3.open("code2.txt", ios::out);
    string answer1 = "";
    int parage = 0;
    cout << "前四段结果" << endl;
    while (!feof(fp2)&&parage<4)
    {
        char temp;
        temp = fgetc(fp2);

        if (temp >= 'a' && temp <= 'z')
        {
            answer1 += HC[temp - 'a' + 1];
            answer1 += ' ';
        }
        else if (temp >= 'A' && temp <= 'Z')
        {
            answer1 += HC[temp - 'A' + 1 + 26];
            answer1 += ' ';
        }
        if (temp == '\n')
        {
            parage++;
            cout << "第"<<parage<<"段"<<endl<<answer1 << endl;
            fp3 << answer1 << endl;
            answer1 = "";
        }
    }
    fp3.close();
    fclose(fp2);

    FILE* fp4 = fopen("code2.txt", "r");
    string answer2 = "";//每一段的结果
    int parage1 = 0;//记录段
    while (!feof(fp4) && parage1 < 2)
    {
        char temp;
        temp = fgetc(fp2);
        char word[53];
        int number = 0;

        //存储该单词的编码
        while (temp == '0' || temp == '1')
        {
            word[number++] = temp;
            temp = fgetc(fp2);
        }
        word[number] = '\0';
        
        //是单词的时候
        if (number != 0)
        {
            //搜索word在HC中对应的下标
            int index = SearchHC(word);
            if (index <= 26)
                answer2 += 'a' + index - 1;
            else if (index > 26)
                answer2 += 'A' + index - 26 - 1;
        }
       
        if (temp == '\n')
        {
            parage1++;
            cout << "第" << parage1 << "段" << endl << answer2 << endl;
            fp3 << answer2 << endl;
            answer2= "";
        }
    }

    return 0;
}