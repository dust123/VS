//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
//

#include "CommandWindow.h"
#include <strsafe.h>
 
//#include "stdafx.h"
//GetUserName
#pragma comment(lib, "Advapi32.lib")
//getip
#pragma comment(lib,"WS2_32.lib")
//getMAC/IP
#include <Iphlpapi.h> 
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��


//time
#include <time.h>

#include<stdio.h>

//mysql
#include "MySqlConn.h"
//--------------- 
#ifndef GLOBAL_H
#define GLOBAL_H 
extern ComAsy SerialCom;
#endif 
ComAsy SerialCom;
//---------------

// Custom messages for managing the behavior of the window thread.
//���ڹ������߳���Ϊ���Զ�����Ϣ��
#define WM_EXIT_THREAD              WM_USER + 1
#define WM_TOGGLE_CONNECTED_STATUS  WM_USER + 2

const TCHAR *g_wszClassName = "EventWindow";
const TCHAR *g_wszConnected = "�豸������";
const TCHAR *g_wszDisconnected = "�豸�Ͽ�����";

char cLogin[30]= "��ˢ��������֤��¼��";//���������Զ����㳤�� 
BOOL SerialWr = FALSE;
unsigned char chData[] = "ready";
LPBYTE WriteBuf = chData;
int len = sizeof(chData);

int RunOnce = 0;
//int tempStrlen = 0;

CCommandWindow::CCommandWindow(void)
{
	OutputDebugStringA("���죺CCommandWindow::CCommandWindow");
    _hWnd = NULL;
    _hInst = NULL;
	_fConnected = FALSE; //TRUE;// FALSE
    _pProvider = NULL;
}

CCommandWindow::~CCommandWindow(void)
{
	OutputDebugStringA("����: CCommandWindow::CCommandWindow");
    // If we have an active window, we want to post it an exit message.
	//���������һ������ڣ�������Ҫ�����˳���Ϣ��
    if (_hWnd != NULL)
    {
        ::PostMessage(_hWnd, WM_EXIT_THREAD, 0, 0);
        _hWnd = NULL;
    }

    // We'll also make sure to release any reference we have to the provider.
	//���ǻ���ȷ�����ṩ�̷������ǵ��κβο���
    if (_pProvider != NULL)
    {
        _pProvider->Release();
        _pProvider = NULL;
    }
}

// Performs the work required to spin off our message so we can listen for events.
//ִ�з������ǵ���Ϣ����Ĺ������Ա����ǿ��Լ����¼�

HRESULT CCommandWindow::Initialize(CSampleProvider *pProvider)//��_InitInstance()��ִ��
{
	OutputDebugStringA("CCommandWindow::Initialize");
	//::MessageBox(NULL, "2", "222222", 0);


	//system("sc stop atest");
	//--------------------------------------------------------------------------------------------------------
	//�ȴ���ͨ��
	//ComAsy SerialCom;
	string a = "COM3";
	LPCSTR Port = a.c_str();;// = "COM4"; 

							 //stringToLPCWSTR(a)

	int SerialIN = 0;
	SerialIN = SerialCom.InitCOM(Port);

	if (SerialIN) {
		//printf("�򿪴���OK\n");
		OutputDebugStringA("�򿪴���OK");
	}
	else {
		//printf("�򿪴���ʧ�ܣ�\n");
		OutputDebugStringA("�򿪴���ʧ�ܣ�");
	} 


    HRESULT hr = S_OK;

    // Be sure to add a release any existing provider we might have, then add a reference
    // to the provider we're working with now.
	//ȷ��������ǿ���ӵ�е��κ������ṩ�ߵİ汾��Ȼ����Ӷ�������������ʹ�õ��ṩ�ߵ����á�
    if (_pProvider != NULL)
    {
        _pProvider->Release();
    }
    _pProvider = pProvider;
    _pProvider->AddRef();
    
    // Create and launch the window thread.���������������̡߳�
    HANDLE hThread = ::CreateThread(NULL, 0, CCommandWindow::_ThreadProc, (LPVOID) this, 0, NULL);//_ThreadProc
    if (hThread == NULL)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }

    return hr;
}

// Wraps our internal connected status so callers can easily access it.
//�������ǵ��ڲ�����״̬���Ա�����߿������ɷ�����
                   
