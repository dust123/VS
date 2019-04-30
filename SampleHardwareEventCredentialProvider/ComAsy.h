#pragma once
#include <Windows.h>
#include<process.h>
#include <iostream>
#include <Cstring>
#include <string>
#include <fstream>
#include <stdlib.h>

#include <cstdlib>

//using std::string;
using namespace std;//string �� cout
string byteToHexStr(unsigned char byte_arr[], int arr_len);

class ComAsy
{
public:
	ComAsy();
	~ComAsy();
	bool InitCOM(LPCTSTR Port);//�򿪴���
	void UninitCOM(); //�رմ��ڲ�����

					  //д������
	bool ComWrite(LPBYTE buf, int &len);

	//��ȡ�߳�
	static unsigned int __stdcall OnRecv(void*);
	
private:
	HANDLE m_hCom;
	OVERLAPPED m_ovWrite;//����д������
	OVERLAPPED m_ovRead;//���ڶ�ȡ����
	OVERLAPPED m_ovWait;//���ڵȴ�����
	volatile bool m_IsOpen;//�����Ƿ��
	HANDLE m_Thread;//��ȡ�߳̾��

};

