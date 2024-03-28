//对1 - 10000的所有质数，建立m = 4的B - tree（每个非叶子结点至少包含1个关键字即2棵子树，最多3个关键字即4棵子树）。
//(1)	依次查询200 - 300的每个数是否在B - tree中，将结果写入文件b - tree1.txt，格式如下:
//XXX no
//XXX yes
//(2)	依次删除500 - 2000中的每个质数，再查询 600 - 700之间的每个质数是否在, 将结果写入文件b - tree2.txt，格式如下 :
//XXX no
//XXX yes
//(3)	在任务（2）基础上的B - tree，依次插入1 - 1000的所有偶数，依次查询100 - 200的每个偶数是否在B - tree中, 将结果写入文件b - tree3.txt，格式如下 :
//XXX no
//XXX yes
#include <iostream>
#include <fstream>
#include <stack>

using namespace std;

//B树关键字个数为m-1
//孩子最多m-1

//关键字索引  0（不记录）  1    2    3    4（不记录具体数值）
//孩子的索引         0        1    2    3    4

//树结点信息
typedef struct Treenode
{
    int level;//树的阶数
    int keyNum;//关键字的个数
    int childNum;//孩子的个数
    int* keys;//关键字数组
    struct Treenode** children;//孩子数组
    struct Treenode* parent;//父亲指针
};

//初始化结点
Treenode* initNode(int level)
{
    Treenode* node = (Treenode*)malloc(sizeof(Treenode));
    node->level = level;
    node->keyNum = 0;
    node->childNum = 0;
    node->keys = (int*)malloc(sizeof(int) * (level+1));//索引，从1开始记录
    node->children = (Treenode**)malloc(sizeof(Treenode*) * level);
    node->parent = NULL;
    for (int i = 0; i < level; i++)
    {
        node->keys[i] = 0;
        node->children[i] = NULL;
    }
    node->keys[level] = 0;//并没有实际用途，只是为了方便返回孩子的索引
    return node;
}

//在节点处寻找合适的插入索引
int findSuiteIndex(Treenode* node, int data)
{
    int index;
    //索引从1开始，因为0是没有用的边界
    for (index = 1; index <= node->keyNum; index++)
        if (data < node->keys[index])
            break;
    return index;
}

//找到合适的叶子结点，先插入叶子（没有孩子的结点）结点再做调整
Treenode* findSuiteLeafNode(Treenode* T, int data)
{
    if (T->childNum == 0)
        return T;
    else
    {
        int index = findSuiteIndex(T, data);
        //找到对应的（索引+1），再去该子节点去搜索,见前面注释
        return findSuiteLeafNode(T->children[index-1], data);
    }
}

//插入数据到B树
void addData(Treenode* node, int data,Treenode**T)
{
    //先找到索引，将data放入关键字数组
    int index = findSuiteIndex(node, data);
    //后面元素后移动，因为本来就预留了多余的空间
    for (int i = node->keyNum; i >= index; i--)
        node->keys[i + 1] = node->keys[i];
    node->keys[index] = data;
    node->keyNum++;

    //判断该结点是否进行分裂。调整主要三个方面
    //1.找出中间结点，把结点前面的给新的左孩子，后面的给新的右孩子
    //2.将该节点的孩子分给新的左孩子和右孩子
    //3.将中间结点插入该节点的父亲关键字数组中，并将新的左孩子和右孩子插入父亲节点的孩子数组
    if (node->keyNum == node->level)
    {
        //取中间节点
        int mid = node->level / 2 + node->level % 2;

        //生成新的左右孩子
        Treenode* lchild = initNode(node->level);
        Treenode* rchild = initNode(node->level);

        //将mid左边的赋给左孩子关键字数组
        for (int i = 1; i < mid; i++)
            addData(lchild, node->keys[i], T);
      
        //将mid右边的赋给右孩子关键字数组
        for (int i = mid + 1; i <= node->keyNum; i++)
            addData(rchild, node->keys[i], T);

        //把该结点下面对应的孩子赋给左孩子的孩子数组
        for (int i = 0; i < mid; i++)
        {
            lchild->children[i] = node->children[i];
            //如果该孩子结点不为空，需要改变孩子的某些信息
            if (node->children[i] != NULL)
            {
                node->children[i]->parent = lchild;
                lchild->childNum++;
            }
        }

        //把该结点下面对应的孩子赋给右孩子的孩子数组
        int k = 0;//右孩子数组的索引值
        for (int i = mid; i < node->childNum; i++)
        {
            rchild->children[k++] = node->children[i];
            //如果该孩子结点不为空，需要改变孩子的某些信息
            if (node->children[i] != NULL)
            {
                node->children[i]->parent = rchild;
                rchild->childNum++;
            }
        }

        //将中间结点插入该节点的父亲关键字数组中，并将新的左孩子和右孩子插入父亲节点的孩子数组
        //node是根节点的时候，分裂会改变根节点
        if (node->parent == NULL)
        {
            //生成新的根结点
            Treenode* newParent = initNode(node->level);
            addData(newParent, node->keys[mid], T);
            newParent->children[0] = lchild;
            newParent->children[1] = rchild;
            newParent->childNum = 2;
            lchild->parent = newParent;
            rchild->parent = newParent;
            *T = newParent;
        }
        else
        {
            //找到mid结点应该插入的node父亲的关键字数组的索引
            int indexparent = findSuiteIndex(node->parent, node->keys[mid]);
            
            lchild->parent = node->parent;
            rchild->parent = node->parent;
            //node原来所有的元素都比index要小，所以之间把左孩子插入index-1，右孩子应该在index的位置
            node->parent->children[indexparent - 1] = lchild;
            //插入新的右孩子前，如果index处有，需要整体后移
            if (node->parent->children[indexparent] != NULL)
            {
                for (int i = node->parent->childNum - 1; i >= indexparent; i--)
                    node->parent->children[i + 1] = node->parent->children[i];
            }
            node->parent->children[indexparent] = rchild;
            node->parent->childNum++;
            //将mid插入父节点位置
            addData(node->parent, node->keys[mid],T);
        }
    }
}