BOOL CCommandWindow::GetConnectedStatus()
{
	//OutputDebugStringA("CCommandWindow::GetConnectedStatus");
	//������Ļ GetConnectedStatus ���ظ�ִ��6��
	//OutputDebugStringA("GetConnectedStatusOK��");
	//::SetWindowText(_hWnd, "GetConnectedStatus");
	//������MessageBox�ǿ��Ե�
	//tempStrlen = strlen(Readinfo.c_str()); 
	//::MessageBox(NULL, "_fConnected", "_fConnected", 0);


	while (FALSE ==_fConnected) //����ѭ��������Ϊ��֤ʧ�ܵĴ���
	{

		if (10 == strlen(Readinfo.c_str()))// && (returndata == true)
		{
			//-----------------------------------
			/*��֤������ͨ��*/
			//char *cmdstr = "cmd /c ping 192.168.0.5 -n 1 -w 1000 > d:\\CarID.txt"; // "cmd /c ping 192.168.0.5 -n 1 -w 1000 >c:\returnpingdata.txt";
			//WinExec(cmdstr, SW_HIDE); //SW_HIDE
			//Sleep(1000);//�ȴ�1000ms
			//bool returndata = CCommandWindow::AnalysisFile();//���������з����ļ����õ������������	
			//-----------------------------------
			/*��֤�û�*/
			//��������
			MySqlConn DbTconn;
			bool isOK;
			char strSQL[500];//SQL insert�õ�
			char * chSQL = "select * from CarTable where CarNumber='%s";
			//-----------------------------------
			string strGetMachineInfo = "";// , tempSC = "";
			char chGetMachineInfo[250];
			char *sqlTime, *sqlMachineName, *sqlIP, *sqlMAC;
			//-----------------------------------


			//G_Readinfochs;
			isOK = DbTconn.initConnection();
			//::MessageBox(NULL, Readinfo.c_str(), Readinfo.c_str(), 0);
			isOK = DbTconn.user_query(chSQL, Readinfo);
			if (isOK)//�鵽��
			{ 
				//--------------------------------------------
				/*ȡ������Ϣ*/

				strGetMachineInfo = CCommandWindow::MachineInfo();
				strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string ��char[]
				sqlTime = strtok(chGetMachineInfo, "^");
				sqlMachineName = strtok(NULL, "^");
				sqlIP = strtok(NULL, "^");
				sqlMAC = strtok(NULL, "^");

				::MessageBox(NULL, DbTconn.GetCarNumber.c_str(), DbTconn.GetUserNumber.c_str(), 0);
				//--------------------------------------------
				sprintf_s(strSQL, sizeof(strSQL), "insert into loginTable(CarNumber,UserName, DTime,MachineName,MachineIP,MachineMAC) values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", DbTconn.GetCarNumber.c_str(), DbTconn.GetUserNumber.c_str(), sqlTime, sqlMachineName, sqlIP, sqlMAC );
				//cout << "strSQL: " << strSQL << endl;
				//::MessageBox(NULL, strSQL, strSQL, 0);
				isOK = DbTconn.user_insert(strSQL);
				//--------------------------------------------
				if (isOK) 
				{
					//RunOnce = 1;
					//�رմ���
					SerialCom.UninitCOM();
					Sleep(1000);
					CCommandWindow::_fConnected = TRUE;
					//tempSC = "sc start atest Cn=";
					//tempSC += DbTconn.GetCarNumber;
					//system( tempSC.c_str() );
					DbTconn.GetCarNumber = "";
					DbTconn.GetUserNumber = "";
					Sleep(50);
					::SetWindowText(_hWnd, "��֤�ɹ���¼��...");//::g_wszConnected
					memset(cLogin, 0, sizeof(cLogin));
					strcpy_s(cLogin, "��֤�ɹ���¼��...");
					InvalidateRect(_hWnd, NULL, true);//InvalidateRect��������ʧЧ�� ����WM_PAINT��Ϣ���ػ�ʧЧ����
													  //::MessageBox(NULL, "isOK", "isOK", 0);
					
					break; //��֤ͨ������
				
				}
				else
				{
					::SetWindowText(_hWnd, "�޷�д�����ݿ⣡");//::g_wszConnected
					memset(cLogin, 0, sizeof(cLogin));
					strcpy_s(cLogin, "��¼ʧ�ܣ�");
					InvalidateRect(_hWnd, NULL, true);//InvalidateRect��������ʧЧ�� ����WM_PAINT��Ϣ���ػ�ʧЧ����
													  //::MessageBox(NULL, "isOK", "isOK", 0);
				}


			}
			else
			{
				//::MessageBox(NULL, "else", "else", 0);
				//-------------------------------------------- 
				//RunOnce = 1;//�������뿨��
				::printf("��������ʧ��\n");

				::SetWindowText(_hWnd, "��֤ʧ����������֤��");//::g_wszConnected
				memset(cLogin, 0, sizeof(cLogin));
				strcpy_s(cLogin, "��֤ʧ����������֤��");
				InvalidateRect(_hWnd, NULL, true);//InvalidateRect��������ʧЧ�� ����WM_PAINT��Ϣ���ػ�ʧЧ����
				//-------------------------------------------- 
				//��һ������
				SerialWr = SerialCom.ComWrite(WriteBuf, len);
				Sleep(500);
				if (!SerialWr) {
					//printf("д����OK\n");
					OutputDebugStringA("д����OK��");
				}
				else {
					//printf("д����ʧ�ܣ�\n");
					OutputDebugStringA("д����ʧ�ܣ�");
				}
				Sleep(50);
				//-------------------------------------------- 
				strGetMachineInfo = CCommandWindow::MachineInfo();
				strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string ��char[]
				sqlTime = strtok(chGetMachineInfo, "^");
				sqlMachineName = strtok(NULL, "^");
				sqlIP = strtok(NULL, "^");
				sqlMAC = strtok(NULL, "^");
				//--------------------------------------------
				sprintf_s(strSQL, sizeof(strSQL), "insert into loginFTable(CarNumber, DTime,MachineName,MachineIP,MachineMAC) values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", DbTconn.GetCarNumber.c_str(),sqlTime, sqlMachineName, sqlIP, sqlMAC);
				//cout << "strSQL: " << strSQL << endl;
				//::MessageBox(NULL, strSQL, strSQL, 0);
				isOK = DbTconn.user_insert(strSQL); 
				//--------------------------------------------
			 
			}

			Readinfo = "";//����string����
		}//endif 

	}



	//::MessageBox(NULL,"GetConnectedStatus", "GetConnectedStatus", 0);
	_fConnected = TRUE;
    return _fConnected;
}

