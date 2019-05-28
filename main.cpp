#pragma warning(disable:4996)
#include <iostream>
#include <iomanip>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

class Replace {
public:
	Replace();
	~Replace();
	void InitSpace(const char* MethodName);   //初始化页号记录
	void Report(void);		    //报告算法执行情况
	void Opt(void);				//最佳置换算法
	void Fifo(void);		    //先进先出算法
	void Lru(void);			    //最近最久未用算法
	void Eclock(void);          //改进的Clock算法 
private:
	int* ReferencePage;		    //存放要访问到的页号
	int* EliminatePage;		    //存放淘汰页号
	int* PageFrames;		    //存放当前正在实存中的页号
	int PageNumber;			    //访问页数
	int FrameNumber;		    //实存帧数
	int FaultNumber;		    //失败页数
};

Replace::Replace() {
	int i;
	cout << "请输入总访问页数:";
	//设定总的访问页数,并分配相应的引用页号和淘汰页号记录数组空间
	cin >> PageNumber;
	ReferencePage = new int[sizeof(int) * PageNumber];
	EliminatePage = new int[sizeof(int) * PageNumber];
	//设定内存实页数(帧数),并分配相应的实页号记录数组空间(页号栈)
	cout << "请输入内存块数:";
	cin >> FrameNumber;
	PageFrames = new int[sizeof(int) * FrameNumber];
	//输入引用页号序列(页面走向),初始化引用页数组
	cout << "随机生成页面访问序列:";
	for (i = 0; i < PageNumber; i++) {
		ReferencePage[i] = rand() % (FrameNumber*2+2) + 1; //引用页暂存引用数组，通过随机数模拟
		cout << ReferencePage[i] << " ";
	}
	cout << endl;
}

Replace::~Replace() {
	delete[] ReferencePage;
	delete[] EliminatePage;
	delete[] PageFrames;
}

void Replace::InitSpace(const char* MethodName)
{
	int i;
	cout << endl << MethodName << endl;
	FaultNumber = 0;
	//引用还未开始,-1 表示无引用页
	for (i = 0; i < PageNumber; i++)
		EliminatePage[i] = -1;
	for (i = 0; i < FrameNumber; i++)
		PageFrames[i] = -1;
}
//分析统计选择的算法对于当前输入的页面走向的性能
void Replace::Report(void) {
	//报告淘汰页顺序
	cout << endl << "淘汰页:";
	for (int i = 0; EliminatePage[i] != -1; i++)
		cout << EliminatePage[i] << ' ';
	//报告缺页数和缺页率
	cout << endl << "缺页数量 = " << FaultNumber << endl;
	cout << setw(6) << setprecision(3);
	cout << "缺页率 = " << 100 * (float)FaultNumber / (float)PageNumber << "%" << endl;
}
//最佳置换算法
void Replace::Opt(void)
{
	int i, j, k, l, next;
	int max = 0, out = 0;
	bool exist = false;
	InitSpace("OPT");
	//循环装入引用页
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//如果引用页已在实存中,报告实存页号
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i])
				break;
		if (i < FrameNumber) {
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";
			cout << endl;
			continue;// 继续引用下一页
		}
		//引用页不在实存中,缺页数加1
		FaultNumber++;
		if (PageFrames[j] == -1) {//内存块未装满时
			EliminatePage[l] = PageFrames[j];//最先入页号记入淘汰页数组
			PageFrames[j] = next;//引用页号放最先入页号处
			j = (j + 1) % FrameNumber;//最先入页号循环下移
		}
		else { //找到未来最久不使用的页面
			for (int m = 0; m < FrameNumber; m++) {
				exist = false;
				for (int n = k; n < PageNumber; n++) {
					if (PageFrames[m] == ReferencePage[n]) {
						exist = true;
						if (n > max) {
							max = n;
							out = m;
						}
						break;
					}
				}
				if (!exist) { //若此后都不用该页面，则直接置换
					out = m;
					break;
				}
			}
			max = 0;
			EliminatePage[l] = PageFrames[out];
			PageFrames[out] = next;//引用页号放入置换位置
		}
		//报告当前实存页号和淘汰页号
		for (i = 0; i < FrameNumber; i++)
			if (PageFrames[i] >= 0)
				cout << PageFrames[i] << " ";

		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//分析统计选择的算法对于当前引用的页面走向的性能
	Report();
}
//最近最久未用置换算法
void Replace::Lru(void)
{
	int i, j, k, l, next;
	InitSpace("LRU");
	//循环装入引用页
	for (k = 0, l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//检测引用页当前是否已在实存
		for (i = 0; i < FrameNumber; i++) {
			if (next == PageFrames[i]) {
				//引用页已在实存将其调整到页记录栈顶
				for (j = i; j > 0; j--)
					PageFrames[j] = PageFrames[j - 1];
				PageFrames[0] = next;
				break;
			}
		}
		if (PageFrames[0] == next) {
			//如果引用页已放栈顶,则为不缺页,报告当前内存页号
			for (j = 0; j < FrameNumber; j++)
				if (PageFrames[j] >= 0)
					cout << PageFrames[j] << " ";
			cout << endl;
			continue;//继续装入下一页
		}
		else
			//如果引用页还未放栈顶,则为缺页,缺页数加1
			FaultNumber++;
		//栈底页号记入淘汰页数组中
		EliminatePage[l] = PageFrames[FrameNumber - 1];
		//向下压栈
		for (j = FrameNumber - 1; j > 0; j--)
			PageFrames[j] = PageFrames[j - 1];
		PageFrames[0] = next;//引用页放栈顶
		//报告当前实存中页号
		for (j = 0; j < FrameNumber; j++)
			if (PageFrames[j] >= 0)
				cout << PageFrames[j] << " ";
		//报告当前淘汰的页号
		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//分析统计选择的算法对于当前引用的页面走向的性能
	Report();
}
//先进先出置换算法
void Replace::Fifo(void)
{
	int i, j, k, l, next;
	InitSpace("FIFO");
	//循环装入引用页
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//如果引用页已在实存中,报告实存页号
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i])
				break;
		if (i < FrameNumber) {
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";
			cout << endl;
			continue;// 继续引用下一页
		}
		//引用页不在实存中,缺页数加1
		FaultNumber++;
		EliminatePage[l] = PageFrames[j];//最先入页号记入淘汰页数组
		PageFrames[j] = next;//引用页号放最先入页号处
		j = (j + 1) % FrameNumber;//最先入页号循环下移
		//报告当前实存页号和淘汰页号
		for (i = 0; i < FrameNumber; i++)
			if (PageFrames[i] >= 0)
				cout << PageFrames[i] << " ";

		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//分析统计选择的算法对于当前引用的页面走向的性能
	Report();
}

