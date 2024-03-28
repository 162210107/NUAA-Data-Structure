//对于1 - 10000的质数序列<2，3，5，7，…，9973>，建立平衡二叉排序树。
//(1)依次查询200 - 300的每个数是否在树中，将结果写入tree1.txt，格式如下:
//XXX no
//XXX yes
//(2)依次删除500 - 2000中的每个质数，再查询 600 - 700之间的每个质数是否在, 将结果写入文件tree2.txt，格式如下 :
//XXX no
//XXX yes
//(3)在任务（2）基础上，依次插入1 - 1000的所有偶数，依次查询100 - 200的每个偶数是否在树中, 将结果写入文件tree3.txt，格式如下 :
//XXX no
//XXX yes
#include <iostream>
#include<fstream>
using namespace std;

int arr[10000]={0};

typedef struct TreeNode
{
	int data;
	int height;//树的高度
	struct TreeNode* lchild;
	struct TreeNode* rchild;
}TreeNode;

int getHeight(TreeNode* node)
{
	return node ? node->height : 0;
}

void rrRotation(TreeNode* node, TreeNode** root)
{
	TreeNode* temp = node->rchild;
	node->rchild = temp->lchild;
	temp->lchild = node;
	node->height = max(getHeight(node->lchild), getHeight(node->rchild)) + 1;
	temp->height = max(getHeight(temp->lchild), getHeight(temp->rchild)) + 1;
	*root = temp;
}

void llRotation(TreeNode* node, TreeNode** root)
{
	TreeNode* temp = node->lchild;
	node->lchild = temp->rchild;
	temp->rchild = node;
	node->height = max(getHeight(node->lchild), getHeight(node->rchild)) + 1;
	temp->height = max(getHeight(temp->lchild), getHeight(temp->rchild)) + 1;
	*root = temp;
}

void avlInsert(TreeNode** T, int data)
{
	if (*T == NULL)
	{
		*T = (TreeNode*)malloc(sizeof(TreeNode));
		(*T)->data = data;
		(*T)->height = 0;
		(*T)->lchild = NULL;
		(*T)->rchild = NULL;
	}
	else if (data < (*T)->data)
	{
		avlInsert(&(*T)->lchild, data);

		int lHeight = getHeight((*T)->lchild);
		int rHeight = getHeight((*T)->rchild);

		//计算高度差=平衡因子，左右子树不平衡就调整
		if (lHeight - rHeight == 2)
		{
			if (data < (*T)->lchild->data)
				//LL调整
				llRotation(*T, T);
			else
			{
				//LR调整
				rrRotation((*T)->lchild, &(*T)->lchild);
				llRotation(*T, T);
			}
		}
	}
	else if (data > (*T)->data)
	{
		avlInsert(&(*T)->rchild, data);

		int lHeight = getHeight((*T)->lchild);
		int rHeight = getHeight((*T)->rchild);

		//计算高度差=平衡因子，左右子树不平衡就调整
		if (lHeight - rHeight == -2)
		{
			if (data > (*T)->rchild->data)
			    //RR调整
				rrRotation(*T, T);
			else
			{
				//RL调整
				llRotation((*T)->rchild, &(*T)->rchild);
				rrRotation(*T, T);
			}
		}
	}
	//更新树结点的高度
	(*T)->height = max(getHeight((*T)->lchild), getHeight((*T)->rchild)) + 1;
}

//判断是否为素数
bool isprime(int key)
{
	for (int i = 2; i <= sqrt(key); i++)
		if (key % i == 0)
			return false;
	return true;
}

//搜索是否存在
bool enquiry(TreeNode* node,int key)
{
	if (node == NULL)//搜索到最后则说明不存在
		return false;
	else if (node->data == key)
		return true;
	else if (node->data > key)//结点值大，搜索左子树
		enquiry(node->lchild, key);
	else if (node->data < key)//结点值小，搜索右子树
		enquiry(node->rchild, key);
}

TreeNode* Getmax(TreeNode* tree)
{
	while (tree->rchild) {
		tree = tree->rchild;
	}
	return tree;
}