//
//  FUNCTION: _MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//ֻ���ڽ��˴�������ӵ�Windows 95�ġ�RegisterClassEx������֮ǰ��Win32ϵͳ����ʱ��
//����Ҫʹ�ô˺��������÷������ô˺�����ʹӦ�ó������õ��γɡ��ǳ���Ҫ�� ��֮��ص�Сͼ�ꡣ

HRESULT CCommandWindow::_MyRegisterClass(void)
{
    HRESULT hr = S_OK;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style            = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = CCommandWindow::_WndProc;    //CCommandWindow::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance        = _hInst;
    wcex.hIcon            = NULL;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName    = NULL;
    wcex.lpszClassName    = ::g_wszClassName;
    wcex.hIconSm        = NULL;

    if (!RegisterClassEx(&wcex))
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }

    return hr;
}

//
//   FUNCTION: _InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window����ʵ�����������������
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//�ڴ˺����У����ǽ�ʵ�����������ȫ�ֱ����У�����������ʾ�����򴰿ڡ�

HRESULT CCommandWindow::_InitInstance()
{  
	//OutputDebugStringA("CCommandWindow::_InitInstance");


	RECT rScreen;
	SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rScreen, 0);

	//RECT rWindow;
	//GetWindowRect(&rWindow);

    HRESULT hr = S_OK;
	//::MessageBox(NULL, "1", "1111111", 0);
    // Create our window to receive events.���������Խ����¼���
    _hWnd = ::CreateWindowEx(
        WS_EX_TOPMOST, 
        ::g_wszClassName, 
        ::g_wszConnected,
        WS_DLGFRAME,
		rScreen.right / 2 -100,	//300,
		rScreen.bottom / 2 -180,	//250,
		200, 80, 
        NULL,
        NULL, _hInst, NULL);
    if (_hWnd == NULL)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }
	 
    if (SUCCEEDED(hr))
    { 
        if (SUCCEEDED(hr))
        {
            // Show and update the window.��ʾ�͸��´��ڡ�
            if (!::ShowWindow(_hWnd, SW_NORMAL))//SW_NORMAL|SW_HIDE
            {
                hr = HRESULT_FROM_WIN32(::GetLastError());
            }

            if (SUCCEEDED(hr))
            {
                if (!::UpdateWindow(_hWnd))
                {
                   hr = HRESULT_FROM_WIN32(::GetLastError());
                }
            }
        }
    }
	 
    return hr;
}

