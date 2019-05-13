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
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,//设置异步标识
		NULL);
	if (INVALID_HANDLE_VALUE == m_hCom)
	{
		return false;
	}
	SetupComm(m_hCom, 4096, 4096);//设置发送接收缓存

	DCB dcb;
	GetCommState(m_hCom, &dcb);
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = CBR_9600;
	dcb.StopBits = ONESTOPBIT;// ONESTOPBIT;
	dcb.ByteSize = 8;           //每个字节有8位
	dcb.Parity = NOPARITY;      //无奇偶校验位
	SetCommState(m_hCom, &dcb);//配置串口

	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

	COMMTIMEOUTS ct;
	ct.ReadIntervalTimeout = MAXDWORD;//读取无延时，因为有WaitCommEvent等待数据
	ct.ReadTotalTimeoutConstant = 0;  //
	ct.ReadTotalTimeoutMultiplier = 0;//

	ct.WriteTotalTimeoutMultiplier = 500;
	ct.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts(m_hCom, &ct);

	//创建事件对象
	m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWait.hEvent = CreateEvent(NULL, false, false, NULL);

	SetCommMask(m_hCom, EV_ERR | EV_RXCHAR);//设置接受事件

											//创建读取线程
	m_Thread = (HANDLE)_beginthreadex(NULL, 0, &ComAsy::OnRecv, this, 0, NULL);
	//printf("\nComAsy::OnRecv 线程 -- 执行完必！\n");
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
	if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//后台读取
	{
		//等待数据写入完成
		if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
		{
			return false;
		}
	}

	len = WriteSize;
	//printf("\n写数据结束\n");
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
	//printf("\nComAsy::OnRecv 线程 -- 启动\n");
	//OutputDebugStringA("ComAsy::OnRecv 线程 -- 启动！");


	//ofstream outfCom("c:\\outSerial.txt");
	//OutputDebugStringA("线程 --打开文件！");
	//if (outfCom.is_open())
	//{
	//}

	while (obj->m_IsOpen)
	{
		//printf("\nComAsy::OnRecv obj->m_IsOpen 线程 -- 启动\n");
		//OutputDebugStringA("ComAsy::OnRecv obj->m_IsOpen 线程 -- 启动！");
		WaitEvent = 0;
		obj->m_ovWait.Offset = 0;
		//WaitCommEvent 等待串口通信事件的发生.
		Status = WaitCommEvent(obj->m_hCom, &WaitEvent, &obj->m_ovWait);
		 
		if (FALSE == Status && GetLastError() == ERROR_IO_PENDING)//
		{		 
			// GetOverlappedResult函数的最后一个参数设为TRUE，
			//函数会一直等待，直到读操作完成或由于错误而返回。
			//如果缓存中无数据线程会停在此，如果hCom关闭会立即返回False
			Status = GetOverlappedResult(obj->m_hCom, &obj->m_ovWait, &Bytes, TRUE);
		}
		//在使用ReadFile 函数进行读操作前,应先使用ClearCommError函数清除错误 
		ClearCommError(obj->m_hCom, &Error, &cs);

		if (TRUE == Status //等待事件成功
			&& WaitEvent&EV_RXCHAR//缓存中有数据到达
			&& cs.cbInQue > 0)//有数据
		{
			Bytes = 0;
			obj->m_ovRead.Offset = 0;
			memset(ReadBuf, 0, sizeof(ReadBuf));
			//数据已经到达缓存区，ReadFile不会当成异步命令，而是立即读取并返回True
			Status = ReadFile(obj->m_hCom, ReadBuf, sizeof(ReadBuf), &Bytes, &obj->m_ovRead);

			//if (Status != FALSE)
			//{
			//	cout << "Read:" << (LPCSTR)ReadBuf << "   Len:" << Bytes << endl;
			//	outfCom << (LPCSTR)ReadBuf;
			//	outfCom << "串口数据.\n";

			//} 
			//Readinfo = byteToHexStr(ReadBuf,30);
			TempReadinfo += (LPCSTR)ReadBuf;

			if ( 32== strlen(TempReadinfo.c_str()) ) {
				Readinfo = TempReadinfo;
				Readinfoch = (char*)Readinfo.c_str(); //将string类型转为char*
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
		WaitForSingleObject(m_Thread, 1000);//等待线程结束
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
		/*借助C++支持的unsigned和int的强制转换，把unsigned char赋值给int的值，那么系统就会自动完成强制转换*/
		int value = byte_arr[i];
		int S = value / 16;
		int Y = value % 16;
		//将C++中unsigned char和int的强制转换得到的商转成字母
		if (S >= 0 && S <= 9)
			hex1 = (char)(48 + S);
		else
			hex1 = (char)(55 + S);
		//将C++中unsigned char和int的强制转换得到的余数转成字母
		if (Y >= 0 && Y <= 9)
			hex2 = (char)(48 + Y);
		else
			hex2 = (char)(55 + Y);
		//最后一步的代码实现，将所得到的两个字母连接成字符串达到目的
		hexstr = hexstr + hex1 + hex2;
	}
	return hexstr;
}