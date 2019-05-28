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
	void InitSpace(const char* MethodName);   //��ʼ��ҳ�ż�¼
	void Report(void);		    //�����㷨ִ�����
	void Opt(void);				//����û��㷨
	void Fifo(void);		    //�Ƚ��ȳ��㷨
	void Lru(void);			    //������δ���㷨
	void Eclock(void);          //�Ľ���Clock�㷨 
private:
	int* ReferencePage;		    //���Ҫ���ʵ���ҳ��
	int* EliminatePage;		    //�����̭ҳ��
	int* PageFrames;		    //��ŵ�ǰ����ʵ���е�ҳ��
	int PageNumber;			    //����ҳ��
	int FrameNumber;		    //ʵ��֡��
	int FaultNumber;		    //ʧ��ҳ��
};

Replace::Replace() {
	int i;
	cout << "�������ܷ���ҳ��:";
	//�趨�ܵķ���ҳ��,��������Ӧ������ҳ�ź���̭ҳ�ż�¼����ռ�
	cin >> PageNumber;
	ReferencePage = new int[sizeof(int) * PageNumber];
	EliminatePage = new int[sizeof(int) * PageNumber];
	//�趨�ڴ�ʵҳ��(֡��),��������Ӧ��ʵҳ�ż�¼����ռ�(ҳ��ջ)
	cout << "�������ڴ����:";
	cin >> FrameNumber;
	PageFrames = new int[sizeof(int) * FrameNumber];
	//��������ҳ������(ҳ������),��ʼ������ҳ����
	cout << "�������ҳ���������:";
	for (i = 0; i < PageNumber; i++) {
		ReferencePage[i] = rand() % (FrameNumber*2+2) + 1; //����ҳ�ݴ��������飬ͨ�������ģ��
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
	//���û�δ��ʼ,-1 ��ʾ������ҳ
	for (i = 0; i < PageNumber; i++)
		EliminatePage[i] = -1;
	for (i = 0; i < FrameNumber; i++)
		PageFrames[i] = -1;
}
//����ͳ��ѡ����㷨���ڵ�ǰ�����ҳ�����������
void Replace::Report(void) {
	//������̭ҳ˳��
	cout << endl << "��̭ҳ:";
	for (int i = 0; EliminatePage[i] != -1; i++)
		cout << EliminatePage[i] << ' ';
	//����ȱҳ����ȱҳ��
	cout << endl << "ȱҳ���� = " << FaultNumber << endl;
	cout << setw(6) << setprecision(3);
	cout << "ȱҳ�� = " << 100 * (float)FaultNumber / (float)PageNumber << "%" << endl;
}
//����û��㷨
void Replace::Opt(void)
{
	int i, j, k, l, next;
	int max = 0, out = 0;
	bool exist = false;
	InitSpace("OPT");
	//ѭ��װ������ҳ
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//�������ҳ����ʵ����,����ʵ��ҳ��
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i])
				break;
		if (i < FrameNumber) {
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";
			cout << endl;
			continue;// ����������һҳ
		}
		//����ҳ����ʵ����,ȱҳ����1
		FaultNumber++;
		if (PageFrames[j] == -1) {//�ڴ��δװ��ʱ
			EliminatePage[l] = PageFrames[j];//������ҳ�ż�����̭ҳ����
			PageFrames[j] = next;//����ҳ�ŷ�������ҳ�Ŵ�
			j = (j + 1) % FrameNumber;//������ҳ��ѭ������
		}
		else { //�ҵ�δ����ò�ʹ�õ�ҳ��
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
				if (!exist) { //���˺󶼲��ø�ҳ�棬��ֱ���û�
					out = m;
					break;
				}
			}
			max = 0;
			EliminatePage[l] = PageFrames[out];
			PageFrames[out] = next;//����ҳ�ŷ����û�λ��
		}
		//���浱ǰʵ��ҳ�ź���̭ҳ��
		for (i = 0; i < FrameNumber; i++)
			if (PageFrames[i] >= 0)
				cout << PageFrames[i] << " ";

		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//����ͳ��ѡ����㷨���ڵ�ǰ���õ�ҳ�����������
	Report();
}
//������δ���û��㷨
void Replace::Lru(void)
{
	int i, j, k, l, next;
	InitSpace("LRU");
	//ѭ��װ������ҳ
	for (k = 0, l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//�������ҳ��ǰ�Ƿ�����ʵ��
		for (i = 0; i < FrameNumber; i++) {
			if (next == PageFrames[i]) {
				//����ҳ����ʵ�潫�������ҳ��¼ջ��
				for (j = i; j > 0; j--)
					PageFrames[j] = PageFrames[j - 1];
				PageFrames[0] = next;
				break;
			}
		}
		if (PageFrames[0] == next) {
			//�������ҳ�ѷ�ջ��,��Ϊ��ȱҳ,���浱ǰ�ڴ�ҳ��
			for (j = 0; j < FrameNumber; j++)
				if (PageFrames[j] >= 0)
					cout << PageFrames[j] << " ";
			cout << endl;
			continue;//����װ����һҳ
		}
		else
			//�������ҳ��δ��ջ��,��Ϊȱҳ,ȱҳ����1
			FaultNumber++;
		//ջ��ҳ�ż�����̭ҳ������
		EliminatePage[l] = PageFrames[FrameNumber - 1];
		//����ѹջ
		for (j = FrameNumber - 1; j > 0; j--)
			PageFrames[j] = PageFrames[j - 1];
		PageFrames[0] = next;//����ҳ��ջ��
		//���浱ǰʵ����ҳ��
		for (j = 0; j < FrameNumber; j++)
			if (PageFrames[j] >= 0)
				cout << PageFrames[j] << " ";
		//���浱ǰ��̭��ҳ��
		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//����ͳ��ѡ����㷨���ڵ�ǰ���õ�ҳ�����������
	Report();
}
//�Ƚ��ȳ��û��㷨
void Replace::Fifo(void)
{
	int i, j, k, l, next;
	InitSpace("FIFO");
	//ѭ��װ������ҳ
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//�������ҳ����ʵ����,����ʵ��ҳ��
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i])
				break;
		if (i < FrameNumber) {
			for (i = 0; i < FrameNumber; i++)
				if (PageFrames[i] >= 0)
					cout << PageFrames[i] << " ";
			cout << endl;
			continue;// ����������һҳ
		}
		//����ҳ����ʵ����,ȱҳ����1
		FaultNumber++;
		EliminatePage[l] = PageFrames[j];//������ҳ�ż�����̭ҳ����
		PageFrames[j] = next;//����ҳ�ŷ�������ҳ�Ŵ�
		j = (j + 1) % FrameNumber;//������ҳ��ѭ������
		//���浱ǰʵ��ҳ�ź���̭ҳ��
		for (i = 0; i < FrameNumber; i++)
			if (PageFrames[i] >= 0)
				cout << PageFrames[i] << " ";

		if (EliminatePage[l] >= 0)
			cout << "->" << EliminatePage[l++] << endl;
		else
			cout << endl;
	}
	//����ͳ��ѡ����㷨���ڵ�ǰ���õ�ҳ�����������
	Report();
}

