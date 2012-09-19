// StartupClient.h : CStartupClient 的声明

#pragma once
#include "resource.h"       // 主符号

#include "I8ClientCommunication_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// CStartupClient

class ATL_NO_VTABLE CStartupClient :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CStartupClient, &CLSID_StartupClient>,
	public IDispatchImpl<IStartupClient, &IID_IStartupClient, &LIBID_I8ClientCommunicationLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    int m_iTimeOut;
public:
    CStartupClient() : m_iTimeOut(5000)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_STARTUPCLIENT)


BEGIN_COM_MAP(CStartupClient)
	COM_INTERFACE_ENTRY(IStartupClient)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

    STDMETHOD(RunGame)(LONG gid, VARIANT_BOOL bRunBackstage, VARIANT_BOOL* bResult);
    STDMETHOD(RunClient)(VARIANT_BOOL* bResult);
    STDMETHOD(IsGameExist)(LONG gid, VARIANT_BOOL* bResult);
    STDMETHOD(SetTimeOut)(LONG iTimeOut, VARIANT_BOOL* bResult);
};

OBJECT_ENTRY_AUTO(__uuidof(StartupClient), CStartupClient)
