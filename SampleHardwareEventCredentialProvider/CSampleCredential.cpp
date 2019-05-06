//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
//

#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include <unknwn.h>
#include "CSampleCredential.h"
#include "guid.h"
//域名
#include <DSRole.h>
#pragma comment(lib, "netapi32.lib")

// CSampleCredential ////////////////////////////////////////////////////////

CSampleCredential::CSampleCredential():
    _cRef(1),
    _pCredProvCredentialEvents(NULL)
{
    DllAddRef();
	//OutputDebugStringA("构造：CSampleCredential::CSampleCredential");
    ZeroMemory(_rgCredProvFieldDescriptors, sizeof(_rgCredProvFieldDescriptors));
    ZeroMemory(_rgFieldStatePairs, sizeof(_rgFieldStatePairs));
    ZeroMemory(_rgFieldStrings, sizeof(_rgFieldStrings));
}

CSampleCredential::~CSampleCredential()
{
	//OutputDebugStringA("析构：CSampleCredential::CSampleCredential");
    if (_rgFieldStrings[SFI_PASSWORD])
    {
        // CoTaskMemFree (below) deals with NULL, but StringCchLength does not.
        size_t lenPassword;
        HRESULT hr = StringCchLengthW(_rgFieldStrings[SFI_PASSWORD], 128, &(lenPassword));
        if (SUCCEEDED(hr))
        {
            SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
        }
        else
        {
            // TODO: Determine how to handle count error here.
        }
    }
    for (int i = 0; i < ARRAYSIZE(_rgFieldStrings); i++)
    {
        CoTaskMemFree(_rgFieldStrings[i]);
        CoTaskMemFree(_rgCredProvFieldDescriptors[i].pszLabel);
    }

    DllRelease();
}

// Initializes one credential with the field information passed in.
// Set the value of the SFI_USERNAME field to pwzUsername.
HRESULT CSampleCredential::Initialize(
    CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
    const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* rgcpfd,
    const FIELD_STATE_PAIR* rgfsp
    )
{
	//OutputDebugStringA("CSampleCredential::Initialize");
	//SetStringValue(SFI_USERNAME, L"Dust");
	//SetStringValue(SFI_PASSWORD, L"123");
	//::MessageBox(NULL, TEXT("CSampeCredential"), TEXT("初始化执行了 "), 0);
    HRESULT hr = S_OK;

    _cpus = cpus;

    // Copy the field descriptors for each field. This is useful if you want to vary the field
    // descriptors based on what Usage scenario the credential was created for.
    for (DWORD i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(_rgCredProvFieldDescriptors); i++)
    {
        _rgFieldStatePairs[i] = rgfsp[i];
        hr = FieldDescriptorCopy(rgcpfd[i], &_rgCredProvFieldDescriptors[i]);
    }

    // Initialize the String value of all the fields.初始化所有字段的String值。
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"CSIC互联认证", &_rgFieldStrings[SFI_USERNAME]);//SHStrDupW在新分配的内存中创建一个字符串的副本
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"12345678-a", &_rgFieldStrings[SFI_PASSWORD]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Submit", &_rgFieldStrings[SFI_SUBMIT_BUTTON]);
    }

    return S_OK;
}

// LogonUI calls this in order to give us a callback in case we need to notify it of anything.
// LogonUI调用它来为我们提供回调，以防我们需要通知它。

HRESULT CSampleCredential::Advise(
    ICredentialProviderCredentialEvents* pcpce
    )
{
	//OutputDebugStringA("CSampleCredential::Advise");
    if (_pCredProvCredentialEvents != NULL)
    {
        _pCredProvCredentialEvents->Release();
    }
    _pCredProvCredentialEvents = pcpce;
    _pCredProvCredentialEvents->AddRef();

    return S_OK;
}

// LogonUI calls this to tell us to release the callback.
HRESULT CSampleCredential::UnAdvise()
{
	//OutputDebugStringA("CSampleCredential::UnAdvise");
    if (_pCredProvCredentialEvents)
    {
        _pCredProvCredentialEvents->Release();
    }
    _pCredProvCredentialEvents = NULL;
    return S_OK;
}

// LogonUI calls this function when our tile is selected (zoomed)
// If you simply want fields to show/hide based on the selected state,
// there's no need to do anything here - you can set that up in the 
// field definitions.  But if you want to do something
// more complicated, like change the contents of a field when the tile is
// selected, you would do it here.