//�Ľ���ʱ���û��㷨
void Replace::Eclock(void)
{
	int i, j, k, l, next;
	InitSpace("Eclock");
	//����λ����
	bool* refbit = new bool[sizeof(bool) * FrameNumber];
	bool* modbit = new bool[sizeof(bool) * FrameNumber];
	for (i = 0; i < FrameNumber; i++) {//�����з���λ���޸�λ��ʼ��Ϊ0
		refbit[i] = false;
		modbit[i] = false;
	}
	//ѭ��װ������ҳ
	for (k = 0, j = l = 0; k < PageNumber; k++) {
		next = ReferencePage[k];
		//�������ҳ����ʵ����,����ʵ��ҳ��
		for (i = 0; i < FrameNumber; i++)
			if (next == PageFrames[i]) {
				refbit[i] = true; //������ҳ������ʱ������λ��Ϊ1
				if (rand() % 100 < 20) {
					modbit[i] = 1; //20%�����޸�Ϊ1
					break;
				}
				else {
					modbit[i] = 0; //80%�����޸�Ϊ0
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
			//����ҳ����ʵ����,ȱҳ����1
			FaultNumber++;
			i = j; //���浱ǰλ�ã������Ƿ񽫶��б������
			int replacePageNum = -1; //����ᱻ�û���ҳ��
			do {
				if (refbit[j]) {//�����ǰҳ�����ʹ���������λ���
					if (!modbit[j])
						replacePageNum = j; //���δ���޸ģ����ܱ��滻
					refbit[j] = false; //�ѷ���λ��Ϊ0
				}
				else if (modbit[j])
					replacePageNum = j; //δ�����ʵ����޸��ˣ����ܱ��滻
				else { //δ������Ҳδ���޸ģ����ҵ�������滻ҳ
					replacePageNum = j;
					break;//��������
				}
				j = (j + 1) % FrameNumber;//�ٴ���һ֡��ʼ��δ����ҳ
			} while (i != j);
			if (-1 != replacePageNum) { //�滻ѡ�еı��û�ҳ
				EliminatePage[l] = PageFrames[replacePageNum];
				PageFrames[replacePageNum] = next;
				refbit[replacePageNum] = true;
				modbit[replacePageNum] = rand() % 2; //�������ģ����һҳ�Ƿ�ᱻ�޸�
			}
			else { //������ȫ������1�飬��һֱû���ҵ���ѡ�û�ҳ��
				EliminatePage[l] = PageFrames[j];
				PageFrames[j] = next;
				refbit[j] = true;
				modbit[j] = rand() % 2; //�������ģ����һҳ�Ƿ�ᱻ�޸�
			}
			//���浱ǰʵ��ҳ�ź���̭ҳ��
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
	//����ͳ��ѡ����㷨���ڵ�ǰ���õ�ҳ�����������
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