// Called from the separate thread to process the next message in the message queue. If
// there are no messages, it'll wait for one.
//�ӵ������̵߳����Դ�����Ϣ�����е���һ����Ϣ�� ���û����Ϣ�������ȴ�һ����Ϣ��
//�����Ӵ�����Ϣ
BOOL CCommandWindow::_ProcessNextMessage()
{
	 
	//OutputDebugStringA("CCommandWindow::_ProcessNextMessage");
    // Grab, translate, and process the message.ץȡ������ʹ�����Ϣ
    MSG msg;
    (void) ::GetMessage(&(msg), _hWnd, 0, 0);
    (void) ::TranslateMessage(&(msg));
    (void) ::DispatchMessage(&(msg));

    switch (msg.message)
    {
    // Return to the thread loop and let it know to exit.�����߳�ѭ��������֪���˳���
    case WM_EXIT_THREAD: return FALSE;

    }
    return TRUE;
}

// Manages window messages on the window thread.�������߳��ϵĴ�����Ϣ��
LRESULT CALLBACK CCommandWindow::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
	case  WM_PAINT:
		//--------------------------------------------------------------------------------------------
		HDC hdc; //�豸�������
		HFONT hFont;


		PAINTSTRUCT ps;//�洢��ͼ�����������Ϣ
		RECT rect;
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		hFont = CreateFont(
			15,							//������߼��߶� 
			7,							//������߼����
			0,							//ָ����λ�������X���ƫת�Ƕ�
			0,							//ָ���ַ��������X���ƫת�Ƕ�
			400,//FW_THIN,					//���������ϸ�̶� ȡֵ��Χ0-1000
			false,						//�Ƿ�����б��
			false,						//�Ƿ������»���
			false,						//�Ƿ�����ɾ����
			DEFAULT_CHARSET,		    //ָ���ַ���   �˴���default��Ȼ���������  //GB2312_CHARSET,
			OUT_CHARACTER_PRECIS,		//�������
			CLIP_CHARACTER_PRECIS,		//���þ���
			DRAFT_QUALITY,				//�������
			FF_MODERN,					//������
			"����"						//������
		);
		SelectObject(hdc, hFont);

		SetTextColor(hdc, RGB(255, 0, 0));
		DrawTextA(
			hdc,
			TEXT(cLogin),
			-1,
			&rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER
		);
		EndPaint(hWnd, &ps);
		//--------------------------------------------------------------------------------------------
    case WM_DEVICECHANGE:
        //::MessageBox(NULL, TEXT("Device change"), TEXT("Device change"), 0);
        break;

    // We assume this was the button being clicked.
    //��ť�������
    case WM_COMMAND:
		//::MessageBox(NULL, TEXT("�����WM_COMMAND"), TEXT("�����WM_COMMAND "), 0);
        ::PostMessage(hWnd, WM_TOGGLE_CONNECTED_STATUS, 0, 0);
        break;

    // To play it safe, we hide the window when "closed" and post a message telling the 
    // thread to exit.
	//Ϊ�˰�ȫ����������ڡ��رա�ʱ���ش��ڲ�������Ϣ�����߳��˳���
	//�߳��˳�
    case WM_CLOSE:
        ::ShowWindow(hWnd, SW_HIDE);
        ::PostMessage(hWnd, WM_EXIT_THREAD, 0, 0);
        break;

    default:
		//::MessageBox(NULL, TEXT("default"), TEXT("default"), 0);�ᱻִ��N��
		//����ע����Ϣ����ϵͳ�Զ�����DefWindowProc
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
 
    return 0;
}

// Our thread procedure. We actually do a lot of work here that could be put back on the 
// main thread, such as setting up the window, etc.
// ���ǵ��̳߳��� ����ʵ�������������˺ܶ๤�������ԷŻ����̣߳��������ô��ڵȡ�

