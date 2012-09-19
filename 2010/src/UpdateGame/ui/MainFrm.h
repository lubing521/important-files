#pragma once
#include "ChildView.h"
#include "UiSplitterWnd.h"
#include "map"
#include "string"
#include "SocketEx.h"

#define  MSG_INITVIEW		WM_USER + 0x0100
#define  WM_SOCKETMSG		WM_USER + 0x0101
#define  WM_DRAWBAR			WM_USER + 0x0102
#define  WM_MYSELECTITEM	WM_USER + 0x0103

#define  SrvName				"I8VDiskSvr"

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)
public:
	CSocketEx*	m_pSocket;
	std::map<std::string,CWnd*> m_mapView;
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	bool IsExistExe(LPCSTR exename);
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar      m_wndStatusBar;
	CChildView		m_wndView;
	CToolBar		m_wndToolBar;
	CImageList		m_ImageLst;
	CUiSplitterWnd	m_wndSplitter1,m_wndSplitter2;
	HANDLE			m_hread,m_hExit;
	bool			m_bInitSyncView;
public:
	afx_msg void OnVdiskinfo();
	afx_msg void OnUpdategameinfo();
	afx_msg void OnSyncinfo();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSetMastrip();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnImupdate();
	afx_msg LRESULT DrawStatusBar(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnHotKey(WPARAM   wParam,LPARAM   lParam);   
	afx_msg	LRESULT InitView(WPARAM wparam,LPARAM lparam);
	DECLARE_MESSAGE_MAP()
private:
	void DrawSyncView(char* pData);
	bool DrawView(char* pData);
	int DrawLog(char* pData);
	int WriteLog(CString strlog);
	int NotiSyncEnd(char* pData);
	bool InstallServer(LPCSTR lpszSvrName,LPCSTR  lpszSvrDispName);
	static UINT _stdcall WorkThread(LPVOID lparam);
};
static CString GetLastErrorText(DWORD dwError)
{
	HLOCAL lpBuffer = NULL;
	CString message;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		NULL, dwError, 0, (LPSTR)&lpBuffer, 0, NULL);
	if (lpBuffer != NULL)
	{
		message = CString((LPSTR)lpBuffer);
		LocalFree(lpBuffer);
	}
	return message;
}
static bool RunServer(LPCSTR SeverName)
{
	SC_HANDLE sc_Handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(sc_Handle == NULL)
	{
		CString SzError(_T("打开服务管理器失败，错误信息：\n")) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}
	SC_HANDLE svcHandle = OpenService(sc_Handle,SeverName,SERVICE_START|SERVICE_STOP|SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS);
	if(svcHandle == NULL)
	{
		CString SzError;
		SzError.Format(_T("打开服务%s失败，错误信息：\n"),SeverName) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}
	if(! StartService(svcHandle,0,NULL))
	{
		DWORD dw = GetLastError();
		if(dw == 1056)
			return true;
		CloseServiceHandle(svcHandle);
		CString SzError;
		SzError.Format(_T("启动服务%s失败，错误信息：\n"),SeverName) ;
		SzError += GetLastErrorText(dw);
		AfxMessageBox(SzError);
		return false;
	}
	return true;
}

static bool  IsExistServer(LPCSTR SeverName)
{
	SC_HANDLE sc_handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(sc_handle == NULL)
	{
		CString SzError(_T("打开服务管理器失败，错误信息：\n")) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}

	LPENUM_SERVICE_STATUS st;
	st = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,128*1024);
	DWORD ret=0;
	DWORD size=0;

	if(! EnumServicesStatus(sc_handle,SERVICE_WIN32,SERVICE_STATE_ALL, (LPENUM_SERVICE_STATUS)st,1024*128,&size,&ret,NULL))
	{
		CString SzError(_T("枚举服务失败，错误信息：\n")) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		CloseServiceHandle(sc_handle);
		LocalFree((HLOCAL)st);
		return false;
	}
	CloseServiceHandle(sc_handle);
	for(DWORD i =0;i<ret;i++)
	{
		CString szName(st[i].lpServiceName);
		if(szName.CompareNoCase(SeverName) == 0)
		{
			LocalFree((HLOCAL)st);
			return true;
		}
	}
	LocalFree((HLOCAL)st);
	return false;

}
static bool StopServer(LPCSTR SeverName)
{
	SC_HANDLE sc_Handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(sc_Handle == NULL)
	{
		CString SzError(_T("打开服务管理器失败，错误信息：\n")) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}
	SC_HANDLE svcHandle = OpenService(sc_Handle,SeverName,SERVICE_START|SERVICE_STOP|SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS);
	if(svcHandle == NULL)
	{
		CString SzError;
		SzError.Format(_T("打开服务%s失败，错误信息：\n"),SeverName) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}
	SERVICE_STATUS st = {0};
	if(! ControlService(svcHandle,SERVICE_CONTROL_STOP,&st))
	{
		CloseServiceHandle(svcHandle);
		CString SzError;
		SzError.Format(_T("停止服务%s失败，错误信息：\n"),SeverName) ;
		SzError += GetLastErrorText(GetLastError());
		AfxMessageBox(SzError);
		return false;
	}
	while ( QueryServiceStatus( svcHandle,&st ) )
	{
		if ( st.dwCurrentState == SERVICE_STOP_PENDING )
		{
			Sleep( 1000 );
		}
		else
		break;
	}
	CloseServiceHandle(svcHandle);
	return true;
}