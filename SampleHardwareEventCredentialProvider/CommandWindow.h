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

    CSampleProvider     *_pProvider;        // Pointer to our owner.ָ�������ߵ�ָ�롣
    HWND                 _hWnd;             // Handle to our window.�������ǵĴ��ڡ�
	HWND                 _hWndEDIT;			//�û���������
    HWND                 _hWndButton;       // Handle to our window's button.�����ڰ�ť��
	HWND				 _hWndButtonBack;	//�˳�����
	HWND				 _hWndLab;          //��̬�ļ�
	HWND				 ThWnd;				//͸������
    HINSTANCE            _hInst;            // Current instance��ǰʵ��
    BOOL                 _fConnected;       // Whether or not we're connected. �����Ƿ�����ϵ��
};
