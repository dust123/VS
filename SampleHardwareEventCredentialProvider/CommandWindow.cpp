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
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库


//time
#include <time.h>

#include<stdio.h>

//mysql
#include "MySqlConn.h"

//map
#include <map>

//文件路径
#include <direct.h>

//--------------- 
#ifndef GLOBAL_H
#define GLOBAL_H 
extern ComAsy SerialCom;
#endif 
ComAsy SerialCom;
//---------------

// Custom messages for managing the behavior of the window thread.
//用于管理窗口线程行为的自定义消息。
#define WM_EXIT_THREAD              WM_USER + 1
#define WM_TOGGLE_CONNECTED_STATUS  WM_USER + 2

const TCHAR *g_wszClassName = "EventWindow";
const TCHAR *g_wszConnected = "设备已连接";
const TCHAR *g_wszDisconnected = "设备断开连接";

char cLogin[30]= "请刷卡进行认证登录！";//编译器会自动计算长度 
BOOL SerialWr = FALSE;
unsigned char chData[] = "ready";
LPBYTE WriteBuf = chData;
int len = sizeof(chData);
LPCSTR Port;
int SerialIN = 0;

int RunOnce = 0;
//int tempStrlen = 0;

CCommandWindow::CCommandWindow(void)
{
	OutputDebugStringA("构造：CCommandWindow::CCommandWindow");
    _hWnd = NULL;
    _hInst = NULL;
	_fConnected = FALSE; //TRUE;// FALSE
    _pProvider = NULL;
}

CCommandWindow::~CCommandWindow(void)
{
	OutputDebugStringA("析构: CCommandWindow::CCommandWindow");
    // If we have an active window, we want to post it an exit message.
	//如果我们有一个活动窗口，我们想要发布退出消息。
    if (_hWnd != NULL)
    {
        ::PostMessage(_hWnd, WM_EXIT_THREAD, 0, 0);
        _hWnd = NULL;
    }

    // We'll also make sure to release any reference we have to the provider.
	//我们还将确保向提供商发布我们的任何参考。
    if (_pProvider != NULL)
    {
        _pProvider->Release();
        _pProvider = NULL;
    }
}

// Performs the work required to spin off our message so we can listen for events.
//执行分离我们的消息所需的工作，以便我们可以监听事件

HRESULT CCommandWindow::Initialize(CSampleProvider *pProvider)//比_InitInstance()先执行
{
	OutputDebugStringA("CCommandWindow::Initialize");
	//::MessageBox(NULL, "2", "222222", 0);


	//system("sc stop atest");
	//--------------------------------------------------------------------------------------------------------
	//读配置文件
	map<string, string> mapConfig;

	ifstream configFile;
	string path = "";

	char *buffer;
	//也可以将buffer作为输出参数
	if ((buffer = _getcwd(NULL, 0)) == NULL)
	{
		::MessageBox(NULL, "读取配置文件路径出错", "路径错误", 0);
		exit(-1);
	}
	else
	{
		printf("%s\n", buffer);
		path = buffer;
		free(buffer);
	}

	path += "\\setting.conf";

	configFile.open(path.c_str());
	string str_line;
	if (configFile.is_open())
	{
		//cout << "configFile is_open" << endl;
		while (!configFile.eof())
		{
			getline(configFile, str_line);
			if (str_line.find('#') == 0) //过滤掉注释信息，即如果首个字符为#就过滤掉这一行
			{
				continue;
			}
			size_t pos = str_line.find('=');
			string str_key = str_line.substr(0, pos);
			string str_value = str_line.substr(pos + 1);
			//cout << "str_key is:" << str_key << endl;
			//cout << "str_value is:" << str_value << endl;
			mapConfig.insert(pair<string, string>(str_key, str_value));
		}

		//cout << "map is_open" << endl;
		map<string, string>::iterator iter_configMap;
		iter_configMap = mapConfig.find(string("Com"));

		if (iter_configMap != mapConfig.end())
		{
			//cout << "path is:" << iter_configMap->second << endl;
			strCom = iter_configMap->second;
		}

		iter_configMap = mapConfig.find(string("User"));
		if (iter_configMap != mapConfig.end())
		{
			//cout << "path is:" << iter_configMap->second << endl;
			strUser = iter_configMap->second;
		}

	}
	else
	{
		::MessageBox(NULL, "读取配置文件出错", "错误", 0);
		//cout << "Cannot open config file setting.ini, path: ";
		exit(-1);
	}


	//--------------------------------------------------------------------------------------------------------
	//先串口通信
	//ComAsy SerialCom;

	if (strCom.length() != 0)
	{
		 Port = strCom.c_str();;// = "COM4"; 
	}else{
		::MessageBox(NULL, "读取串口配置文件出错", "错误COM", 0);
		//cout << "Cannot open config file setting.ini, path: ";
		exit(-1);
	}
	 
	SerialIN = 0;
	SerialIN = SerialCom.InitCOM(Port);

	if (SerialIN) {
		//printf("打开串口OK\n");
		OutputDebugStringA("打开串口OK");
	}
	else {
		//printf("打开串口失败！\n");
		OutputDebugStringA("打开串口失败！");
	} 


    HRESULT hr = S_OK;

    // Be sure to add a release any existing provider we might have, then add a reference
    // to the provider we're working with now.
	//确保添加我们可能拥有的任何现有提供者的版本，然后添加对我们现在正在使用的提供者的引用。
    if (_pProvider != NULL)
    {
        _pProvider->Release();
    }
    _pProvider = pProvider;
    _pProvider->AddRef();
    
    // Create and launch the window thread.创建并启动窗口线程。
    HANDLE hThread = ::CreateThread(NULL, 0, CCommandWindow::_ThreadProc, (LPVOID) this, 0, NULL);//_ThreadProc
    if (hThread == NULL)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }

    return hr;
}

