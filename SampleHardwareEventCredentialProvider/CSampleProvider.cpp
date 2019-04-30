//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
// CSampleProvider implements ICredentialProvider, which is the main
// interface that logonUI uses to decide which tiles to display.
// This sample illustrates processing asynchronous external events and 
// using them to provide the user with an appropriate set of credentials.
// In this sample, we provide two credentials: one for when the system
// is "connected" and one for when it isn't. When it's "connected", the
// tile provides the user with a field to log in as the administrator.
// Otherwise, the tile asks the user to connect first.
//

#include <credentialprovider.h>
#include "CSampleCredential.h"
#include "CommandWindow.h"
#include "guid.h"


//--------------- 
//#ifndef GLOBAL_H
//#define GLOBAL_H 
//extern ComAsy SerialCom;
//#endif 
//ComAsy SerialCom;
//---------------


// CSampleProvider ////////////////////////////////////////////////////////

CSampleProvider::CSampleProvider():
    _cRef(1)
{
	//OutputDebugStringA("构造：CSampleProvider::CSampleProvider");
    DllAddRef();

    _pcpe = NULL;
    _pCommandWindow = NULL;
    _pCredential = NULL;
    _pMessageCredential = NULL;
}

CSampleProvider::~CSampleProvider()
{
	//OutputDebugStringA("析构：CSampleProvider::CSampleProvider");
    if (_pCredential != NULL)
    {
        _pCredential->Release();
        _pCredential = NULL;
    }

    if (_pCommandWindow != NULL)
    {
        delete _pCommandWindow;
    }

    DllRelease();
}

// This method acts as a callback for the hardware emulator. When it's called, it simply
// tells the infrastructure that it needs to re-enumerate the credentials.
void CSampleProvider::OnConnectStatusChanged()
{
	//OutputDebugStringA("CSampleProvider::OnConnectStatusChanged");
    if (_pcpe != NULL)
    {
        _pcpe->CredentialsChanged(_upAdviseContext);
    }
}