DWORD WINAPI CCommandWindow::_ThreadProc(LPVOID lpParameter)
{
    CCommandWindow *pCommandWindow = static_cast<CCommandWindow *>(lpParameter);
    if (pCommandWindow == NULL)
    {
        // TODO: What's the best way to raise this error?����˴������ѷ�����ʲô��
        return 0;
    }

    HRESULT hr = S_OK;

    // Create the window.�������ڡ�
    pCommandWindow->_hInst = ::GetModuleHandle(NULL);
    if (pCommandWindow->_hInst != NULL)
    {            
        hr = pCommandWindow->_MyRegisterClass();
        if (SUCCEEDED(hr))
        {
            hr = pCommandWindow->_InitInstance();
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }

    // ProcessNextMessage will pump our message pump and return false if it comes across
    // a message telling us to exit the thread.
	// ����������������˳��̵߳���Ϣ��ProcessNextMessage����ȡ���ǵ���Ϣ�ò�����false��

    if (SUCCEEDED(hr))
    {        
        while (pCommandWindow->_ProcessNextMessage()) 
        {
        }
    }
    else
    {
        if (pCommandWindow->_hWnd != NULL)
        {
            pCommandWindow->_hWnd = NULL;
        }
    }

    return 0;
}

BOOL WINAPI CCommandWindow::AnalysisFile()
{
	bool rState;//����״̬
	FILE *file;
	char ln[80];
	fopen_s(&file, "d:\\CarID.txt", "r");

	fgets(ln, 80, file);//������У�����
	fgets(ln, 80, file);//����ping��Ϣ������
	fgets(ln, 80, file);//����ping���󷵻�ֵ����������

	string data = ln;
	string::size_type iPos = data.find("=");
	data = data.substr(iPos + 1, 3);//��ȡ�ַ��������ֽ���
	int  n = atoi(data.c_str());
	rState = n > 0;
	fclose(file);
	return rState;
}

string CCommandWindow::MachineInfo()
{

	//--------------------------------------------
	/*ȡ������*/
	const int MAX_BUFFER_LEN = 100;
	char  szBufCPName[MAX_BUFFER_LEN] = {0};
	DWORD dwNameLen;

	dwNameLen = MAX_BUFFER_LEN;
	if (!GetComputerName(szBufCPName, &dwNameLen))
		//printf("Error  %d\n", GetLastError());
		strcpy_s(szBufCPName, "δȡ��������");

	/*ȡʱ��*/
	char chTime[40] = {0};
	SYSTEMTIME sdt = { 0 };
	GetLocalTime(&sdt);
	sprintf_s(chTime, sizeof(chTime), "%d-%02d-%02d %02d:%02d:%02d",
		sdt.wYear,
		sdt.wMonth,
		sdt.wDay,
		sdt.wHour,
		sdt.wMinute,
		sdt.wSecond);
	if (0 == strlen(chTime)) 
	{
		strcpy_s(chTime, "2055-55-55 55:55:55"); //string ��char[] chTime = "2019-03-26 12:44:08";
	}
	
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//��¼��������
	int netCardNum = 0;
	//��¼ÿ�������ϵ�IP��ַ����
	int IPnumPerNetCard = 0;
	string strMAC = "";
	string strIP = "";
	char mactemp[10];
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;
		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//���������Ϣ
		//�����ж�����,���ͨ��ѭ��ȥ�ж�
		while (pIpAdapterInfo)
		{
			if (strMAC.size() > 10)
			{
				strMAC += "/";
				//strIP += "/";
			}
			if (strIP.size() > 10)
			{
				//strMAC += "/";
				strIP += "/";
			}
			//if (strMAC.size) {}
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
				if (i < pIpAdapterInfo->AddressLength - 1)
				{
					//printf("%02X-", pIpAdapterInfo->Address[i]);
					sprintf_s(mactemp, sizeof(mactemp), "%02X-", pIpAdapterInfo->Address[i]);
					strMAC += mactemp;
					//strMAC += "-";
				}
				else
				{
					//printf("%02X\n", pIpAdapterInfo->Address[i]);
					sprintf_s(mactemp, sizeof(mactemp), "%02X", pIpAdapterInfo->Address[i]);
					strMAC += mactemp;
					strMAC += "";
				}
			//cout << "����IP��ַ���£�" << endl;
			//���������ж�IP,���ͨ��ѭ��ȥ�ж�
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{

				strIP += pIpAddrString->IpAddress.String;

				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);

			pIpAdapterInfo = pIpAdapterInfo->Next;
			//cout << "--------------------------------------------------------------------" << endl;
		}//endwhile

	}
	//�ͷ��ڴ�ռ�
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	//chTime, szBufCPName, strIP.c_str(), strMAC.c_str()
	if (0 == strIP.length()) { strIP = "δ������IP"; }
	if (0 == strMAC.length()) { strIP = "δ������MAC"; }
	string strReturn = "";
	strReturn += chTime;
	strReturn += "^";
	strReturn += szBufCPName;
	strReturn += "^";
	strReturn += strIP;
	strReturn += "^";
	strReturn += strMAC;
	return strReturn;
  
}