// Wraps our internal connected status so callers can easily access it.
//包含我们的内部连接状态，以便呼叫者可以轻松访问它
                   
BOOL CCommandWindow::GetConnectedStatus()
{
	//OutputDebugStringA("CCommandWindow::GetConnectedStatus");
	//锁定屏幕 GetConnectedStatus 会重复执行6次
	//OutputDebugStringA("GetConnectedStatusOK！");
	//::SetWindowText(_hWnd, "GetConnectedStatus");
	//不启用MessageBox是可以的
	//tempStrlen = strlen(Readinfo.c_str()); 
	//::MessageBox(NULL, "_fConnected", "_fConnected", 0);


	while (FALSE ==_fConnected) //加上循环计数作为认证失败的次数
	{

		if (32 == strlen(Readinfo.c_str()))// && (returndata == true)
		{
			//-----------------------------------
			/*验证网络连通性*/
			//char *cmdstr = "cmd /c ping 192.168.0.5 -n 1 -w 1000 > d:\\CarID.txt"; // "cmd /c ping 192.168.0.5 -n 1 -w 1000 >c:\returnpingdata.txt";
			//WinExec(cmdstr, SW_HIDE); //SW_HIDE
			//Sleep(1000);//等待1000ms
			//bool returndata = CCommandWindow::AnalysisFile();//分析命令行返回文件，得到网络连接情况	
			//-----------------------------------
			/*验证用户*/
			//声名变量
			MySqlConn DbTconn;
			bool isOK;
			char strSQL[500];//SQL insert用到
			//char * chSQL = "select * from CarTable where CarNumber='%s";
			//-----------------------------------
			string strGetMachineInfo = "";// , tempSC = "";
			char chGetMachineInfo[250];
			char *sqlTime, *sqlMachineName, *sqlIP, *sqlMAC;
			//-----------------------------------


			//G_Readinfochs;
			isOK = DbTconn.initConnection();
			//::MessageBox(NULL, Readinfo.c_str(), Readinfo.c_str(), 0);
			isOK = DbTconn.user_query(Readinfo);
			if (isOK)//查到了
			{ 
				//--------------------------------------------
				/*取机器信息*/

				strGetMachineInfo = CCommandWindow::MachineInfo();
				strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string 到char[]
				sqlTime = strtok(chGetMachineInfo, "^");
				sqlMachineName = strtok(NULL, "^");
				sqlIP = strtok(NULL, "^");
				sqlMAC = strtok(NULL, "^");

				//::MessageBox(NULL, DbTconn.GetCarNumber.c_str(), DbTconn.GetUserNumber.c_str(), 0);
				//--------------------------------------------
				sprintf_s(strSQL, sizeof(strSQL), "insert into loginTable(CarNumber,UserName,NickName, DTime,MachineName,MachineIP,MachineMAC) values( \"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", DbTconn.GetCarNumber.c_str(), DbTconn.GetUserNumber.c_str(), DbTconn.GetNickNume.c_str(), sqlTime, sqlMachineName, sqlIP, sqlMAC );
				//cout << "strSQL: " << strSQL << endl;
				//::MessageBox(NULL, strSQL, strSQL, 0);
				isOK = DbTconn.user_insert(strSQL);
				//--------------------------------------------
				if (isOK) 
				{
					//RunOnce = 1;
					//关闭串口
					SerialCom.UninitCOM();
					Sleep(1000);
					CCommandWindow::_fConnected = TRUE;
					//tempSC = "sc start atest Cn=";
					//tempSC += DbTconn.GetCarNumber;
					//system( tempSC.c_str() );
					DbTconn.GetCarNumber  = "";
					DbTconn.GetUserNumber = "";
					DbTconn.GetNickNume   ="";
					Sleep(50);
					::SetWindowText(_hWnd, "认证成功");//::g_wszConnected
					memset(cLogin, 0, sizeof(cLogin));
					strcpy_s(cLogin, "认证成功登录中...");
					InvalidateRect(_hWnd, NULL, true);//InvalidateRect发送区域失效， 产生WM_PAINT消息，重绘失效区域
													  //::MessageBox(NULL, "isOK", "isOK", 0);
					
					break; //认证通过跳出
				
				}
				else
				{
					::SetWindowText(_hWnd, "无法写入数据库！");//::g_wszConnected
					memset(cLogin, 0, sizeof(cLogin));
					strcpy_s(cLogin, "登录失败！");
					InvalidateRect(_hWnd, NULL, true);//InvalidateRect发送区域失效， 产生WM_PAINT消息，重绘失效区域
													  //::MessageBox(NULL, "isOK", "isOK", 0);
				}


			}
			else
			{
				//::MessageBox(NULL, "else", "else", 0);
				//-------------------------------------------- 
				//RunOnce = 1;//主动申请卡号
				::printf("网络连接失败\n");

				::SetWindowText(_hWnd, "认证失败！");//::g_wszConnected
				memset(cLogin, 0, sizeof(cLogin));
				strcpy_s(cLogin, "认证失败请重新认证！");
				InvalidateRect(_hWnd, NULL, true);//InvalidateRect发送区域失效， 产生WM_PAINT消息，重绘失效区域
				//-------------------------------------------- 
				//发一次请求
				SerialWr = SerialCom.ComWrite(WriteBuf, len);
				Sleep(500);
				if (!SerialWr) {
					//printf("写数据OK\n");
					OutputDebugStringA("写数据OK！");
				}
				else {
					//printf("写数据失败！\n");
					OutputDebugStringA("写数据失败！");
				}
				Sleep(50);
				//-------------------------------------------- 
				strGetMachineInfo = CCommandWindow::MachineInfo();
				strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string 到char[]
				sqlTime = strtok(chGetMachineInfo, "^");
				sqlMachineName = strtok(NULL, "^");
				sqlIP = strtok(NULL, "^");
				sqlMAC = strtok(NULL, "^");
				//--------------------------------------------
				//sprintf_s(strSQL, sizeof(strSQL), "insert into loginFTable(CarNumber, DTime,MachineName,MachineIP,MachineMAC) values( md5(\"_Z&%sl^_\"),\"%s\",\"%s\",\"%s\",\"%s\");", DbTconn.GetCarNumber.c_str(),sqlTime, sqlMachineName, sqlIP, sqlMAC);
				sprintf_s(strSQL, sizeof(strSQL), "insert into loginFTable(CarNumber, DTime,MachineName,MachineIP,MachineMAC) values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", DbTconn.GetCarNumber.c_str(), sqlTime, sqlMachineName, sqlIP, sqlMAC);
				//cout << "strSQL: " << strSQL << endl;
				//::MessageBox(NULL, strSQL, strSQL, 0);
				isOK = DbTconn.user_insert(strSQL); 
				//--------------------------------------------
			 
			}

			Readinfo = "";//清理string卡号
		}//endif 
		else
		{


			//测试到串口设备断开连接了，需要重新连接串口设备
			if (1 == isopenCOM)
			{
				//先清再掉以前的连接信息
				SerialCom.UninitCOM();

				::SetWindowText(_hWnd, "通信中断！");//::g_wszConnected
				memset(cLogin, 0, sizeof(cLogin));
				strcpy_s(cLogin, "设备断开连接,通信中断！");
				InvalidateRect(_hWnd, NULL, true);//InvalidateRect发送区域失效， 产生WM_PAINT消息，重绘失效区域 

				//打开串口通信
				//ComAsy SerialCom;
				if (strCom.length() != 0)
				{
					Port = strCom.c_str();;// = "COM4"; 
				}
				else {
					::MessageBox(NULL, "读取串口配置文件出错", "错误COM", 0);
					//cout << "Cannot open config file setting.ini, path: ";
					exit(-1);
				}
				SerialIN = 0;
				SerialIN = SerialCom.InitCOM(Port);
				//有没有连接成功
				if (SerialIN) {
					printf("打开串口OK\n");

					::SetWindowText(_hWnd, "设备已连接！");//::g_wszConnected
					memset(cLogin, 0, sizeof(cLogin));
					strcpy_s(cLogin, "请刷卡进行认证登录！");
					InvalidateRect(_hWnd, NULL, true);//InvalidateRect发送区域失效， 产生WM_PAINT消息，重绘失效区域 

					isopenCOM = 0; //连接成功那么退出

				}
				else {
					printf("打开串口失败！\n");
				}
				Sleep(1000);

			}

 
		 
	 


		}

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
//只有在将此代码与添加到Windows 95的“RegisterClassEx”函数之前的Win32系统兼容时，
//才需要使用此函数及其用法。调用此函数以使应用程序“良好地形成”非常重要。 与之相关的小图标。

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
//   PURPOSE: Saves instance handle and creates main window保存实例句柄并创建主窗口
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//在此函数中，我们将实例句柄保存在全局变量中，并创建并显示主程序窗口。

HRESULT CCommandWindow::_InitInstance()
{  
	//OutputDebugStringA("CCommandWindow::_InitInstance");


	RECT rScreen;
	SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rScreen, 0);

	//RECT rWindow;
	//GetWindowRect(&rWindow);

    HRESULT hr = S_OK;
	//::MessageBox(NULL, "1", "1111111", 0);
    // Create our window to receive events.创建窗口以接收事件。
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
            // Show and update the window.显示和更新窗口。
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
//从单独的线程调用以处理消息队列中的下一条消息。 如果没有消息，它将等待一个消息。
//处理子窗口消息
BOOL CCommandWindow::_ProcessNextMessage()
{
	 
	//OutputDebugStringA("CCommandWindow::_ProcessNextMessage");
    // Grab, translate, and process the message.抓取，翻译和处理消息
    MSG msg;
    (void) ::GetMessage(&(msg), _hWnd, 0, 0);
    (void) ::TranslateMessage(&(msg));
    (void) ::DispatchMessage(&(msg));

    switch (msg.message)
    {
    // Return to the thread loop and let it know to exit.返回线程循环，让它知道退出。
    case WM_EXIT_THREAD: return FALSE;

    }
    return TRUE;
}

// Manages window messages on the window thread.管理窗口线程上的窗口消息。
LRESULT CALLBACK CCommandWindow::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
	case  WM_PAINT:
		//--------------------------------------------------------------------------------------------
		HDC hdc; //设备环境句柄
		HFONT hFont;


		PAINTSTRUCT ps;//存储绘图环境的相关信息
		RECT rect;
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		hFont = CreateFont(
			15,							//字体的逻辑高度 
			7,							//字体的逻辑宽度
			0,							//指定移位向量相对X轴的偏转角度
			0,							//指定字符基线相对X轴的偏转角度
			400,//FW_THIN,					//设置字体粗细程度 取值范围0-1000
			false,						//是否启用斜体
			false,						//是否启用下划线
			false,						//是否启用删除线
			DEFAULT_CHARSET,		    //指定字符集   此处用default不然字体会乱码  //GB2312_CHARSET,
			OUT_CHARACTER_PRECIS,		//输出精度
			CLIP_CHARACTER_PRECIS,		//剪裁精度
			DRAFT_QUALITY,				//输出质量
			FF_MODERN,					//字体族
			"宋体"						//字体名
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
    //按钮被点击。
    case WM_COMMAND:
		//::MessageBox(NULL, TEXT("点击了WM_COMMAND"), TEXT("点击了WM_COMMAND "), 0);
        ::PostMessage(hWnd, WM_TOGGLE_CONNECTED_STATUS, 0, 0);
        break;

    // To play it safe, we hide the window when "closed" and post a message telling the 
    // thread to exit.
	//为了安全起见，我们在“关闭”时隐藏窗口并发布消息告诉线程退出。
	//线程退出
    case WM_CLOSE:
        ::ShowWindow(hWnd, SW_HIDE);
        ::PostMessage(hWnd, WM_EXIT_THREAD, 0, 0);
        break;

    default:
		//::MessageBox(NULL, TEXT("default"), TEXT("default"), 0);会被执行N次
		//不关注的信息交给系统自动处理。DefWindowProc
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
 
    return 0;
}

// Our thread procedure. We actually do a lot of work here that could be put back on the 
// main thread, such as setting up the window, etc.
// 我们的线程程序 我们实际上在这里做了很多工作，可以放回主线程，比如设置窗口等。

DWORD WINAPI CCommandWindow::_ThreadProc(LPVOID lpParameter)
{
    CCommandWindow *pCommandWindow = static_cast<CCommandWindow *>(lpParameter);
    if (pCommandWindow == NULL)
    {
        // TODO: What's the best way to raise this error?提出此错误的最佳方法是什么？
        return 0;
    }

    HRESULT hr = S_OK;

    // Create the window.创建窗口。
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
	// 如果遇到告诉我们退出线程的消息，ProcessNextMessage将抽取我们的消息泵并返回false。

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
	bool rState;//返回状态
	FILE *file;
	char ln[80];
	fopen_s(&file, "d:\\CarID.txt", "r");

	fgets(ln, 80, file);//读入空行，舍弃
	fgets(ln, 80, file);//读入ping信息，舍弃
	fgets(ln, 80, file);//读入ping对象返回值，用来分析

	string data = ln;
	string::size_type iPos = data.find("=");
	data = data.substr(iPos + 1, 3);//截取字符串返回字节数
	int  n = atoi(data.c_str());
	rState = n > 0;
	fclose(file);
	return rState;
}

string CCommandWindow::MachineInfo()
{

	//--------------------------------------------
	/*取机器名*/
	const int MAX_BUFFER_LEN = 100;
	char  szBufCPName[MAX_BUFFER_LEN] = {0};
	DWORD dwNameLen;

	dwNameLen = MAX_BUFFER_LEN;
	if (!GetComputerName(szBufCPName, &dwNameLen))
		//printf("Error  %d\n", GetLastError());
		strcpy_s(szBufCPName, "未取到机器名");

	/*取时间*/
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
		strcpy_s(chTime, "2055-55-55 55:55:55"); //string 到char[] chTime = "2019-03-26 12:44:08";
	}
	
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	string strMAC = "";
	string strIP = "";
	char mactemp[10];
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息
		//可能有多网卡,因此通过循环去判断
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
			//cout << "网卡IP地址如下：" << endl;
			//可能网卡有多IP,因此通过循环去判断
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
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	//chTime, szBufCPName, strIP.c_str(), strMAC.c_str()
	if (0 == strIP.length()) { strIP = "未到本机IP"; }
	if (0 == strMAC.length()) { strMAC = "未到本机MAC"; }
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