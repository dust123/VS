//#include "afx.h"
#include "ComAsy.h"
#include "Global.h"

ComAsy::ComAsy() :
	m_hCom(INVALID_HANDLE_VALUE),
	m_IsOpen(false),
	m_Thread(NULL)
{
	memset(&m_ovWait, 0, sizeof(m_ovWait));
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	memset(&m_ovRead, 0, sizeof(m_ovRead));
}

ComAsy::~ComAsy()
{
	UninitCOM();
}

bool ComAsy::InitCOM(LPCTSTR Port)
{
	m_hCom = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,//�����첽��ʶ
		NULL);
	if (INVALID_HANDLE_VALUE == m_hCom)
	{
		return false;
	}
	SetupComm(m_hCom, 4096, 4096);//���÷��ͽ��ջ���

	DCB dcb;
	GetCommState(m_hCom, &dcb);
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = CBR_9600;
	dcb.StopBits = ONESTOPBIT;// ONESTOPBIT;
	dcb.ByteSize = 8;           //ÿ���ֽ���8λ
	dcb.Parity = NOPARITY;      //����żУ��λ
	SetCommState(m_hCom, &dcb);//���ô���

	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

	COMMTIMEOUTS ct;
	ct.ReadIntervalTimeout = MAXDWORD;//��ȡ����ʱ����Ϊ��WaitCommEvent�ȴ�����
	ct.ReadTotalTimeoutConstant = 0;  //
	ct.ReadTotalTimeoutMultiplier = 0;//

	ct.WriteTotalTimeoutMultiplier = 500;
	ct.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts(m_hCom, &ct);

	//�����¼�����
	m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWait.hEvent = CreateEvent(NULL, false, false, NULL);

	SetCommMask(m_hCom, EV_ERR | EV_RXCHAR);//���ý����¼�

											//������ȡ�߳�
	m_Thread = (HANDLE)_beginthreadex(NULL, 0, &ComAsy::OnRecv, this, 0, NULL);
	//printf("\nComAsy::OnRecv �߳� -- ִ����أ�\n");
	m_IsOpen = true;
	return true;
}
bool ComAsy::ComWrite(LPBYTE buf, int &len)
{
	BOOL rtn = FALSE;
	DWORD WriteSize = 0;

	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
	m_ovWait.Offset = 0;
	rtn = WriteFile(m_hCom, buf, len, &WriteSize, &m_ovWrite);

	len = 0;
	if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//��̨��ȡ
	{
		//�ȴ�����д�����
		if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
		{
			return false;
		}
	}

	len = WriteSize;
	//printf("\nд���ݽ���\n");
	return rtn != FALSE;
}
unsigned int __stdcall ComAsy::OnRecv(void* LPParam)
{
	ComAsy *obj = static_cast<ComAsy*>(LPParam);

	DWORD WaitEvent = 0, Bytes = 0;
	BOOL Status = FALSE;
	BYTE ReadBuf[4096];
	DWORD Error;
	COMSTAT cs = { 0 };
	 
	Readinfo = "";
	char * Readinfoch;
	string TempReadinfo="";
	//printf("\nComAsy::OnRecv �߳� -- ����\n");
	//OutputDebugStringA("ComAsy::OnRecv �߳� -- ������");


	//ofstream outfCom("c:\\outSerial.txt");
	//OutputDebugStringA("�߳� --���ļ���");
	//if (outfCom.is_open())
	//{
	//}

	while (obj->m_IsOpen)
	{
		//printf("\nComAsy::OnRecv obj->m_IsOpen �߳� -- ����\n");
		//OutputDebugStringA("ComAsy::OnRecv obj->m_IsOpen �߳� -- ������");
		WaitEvent = 0;
		obj->m_ovWait.Offset = 0;
		//WaitCommEvent �ȴ�����ͨ���¼��ķ���.
		Status = WaitCommEvent(obj->m_hCom, &WaitEvent, &obj->m_ovWait);
		 
		if (FALSE == Status && GetLastError() == ERROR_IO_PENDING)//
		{		 
			// GetOverlappedResult���������һ��������ΪTRUE��
			//������һֱ�ȴ���ֱ����������ɻ����ڴ�������ء�
			//����������������̻߳�ͣ�ڴˣ����hCom�رջ���������False
			Status = GetOverlappedResult(obj->m_hCom, &obj->m_ovWait, &Bytes, TRUE);
		}
		//��ʹ��ReadFile �������ж�����ǰ,Ӧ��ʹ��ClearCommError����������� 
		ClearCommError(obj->m_hCom, &Error, &cs);

		if (TRUE == Status //�ȴ��¼��ɹ�
			&& WaitEvent&EV_RXCHAR//�����������ݵ���
			&& cs.cbInQue > 0)//������
		{
			Bytes = 0;
			obj->m_ovRead.Offset = 0;
			memset(ReadBuf, 0, sizeof(ReadBuf));
			//�����Ѿ����ﻺ������ReadFile���ᵱ���첽�������������ȡ������True
			Status = ReadFile(obj->m_hCom, ReadBuf, sizeof(ReadBuf), &Bytes, &obj->m_ovRead);

			//if (Status != FALSE)
			//{
			//	cout << "Read:" << (LPCSTR)ReadBuf << "   Len:" << Bytes << endl;
			//	outfCom << (LPCSTR)ReadBuf;
			//	outfCom << "��������.\n";

			//} 
			//Readinfo = byteToHexStr(ReadBuf,30);
			TempReadinfo += (LPCSTR)ReadBuf;

			if ( 32== strlen(TempReadinfo.c_str()) ) {
				Readinfo = TempReadinfo;
				Readinfoch = (char*)Readinfo.c_str(); //��string����תΪchar*
				strcpy_s(G_Readinfochs, Readinfoch);
				TempReadinfo = "";
			}


			//cout << "string Readinfo this: " << Readinfoch << endl; 
			PurgeComm(obj->m_hCom, PURGE_RXCLEAR | PURGE_RXABORT);
		}

	}
	//outfCom.close();

	return 0;
}
void ComAsy::UninitCOM()
{
	m_IsOpen = false;
	if (INVALID_HANDLE_VALUE != m_hCom)
	{
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
	if (NULL != m_ovRead.hEvent)
	{
		CloseHandle(m_ovRead.hEvent);
		m_ovRead.hEvent = NULL;
	}
	if (NULL != m_ovWrite.hEvent)
	{
		CloseHandle(m_ovWrite.hEvent);
		m_ovWrite.hEvent = NULL;
	}
	if (NULL != m_ovWait.hEvent)
	{
		CloseHandle(m_ovWait.hEvent);
		m_ovWait.hEvent = NULL;
	}
	if (NULL != m_Thread)
	{
		WaitForSingleObject(m_Thread, 1000);//�ȴ��߳̽���
		CloseHandle(m_Thread);
		m_Thread = NULL;
	}
}


string byteToHexStr(unsigned char byte_arr[], int arr_len)
{
	string hexstr ="";
	for (int i = 0; i<arr_len; i++)
	{
		char hex1;
		char hex2;
		/*����C++֧�ֵ�unsigned��int��ǿ��ת������unsigned char��ֵ��int��ֵ����ôϵͳ�ͻ��Զ����ǿ��ת��*/
		int value = byte_arr[i];
		int S = value / 16;
		int Y = value % 16;
		//��C++��unsigned char��int��ǿ��ת���õ�����ת����ĸ
		if (S >= 0 && S <= 9)
			hex1 = (char)(48 + S);
		else
			hex1 = (char)(55 + S);
		//��C++��unsigned char��int��ǿ��ת���õ�������ת����ĸ
		if (Y >= 0 && Y <= 9)
			hex2 = (char)(48 + Y);
		else
			hex2 = (char)(55 + Y);
		//���һ���Ĵ���ʵ�֣������õ���������ĸ���ӳ��ַ����ﵽĿ��
		hexstr = hexstr + hex1 + hex2;
	}
	return hexstr;
}