//插入结点函数
void insert(Treenode** T, int data)
{
    Treenode* node = findSuiteLeafNode(*T,data);
    //找到要插入的叶子结点
    addData(node, data, T);//插入树
}

// 合并操作
void merge(Treenode* P, int pos)
{
    //          pos+1(P)
    // pos(LC)        pos+1(RC)
    Treenode* LC = P->children[pos];
    Treenode* RC = P->children[pos + 1];

    LC->keys[LC->keyNum + 1] = P->keys[pos + 1];
    ++LC->keyNum;

    //将右兄弟移入LC
    int m = LC->keyNum;
    for (int i = 1; i <= RC->keyNum; ++i)
    {
        LC->keys[m + i] = RC->keys[i];
        ++LC->keyNum;
    }
    int n = LC->childNum;
    for (int i = 0; i < RC->childNum; ++i)
    {
        LC->children[ n+ i] = RC->children[i];
        ++LC->childNum;
    }

    for (int i = pos + 1; i < P->keyNum; ++i)
    {
        P->keys[i] = P->keys[i + 1];
    }
    for (int i = pos + 1; i < P->childNum-1; ++i)
    {
        P->children[i] = P->children[i + 1];
    }
    --P->keyNum;
    --P->childNum;
}

//删除键值
bool deleteKey(int key, Treenode** Root)
{
    //如果根节点为空或者没有键值时
    if (NULL == *Root || 0 == (*Root)->keyNum) return false;

    Treenode* T = *Root;//复制一份

    stack<Treenode*> NODE;//存储调整的那棵子树

    while (T) 
    {
        NODE.push(T);
        //找到第一个比他大的索引
        int i = findSuiteIndex(T, key);
        //         i-1(可能相等的点）     i(key比它小）
        //孩子  i-2            i-1                    i

        if (i-1 <= T->keyNum && T->keys[i-1] == key) 
        {// 删除键值在该节点中
            if (T->childNum==0) 
            {// 叶子节点，删除
                for (int k=i-1; i <= T->keyNum - 1; ++i) 
                    T->keys[k] = T->keys[k + 1];
                --T->keyNum;
                break;
            }
            else 
            {// 非叶子节点，找后继/也可以找前驱（必存在）
                Treenode* RC = T->children[i - 1];// 右孩子
                //找右孩子最小值
                while (RC->childNum!=0) RC = RC->children[0];

                T->keys[i-1] = RC->keys[1];//替换
                key = RC->keys[1];//改变待删除值
                T = T->children[i - 1];
            }
        }
        else // 删除节点不在该节点中
            T = T->children[i-1];
    }
    // 删除后调整
    Treenode* P = NODE.top();
    NODE.pop();

    while (!NODE.empty()) 
    {
        T = P;
        P = NODE.top();//该节点根树
        NODE.pop();

        //比最小键值数小，做调整
        if (T->keyNum < 1) 
        {
            //找到该结点属于该根节点的第几个孩子
            int i = 0;
            for (; i <T->childNum; ++i) 
                if (T == P->children[i]) break;
            
            //找该节点两边的兄弟结点
            Treenode* LB = i > 0 ? P->children[i - 1] : NULL;
            Treenode* RB = i < P->childNum-1 ? P->children[i + 1] : NULL;

            if (LB && LB->keyNum > 1) 
            {// 左兄弟存在且键值富余
             //        i(P)
             // i-1(LB)    i(T)
                //T中键值往后移动，便于插入
                for (int k = T->keyNum + 1; k > 1; --k)
                    T->keys[k] = T->keys[k - 1];
                    
                for (int k = T->childNum; k > 0; --k)
                    T->children[k] = T->children[k - 1];
             
                //旋转
                T->keys[1] = P->keys[i];
                T->children[0] = LB->children[LB->childNum - 1];
                ++T->keyNum;
                ++T->childNum;

                P->keys[i] = LB->keys[LB->keyNum];
                //LB->children[LB->childNum - 1] = NULL;
                --LB->childNum;
                --LB->keyNum;
            }
            else if (RB && RB->keyNum > 1) 
            {// 右兄弟存在且键值富余
                //        i+1(P)
                //   i(T)      i+1(RB)
                T->keys[T->keyNum+1] = P->keys[i+1];
                T->children[T->childNum] = RB->children[0];
                ++T->keyNum;
                ++T->childNum;

                P->keys[i+1] = RB->keys[1];
                for (int k = 1; k <= RB->keyNum - 1; ++k)
                    RB->keys[k] = RB->keys[k + 1];
                
                for (int k = 0; k < RB->childNum - 1; ++k)
                    RB->children[k] = RB->children[k + 1];
                
                --RB->keyNum;
                --RB->childNum;
            }
            else if (LB) 
            { // 左兄弟存在但不富余
                merge(P, i - 1);
                T = P->children[i - 1];
            }
            else if (RB) 
            {// 右兄弟存在但不富余
                merge(P, i);
                T = P->children[i];
            }
        }
    }
    // 树根被借走，树高 -1
    if (*Root == P && P->keyNum == 0)
    {
        *Root = P->children[0];
    }
    return true;
}

