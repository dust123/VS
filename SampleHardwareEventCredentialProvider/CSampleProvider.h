//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//

#pragma once

#include <credentialprovider.h>
#include <windows.h>
#include <strsafe.h>

#include "CommandWindow.h"
#include "CSampleCredential.h"
#include "MessageCredential.h"
#include "helpers.h"

// Forward references for classes used here.
class CCommandWindow;
class CSampleCredential;
class CMessageCredential;

class CSampleProvider : public ICredentialProvider
{
  public:
    // IUnknown
    STDMETHOD_(ULONG, AddRef)()
    {
        return _cRef++;
    }
    
    STDMETHOD_(ULONG, Release)()
    {
        LONG cRef = _cRef--;
        if (!cRef)
        {
            delete this;
        }
        return cRef;
    }

    STDMETHOD (QueryInterface)(REFIID riid, void** ppv)
    {
        HRESULT hr;
        if (IID_IUnknown == riid || 
            IID_ICredentialProvider == riid)
        {
            *ppv = this;
            reinterpret_cast<IUnknown*>(*ppv)->AddRef();
            hr = S_OK;
        }
        else
        {
            *ppv = NULL;
            hr = E_NOINTERFACE;
        }
        return hr;
    }

  public:
    IFACEMETHODIMP SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags);
    IFACEMETHODIMP SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs);

    IFACEMETHODIMP Advise(__in ICredentialProviderEvents* pcpe, UINT_PTR upAdviseContext);
    IFACEMETHODIMP UnAdvise();

    IFACEMETHODIMP GetFieldDescriptorCount(__out DWORD* pdwCount);
    IFACEMETHODIMP GetFieldDescriptorAt(DWORD dwIndex,  __deref_out CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd);

    IFACEMETHODIMP GetCredentialCount(__out DWORD* pdwCount,
                                      __out DWORD* pdwDefault,
                                      __out BOOL* pbAutoLogonWithDefault);
    IFACEMETHODIMP GetCredentialAt(DWORD dwIndex, 
                                   __out ICredentialProviderCredential** ppcpc);

    friend HRESULT CSampleProvider_CreateInstance(REFIID riid, __deref_out void** ppv);

public:
    void OnConnectStatusChanged();

  protected:
    CSampleProvider();
    __override ~CSampleProvider();
    
private:
    CCommandWindow              *_pCommandWindow;       // Emulates external events.模拟外部事件。
    LONG                        _cRef;                  // Reference counter. 参考计数器
    CSampleCredential           *_pCredential;          // Our "connected" credential. 我们的“连通”的证书。
    CMessageCredential          *_pMessageCredential;   // Our "disconnected" credential. 我们的“disconnected”证书。
    ICredentialProviderEvents   *_pcpe;                 // Used to tell our owner to re-enumerate credentials. 用于告诉所有者重新枚举凭据。
    UINT_PTR                    _upAdviseContext;       // Used to tell our owner who we are when asking to 用来告诉我们的主人我们要求什么
                                                        // re-enumerate credentials.重新枚举凭据。
    CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus;
};
