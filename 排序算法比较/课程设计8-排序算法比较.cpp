#include <iostream>
#include<fstream>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>     /* srand, rand */
#include <vector>
#include <queue>
#include <Windows.h>

using namespace std;

int a1[50001];
int a2[50001];
int a3[50001];
int a4[50001];
int a5[50001];
int a6[50001];
int a7[50001];
int a8[50001];
int a[9][11];
const int val_count = 50000; //8seconds for selection, 0 seconds 
int num = 0;

//冒泡排序
void bubblesort(int *a)
{
    for (int i = 0; i < num - 1; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            if (a[i] > a[j])
            {
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

//选择排序
void selectionsort(int *a)
{
    for (int i = 0; i < num - 1; i++)
    {
        int key = i;
        for (int j = i + 1; j < num; j++)
        {
            if (a[j] < a[key])
                key = j;
        }
        int temp = a[i];
        a[i] = a[key];
        a[key] = temp;
    }
}

//插入排序
void insertsort(int *a)
{
    for (int i = 1; i < num; i++)
    {
        int tmp = a[i];
        int j = 0;
        for (j = i - 1; j >= 0; j--)
        {
            if (a[j] > tmp)
                a[j + 1] = a[j];
            else
                break;
        }
        a[j + 1] = tmp;
    }
}

void shellfunction(int *a,int gap)
{
    for (int i = gap; i < num; i++)
    {
        int tmp = a[i];
        int j = 0;
        for (j = i - gap; j >= 0; j = j - gap)
        {
            if (a[j] > tmp)
                a[j + gap] = a[j];
            else
                break;
        }
        a[j + gap] = tmp;
    }
}

//希尔排序
void shellsort(int *a)
{
    int gap = num;//间隔
    while (gap > 1)
    {
        gap = gap / 2;
        shellfunction(a,gap);
    }
}

//归并排序
void mergesort(int* a,int left,int right)
{
    if (a==NULL||left >= right)
        return;
    int mid = (left + right) / 2;
    mergesort(a, left, mid);
    mergesort(a, mid + 1, right);

    int* tmp = (int*)malloc((right - left + 1) * sizeof(int));
    // tmp是汇总2个有序区间的临时区域。
    int i = left; // 第一个有序区的索引
    int j = mid + 1; // 第二个有序区的索引
    int k = 0; // 临时区域的索引
    while (i <= mid && j <= right) 
    {
        if (a[i] <= a[j]) 
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }
    while (i <= mid) 
        tmp[k++] = a[i++];
  
    while (j <= right) 
        tmp[k++] = a[j++]; // 将两个有序区间合并
    
    // 排序后的元素，全部都整合到数组a中
    for (i = 0; i < k; i++) 
        a[left + i] = tmp[i];
}

void maxheap_sort(int* a, int start, int end)
{
    int c = start; // 当前（current）节点的位置
    int l = 2 * c + 1; // 左（left）孩子的位置
    int tmp = a[c]; // 当前（current）节点的大小
    for (; l <= end; c = l, l = 2 * l + 1)
    {
        // “l”是左孩子，“l+1”是右孩子
        if (l < end && a[l] < a[l + 1])
            l++; // 左右两个孩子中选择较大者，即m_heap[l+1]
        if (tmp >= a[l])
            break; // 调整结束d
        else
        {// 交换值
            a[c] = a[l];
            a[l] = tmp;
        }
    }
}

//堆排序
void heapsort(int* a)
{
    int i;
    // 从（n/2-1）--> 0 逐次遍历。遍历之后，得到的数组实际上是一个（最大）二叉堆
    for (i = num / 2 - 1; i >= 0; i--) {
        maxheap_sort(a, i, num - 1);
    }
    // 从最后一个元素开始对序列进行调整，不断地缩小调整的范围直到第一个元素
    for (i = num - 1; i > 0; i--) {
        // 交换a[0]和a[i]。交换后，a[i]是a[0..i]中最大的。
        int temp = a[0];
        a[0] = a[i];
        a[i] = temp;
        // 调整a[0..i-1]，使得a[0..i-1]仍然是一个最大堆。
        // 即，保证a[i-1]是a[0..i-1]中的最大值。
        maxheap_sort(a, 0, i - 1);
    }
}

//快速排序
void quicksort(int *a,int start,int end)
{
    if (start > end)
        return;

    int i = start;
    int j = end;
    int pirot = a[i];
    while (i < j)
    {
        while (i<j && a[j]>pirot)
            j--;
        while (i < j && a[i] <= pirot)
            i++;
        if (i < j)
        {
            int tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }
    a[start] = a[i];
    a[i] = pirot;
    quicksort(a,start, i-1);
    quicksort(a,i + 1, end);
}

//基数排序
void radixsort(int* a)
{
    //max为数组中最大值
    int max = a[0];
    int base = 1;

    //找出数组中的最大值
    for (int i = 0; i < num; i++)
        if (a[i] > max)
            max = a[i];
    //循环结束max就是数组最大值

    //临时存放数组元素的空间
    int* tmp = (int*)malloc(sizeof(int) * num);

    //循环次数为最大数的位数
    while (max / base > 0)
    {
        //定义十个桶，桶里面装的不是数据本身，而是每一轮排序对应（十、白、千...）位的个数
        //统计每个桶里面装几个数
        int bucket[10] = { 0 };
        for (int i = 0; i < num; i++)
            //arr[i] / base % 10可以取到个位、十位、百位对应的数字
            bucket[a[i] / base % 10]++;
        //循环结束就已经统计好了本轮每个桶里面应该装几个数

        //将桶里面的元素依次累加起来，就可以知道元素存放在临时数组中的位置
        for (int i = 1; i < 10; i++)
            bucket[i] += bucket[i - 1];
        //循环结束现在桶中就存放的是每个元素应该存放到临时数组的位置

        //开始放数到临时数组tmp
        for (int i = num - 1; i >= 0; i--)
        {
            tmp[bucket[a[i] / base % 10] - 1] = a[i];
            bucket[a[i] / base % 10]--;
        }
        //不能从前往后放，因为这样会导致十位排好了个位又乱了，百位排好了十位又乱了
        /*for (int i = 0; i < n; i++)
        {
            tmp[bucket[arr[i] / base % 10] - 1] = arr[i];
            bucket[arr[i] / base % 10]--;
        }*/

        //把临时数组里面的数拷贝回去
        for (int i = 0; i < num; i++)
            a[i] = tmp[i];
        base *= 10;
    }
    free(tmp);
}

void print(int* a)
{
    cout << "数组是:" << endl;
    for (int i = 0; i < num; i++)
        cout << a[i] << "  ";
    cout << endl;
}

void test(int i)
{
    srand(time(NULL));
    fstream file;
    file.open("big-a.txt", ios::out);
    for (int i = 0; i < val_count - 1; i++) {
        int val = (rand() % 5000000);
        file << val << endl;
    }
    int val = (rand() % 5000000);
    file << val;
    file.close();

    //样本2
    num = 0;
    fstream fp;
    fp.open("big-a.txt", ios::in);
    while (!fp.eof())
    {
        fp >> a1[num];
        a2[num] = a1[num];
        a3[num] = a1[num];
        a4[num] = a1[num];
        a5[num] = a1[num];
        a6[num] = a1[num];
        a7[num] = a1[num];
        a8[num] = a1[num];
        num++;
    }
    fp.close();

    DWORD start, end;
    start = GetTickCount();
    insertsort(a1);
    end = GetTickCount();
    a[1][i] = end - start;

    start = GetTickCount();
    shellsort(a2);
    end = GetTickCount();
    a[2][i] = end - start;

    start = GetTickCount();
    bubblesort(a3);
    end = GetTickCount();
    a[3][i] = end - start;

    start = GetTickCount();
    quicksort(a4, 0, num - 1);
    end = GetTickCount();
    a[4][i] = end - start;

    start = GetTickCount();
    selectionsort(a5);
    end = GetTickCount();
    a[5][i] = end - start;

    start = GetTickCount();
    heapsort(a6);
    end = GetTickCount();
    a[6][i] = end - start;

    start = GetTickCount();
    mergesort(a7, 0, num - 1);
    end = GetTickCount();
    a[7][i] = end - start;

    start = GetTickCount();
    radixsort(a8);
    end = GetTickCount();
    a[8][i] = end - start;
}

int main()
{
    test(1);
    test(2);
    test(3); 
    test(4); 
    test(5); 
    test(6); 
    test(7); 
    test(8);
    test(9);
    test(10);

    //测试
    //print(a1); print(a2); print(a3); print(a4); print(a5); print(a6); print(a7); print(a8);

    cout << "排序算法/时间" << endl;

    cout << "插入排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[1][i] << "  ";
    cout << endl;

    cout << "希尔排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[2][i] << "  ";
    cout << endl;

    cout << "冒泡排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[3][i] << "  ";
    cout << endl;

    cout << "快速排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[4][i] << "  ";
    cout << endl;

    cout << "选择排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[5][i] << "  ";
    cout << endl;

    cout << "  堆排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[6][i] << "  ";
    cout << endl;

    cout << "归并排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[7][i] << "  ";
    cout << endl;

    cout << "基数排序：";
    for (int i = 1; i <= 10; i++)
        cout << a[8][i] << "  ";
    cout << endl;

    return 0;
}