//查询该节点
bool enquirytree(Treenode* node, int data)
{
    //如果该节点没有值就没有这个data
    if (node == NULL)
        return 0;
    //查询该结点的关键字索引
    for (int i = 1; i <= node->keyNum; i++)
    {
        if (data == node->keys[i])
            return 1;
        if (data < node->keys[i])//如果比i小，查询孩子
            return enquirytree(node->children[i - 1],data);
    }
    //比最后的大
    return enquirytree(node->children[node->keyNum], data);
}

//判断素数
bool isPrime(int num)
{
    for (int i = 2; i <= sqrt(num); i++)
        if (num % i == 0)
            return false;
    return true;
}

void task1(Treenode* node)
{
    ofstream fp1;
    fp1.open("tree1.txt", ios::out);

    for (int i = 200; i <301; i++)
    {
        bool answer = enquirytree(node, i);
        if (answer == 1)
            fp1 << i << " yes" << endl;
        else
            fp1 << i << " no" << endl;
    }
    fp1.close();
}

void task2(Treenode** node)
{
    for (int i = 500; i <= 2000; i++)
        if (isPrime(i) == true)
            deleteKey(i, node);

    fstream fp2;
    fp2.open("tree2.txt", ios::out);

    for (int i = 600; i < 701; i++)
    {
        if (isPrime(i) == true)
        {
            bool answer = enquirytree(*node, i);
            if (answer == 1)
                fp2 << i << " yes" << endl;
            else
                fp2 << i << " no" << endl;
        }
    }
    fp2.close();
}

void task3(Treenode** node)
{
    for (int i = 1; i <= 1000; i++)
        if (i % 2 == 0)
            insert(node, i);

    fstream fp3;
    fp3.open("tree3.txt", ios::out);

    for (int i = 100; i < 201; i++)
    {
        if (i % 2 == 0)
        {
            bool answer = enquirytree(*node, i);
            if (answer == 1)
                fp3 << i << " yes" << endl;
            else
                fp3 << i << " no" << endl;
        }
    }
    fp3.close();
}

void printTree(Treenode* T)
{
    if (T != NULL)
    {
        for (int i = 1; i <= T->keyNum; i++)
            cout << T->keys[i] << " ";
        cout << endl;
        for (int i = 0; i < T->childNum; i++)
            printTree(T->children[i]);
    }
}

int main()
{
    Treenode* T = initNode(4);

    fstream fp;
    fp.open("tree.txt", ios::in);
    if (!fp.is_open())
        return 0;
    while (!fp.eof())
    {
        int temp;
        fp >> temp;
        insert(&T,temp);
    }
    fp.close();
    //printTree(T);

    //deleteKey(2, &T);
    //deleteKey(29, &T);
    //printTree(T);
    task1(T);
    task2(&T);
    task3(&T);
}