//当我们的磁贴被选中（缩放）时，LogonUI调用此函数如果您只想根据所选状态显示/隐藏字段，
//则无需在此处执行任何操作 - 您可以在字段定义中进行设置。 
//但是如果你想做一些更复杂的事情，比如在选择图块时改变字段的内容，你可以在这里做。
//
//下面来自网络
//用户在登录时选中智能卡标题图片时，即可调用智能卡登录对应的Windows Credential Provider接口函数，
//并调用ICredentialProviderCredential::SetSelected函数，在SetSelected函数中判断智能卡是否已经插入，若插入则允许切换到输入PIN码框界面。
//	if (CheckKeyIsInsert() == true)
//	{
//		SetConnect();
//	}
//用户在PIN码输入框完成PIN码键入后，智能卡Windows Credential Provider调用ICredentialProviderCredential::GetSerialization函数。
//在GetSerialization函数中调用智能卡API校验SFI_PIN对应控件的PIN码是否正确，
//若检验失败，设置SFI_TIP_TEXT对应的提示文字为PIN码校验结果提示信息。若校验成功，则读取和解密文件系统中加密保存的用户登录信息，并返回到GetSerialization函数的输出参数中。
//返回的用户登录信息将被Windows登录应用程序验证，若结果匹配则结束登录界面现在Windows桌面，否则提示错误信息。

//系统自动登录
HRESULT CSampleCredential::SetSelected(BOOL* pbAutoLogon)  
{
	if (NULL != _pCredProvCredentialEvents)
	{
 
		// 设置密码输入控件具备焦点
		_pCredProvCredentialEvents->SetFieldInteractiveState(this, SFI_PASSWORD, CPFIS_FOCUSED);
	}
	 
    *pbAutoLogon = TRUE;  //TRUE   FALSE
    return S_OK;
}

// Similarly to SetSelected, LogonUI calls this when your tile was selected
// and now no longer is.  The most common thing to do here (which we do below)
// is to clear out the password field.

//与SetSelected类似，LogonUI在您的磁贴被选中时调用此方法，现在不再是。 这里最常见的事情（我们在下面做）是清除密码字段。



HRESULT CSampleCredential::SetDeselected()
{
	//OutputDebugStringA("CSampleCredential::SetDeselected");
    HRESULT hr = S_OK;
    if (_rgFieldStrings[SFI_PASSWORD])
    {
        // CoTaskMemFree (below) deals with NULL, but StringCchLength does not.
		//CoTaskMemFree（下面）处理NULL，但StringCchLength不处理。
        size_t lenPassword;
        hr = StringCchLengthW(_rgFieldStrings[SFI_PASSWORD], 128, &(lenPassword));
        if (SUCCEEDED(hr))
        {
            SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
        
            CoTaskMemFree(_rgFieldStrings[SFI_PASSWORD]);
            hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);
        }

        if (SUCCEEDED(hr) && _pCredProvCredentialEvents)
        {
            _pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, _rgFieldStrings[SFI_PASSWORD]);
        }
    }

    return hr;
}