// SetUsageScenario is the provider's cue that it's going to be asked for tiles
// in a subsequent call.
//SetUsageScenario是提供者的提示，它将在后续调用中要求切片。
HRESULT CSampleProvider::SetUsageScenario(
    CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
    DWORD dwFlags
    )
{
	//OutputDebugStringA("CSampleProvider::SetUsageScenario");
    UNREFERENCED_PARAMETER(dwFlags);
    HRESULT hr;



	
	//--------------------------------------------------------------------------------------------------------
	//先串口通信
	//ComAsy SerialCom;
	//string a = "COM4";
	//LPCSTR Port = a.c_str();;// = "COM4"; 

	//						 //stringToLPCWSTR(a)

	//int SerialIN = 0;
	//SerialIN = SerialCom.InitCOM(Port);
	//Sleep(10);
	//if (SerialIN) {
	//	//printf("打开串口OK\n");
	//	OutputDebugStringA("打开串口OK");
	//}
	//else {
	//	//printf("打开串口失败！\n");
	//	OutputDebugStringA("打开串口失败！");
	//}
	//Sleep(500);
	//BOOL SerialWr = FALSE;

	//unsigned char chData[] = "ready";
	//LPBYTE WriteBuf = chData;
	//int len = sizeof(chData);;

	//SerialWr = SerialCom.ComWrite(WriteBuf, len);
	//Sleep(500);
	//if (!SerialWr) {
	//	//printf("写数据OK\n");
	//	OutputDebugStringA("写数据OK！");
	//}
	//else {
	//	//printf("写数据失败！\n");
	//	OutputDebugStringA("写数据失败！");
	//}
	//SerialCom.UninitCOM();
	//::MessageBox(NULL, G_Readinfochs, "串口返回信息1", 0);
	//--------------------------------------------------------------------------------------------------------




    // Decide which scenarios to support here. Returning E_NOTIMPL simply tells the caller
    // that we're not designed for that scenario.

	//在这里决定支持哪些方案。返回e_NotImpl只是告诉调用者我们不是为那个场景而设计的。

    switch (cpus)
    {
    case CPUS_LOGON:
    case CPUS_UNLOCK_WORKSTATION:       
        _cpus = cpus;

        // Create the CSampleCredential (for connected scenarios), the CMessageCredential
        // (for disconnected scenarios), and the CCommandWindow (to detect commands, such
        // as the connect/disconnect here).  We can get SetUsageScenario multiple times
        // (for example, cancel back out to the CAD screen, and then hit CAD again), 
        // but there's no point in recreating our creds, since they're the same all the
        // time
        //创建CSampleCredential（对于已连接的方案）、CMessageCredential（对于已断开连接的方案）
		//和CCommandWindow（用于检测命令，如此处的connect/disconnect）。我们可以多次获取setusagescenario
		//（例如，取消返回到CAD屏幕，然后再次点击CAD），但是没有必要重新创建我们的凭证，因为它们一直是相同的。
		//::MessageBox(NULL, TEXT("CSampeProvider"), TEXT("执行了 "), 0);
        if (!_pCredential && !_pMessageCredential && !_pCommandWindow)
        {
            // For the locked case, a more advanced credprov might only enumerate tiles for the 
            // user whose owns the locked session, since those are the only creds that will work
            _pCredential = new CSampleCredential();
            if (_pCredential != NULL)
            {
                _pMessageCredential = new CMessageCredential();
                if (_pMessageCredential)
                {
                    _pCommandWindow = new CCommandWindow();
                    if (_pCommandWindow != NULL)
                    {
                        // Initialize each of the object we've just created. 
                        // - The CCommandWindow needs a pointer to us so it can let us know 
                        // when to re-enumerate credentials.
                        // - The CSampleCredential needs field descriptors.
                        // - The CMessageCredential needs field descriptors and a message.
						
						//初始化我们刚刚创建的每个对象。CCommandWindow需要一个指向我们的指针，
						//以便它可以告诉我们何时重新枚举凭据。CSampleCredential需要字段描述符。
						//CMessageCredential需要字段描述符和消息。
						//初始化 CCommandWindow 类
                        hr = _pCommandWindow->Initialize(this);
                        if (SUCCEEDED(hr))
                        {
                            hr = _pCredential->Initialize(_cpus, s_rgCredProvFieldDescriptors, s_rgFieldStatePairs);
                            if (SUCCEEDED(hr))
                            {
								//::MessageBox(NULL, TEXT("执行情况"), TEXT("执行情况"), 0);
                                hr = _pMessageCredential->Initialize(s_rgMessageCredProvFieldDescriptors, s_rgMessageFieldStatePairs, L"请连接");
                            }
                        }
                    }
                    else
                    {
                        hr = E_OUTOFMEMORY;
                    }
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
            // If anything failed, clean up.
            if (FAILED(hr))
            {
                if (_pCommandWindow != NULL)
                {
                    delete _pCommandWindow;
                    _pCommandWindow = NULL;
                }
                if (_pCredential != NULL)
                {
                    _pCredential->Release();
                    _pCredential = NULL;
                }
                if (_pMessageCredential != NULL)
                {
                    _pMessageCredential->Release();
                    _pMessageCredential = NULL;
                }
            }
        }
        else
        {
            //everything's already all set up
            hr = S_OK;
        }
        break;

    case CPUS_CREDUI:
    case CPUS_CHANGE_PASSWORD:
        hr = E_NOTIMPL;
        break;

    default:
        hr = E_INVALIDARG;
        break;
    }

    return hr;
}

// SetSerialization takes the kind of buffer that you would normally return to LogonUI for
// an authentication attempt.  It's the opposite of ICredentialProviderCredential::GetSerialization.
// GetSerialization is implement by a credential and serializes that credential.  Instead,
// SetSerialization takes the serialization and uses it to create a tile.
//
// SetSerialization采用您通常返回LogonUI进行身份验证尝试的缓冲区类型。 
//它与ICredentialProviderCredential :: GetSerialization相反。 GetSerialization由凭证实现并序列化该凭证。 
//相反，SetSerialization采用序列化并使用它来创建切片。
//
// SetSerialization is called for two main scenarios.  The first scenario is in the credui case
// where it is prepopulating a tile with credentials that the user chose to store in the OS.
// The second situation is in a remote logon case where the remote client may wish to 
// prepopulate a tile with a username, or in some cases, completely populate the tile and
// use it to logon without showing any UI.
//
//为两个主要方案调用SetSerialization。 
//第一种情况是在信任情况下，它正在预先填充具有用户选择存储在OS中的凭证的磁贴。
//第二种情况是远程登录情况，远程客户端可能希望使用用户名预填充磁贴，或者在某些情况下，
//完全填充磁贴并使用它登录而不显示任何UI。
//
// If you wish to see an example of SetSerialization, please see either the SampleCredentialProvider
// sample or the SampleCredUICredentialProvider sample.  [The logonUI team says, "The original sample that
// this was built on top of didn't have SetSerialization.  And when we decided SetSerialization was
// important enough to have in the sample, it ended up being a non-trivial amount of work to integrate
// it into the main sample.  We felt it was more important to get these samples out to you quickly than to
// hold them in order to do the work to integrate the SetSerialization changes from SampleCredentialProvider 
// into this sample.]
//
//如果您希望查看SetSerialization的示例，请参阅SampleCredentialProvider示例或SampleCredUICredentialProvider示例。 
//[logonUI团队说，“这个建立在它之上的原始样本没有SetSerialization。当我们决定SetSerialization在样本中足够重要时，
//它最终成为一个非常重要的工作量来集成 它进入主要样本。我们认为将这些样本快速送到您手中比保留它们更为重要，
//以便将SampleCredentialProvider中的SetSerialization更改集成到此示例中。]

STDMETHODIMP CSampleProvider::SetSerialization(
    const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs
    )
{
	//OutputDebugStringA("CSampleProvider::SetSerialization");
    UNREFERENCED_PARAMETER(pcpcs);
    return E_NOTIMPL;
}

// Called by LogonUI to give you a callback. Providers often use the callback if they
// some event would cause them to need to change the set of tiles that they enumerated
//由LogonUI调用以提供回调。 如果某些事件导致它们需要更改它们枚举的瓦片集，则提供者通常会使用回调

HRESULT CSampleProvider::Advise(
    ICredentialProviderEvents* pcpe,
    UINT_PTR upAdviseContext
    )
{
	//OutputDebugStringA("CSampleProvider::Advise");
    if (_pcpe != NULL)
    {
        _pcpe->Release();
    }
    _pcpe = pcpe;
    _pcpe->AddRef();
    _upAdviseContext = upAdviseContext;
    return S_OK;
}

// Called by LogonUI when the ICredentialProviderEvents callback is no longer valid.
//当ICredentialProviderEvents回调不再有效时由LogonUI调用。
HRESULT CSampleProvider::UnAdvise()
{
	//OutputDebugStringA("CSampleProvider::UnAdvise");
    if (_pcpe != NULL)
    {
        _pcpe->Release();
        _pcpe = NULL;
    }
    return S_OK;
}

// Called by LogonUI to determine the number of fields in your tiles. We return the number
// of fields to be displayed on our active tile, which depends on our connected state. The
// "connected" CSampleCredential has SFI_NUM_FIELDS fields, whereas the "disconnected" 
// CMessageCredential has SMFI_NUM_FIELDS fields.
//由LogonUI调用以确定切片中的字段数。 我们返回要在活动磁贴上显示的字段数，这取决于我们的连接状态。 
//“连接的”CSampleCredential具有SFI_NUM_FIELDS字段，而“断开连接的”CMessageCredential具有SMFI_NUM_FIELDS字段。



HRESULT CSampleProvider::GetFieldDescriptorCount(
    DWORD* pdwCount
    )
{
	//OutputDebugStringA("CSampleProvider::GetFieldDescriptorCount");
    if (_pCommandWindow->GetConnectedStatus())
    {
        *pdwCount = SFI_NUM_FIELDS;
    }
    else
    {
        *pdwCount = SMFI_NUM_FIELDS;
    }
  
    return S_OK;
}

// Gets the field descriptor for a particular field. Note that we need to determine which
// tile to use based on the "connected" status.
//获取特定字段的字段描述符。 请注意，我们需要根据“已连接”状态确定要使用的磁贴。

HRESULT CSampleProvider::GetFieldDescriptorAt(
    DWORD dwIndex, 
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd
    )
{   
	//OutputDebugStringA("CSampleProvider::GetFieldDescriptorAt");
    HRESULT hr;

    if (_pCommandWindow->GetConnectedStatus())
    {
        // Verify dwIndex is a valid field.
        if ((dwIndex < SFI_NUM_FIELDS) && ppcpfd)
        {
            hr = FieldDescriptorCoAllocCopy(s_rgCredProvFieldDescriptors[dwIndex], ppcpfd);
        }
        else
        { 
            hr = E_INVALIDARG;
        }
    }
    else
    {
        // Verify dwIndex is a valid field.
        if ((dwIndex < SMFI_NUM_FIELDS) && ppcpfd)
        {
            hr = FieldDescriptorCoAllocCopy(s_rgMessageCredProvFieldDescriptors[dwIndex], ppcpfd);
        }
        else
        { 
            hr = E_INVALIDARG;
        }
    }

    return hr;
}

// We only use one tile at any given time since the system can either be "connected" or 
// "disconnected". If we decided that there were multiple valid ways to be connected with
// different sets of credentials, we would provide a combobox in the "connected" tile so
// that the user could pick which one they want to use.
// The last cred prov used gets to select the default user tile
//我们在任何给定时间只使用一个磁贴，因为系统可以“连接”或“断开连接”。 
//如果我们确定有多种有效的方法可以与不同的凭证组连接，我们将在“连接”磁贴中提供一个组合框，
//以便用户可以选择他们想要使用的组合。 使用的最后一个信用证来选择默认用户磁贴

HRESULT CSampleProvider::GetCredentialCount(
    DWORD* pdwCount,
    DWORD* pdwDefault,
    BOOL* pbAutoLogonWithDefault
    )
{
	//OutputDebugStringA("CSampleProvider::GetCredentialCount");
    *pdwCount = 1;
    *pdwDefault = 0;
    *pbAutoLogonWithDefault = FALSE;
    return S_OK;
}

// Returns the credential at the index specified by dwIndex. This function is called
// to enumerate the tiles. Note that we need to return the right credential, which depends
// on whether we're connected or not.
//返回dwIndex指定的索引处的凭证。 调用此函数以枚举切片。 请注意，我们需要返回正确的凭据，这取决于我们是否已连接。

HRESULT CSampleProvider::GetCredentialAt(
    DWORD dwIndex, 
    ICredentialProviderCredential** ppcpc
    )
{
	//OutputDebugStringA("CSampleProvider::GetCredentialAt");
    HRESULT hr;
    // Make sure the parameters are valid.确保参数有效。
    if ((dwIndex == 0) && ppcpc)
    {
        if (_pCommandWindow->GetConnectedStatus())
        {
            hr = _pCredential->QueryInterface(IID_ICredentialProviderCredential, reinterpret_cast<void**>(ppcpc));
        }
        else
        {
            hr = _pMessageCredential->QueryInterface(IID_ICredentialProviderCredential, reinterpret_cast<void**>(ppcpc));
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }
        
    return hr;
}

// Boilerplate method to create an instance of our provider. 
HRESULT CSampleProvider_CreateInstance(REFIID riid, void** ppv)
{
	//OutputDebugStringA("CSampleProvider_CreateInstance");
    HRESULT hr;

    CSampleProvider* pProvider = new CSampleProvider();

    if (pProvider)
    {
        hr = pProvider->QueryInterface(riid, ppv);
        pProvider->Release();
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    
    return hr;
}
