//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
// CCommandWindow provides a way to emulate external "connect" and "disconnect" 
// events, which are invoked via toggle button on a window. The window is launched
// and managed on a separate thread, which is necessary to ensure it gets pumped.
//

#pragma once

#include <windows.h>
#include "CSampleProvider.h"

#include "Global.h"
#include "ComAsy.h"





class CCommandWindow
{
public:
    CCommandWindow(void);
    ~CCommandWindow(void);
    HRESULT Initialize(CSampleProvider *pProvider);
    BOOL GetConnectedStatus();
	BOOL AnalysisFile(string FilePath);
	string GetProgramDir();
	string MachineInfo();
private:
    HRESULT _MyRegisterClass(void);
    HRESULT _InitInstance();
    BOOL _ProcessNextMessage();
    
    static DWORD WINAPI _ThreadProc(LPVOID lpParameter);
    static LRESULT CALLBACK    _WndProc(HWND, UINT, WPARAM, LPARAM);

    CSampleProvider     *_pProvider;        // Pointer to our owner.指向所有者的指针。
    HWND                 _hWnd;             // Handle to our window.处理我们的窗口。
	HWND                 _hWndEDIT;			//用户密码输入
    HWND                 _hWndButton;       // Handle to our window's button.处理窗口按钮。
	HWND				 _hWndButtonBack;	//退出返回
	HWND				 _hWndLab;          //静态文件
	HWND				 ThWnd;				//透明窗口
    HINSTANCE            _hInst;            // Current instance当前实例
    BOOL                 _fConnected;       // Whether or not we're connected. 我们是否有联系。
};