// Get info for a particular field of a tile. Called by logonUI to get information to 
// display the tile.
//获取图块特定字段的信息。 由logonUI调用以获取显示磁贴的信息。
HRESULT CSampleCredential::GetFieldState(
    DWORD dwFieldID,
    CREDENTIAL_PROVIDER_FIELD_STATE* pcpfs,
    CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE* pcpfis
    )
{
	//OutputDebugStringA("CSampleCredential::GetFieldState");
    HRESULT hr;
    
    if (dwFieldID < ARRAYSIZE(_rgFieldStatePairs) && pcpfs && pcpfis)
    {
        *pcpfis = _rgFieldStatePairs[dwFieldID].cpfis;
        *pcpfs = _rgFieldStatePairs[dwFieldID].cpfs;

        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

// Sets ppwsz to the string value of the field at the index dwFieldID.
//将ppwsz设置为索引dwFieldID处字段的字符串值。
HRESULT CSampleCredential::GetStringValue(
    DWORD dwFieldID, 
    PWSTR* ppwsz
    )
{
	//OutputDebugStringA("CSampleCredential::GetStringValue");
    HRESULT hr;

    // Check to make sure dwFieldID is a legitimate index.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && ppwsz) 
    {
        // Make a copy of the string and return that. The caller
        // is responsible for freeing it.
        hr = SHStrDupW(_rgFieldStrings[dwFieldID], ppwsz);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Get the image to show in the user tile.
HRESULT CSampleCredential::GetBitmapValue(
    DWORD dwFieldID, 
    HBITMAP* phbmp
    )
{
	//OutputDebugStringA("CSampleCredential::GetBitmapValue");
    HRESULT hr;
    if ((SFI_TILEIMAGE == dwFieldID) && phbmp)
    {
        HBITMAP hbmp = LoadBitmap(HINST_THISDLL, MAKEINTRESOURCE(IDB_TILE_IMAGE));
        if (hbmp != NULL)
        {
            hr = S_OK;
            *phbmp = hbmp;
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Sets pdwAdjacentTo to the index of the field the submit button should be 
// adjacent to. We recommend that the submit button is placed next to the last
// field which the user is required to enter information in. Optional fields
// should be below the submit button.
//将pdwAdjacentTo设置为提交按钮应与其相邻的字段的索引。
//我们建议将提交按钮放在用户需要输入信息的最后一个字段旁边。可选字段应位于提交按钮下方。
HRESULT CSampleCredential::GetSubmitButtonValue(
    DWORD dwFieldID,
    DWORD* pdwAdjacentTo
    )
{
	//OutputDebugStringA("CSampleCredential::GetSubmitButtonValue");
    HRESULT hr;

    if (SFI_SUBMIT_BUTTON == dwFieldID && pdwAdjacentTo)
    {
        // pdwAdjacentTo is a pointer to the fieldID you want the submit button to 
        // appear next to. 
		//pdwAdjacentTo是指向希望提交按钮显示在旁边的fieldID的指针。
		//::MessageBox(NULL, TEXT("点击登录"), TEXT("点击点击登录 "), 0);
        *pdwAdjacentTo = SFI_PASSWORD;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

// Sets the value of a field which can accept a string as a value.
// This is called on each keystroke when a user types into an edit field
HRESULT CSampleCredential::SetStringValue(
    DWORD dwFieldID, 
    PCWSTR pwz      
    )
{
	//OutputDebugStringA("CSampleCredential::SetStringValue");
    HRESULT hr;

    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
       (CPFT_EDIT_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft || 
        CPFT_PASSWORD_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft)) 
    {
        PWSTR* ppwszStored = &_rgFieldStrings[dwFieldID];
        CoTaskMemFree(*ppwszStored);

        hr = SHStrDupW(pwz, ppwszStored);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

//------------- 
// The following methods are for logonUI to get the values of various UI elements and then communicate
// to the credential about what the user did in that field.  However, these methods are not implemented
// because our tile doesn't contain these types of UI elements
HRESULT CSampleCredential::GetCheckboxValue(
    DWORD dwFieldID, 
    BOOL* pbChecked,
    PWSTR* ppwszLabel
    )
{
	//OutputDebugStringA("CSampleCredential::GetCheckboxValue");
    UNREFERENCED_PARAMETER(dwFieldID);
    UNREFERENCED_PARAMETER(pbChecked);
    UNREFERENCED_PARAMETER(ppwszLabel);

    return E_NOTIMPL;
}

HRESULT CSampleCredential::GetComboBoxValueCount(
    DWORD dwFieldID, 
    DWORD* pcItems, 
    DWORD* pdwSelectedItem
    )
{
	//OutputDebugStringA("CSampleCredential::GetComboBoxValueCount");
    UNREFERENCED_PARAMETER(dwFieldID);
    UNREFERENCED_PARAMETER(pcItems);
    UNREFERENCED_PARAMETER(pdwSelectedItem);
    return E_NOTIMPL;
}

HRESULT CSampleCredential::GetComboBoxValueAt(
    DWORD dwFieldID, 
    DWORD dwItem,
    PWSTR* ppwszItem
    )
{
	//OutputDebugStringA("CSampleCredential::GetComboBoxValueAt");
    UNREFERENCED_PARAMETER(dwFieldID);
    UNREFERENCED_PARAMETER(dwItem);
    UNREFERENCED_PARAMETER(ppwszItem);
    return E_NOTIMPL;
}

HRESULT CSampleCredential::SetCheckboxValue(
    DWORD dwFieldID, 
    BOOL bChecked
    )
{
	//OutputDebugStringA("CSampleCredential::SetCheckboxValue");
    UNREFERENCED_PARAMETER(dwFieldID);
    UNREFERENCED_PARAMETER(bChecked);

    return E_NOTIMPL;
}

HRESULT CSampleCredential::SetComboBoxSelectedValue(
    DWORD dwFieldId,
    DWORD dwSelectedItem
    )
{
	//OutputDebugStringA("CSampleCredential::SetComboBoxSelectedValue");
    UNREFERENCED_PARAMETER(dwFieldId);
    UNREFERENCED_PARAMETER(dwSelectedItem);
    return E_NOTIMPL;
}

HRESULT CSampleCredential::CommandLinkClicked(DWORD dwFieldID)
{
	//OutputDebugStringA("CSampleCredential::CommandLinkClicked");
    UNREFERENCED_PARAMETER(dwFieldID);
    return E_NOTIMPL;
}
//------ end of methods for controls we don't have in our tile ----//

// Collect the username and password into a serialized credential for the correct usage scenario 
// (logon/unlock is what's demonstrated in this sample).  LogonUI then passes these credentials 
// back to the system to log on.
HRESULT CSampleCredential::GetSerialization(
    CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE* pcpgsr,
    CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs, 
    PWSTR* ppwszOptionalStatusText, 
    CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
	//OutputDebugStringA("CSampleCredential::GetSerialization");
    UNREFERENCED_PARAMETER(ppwszOptionalStatusText);
    UNREFERENCED_PARAMETER(pcpsiOptionalStatusIcon);

    KERB_INTERACTIVE_LOGON kil;
    ZeroMemory(&kil, sizeof(kil));

    HRESULT hr;

    WCHAR wsz[MAX_COMPUTERNAME_LENGTH+1];
    DWORD cch = ARRAYSIZE(wsz);


	SHStrDupW(L"Web01", &_rgFieldStrings[SFI_USERNAME]);

	//::MessageBox(NULL, TEXT("CSampeCredential"), TEXT("应该在最后执行"), 0);
	//::MessageBox(NULL, "csic710\\lulanglang", G_Readinfochs, 0);
	//::MessageBox(NULL, G_Readinfochs, G_Readinfochs, 0);
	//SetStringValue(SFI_USERNAME, L"dust");			//此处用户未生效
	//SetStringValue(SFI_PASSWORD, L"12345678-a");	//此处密码未生效
	//SetStringValue(SFI_USERNAME, L"csic710\\lulanglang");			//此处用户未生效
	SetStringValue(SFI_USERNAME, L"Web01");			//此处用户未生效
	SetStringValue(SFI_PASSWORD, L"12345678-a");	//此处密码未生效
	 
	//::MessageBox(NULL, G_Readinfochs, G_Readinfochs, 0);

    if (GetComputerNameW(wsz, &cch))// 取计算机名
    {
        PWSTR pwzProtectedPassword;

        hr = ProtectIfNecessaryAndCopyPassword(_rgFieldStrings[SFI_PASSWORD], _cpus, &pwzProtectedPassword);

        if (SUCCEEDED(hr))
        {
            KERB_INTERACTIVE_UNLOCK_LOGON kiul;
			LPWSTR lpwszStrDNameDns;
			//-------------------------------------------------------------------------
			//取域名
			DSROLE_PRIMARY_DOMAIN_INFO_BASIC * info;
			DWORD dw;
			dw = DsRoleGetPrimaryDomainInformation(NULL,
				DsRolePrimaryDomainInfoBasic,
				(PBYTE *)&info);
			if (dw != ERROR_SUCCESS)
			{
				// wprintf(L"DsRoleGetPrimaryDomainInformation: %u\n", dw);
				// Initialize kiul with weak references to our credential.使用我们的凭证的弱引用初始化kiul。
				hr = KerbInteractiveUnlockLogonInit(wsz, _rgFieldStrings[SFI_USERNAME], pwzProtectedPassword, _cpus, &kiul);
			}
			else
			{
				if (info->DomainNameFlat != NULL)
				{
					//info->DomainNameDns //csic710.com
					lpwszStrDNameDns = info->DomainNameFlat;//csic710

					// Initialize kiul with weak references to our credential.使用我们的凭证的弱引用初始化kiul。
					hr = KerbInteractiveUnlockLogonInit(lpwszStrDNameDns, _rgFieldStrings[SFI_USERNAME], pwzProtectedPassword, _cpus, &kiul);
				}
			}
			//-------------------------------------------------------------------------

            
           
			

            if (SUCCEEDED(hr))
            {
                // We use KERB_INTERACTIVE_UNLOCK_LOGON in both unlock and logon scenarios.  It contains a
                // KERB_INTERACTIVE_LOGON to hold the creds plus a LUID that is filled in for us by Winlogon
                // as necessary.
                hr = KerbInteractiveUnlockLogonPack(kiul, &pcpcs->rgbSerialization, &pcpcs->cbSerialization);

                if (SUCCEEDED(hr))
                {
                    ULONG ulAuthPackage;
                    hr = RetrieveNegotiateAuthPackage(&ulAuthPackage);
                    if (SUCCEEDED(hr))
                    {
                        pcpcs->ulAuthenticationPackage = ulAuthPackage;
                        pcpcs->clsidCredentialProvider = CLSID_CSampleProvider;
 
                        // At this point the credential has created the serialized credential used for logon
                        // By setting this to CPGSR_RETURN_CREDENTIAL_FINISHED we are letting logonUI know
                        // that we have all the information we need and it should attempt to submit the 
                        // serialized credential.
                        *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
                    }
                }
            }

            CoTaskMemFree(pwzProtectedPassword);
        }
    }
    else
    {
        DWORD dwErr = GetLastError();
        hr = HRESULT_FROM_WIN32(dwErr);
    }

    return hr;
}
struct REPORT_RESULT_STATUS_INFO
{
    NTSTATUS ntsStatus;
    NTSTATUS ntsSubstatus;
    PWSTR     pwzMessage;
    CREDENTIAL_PROVIDER_STATUS_ICON cpsi;
};

static const REPORT_RESULT_STATUS_INFO s_rgLogonStatusInfo[] =
{
    { STATUS_LOGON_FAILURE, STATUS_SUCCESS, L"用户名或密码错误！请重新输入！", CPSI_ERROR, },
    { STATUS_ACCOUNT_RESTRICTION, STATUS_ACCOUNT_DISABLED, L"用户不存在", CPSI_WARNING },
};

// ReportResult is completely optional.  Its purpose is to allow a credential to customize the string
// and the icon displayed in the case of a logon failure.  For example, we have chosen to 
// customize the error shown in the case of bad username/password and in the case of the account
// being disabled.
HRESULT CSampleCredential::ReportResult(
    NTSTATUS ntsStatus, 
    NTSTATUS ntsSubstatus,
    PWSTR* ppwszOptionalStatusText, 
    CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
	//OutputDebugStringA("CSampleCredential::ReportResult");
    *ppwszOptionalStatusText = NULL;
    *pcpsiOptionalStatusIcon = CPSI_NONE;

    DWORD dwStatusInfo = (DWORD)-1;

    // Look for a match on status and substatus.
    for (DWORD i = 0; i < ARRAYSIZE(s_rgLogonStatusInfo); i++)
    {
        if (s_rgLogonStatusInfo[i].ntsStatus == ntsStatus && s_rgLogonStatusInfo[i].ntsSubstatus == ntsSubstatus)
        {
            dwStatusInfo = i;
            break;
        }
    }

    if ((DWORD)-1 != dwStatusInfo)
    {
        if (SUCCEEDED(SHStrDupW(s_rgLogonStatusInfo[dwStatusInfo].pwzMessage, ppwszOptionalStatusText)))
        {
            *pcpsiOptionalStatusIcon = s_rgLogonStatusInfo[dwStatusInfo].cpsi;
        }
    }

    // If we failed the logon, try to erase the password field.
    if (!SUCCEEDED(HRESULT_FROM_NT(ntsStatus)))
    {
        if (_pCredProvCredentialEvents)
        {
            _pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, L"");
        }
    }

    // Since NULL is a valid value for *ppwszOptionalStatusText and *pcpsiOptionalStatusIcon
    // this function can't fail.
    return S_OK;
}