void deletenode(TreeNode** T, int key)
{
	if ((*T) == NULL)
	{
		cout << "不存在" << key << endl;
		return;
	}
	else if ((*T)->data == key)
	{
		if ((*T)->lchild == NULL && (*T)->rchild == NULL)
		{
			//为叶子结点，直接删除
			TreeNode* temp = *T;
			*T = NULL;
			free(temp);
			return;//直接返回上一次循环去判断
		}
		else if ((*T)->lchild == NULL && (*T)->rchild) 
		{
			//删除结点没有左子树，将右子树上移就可以
			TreeNode* temp = *T;
			*T = (*T)->rchild;
			free(temp);
			return;
		}
		else if ((*T)->rchild == NULL && (*T)->lchild)
		{
			//删除结点没有右子树，将左子树上移就可以
			TreeNode* temp = *T;
			*T = (*T)->lchild;
			free(temp);
			return;
		}
		else if ((*T)->lchild&& (*T)->rchild) 
		{
			//左右子树都存在
			if (getHeight((*T)->lchild) > getHeight((*T)->rchild))
			{
				//.找到左子树中的最大值，将值赋给给节点，然后将左子树最大值这个节点删除
				TreeNode* temp = Getmax((*T)->lchild);
				int maxnode = temp->data;
				(*T)->data = maxnode;
				deletenode(&(*T)->lchild, maxnode);
			}
			else
			{
				//寻找后继节点post。
				TreeNode* post = (*T)->rchild;
				while (post->lchild)
					post = post->lchild;
				//用post替换*T。
				(*T)->data = post->data;

				//删除节点post。
				//虽然能够确定post所属最小子树的根结点为&post，
				//但是不采用AVLDelete(&post,post->data)删除post，目的是方便递归更改节点的高度。
				deletenode(&((*T)->rchild), post->data);
			}
			return;
		}
	}
	else if ((*T)->data > key)
	{
		deletenode(&(*T)->lchild, key);

		//更新树结点的高度
		(*T)->height = max(getHeight((*T)->lchild), getHeight((*T)->rchild)) + 1;

		int lHeight = getHeight((*T)->lchild);
		int rHeight = getHeight((*T)->rchild);

		//计算高度差=平衡因子，左右子树不平衡就调整
		if (lHeight - rHeight == -2)
		{
			if (getHeight((*T)->rchild->lchild) - getHeight((*T)->rchild->rchild) == -1)
				//RR调整
				rrRotation(*T, T);
			else
			{
				//RL调整
				llRotation((*T)->rchild, &(*T)->rchild);
				rrRotation(*T, T);
			}
		}
	}
	else if ((*T)->data < key)
	{
		deletenode(&(*T)->rchild, key);

		//更新树结点的高度
		(*T)->height = max(getHeight((*T)->lchild), getHeight((*T)->rchild)) + 1;

		int lHeight = getHeight((*T)->lchild);
		int rHeight = getHeight((*T)->rchild);

		//计算高度差=平衡因子，左右子树不平衡就调整
		if (lHeight - rHeight == 2)
		{
			if (getHeight((*T)->lchild->lchild) - getHeight((*T)->lchild->rchild) == 1)
				//LL调整
				llRotation(*T, T);
			else
			{
				//LR调整
				rrRotation((*T)->lchild, &(*T)->lchild);
				llRotation(*T, T);
			}
		}
	}
	
}

int main()
{
	int number = 0;//记录文件中元素个数

	fstream fp;
	fp.open("tree.txt", ios::in);
	if (!fp.is_open())
		return 0;
	while (!fp.eof())
		fp>>arr[number++];
	fp.close();

	//建树
	TreeNode* T = NULL;
	for (int i = 0; i < number; i++)
		avlInsert(&T, arr[i]);

	//查询
	fstream fp1;
	fp1.open("tree1.txt", ios::out);
	if (!fp1.is_open())
		return 0;
	for (int temp = 200; temp < 301; temp++)
	{
		bool result = enquiry(T, temp);
		if (result == 0)
			fp1 << temp << " no" << endl;
		else
			fp1 << temp << " yes" << endl;
	}
	
	//删除
	for (int i = 500; i < 2001; i++)
	{
		if(isprime(i)==true)
			deletenode(&T, i);
	}
	
	//查询
	fstream fp2;
	fp2.open("tree2.txt", ios::out);
	if (!fp2.is_open())
		return 0;
	for (int temp = 600; temp < 701; temp++)
	{
		if (isprime(temp) == true)
		{
			bool result = enquiry(T, temp);
			if (result == 0)
				fp2 << temp << " no" << endl;
			else
				fp2 << temp << " yes" << endl;
		}
	}

	//插入偶数
	for (int i = 1; i < 1001; i++)
	{
		if (i % 2 == 0)
			avlInsert(&T, i);
	}
	//查询
	fstream fp3;
	fp3.open("tree3.txt", ios::out);
	if (!fp3.is_open())
		return 0;
	for (int temp = 100; temp < 201; temp++)
	{
		if (temp % 2 == 0)
		{
			bool result = enquiry(T, temp);
			if (result == 0)
				fp3 << temp << " no" << endl;
			else
				fp3 << temp << " yes" << endl;
		}
	}
}