//改进的时钟置换算法
void Replace::Eclock(void)
{
	int i, j, k, l, next;
	InitSpace("Eclock");
	//访问位数组
	bool* refbit = new bool[sizeof(bool) * FrameNumber];
	bool* modbit = new bool[sizeof(bool) * FrameNumber];
	for (i = 0; i < FrameNumber; i++) {//将所有访问位和修改位初始化为0
		refbit[i] = false;
		modbit[i] = false;
	}
	//循环装入引用页
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//如果引用页已在实存中,报告实存页号
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i]) {
				refbit[i] = true; //当引用页被访问时，访问位置为1
				if (rand() % 100 < 20) {
					modbit[i] = 1; //20%概率修改为1
					break;
				}
				else {
					modbit[i] = 0; //80%概率修改为0
					break;
				}
			}
		if (i < FrameNumber) {
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";
			cout << endl;
		}
		else {
			//引用页不在实存中,缺页数加1
			FaultNumber++;
			i = j; //保存当前位置，检验是否将队列遍历完毕
			int replacePageNum = -1; //保存会被置换的页号
			do {
				if (refbit[j]) {//如果当前页被访问过，给予二次机会
					if (!modbit[j])
						replacePageNum = j; //如果未被修改，可能被替换
					refbit[j] = false; //把访问位置为0
				}
				else if (modbit[j])
					replacePageNum = j; //未被访问但被修改了，可能被替换
				else { //未被访问也未被修改，即找到了最佳替换页
					replacePageNum = j;
					break;//立刻跳出
				}
				j = (j + 1) % FrameNumber;//再从下一帧开始找未访问页
			} while (i != j);
			if (-1 != replacePageNum) { //替换选中的被置换页
				EliminatePage[l] = PageFrames[replacePageNum];
				PageFrames[replacePageNum] = next;
				refbit[replacePageNum] = true;
				modbit[replacePageNum] = rand() % 2; //用随机数模拟这一页是否会被修改
			}
			else { //队列完全被遍历1遍，且一直没有找到可选置换页面
				EliminatePage[l] = PageFrames[j];
				PageFrames[j] = next;
				refbit[j] = true;
				modbit[j] = rand() % 2; //用随机数模拟这一页是否会被修改
			}
			//报告当前实存页号和淘汰页号
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";

			if (EliminatePage[l] >= 0)
				cout << "->" << EliminatePage[l++] << endl;
			else
				cout << endl;
		}
		for (i = 0; i < FrameNumber; i++)
			cout << "(" << refbit[i] << ',' << modbit[i] << ") ";
		cout << endl;
	}
	//分析统计选择的算法对于当前引用的页面走向的性能
	Report();
}



int main(int argc, char* argv[]) {
	srand(time(0));
	Replace* pages = new Replace();
	pages->Opt();
	pages->Fifo();
	pages->Lru();
	pages->Eclock();
	delete pages;
	return 0;
}
