// LogRpt.cpp : 用户验证与游戏，菜单上报插件。

#include "stdafx.h"
#include <process.h>
#include <time.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <Iphlpapi.h>
#include <atlenc.h>
#include <psapi.h>
#include <winhttp.h>

#include <algorithm>

#include "MD5.h"
#include "ftp.h"
#include "../../include/Utility/SmartHandle.hpp"
#include "../../include/Extend STL/StdEx.h"



#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "psapi.lib")

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <wbemidl.h>

#pragma comment(lib, "Wbemuuid") 

using namespace std;

#include "LogRpt.h"
#include "XZip.h"
#include "Markup.h"

#import "../../lib/Winhttp.dll"

#define PLUG_VERSION		0x01000000L
#define PLUG_COMPANY		TEXT("Goyoo")
#define PLUG_DESCRIPTION	TEXT("LogRptPlug")
#define PLUG_PORT			0

namespace i8desk
{
	class tagUserInfo
	{
	public:
		//in param.
		DWORD	dwCpuID;		//硬件ID
		tstring szUser;			//用户名
		tstring szPassword;		//密码
		tstring szSrvMac;		//服务器MAC地址
 		tstring szSrvVersion;	//服务器程序版本

		//out param.
		DWORD   dwResCode;		//表示用户验证返回的flag值
		DWORD   dwNibID;		//网吧nid		
		DWORD	dwSID;			//网吧服务器sid
		DWORD	dwRID;			//用户区域rid
		DWORD	dwYear;			//用户过期日期年
		DWORD	dwMonth;		//用户过期日期月
		DWORD	dwDay;			//用户过期日期
		bool    dwUploadLog;	//是否上传日志
		tstring szNbName;		//网吧名称:显示到菜单网吧地址的第一行
		tstring szNbProvince;	//网吧所在省
		tstring szNbCity;		//网吧所在市
		tstring szNbRealityUrl;	//网吧实名信息修改网址
		tstring szNbAddress;	//网吧地址：真实的网吧地址
		tstring szNbPhone;		//网吧电话（座机）
		tstring szNbMobile;		//网吧移动电话（手机）
		tstring szNbTitle;		//显示到菜单网吧名称的第二行。
		tstring szNbSubTitle;	//显示到菜单网吧名称的第三行
		tstring szNbTitleUrl;	//网吧首页：点点击菜单网吧地址后，弹出这个url.如果为空，则点周网吧名称后显示公告
		DWORD	dwConShowDialog;//中心下发给控制台的信息:是否弹出对话框(1|0)
		DWORD   dwConShowCancel;//中心下发给控制台的信息:是否显示取消按钮
		tstring szConMessage;	//中心下发给控制台的信息:信息内容
		tstring szConOkUrl;		//中心下发给控制台的信息:点确定后弹出的网址
		tstring szOemID;		//渠道id.
		tstring szOemName;		//渠道名称
		tstring szUserType;		//用户类型
		tstring szMessage;		//中心给用户提示信息
		tstring szPlugParam;	//注册插件的参数信息
		tstring szTaskInfo;		//记录开机任务的信息
		tstring szError;		//返回的错误信息(当flag不等于1时)
		tstring szMenuGGao;		//菜单公告弹出控制.p1=-1|0|1(表示按网吧设置|强制关|强制开),p2=n保留
		tstring szMenuPop;		//菜单右下角广告弹出控制.p1=0|1(表示关|开),p2=n保留
		tstring szBotLayer;		//菜单底部广告
		tstring szMenuPage;		//菜单默认页
		tstring szUrlNav;		//网址导航列表
		tstring szVdMus;		//视频音乐列表
		tstring szDotCard;		//点卡商城地址
		tstring szGameRoomUrl;	//游戏大厅后置网址
		tstring szPersonUrl;	//菜单个人用户中心网址

		tstring szNic;			//网卡名称
		int uAdslSpeed;			// ADSL Speed 0是不上传，-1是不限制，其它是限制速度
		tagUserInfo()
		{
			dwCpuID = 0;
			dwResCode = 0;
			dwNibID	= 0;
			dwSID	= 0;
			dwRID	= 0;
			dwYear  = 0;
			dwMonth	= 0;
			dwDay	= 0;	

			uAdslSpeed = 0;
		}
	};

	class CLogRptPlug : public IPlug
	{
	private:
		IPlugMgr*	m_pPlugMgr;
		ILogger*	m_pLogger;
		HANDLE		m_hExited;
		HANDLE		m_hThread;
		HANDLE		m_hNotify;
		tagUserInfo*m_pUserInfo;
	public:
		CLogRptPlug():m_pPlugMgr(NULL), m_pLogger(NULL), m_hExited(NULL), m_hThread(NULL)
		{
			m_pUserInfo = new tagUserInfo;
		}
		virtual ~CLogRptPlug()				{  delete m_pUserInfo;      }
		virtual void Release()				{  delete this;				}
	public:
		virtual LPCTSTR GetPlugName()		{  return LOGRPT_PLUG_NAME;	}
		virtual DWORD  	GetVersion() 		{  return PLUG_VERSION;		}
		virtual LPCTSTR GetCompany()		{  return PLUG_COMPANY;		}
		virtual LPCTSTR GetDescription() 	{  return PLUG_DESCRIPTION; }
		virtual DWORD   GetListPort()		{  return PLUG_PORT;		}

		virtual bool	Initialize(IPlugMgr* pPlugMgr);
		virtual bool	UnInitialize();



		/*
			什么时候、什么组件会发送1、2呢？
		
		*/
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2)	
		{
			if (cmd == 1)	//通知验证.
			{
				SetEvent(m_hNotify);
			}
			else if (cmd == 2)	//取得验证状态，是否验证成功.(返回１表示成功，返回０表示失败)
			{
				return (m_pUserInfo->dwResCode == 1 ? 1 : 0);
			}
			return ERROR_SUCCESS; 
		}

		//该插件不需要处理客户端的网络命令.
		virtual void	OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}
		virtual void	OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
		virtual void	OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
		virtual void	OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}
	private:
		static UINT __stdcall WorkThread(PVOID lpVoid);	//用户验证与上报工作线程
		void InitUserInfo(IPlug* pPlug);				//初始化用户信息
		void UserLogin(IPlug* pPlug);					//用户验证
		void WriteInfoToSysOpt(IPlug* pPlug);			//验证成功后把信息写到选项表.
		void GetAvdInfo(IPlug* pPlug);					//得到广告控制信息
		void GetTask(IPlug* pPlug);						//得到
		void ReportCliInfo(IPlug* pPlug);				//菜单运行次数
		void ReportGameClick(IPlug* pPlug);				//报告客户端游戏点击数
		void UpLoadLog();								//向中心上传日志
		void DeleteMoreLogFile(HANDLE m_hExit);			//删除7天以前的日志文件。
		_bstr_t GetDevInfoItem(IWbemServices* pService, LPCTSTR lpszQuery, LPCTSTR lpszField);
		tstring GetDeviceInfo();
		tstring GetOtherSvrDeviceInfo();


		tstring GetSvrMacAddr()
		{
			TCHAR szMac[20] = TEXT("00-00-00-00-00-00");
			IP_ADAPTER_INFO adapter[10] = {0};
			DWORD dwSize = sizeof(adapter);
			if (ERROR_SUCCESS != GetAdaptersInfo(adapter, &dwSize))
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, "GetSvrMacAddr Error:%d:%s", dwError, Error.ErrorMessage());
				return szMac;
			}

			PIP_ADAPTER_INFO pAdapter = adapter;
			while (pAdapter!= NULL)
			{
				if (pAdapter->Type == MIB_IF_TYPE_ETHERNET &&
					strstr(pAdapter->Description, "VMware") == NULL 
					)
				{
					_stprintf_s(szMac, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
						pAdapter->Address[0],
						pAdapter->Address[1],
						pAdapter->Address[2],
						pAdapter->Address[3],
						pAdapter->Address[4],
						pAdapter->Address[5]);
					m_pUserInfo->szNic = pAdapter->Description;
					break;
				}

				pAdapter = pAdapter->Next;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("GetMacAddr:%s"), szMac);
			return tstring(szMac);
		}
		
	};

	bool CLogRptPlug::Initialize(IPlugMgr* pPlugMgr)
	{
		m_pPlugMgr =  pPlugMgr;
		
		if ((m_pLogger = m_pPlugMgr->CreateLogger()) == NULL)
			return false;
		m_pLogger->SetLogFileName(LOGRPT_PLUG_NAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(LM_INFO, TEXT("=============================================================="));
		m_pLogger->SetAddDateBefore(true);
		m_pLogger->WriteLog(LM_INFO, TEXT("Plug Initialize."));
		DWORD dwError = 0;
		if ((m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
			dwError = GetLastError();
		if ((m_hNotify = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
			dwError = GetLastError();
	
		if (dwError)
		{
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create Event Fail:%d:%s"), dwError, Error.ErrorMessage());
		}
		else
		{
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
			if (m_hThread != NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Create WorkThread Success."));
			}
			else
			{
				dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Create WorkThread Fail:%d:%s"), dwError, Error.ErrorMessage());
			}
		}
		return m_hThread != NULL;
	}

	bool CLogRptPlug::UnInitialize()
	{
		if (m_hExited != NULL && m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
		}
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
		if (m_hExited != NULL)
			CloseHandle(m_hExited);
		if (m_hNotify != NULL)
			CloseHandle(m_hNotify);
		m_hThread = NULL;
		m_hExited = NULL;
		m_hNotify = NULL;

		m_pLogger->WriteLog(LM_INFO, TEXT("Plug UnInitialize."));
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(LM_INFO, TEXT("==============================================================\r\n\r\n"));
		Release_Interface(m_pLogger);
		return true;
	}

	UINT __stdcall CLogRptPlug::WorkThread(PVOID lpVoid)
	{
		CoInitialize(NULL);
		CLogRptPlug* pThis = reinterpret_cast<CLogRptPlug*>(lpVoid);
		HANDLE hEvent[2] = {pThis->m_hExited, pThis->m_hNotify};
		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(2, hEvent, FALSE, 1 * 1000);
			if (dwRet == WAIT_OBJECT_0)
				break;
			else if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
			{
				static int nInterval = 0;
				static int nInterval2 = 1;
				static IPlug* pPlug = pThis->m_pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);

				if (nInterval % (2 * 60 * 60) == 0 || dwRet == WAIT_OBJECT_0 + 1)
				{
					nInterval = 0;
					pThis->InitUserInfo(pPlug);

					if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
						break;

					pThis->UserLogin(pPlug);
					if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
						break;

					if (pThis->m_pUserInfo->dwResCode == 1)
					{
						pThis->GetAvdInfo(pPlug);
						if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
							break;

						pThis->ReportCliInfo(pPlug);
						if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
							break;

						//write info to tSysOpt table.
						pThis->WriteInfoToSysOpt(pPlug);

						pThis->UpLoadLog();
					}

					pThis->DeleteMoreLogFile(pThis->m_hExited);
				}
				if (nInterval2 % (12 * 60 * 60) == 0 || dwRet == WAIT_OBJECT_0 + 1)
				{
					nInterval2 = 0;
					if (pThis->m_pUserInfo->dwResCode == 1)
					{
						pThis->ReportGameClick(pPlug);
					}
				}
				nInterval++;
				nInterval2++;
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("WorkThread WaitForMultipleObjects Fail:%d:%s."), 
					dwError, Error.ErrorMessage());
			}
		}
		CoUninitialize();
		pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Exit WorkThread."));
		return 0;
	}
	
	void CLogRptPlug::InitUserInfo(IPlug* pPlug)
	{
		TCHAR buf[1024] = {0};
		ISysOpt* pSysOpt = m_pPlugMgr->GetSysOpt();

		m_pUserInfo->dwCpuID = 0;

		pSysOpt->GetOpt(OPT_U_USERNAME, buf, TEXT(""));
		m_pUserInfo->szUser			= buf;
		pSysOpt->GetOpt(OPT_U_PASSWORD, buf, TEXT(""));
		m_pUserInfo->szPassword		= buf;

		m_pUserInfo->dwNibID        = pSysOpt->GetOpt(OPT_U_NID, 0L);
		m_pUserInfo->dwSID			= pSysOpt->GetOpt(OPT_U_SID, 0L);
		m_pUserInfo->dwRID			= pSysOpt->GetOpt(OPT_U_RID, 0L);
		m_pUserInfo->szSrvMac		= GetSvrMacAddr();
		m_pUserInfo->szSrvVersion	= GetFileVersion(TEXT("I8DeskSvr.exe"));
	}

	void CLogRptPlug::UserLogin(IPlug* pPlug)
	{
		/*/////////////////////////////////////////////////////////////////////////
		//为了不需要登陆用户进行的演示。
		if (m_pUserInfo->szUser == "i8test")
		{
			m_pUserInfo->dwResCode	= 1;
			m_pUserInfo->dwYear		= 2010;
			m_pUserInfo->dwMonth	= 6;
			m_pUserInfo->dwDay		= 30;
			m_pPlugMgr->GetSysOpt()->SetOpt(OPT_U_ERRINFO, "");
			return ;
		}
		//////////////////////////////////////////////////////////////////////////
		*/

		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));
		_bstr_t url =  TEXT("http://desk.cgi.i8cn.com/Cybercafe/Server/Login.koc");

		try
		{
			http->Open(_bstr_t("POST"), url, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("close"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
			stringstream out;
			out << "username="		<< uri_encode(m_pUserInfo->szUser.c_str());
			out << "&password="		<< uri_encode(MDString(m_pUserInfo->szPassword.c_str()).c_str());
			out << "&cpuid="		<< m_pUserInfo->dwCpuID;
			out << "&smac="			<< uri_encode(m_pUserInfo->szSrvMac.c_str());
			out << "&sver="			<< uri_encode(m_pUserInfo->szSrvVersion.c_str());
			//检查oem版本。看是否上行parentid
			{
				tstring iniFile = GetAppPath() + TEXT("oem\\oem.ini");
				tstring pid = GetOpt(TEXT("OEM"), TEXT("oemid"), TEXT(""));
				if (pid.size())
				{
					out << "&parentid=" << pid;
				}
			}
			string strSend = out.str();

			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				return;

			http->Send(_bstr_t(strSend.c_str()));
			if (200 != http->Status)
			{
				TCHAR szError[MAX_PATH] = {0};
				_stprintf(szError, "http返回代码错:%d", http->Status);
				m_pUserInfo->szError = szError;
				m_pLogger->WriteLog(LM_INFO, TEXT("Check StatusCode Error Result:%d."), http->Status);

				//只要是http访问出错，就再去另一个验证。
				_com_raise_error(0);
			}
			else
			{
				tstring str = http->ResponseText;
				if (str.substr(0, 5) != tstring(TEXT("flag=")))
				{
					tstring result;
					size_t pos = str.find(TEXT("flag="));
					if (pos != tstring::npos)
						result = str.substr(pos);
					if (result.size() == 0)
					{
						m_pUserInfo->szError = TEXT("数据包有错:可能是中了病毒");
						m_pLogger->WriteLog(LM_INFO, TEXT("User Check Error:%s."), str.c_str());
						m_pLogger->WriteLog(LM_INFO, m_pUserInfo->szError.c_str());
						_com_raise_error(0);
					}
					else {	str = result; }
				}
				vector<tstring> splits;
				splitString(str, splits, TEXT('&'));
				for (size_t idx=0; idx<splits.size(); idx++)
				{
					tstring xItem = splits[idx];
					tstring::size_type pos = xItem.find_first_of(TEXT('='));
					if (pos == tstring::npos)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("Error Item:%s"), xItem.c_str());
						continue;
					}
					tstring name = xItem.substr(0, pos);
					tstring value = xItem.substr(pos+1);
					
					if (_strcmpi(name.c_str(), TEXT("flag")) == 0)
					{
						m_pUserInfo->dwResCode = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("errinfo")) == 0)
					{
						m_pUserInfo->szError = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nid")) == 0)
					{
						m_pUserInfo->dwNibID = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("nbname")) == 0)
					{
						m_pUserInfo->szNbName = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbtitle")) == 0)
					{
						m_pUserInfo->szNbTitle = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbsubtitle")) == 0)
					{
						m_pUserInfo->szNbSubTitle = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbtitleurl")) == 0)
					{
						m_pUserInfo->szNbTitleUrl = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbaddress")) == 0)
					{
						m_pUserInfo->szNbAddress = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbphone")) == 0)
					{
						m_pUserInfo->szNbPhone = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("nbmobile")) == 0)
					{
						m_pUserInfo->szNbMobile = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("sid")) == 0)
					{
						m_pUserInfo->dwSID = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("rid")) == 0)
					{
						m_pUserInfo->dwRID = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("dateend")) == 0)
					{
						_stscanf(value.c_str(), TEXT("%d-%d-%d"), 
							&m_pUserInfo->dwYear,
							&m_pUserInfo->dwMonth,
							&m_pUserInfo->dwDay);
					}
					else if (_strcmpi(name.c_str(), TEXT("alertinfo")) == 0)
					{
						//m_pUserInfo->szMessage = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("usertype")) == 0)
					{
						m_pUserInfo->szUserType = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("the3params")) == 0)
					{
						m_pUserInfo->szPlugParam = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("bindoemid")) == 0)
					{
						m_pUserInfo->szOemID = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("oemname")) == 0)
					{
						m_pUserInfo->szOemName = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("province")) == 0)
					{
						m_pUserInfo->szNbProvince = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("city")) == 0)
					{
						m_pUserInfo->szNbCity = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("infoupdateurl")) == 0)
					{
						m_pUserInfo->szNbRealityUrl = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("dialog")) == 0)
					{
						m_pUserInfo->dwConShowDialog = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("dcancel")) == 0)
					{
						m_pUserInfo->dwConShowCancel = _ttoi(value.c_str());
					}
					else if (_strcmpi(name.c_str(), TEXT("dmessage")) == 0)
					{
						m_pUserInfo->szConMessage = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("durl")) == 0)
					{
						m_pUserInfo->szConOkUrl = value;
					}
					else if (_strcmpi(name.c_str(), TEXT("UploadLog")) == 0)
					{
						m_pUserInfo->dwUploadLog = (_ttoi(value.c_str()) != 0);
					}
					else if (_strcmpi(name.c_str(), TEXT("p2puploadlimit")) == 0)
					{
						m_pUserInfo->uAdslSpeed = _ttoi(value.c_str());
					}
				}
				if (m_pUserInfo->dwResCode != 1)	//flag == 1表示登陆成功
				{
					if (m_pUserInfo->szError.size() == 0)
						m_pUserInfo->szError = TEXT("验证失败:flag 不等于 1");
					m_pLogger->WriteLog(LM_INFO, TEXT("User Check Error:%s"), m_pUserInfo->szError.c_str());
					m_pPlugMgr->GetSysOpt()->SetOpt(OPT_U_ERRINFO, m_pUserInfo->szError.c_str());
				}
				else
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("User Check Success."));
					m_pPlugMgr->GetSysOpt()->SetOpt(OPT_U_ERRINFO, "");
				}
			}
		}
		catch (_com_error& e) 
		{
			if (e.Error() != 0)
			{
				m_pUserInfo->szError = e.ErrorMessage();
				WORD w = LOWORD(e.Error());
				switch(w) 
				{
				case ERROR_WINHTTP_NAME_NOT_RESOLVED:
					m_pUserInfo->szError = TEXT("域名无法解析");
					break;
				case ERROR_WINHTTP_CANNOT_CONNECT:
				case ERROR_WINHTTP_CONNECTION_ERROR:
					m_pUserInfo->szError = TEXT("连接服务器失败");
					break;
				case ERROR_WINHTTP_TIMEOUT:
					m_pUserInfo->szError = TEXT("验证时超时错");
					break;
				default:
					m_pUserInfo->szError = TEXT("http访问出错.");
					break;
				}
			}
			m_pPlugMgr->GetSysOpt()->SetOpt(OPT_U_ERRINFO, m_pUserInfo->szError.c_str());

			m_pLogger->WriteLog(LM_INFO, TEXT("User Check Error:%s:%s:%s"), (LPCTSTR)url, (LPCTSTR)e.Description(), m_pUserInfo->szError.c_str());
		}
		catch (...) {	m_pLogger->WriteLog(LM_INFO, TEXT("User Check Unknown Error."));}
	}

	void CLogRptPlug::WriteInfoToSysOpt(IPlug* pPlug)
	{
		if (m_pUserInfo->dwResCode != 1)
			return ;
		ISysOpt* pSysOpt = m_pPlugMgr->GetSysOpt();
		if (pSysOpt == NULL)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("WriteInfoToSysOpt GetSysOpt Inferace is null"));
			return ;
		}

		pSysOpt->SetOpt(OPT_U_ERRINFO,		TEXT(""));
		pSysOpt->SetOpt(OPT_U_CPUID,		(long)m_pUserInfo->dwCpuID);
		pSysOpt->SetOpt(OPT_U_SMAC,			m_pUserInfo->szSrvMac.c_str());
		pSysOpt->SetOpt(OPT_U_SVER,			m_pUserInfo->szSrvVersion.c_str());
		pSysOpt->SetOpt(OPT_U_NID,			(long)m_pUserInfo->dwNibID);
		pSysOpt->SetOpt(OPT_U_SID,			(long)m_pUserInfo->dwSID);
		pSysOpt->SetOpt(OPT_U_RID,			(long)m_pUserInfo->dwRID);
		pSysOpt->SetOpt(OPT_U_OEMID,		m_pUserInfo->szOemID.c_str());
		pSysOpt->SetOpt(OPT_U_OEMNAME,		m_pUserInfo->szOemName.c_str());
		pSysOpt->SetOpt(OPT_U_NBNAME,		m_pUserInfo->szNbName.c_str());
		pSysOpt->SetOpt(OPT_U_PROVINCE,		m_pUserInfo->szNbProvince.c_str());
		pSysOpt->SetOpt(OPT_U_CITY,			m_pUserInfo->szNbCity.c_str());
		pSysOpt->SetOpt(OPT_U_REALITYURL,	m_pUserInfo->szNbRealityUrl.c_str());
		pSysOpt->SetOpt(OPT_U_NBADDRESS,	m_pUserInfo->szNbAddress.c_str());
		pSysOpt->SetOpt(OPT_U_NBPHONE,		m_pUserInfo->szNbPhone.c_str());
		pSysOpt->SetOpt(OPT_U_NBMOBILE,		m_pUserInfo->szNbMobile.c_str());
		pSysOpt->SetOpt(OPT_U_NBTITLE,		m_pUserInfo->szNbTitle.c_str());
		pSysOpt->SetOpt(OPT_U_NBSUBTITLE,	m_pUserInfo->szNbSubTitle.c_str());
		pSysOpt->SetOpt(OPT_U_NBTITLEURL,	m_pUserInfo->szNbTitleUrl.c_str());
		pSysOpt->SetOpt(OPT_U_CONSHOWDIALOG,(long)m_pUserInfo->dwConShowDialog);
		pSysOpt->SetOpt(OPT_U_CONSHOWCANCEL,(long)m_pUserInfo->dwConShowCancel);
		pSysOpt->SetOpt(OPT_U_CONMESSAGE,	m_pUserInfo->szConMessage.c_str());
		pSysOpt->SetOpt(OPT_U_CONOKURL,		m_pUserInfo->szConOkUrl.c_str());
		pSysOpt->SetOpt(OPT_U_USERTYPE,		m_pUserInfo->szUserType.c_str());
		pSysOpt->SetOpt(OPT_U_MESSAGE,		m_pUserInfo->szMessage.c_str());
		pSysOpt->SetOpt(OPT_U_PLUGINFO,		m_pUserInfo->szPlugParam.c_str());
		pSysOpt->SetOpt(OPT_U_TASKINFO,		m_pUserInfo->szTaskInfo.c_str());
		pSysOpt->SetOpt(OPT_M_GGONOFF,		m_pUserInfo->szMenuGGao.c_str());
		pSysOpt->SetOpt(OPT_M_RPOPONOFF,	m_pUserInfo->szMenuPop.c_str());
		pSysOpt->SetOpt(OPT_M_BOTLAYER,		m_pUserInfo->szBotLayer.c_str());
		pSysOpt->SetOpt(OPT_M_MEMUPAGE,		m_pUserInfo->szMenuPage.c_str());
		pSysOpt->SetOpt(OPT_M_DOTCARD,		m_pUserInfo->szDotCard.c_str());
		pSysOpt->SetOpt(OPT_M_URLNAV,		m_pUserInfo->szUrlNav.c_str());
		pSysOpt->SetOpt(OPT_M_VDMUS,		m_pUserInfo->szVdMus.c_str());
		pSysOpt->SetOpt(OPT_M_PERSONURL,	m_pUserInfo->szPersonUrl.c_str());
		pSysOpt->SetOpt(OPT_M_GAMEROOMURL,	m_pUserInfo->szGameRoomUrl.c_str());

		TCHAR buf[MAX_PATH] = {0};
		_stprintf(buf, TEXT("%04d-%02d-%02d"), m_pUserInfo->dwYear, m_pUserInfo->dwMonth, m_pUserInfo->dwDay);
		pSysOpt->SetOpt(OPT_U_DATEEND, buf);

		pSysOpt->SetOpt(OPT_D_ADSLSPEED,	(long)m_pUserInfo->uAdslSpeed);
	}

	void CLogRptPlug::GetAvdInfo(IPlug* pPlug)
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));
		_bstr_t url = TEXT("http://advs.i8cn.com/AppreciationStrategy.koc");
		try
		{
			http->Open(TEXT("POST"), url, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("close"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
			
			stringstream out;
			out << "id=" << m_pUserInfo->dwNibID;
			string strSend = out.str();

			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				return;

			http->Send(_bstr_t(strSend.c_str()));
			if (200 != http->Status)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Get AdvInfo Error: %d."), http->Status);
				_com_raise_error(0);
			}
			else
			{
				_bstr_t bstrXml = http->ResponseText;
				CMarkup xml;
				if (!xml.SetDoc(bstrXml))
					m_pLogger->WriteLog(LM_INFO, TEXT("Get AdvInfo Reponse xml format error."));
				else
				{
					xml.ResetPos();
					bool bSucess = false;
					tstring strError;
					if (xml.FindElem("Appreciation") && xml.IntoElem() && xml.FindElem("ReturnValue") && xml.IntoElem())
					{
						if (xml.FindElem("Code") && xml.GetData() == tstring(TEXT("000")))
							bSucess = true;
						if (xml.FindElem("Message"))
							strError = xml.GetData();
					}
					if (!bSucess)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("get advinfo error:%s"), strError.c_str());
					}
					else
					{
						xml.ResetPos();
						if (xml.FindElem("Appreciation") && xml.IntoElem() && xml.FindElem("AppreciationList") && xml.IntoElem())
						{
							while (xml.FindElem("Item"))
							{
								if (xml.IntoElem() && xml.FindElem("ID"))
								{
									tstring AdvID = xml.GetData();
									tstring p1, p2;
									if (xml.FindElem("ParameterOne"))
										p1 = xml.GetData();
									if (xml.FindElem("ParameterTwo"))
										p2 = xml.GetData();

									TCHAR buf[100] = {0};
									_stprintf(buf, "p1$%s,p2$%s", p1.c_str(), p2.c_str());
									if (AdvID == "1004")		//菜单公告控制
										m_pUserInfo->szMenuGGao = buf;
									else if (AdvID == "1005")	//菜单右下解弹出广告
										m_pUserInfo->szMenuPop = buf;
									else if (AdvID == "1006")	//菜单启动默认页
										m_pUserInfo->szMenuPage = buf;
									else if (AdvID == "1007")	//菜单底部浮动广告
										m_pUserInfo->szBotLayer = buf;
									else if (AdvID == "1008")	//上网浏览网址列表
									{
										m_pUserInfo->szUrlNav.clear();
										if (p1 == "1" && xml.FindElem("ParameterOther") && xml.IntoElem())
										{
											while (xml.FindElem("Item"))
											{
												m_pUserInfo->szUrlNav += xml.GetData();
												m_pUserInfo->szUrlNav += "^";
											}
											if (m_pUserInfo->szUrlNav.size())
												m_pUserInfo->szUrlNav.resize(m_pUserInfo->szUrlNav.size()-1);
											xml.OutOfElem();
										}
									}
									else if (AdvID == "1009")	//在线商城网址
									{
										m_pUserInfo->szDotCard.clear();
										if (p1 == "1" && xml.FindElem("ParameterOther") && xml.IntoElem())
										{
											if (xml.FindElem("Item"))
											{
												m_pUserInfo->szDotCard = xml.GetData();
											}
											xml.OutOfElem();													
										}
									}
									else if (AdvID == "1010")	//在线影音网址
									{
										m_pUserInfo->szVdMus.clear();
										if (p1 == "1" && xml.FindElem("ParameterOther") && xml.IntoElem())
										{
											while (xml.FindElem("Item"))
											{
												m_pUserInfo->szVdMus += xml.GetData();
												m_pUserInfo->szVdMus += "^";
											}
											if (m_pUserInfo->szVdMus.size())
												m_pUserInfo->szVdMus.resize(m_pUserInfo->szVdMus.size()-1);
											xml.OutOfElem();
										}
									}
									else if (AdvID == "1012")	//一点通个人用户中心网址
									{
										m_pUserInfo->szPersonUrl.clear();
										if (p1 == "1" && xml.FindElem("ParameterOther") && xml.IntoElem())
										{
											if (xml.FindElem("Item"))
											{
												tstring str = xml.GetData();
												int npos = str.find('|');
												if (npos != str.npos)
													m_pUserInfo->szPersonUrl = str.substr(npos+1);
											}
										}
									}
									else if (AdvID == "1013")	//一点通游戏大厅后置网址
									{
										m_pUserInfo->szGameRoomUrl.clear();
										if (p1 == "1" && xml.FindElem("ParameterOther") && xml.IntoElem())
										{
											while (xml.FindElem("Item"))
											{
												m_pUserInfo->szGameRoomUrl += xml.GetData();
												m_pUserInfo->szGameRoomUrl += "^";
											}
											if (m_pUserInfo->szGameRoomUrl.size())
												m_pUserInfo->szGameRoomUrl.resize(m_pUserInfo->szGameRoomUrl.size()-1);
											xml.OutOfElem();
										}
									}									
								}
								xml.OutOfElem();
							}
						}
						m_pLogger->WriteLog(LM_INFO, TEXT("Get AdvInfo Sucess."));
					}
				}
			}
		}
		catch (_com_error& e)
		{
			if (e.Error() != 0)
				m_pLogger->WriteLog(LM_INFO, TEXT("GetAvd Info Error:%s"), (LPCTSTR)e.Description());
		}
		catch (...) {	m_pLogger->WriteLog(LM_INFO, TEXT("GetAvd Info Unknown Error."));}
	}

	/*void CLogRptPlug::GetTask(IPlug* pPlug)
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));
		_bstr_t url = GetOpt(TEXT("3up"), TEXT("gettask.domainurl"), 
			TEXT("http://3up.i8desk.com:2821/gettask/gettask.do")).c_str();

		TCHAR param[MAX_PATH] = {0};
		_stprintf(param, TEXT("?nid=%d&3upver=%s&sver=%s"), 
			m_pUserInfo->dwNibID, 
			m_pUserInfo->szSrvVersion.c_str(), 
			GetFileVersion(TEXT("I8DeskSvr.exe")).c_str());
_start:
		try
		{
			_bstr_t str = url + param;
			http->Open(TEXT("GET"), str, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("close"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));

			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				return;

			http->Send(TEXT(""));
			DWORD dwStatus = http->Status;
			if (dwStatus != 200)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("GetTask Error: %d."), dwStatus);
				_com_raise_error(0);
			}
			else
			{
				tstring str = http->ResponseText;
				tstring::size_type pos1 = str.find(TEXT("flag=1"));
				tstring::size_type pos2 = str.find(TEXT(";;&rtnlen"));
				if (pos1 == tstring::npos || pos2 == tstring::npos)
					_com_raise_error(0);

				str = str.substr(pos1+7, (pos2-pos1-7) ? 0 : (pos2-pos1-7));
				m_pUserInfo->szTaskInfo = str;
				m_pLogger->WriteLog(LM_INFO, TEXT("GetTask Success:[%s]"), str.c_str());
				return ;
			}
		}
		catch (_com_error& e)
		{
			if (e.Error() != 0)
				m_pLogger->WriteLog(LM_INFO, TEXT("GetTask Error:%s"), (LPCTSTR)e.Description());
			_bstr_t newurl = GetOpt(TEXT("3UP"), TEXT("gettask.ipurl"), 
				TEXT("http://118.102.24.149:2821/gettask/gettask.do")).c_str();
			if (lstrcmpi(newurl, (LPCTSTR)url) != 0)
			{
				url = newurl;
				goto _start;
			}
		}
		catch (...) {	m_pLogger->WriteLog(LM_INFO, TEXT("GetTask Unknown Error."));}
	}
	*/
	struct  CClientFilter : public IClientFilter
	{
	private:
		PDWORD m_pdwData;
		DWORD  m_dwCount;
	public:
		CClientFilter(PDWORD pdwData, DWORD dwOnlineCount)
		{
			m_pdwData = pdwData;
			m_dwCount = dwOnlineCount;
		}
		
		virtual bool bypass(const db::tClient *const c)
		{
			return false;
			if (0 == IsOnline(c->IP))
				return true;
			return false;
		}

		DWORD IsOnline(DWORD ip)
		{
			if (m_pdwData != NULL && m_dwCount > 0 && std::find(m_pdwData, m_pdwData+m_dwCount, ip) != (m_pdwData + m_dwCount))
				return 1;
			return 0;
		}
	};

	DWORD IsSystemType()
	{
		char buf[10] = {0};
		DWORD dwType = 0;
		DWORD dwSize = sizeof(buf) - 1;
		SHGetValue(HKEY_LOCAL_MACHINE, "Software\\Goyoo\\i8desk", "Entironment", &dwType, buf, &dwSize);
		return atoi(buf) == 1 ? 1 : 0;
	}

	DWORD CheckProcess(LPCTSTR lpszProcName)
	{
		DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		char path[MAX_PATH] = {0};
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return	0;
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			i8desk::CAutoHandle hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
			if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
			{
				GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
				if (lstrcmpi(path, lpszProcName) == 0)
				{
					return 1;
				}
			}
		}
		return 0;
	}

	_bstr_t CLogRptPlug::GetDevInfoItem(IWbemServices* pService, LPCTSTR lpszQuery, LPCTSTR lpszField)
	{
		CComVariant variant("");
		IEnumWbemClassObject *pEnumObject  = NULL;
		if (S_OK == pService->ExecQuery(CComBSTR("WQL"), CComBSTR(lpszQuery), WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumObject))
		{
			ULONG dwCount = 1, dwRetSize;
			IWbemClassObject *pClassObject = NULL;
			if(S_OK == pEnumObject->Reset())
			{
				if (S_OK == pEnumObject->Next(5000, dwCount, &pClassObject, &dwRetSize))
				{
					pClassObject->Get(CComBSTR(lpszField),  0, &variant , NULL , 0);
					pClassObject->Release();
				}
				else
				{
					m_pLogger->WriteLog(LM_INFO, "wql error:%s", lpszQuery);
				}
			}
			pEnumObject->Release();
		}

		return _bstr_t(variant);
	}

	tstring handleStr(LPCTSTR src)
	{
		tstring dst = src;
		
		for (size_t idx=0; idx<dst.size(); idx++)
		{
			if (dst[idx] == ',' || dst[idx] == '|')
			{
				dst[idx] = ' ';
			}
		}
		return dst;
	}

	inline tstring handleStr(tstring &str)
	{
		// MS tr1::bind源码BUG，不支持内置类型引用

		/*replace_if(str.begin(), str.end(), 
			tr1::bind(
			logical_or<bool>(), 
			tr1::bind(equal_to<char>(), cref(_1), '|'), 
			tr1::bind(equal_to<char>(), cref(_1), ',')), ' ');*/

		std::replace_if(str.begin(), str.end(),
			stdex::Compose2(
				std::logical_or<bool>(), 
				std::bind2nd(std::equal_to<tstring::value_type>(), '|'), 
				std::bind2nd(std::equal_to<tstring::value_type>(), ',')),
				' ');

		return str;
	}

	tstring CLogRptPlug::GetDeviceInfo()
	{
		HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, 
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

		tstring sDevInfo;
		IWbemLocator  *pIWbemLocator = NULL;
		IWbemServices *pWbemServices = NULL;
		if (S_OK != CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown ,(void**)&pIWbemLocator))
			return sDevInfo;

		if (S_OK != pIWbemLocator->ConnectServer(CComBSTR("root\\cimv2"),  NULL, NULL, NULL, 0, NULL, NULL, &pWbemServices))
		{
			pIWbemLocator->Release();
			return sDevInfo;
		}
		hr = CoSetProxyBlanket(pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

		
		string freespace;
		{
			stringstream stream;
			char buf[] = "X:\\";
			for (char idx='C'; idx<='Z'; idx++)
			{
				buf[0] = idx;
				if (GetDriveType(buf) == DRIVE_FIXED)
				{
					ULARGE_INTEGER ui = {0};
					ULARGE_INTEGER ui2 = {0};
					GetDiskFreeSpaceEx(buf, &ui2, &ui, NULL);
					char sSize[10] = {0};
					char sSize2[10] = {0};
					sprintf(sSize, "%.1f", ui.QuadPart / 1000.0 / 1000 / 1000);
					sprintf(sSize2, "%.1f", ui2.QuadPart / 1000.0 / 1000 / 1000);
					stream << idx << ":" << sSize << "*" << sSize2 << "-";
				}
			}
			freespace = stream.str();
			if (freespace.size())
				freespace.resize(freespace.size()-1);
		}

		try
		{
			string cpu		 = GetDevInfoItem(pWbemServices, "Select * from Win32_processor", "Name");
			string memory	 = GetDevInfoItem(pWbemServices, "Select * from Win32_LogicalMemoryConfiguration", "TotalPhysicalMemory");
			string disk		 = GetDevInfoItem(pWbemServices, "Select * from Win32_DiskDrive", "Model");		 
			string graphics	 = GetDevInfoItem(pWbemServices, "Select * from Win32_VideoController", "Name");
			string mainboard = GetDevInfoItem(pWbemServices, "Select * from Win32_BaseBoard", "Product");
			string nic		 = m_pUserInfo->szNic;
			string webcam	 = "";
			string sysver    = GetDevInfoItem(pWbemServices, "Select * from Win32_OperatingSystem", "Version");
		
			stringstream out;
			out << handleStr(cpu) <<", " << _atoi64(memory.c_str()) / 1000 /1000 <<", " << handleStr(disk) <<", " 
				<< handleStr(freespace)  <<", " << handleStr(graphics) <<", " << handleStr(mainboard) <<", " 
				<< handleStr(nic) <<", " << handleStr(webcam) <<", " << handleStr(sysver);

			pWbemServices->Release();
			pIWbemLocator->Release();
			sDevInfo = out.str();
		}
		catch(...)
		{}


		return sDevInfo;
	}

	tstring CLogRptPlug::GetOtherSvrDeviceInfo()
	{
		IPlug* pRTPlug = m_pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
		IRTDataSvr* pRTDatabase = (pRTPlug == NULL) ? NULL :
			reinterpret_cast<IRTDataSvr*>(pRTPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));

		if (pRTDatabase == NULL)
			return tstring("");

		IServerStatusRecordset *pRs = NULL;
		pRTDatabase->GetServerStatusTable()->Select(&pRs, NULL, NULL);
		if (pRs == NULL)
			return tstring("");

		TCHAR szCmpName[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szCmpName)-1;
		GetComputerName(szCmpName, &dwSize);
		tstring str;
		for (DWORD idx=0; idx<pRs->GetCount(); idx++)
		{
			db::tServerStatus* pST = pRs->GetData(idx);
			if (lstrcmpi(pST->HostName, szCmpName) != 0)
			{
				str += uri_encode(pST->DeviceInfo);
				str += "|";
			}
		}
		if (str.size())
			str.resize(str.size() - 1);
		pRs->Release();
		return str;
	}

	struct CFilter : IModuleUsageFilter
	{
		virtual bool bypass(const db::tModuleUsage *const mu)
		{
			return false;
		}
	};
	void CLogRptPlug::ReportCliInfo(IPlug* pPlug)
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));
		_bstr_t url = TEXT("http://desk.cgi.i8cn.com/Cybercafe/Server/ClientReport.koc");
		try
		{
			http->Open(_bstr_t("POST"), url, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("close"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));

			SYSTEMTIME st;
			GetLocalTime(&st);
			TCHAR buf[MAX_PATH] = {0};
			_stprintf(buf, TEXT("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			DWORD dwCount = 0;
			//generator header info.
			tstring header, clist, nav, sign;
			{
				stringstream out;
				out << "nid="			<< m_pUserInfo->dwNibID;
				out << "&sid="			<< m_pUserInfo->dwSID;
				out << "&snowdt="		<< uri_encode(buf);
				out << "&entironment="	<< IsSystemType();
				out << "&virtualdisk="	<< CheckProcess(TEXT("I8VDiskSvr.exe"));
				out << "&threeupdate="	<< CheckProcess(TEXT("DNAService.exe"));
				out << "&sdevice="		<< uri_encode(GetDeviceInfo().c_str());
				out << "&ssdevice="		<< GetOtherSvrDeviceInfo();
				header = out.str();
			}
			IPlug* pRTPlug = m_pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
			IRTDataSvr* pRTDatabase = (pRTPlug == NULL) ? NULL :
				reinterpret_cast<IRTDataSvr*>(pRTPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));

			//generator list info.
			{
				stringstream out;
				PDWORD  pdwOnlineData = NULL;
				DWORD   dwOnlineCount = 0;
				if (pPlug != NULL)
				{
					DWORD dwValue = pPlug->SendMessage(RTDS_CMD_GET_CLIENTLIST, reinterpret_cast<DWORD>(&pdwOnlineData), 
						reinterpret_cast<DWORD>(&dwOnlineCount)); 
				}
				CClientFilter filter(pdwOnlineData, dwOnlineCount);
				IClientRecordset *pRs = NULL;
				if (pRTDatabase != NULL && 0 == pRTDatabase->GetClientTable()->Select(&pRs, &filter, NULL))
				{
					dwCount = pRs->GetCount();
				}
				
				out << "&ccount=" << dwCount;
				DWORD nLoopAppend = (dwCount +5)/ 5;
				DWORD nIndex = 0;
				for (DWORD nLoop=0; nLoop<5; nLoop++)
				{
					out << "&clist" << nLoop+1 << "=";
					for (DWORD idx=0; idx<nLoopAppend && nIndex < dwCount; idx++)
					{
						db::tClient* pClient = pRs->GetData(nIndex);

						DWORD dwRun = 0;
						db::tModuleUsage mu = {0};
						if (0 == pRTDatabase->GetModuleUsageTable()->GetData(pClient->MAC, -1, &mu))//菜单ID为-1.
							dwRun = mu.ClickCount;

						out << uri_encode(pClient->MAC)							<< ","
							<< dwRun											<< ","
							<< filter.IsOnline(pClient->IP)						<< ","
							<< uri_encode(pClient->CliSvrVer)					<< ","
							<< uri_encode(pClient->ProtVer)						<< ","
							<< uri_encode(pClient->ProtArea)					<< ","
							<< pClient->GPFreeSize								<< ","
							<< pClient->SCStatus								<< ","
							<< uri_encode(handleStr(pClient->CPU).c_str())		<< ","
							<< uri_encode(handleStr(pClient->Memory).c_str())	<< ","
							<< uri_encode(handleStr(pClient->Disk).c_str())		<< ","
							<< uri_encode(handleStr(pClient->Graphics).c_str())	<< ","
							<< uri_encode(handleStr(pClient->Mainboard).c_str())<< ","
							<< uri_encode(handleStr(pClient->NIC).c_str())		<< ","
							<< uri_encode(handleStr(pClient->Webcam).c_str())	<< ","
							<< uri_encode(handleStr(pClient->System).c_str());
						if (nIndex < dwCount -1)
							out << "|";

						nIndex++;
					}
				}
				Release_Interface(pRs);
				clist = out.str();
			}
			//generator nav info.
			{
				DWORD dwClick1 = 0, dwClick2 = 0, dwClick3 = 0, dwClick4 = 0, dwClick5 =0, dwClick6 = 0;
				DWORD dwTime1 = 0, dwTime2 = 0, dwTime3 = 0, dwTime4 = 0, dwTime5 =0, dwTime6 = 0;

				IModuleUsageRecordset* pRs = NULL;
				if (pRTDatabase != NULL && 0 == pRTDatabase->GetModuleUsageTable()->Select(&pRs, (IModuleUsageFilter*)NULL, NULL))
				{
					//删除所有的临时记录
					CFilter filter;
					db::tModuleUsage mu = {0};
					pRTDatabase->GetModuleUsageTable()->Delete(&filter);

					//统计
					DWORD dwSize = pRs->GetCount();		
					for (DWORD idx=0; idx<dwSize; idx++)
					{
						db::tModuleUsage* pMu = pRs->GetData(idx);
						switch (pMu->MID)
						{
						case 1:
							{
								dwClick1 += pMu->ClickCount;
								dwTime1  += pMu->UsedTime;
							}
							break;
						case 2:
							{
								dwClick2 += pMu->ClickCount;
								dwTime2  += pMu->UsedTime;
							}
							break;
						case 3:
							{
								dwClick3 += pMu->ClickCount;
								dwTime3  += pMu->UsedTime;
							}
							break;
						case 4:
							{
								dwClick4 += pMu->ClickCount;
								dwTime4  += pMu->UsedTime;
							}
							break;
						case 5:
							{
								dwClick5 += pMu->ClickCount;
								dwTime5  += pMu->UsedTime;
							}
							break;
						case 6:
							{
								dwClick6 += pMu->ClickCount;
								dwTime6  += pMu->UsedTime;
							}
							break;
						}
					}
					Release_Interface(pRs);
				}
				//输出字符串。
				stringstream out;
				out << "&nav=1," << dwClick1 << "," << dwTime1 << "|"
					<<"2," << dwClick2 << "," << dwTime2 << "|"
					<<"3," << dwClick3 << "," << dwTime3 << "|"
					<<"4," << dwClick4 << "," << dwTime4 << "|"
					<<"5," << dwClick5 << "," << dwTime5 << "|"
					<<"6," << dwClick6 << "," << dwTime6 ;
				nav = out.str();
			}

			//append sign info.
			{
				stringstream out;
				out << "nid=" <<m_pUserInfo->dwNibID << "&sid=" << m_pUserInfo->dwSID  << "&ccount=" << dwCount
					<< "&snowdt=" << buf << "|||i8desk|||smac=" << m_pUserInfo->szSrvMac;
				sign = out.str();
				sign = MDString(sign.c_str());
				sign.insert(0, "&sign=");
			}

			//send data.
			http->Send(_bstr_t((header + clist + nav + sign).c_str()));
			DWORD dwStatus = http->Status;
			_bstr_t bstrResponse = http->ResponseText;
			if (dwStatus == 200 && _strcmpi("flag=1", (LPCSTR)bstrResponse) == 0)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Report CliInfo Success."));
				return ;
			}
			else
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Report CliInfo Error: %d, %s"), dwStatus, (LPCSTR)bstrResponse);
				_com_raise_error(0);
			}
		}
		catch (_com_error& e)
		{
			if (e.Error() != 0)
				m_pLogger->WriteLog(LM_INFO, TEXT("CliInfo Error:%s"), (LPCTSTR)e.Description());
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("Report CliInfo Unknown Error.")); }
	}

	struct CGameFilter : public IGameFilter
	{
		bool bypass(const db::tGame *const g)
		{
			if (g->Status != 1)
				return true;
			return false;
		}
	};

	void CLogRptPlug::ReportGameClick(IPlug* pPlug)
	{
		wstring wboundary(L"---------------------------7d83e12904aa");
		string   boundary( "---------------------------7d83e12904aa");
		_bstr_t strUrl = TEXT("http://desk.cgi.i8cn.com/Cybercafe/Server/GameReport.koc");
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
 
		SYSTEMTIME st;
		GetLocalTime(&st);
		TCHAR tbuf[MAX_PATH] = {0};
		_stprintf(tbuf, TEXT("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		char* pData				= NULL;	//总共需要发送的数据缓存。
		HZIP  hZip				= NULL;	
		char *pBinaryData		= NULL;	//存放二进制数据。
		DWORD dwBinaryLength	= 0;	//二进制数据的长度。
		try
		{
			wchar_t url[MAX_PATH] = {0};
			wchar_t host[MAX_PATH] = {0};
			wchar_t req [MAX_PATH] = {0};
			WORD    port = 80;
			URL_COMPONENTS urlComp;
			ZeroMemory(&urlComp, sizeof(urlComp));
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.lpszHostName = host;
			urlComp.dwHostNameLength  = MAX_PATH;
			urlComp.lpszUrlPath = req;
			urlComp.dwUrlPathLength   = MAX_PATH;
			WinHttpCrackUrl(strUrl, 0, 0, &urlComp);
			port = urlComp.nPort;

			//打开http连接
			if ((hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)) == NULL)
				throw GetLastError();
			if ((hConnect = WinHttpConnect( hSession, host, port, 0)) == NULL)
				throw GetLastError();
			if ((hRequest = WinHttpOpenRequest( hConnect, L"POST", req, NULL, WINHTTP_NO_REFERER, 
				WINHTTP_DEFAULT_ACCEPT_TYPES, 0)) == NULL)
				throw GetLastError();

			//设置选项头，说明上传的数据类型是：multipart/form-data
			{
				wstringstream stream;
				stream << L"Content-Type: multipart/form-data; boundary=" << wboundary;
				wstring header = stream.str();
				WinHttpAddRequestHeaders(hRequest, header.c_str(), header.size(), WINHTTP_ADDREQ_FLAG_ADD);
			}

			string start;
			stringstream stream;

			//生成表单参数(name=info.)
			stream << "--" << boundary << "\r\n";
			stream << "Content-Disposition: form-data; name=\"info\"\r\n\r\n";
			{
				string sign;
				{
					stringstream out;
					out << "nid=" <<m_pUserInfo->dwNibID << "&sid=" << m_pUserInfo->dwSID << "&snowdt=" << tbuf << "|||i8desk|||smac=" << m_pUserInfo->szSrvMac;
					sign = out.str();
					sign = MDString(sign.c_str());
				}
				{
					stringstream out;
					out << "nid="		<< m_pUserInfo->dwNibID;
					out << "&sid="		<< m_pUserInfo->dwSID;
					out << "&snowdt="	<< (LPCSTR)_bstr_t(tbuf);
					out << "&sign="		<< sign;
					stream << "\"" << out.str() << "\"" << "\r\n";
				}
			}
			
			//写二进制数据
			stream << "--" << boundary << "\r\n";
			stream << "Content-Disposition: form-data; name=\"zipfile\"; " << "filename=\"c:\\zip.dat\"\r\n";
			stream << "Content-Type: application/x-zip-compressed\r\n\r\n";
			start = stream.str();

			//生成二进制压缩数据。
			if (pData == NULL)
			{
				stream.str("");
				IPlug* pRTPlug = m_pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
				IRTDataSvr* pRTDatabase = (pRTPlug == NULL) ? NULL :
					reinterpret_cast<IRTDataSvr*>(pRTPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));

				CGameFilter filter;
				IGameRecordset *pRs = NULL;

				IGameTable*		pGameTable     = (pRTDatabase == NULL) ? NULL : pRTDatabase->GetGameTable();
				IGameAreaTable* pGameAreaTable = (pRTDatabase == NULL) ? NULL : pRTDatabase->GetGameAreaTable();
				IVDiskTable*    pVDiskTable    = (pRTDatabase == NULL) ? NULL : pRTDatabase->GetVDiskTable();
				IVDiskRecordset* pVDRs = NULL;

				if (pGameTable == NULL || pGameAreaTable == NULL || pVDiskTable == NULL ||
					0 != pVDiskTable->Select(&pVDRs, NULL, NULL) || 0 != pGameTable->Select(&pRs, &filter, NULL))
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("Report GameClick Error:Get data table error."));
					throw 0;
				}

				{
					DWORD dwCount = pRs->GetCount();
					for (DWORD idx=0; idx<dwCount; idx++)
					{
						db::tGame* pGame = pRs->GetData(idx);

						std::stringstream out;
						IGameAreaRecordset* pRTRs = NULL;
						if (0== pGameAreaTable->Select(&pRTRs, pGame->GID, NULL))
						{
							DWORD dwSize = pRTRs->GetCount();
							for (DWORD nLoop=0; nLoop<dwSize; nLoop++)
							{
								db::tGameArea *pGameArea = pRTRs->GetData(nLoop);
								out << pGameArea->AID << "_";
								if (pGameArea->RunType != 2)
									out << pGameArea->RunType;
								else 	//虚拟盘运行
								{
									DWORD dwVDCount = pVDRs->GetCount();
									for (DWORD k=0; k<dwVDCount; k++)
									{
										db::tVDisk* pVDisk = pVDRs->GetData(k);
										if (stricmp(pVDisk->VID, pGameArea->VID) == 0)
										{
											if (pVDisk->Type == 0)	//i8desk virtual disk.
												out << "4" << pGameArea->VID;
											else
												out << "5"	<< pGameArea->VID;
										}
										break;
									}
								}

								if (nLoop < dwSize - 1) out << "+" ;
							}
							pRTRs->Release();
						}

						stream << pGame->GID		<< ","	<< pGame->Name		<< "," 
							   << out.str()			<< ","	<< pGame->SvrClick2 << ","
							   << pGame->CliPath	<<","	<< pGame->SvrPath	<< ","
							   << pGame->Priority   << "|";

						pGame->SvrClick2 = 0;
						pGameTable->Update(pGame->GID, pGame, MASK_TGAME_SVRCLICK2);
					}
					Release_Interface(pRs);
				}
				string Content = stream.str();
				if (Content.size() == 0)
				{
					dwBinaryLength = 0;
					m_pLogger->WriteLog(LM_INFO, "Report GameClick error:game count is 0");
				}
				else
				{
					Content.resize(Content.size()-1);
					//压缩数据。
					hZip = CreateZip(0, 1024 * 1024 * 5, ZIP_MEMORY);					//创建一个5M内存zip文件。
					ZipAdd(hZip, "zip.dat", (void*)Content.c_str(), Content.size(), ZIP_MEMORY);	//压缩内存文件.
					ZipGetMemory(hZip, (void**)&pBinaryData, &dwBinaryLength);
					//for debug. output zip to C:\1.zip.
					/*HANDLE hFile = CreateFile(TEXT("C:\\1.zip"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						DWORD dwWriteBytes = 0;
						WriteFile(hFile, pBinaryData, dwBinaryLength, &dwWriteBytes, NULL);
						CloseHandle(hFile);
					}*/
				}
			}
			if (dwBinaryLength)
			{
				//write end data.
				string end;
				{
					stream.str("");		
					stream << "\r\n--" << boundary << "--\r\n" ;
					end = stream.str();
				}

				//发送并接收数据。
				DWORD dwBytes = 0;
				DWORD dwTotal = start.size() + end.size() + dwBinaryLength;
				pData = new char[dwTotal + 10];
				CopyMemory(pData, start.c_str(), start.size());
				CopyMemory(&pData[start.size()], pBinaryData, dwBinaryLength);
				CopyMemory(&pData[start.size() + dwBinaryLength], end.c_str(), end.size());			

				if (!WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
					WINHTTP_NO_REQUEST_DATA, 0, dwTotal, 0))
					throw GetLastError();
				if (!WinHttpWriteData( hRequest, pData, dwTotal, &dwBytes))
					throw GetLastError();
				if (!WinHttpReceiveResponse( hRequest, NULL))
					throw GetLastError();

				char szResponse[1024] = {0};
				if (!WinHttpReadData(hRequest, szResponse, sizeof(szResponse)-1, &dwBytes))
					throw GetLastError();
				szResponse[dwBytes] = 0;
				if (_strcmpi(szResponse, "flag=1") != 0)
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("Report GameClick Result:%s."), (LPCTSTR)_bstr_t(szResponse));
					throw (DWORD)0;
				}
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("Report GameClick Success."));
		}
		catch (DWORD err)
		{
			if (err != 0)
			{
				_com_error Error(err);
				m_pLogger->WriteLog(LM_INFO, TEXT("Report GameClick Error:%d:%s"), err, Error.ErrorMessage());
			}
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("Report GameClick Unknown Error.")); }
		if (hRequest)	WinHttpCloseHandle(hRequest);
		if (hConnect)	WinHttpCloseHandle(hConnect);
		if (hSession)	WinHttpCloseHandle(hSession);
		if (hZip)		CloseZip(hZip);
		if (pData)		delete[]pData;
	}

	int AppendLogDir(tstring& dir, tstring filter, HZIP hZip)
	{
		int nFiles = 0;
		tstring src = dir;
		if (src.size() && *src.rbegin() != '\\')
			src += "\\";

		if (hZip == 0)	return nFiles;

		WIN32_FIND_DATA wfd = {0};
		i8desk::CAutoFindFile hFinder = FindFirstFile((src + filter).c_str(), &wfd);
		if (hFinder.IsValid())
			return nFiles;

		while (FindNextFile(hFinder, &wfd))
		{
			if ((wfd.nFileSizeLow < 2 * 1024 * 1024 && wfd.nFileSizeHigh == 0) &&  //文件大小小于2M的文件才上传
				!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				tstring file = src + wfd.cFileName;
				if (ZR_OK == ZipAdd(hZip, wfd.cFileName, (void*)file.c_str(), file.size(), ZIP_FILENAME))
				{
					DeleteFile(file.c_str());
					nFiles++;
				}
			}
		}

		return nFiles;
	}

	void CLogRptPlug::UpLoadLog()
	{
		if (!m_pUserInfo->dwUploadLog)
			return;

		//产生日志文件压缩包
		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		TCHAR szZipFile[MAX_PATH] = {0};
		_stprintf(szZipFile, "%sLog\\Log-%02d%02d%02d%02d%02d%02d-%d.zip", 
			GetAppPath().c_str(), 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
			m_pUserInfo->dwNibID);
		DeleteFile(szZipFile);
		HZIP hZip = CreateZip((void*)szZipFile, 0, ZIP_FILENAME);
		if (hZip == 0)
			return ;
		int nFiles = AppendLogDir(GetAppPath() + TEXT("Log\\"), TEXT("*.log"), hZip);
		nFiles += AppendLogDir(GetAppPath() + TEXT("Dump\\"), TEXT("*.dmp"), hZip);
		CloseZip(hZip);

		//上传日志文件
		if (nFiles)
			i8desk_ftpWriteFtp(szZipFile, m_hExited);
		else
			DeleteFile(szZipFile);
	}

	void DeleteMoreLogFileImp(tstring& dir, tstring filter, HANDLE m_hExit)
	{
		tstring src = dir;
		if (src.size() && *src.rbegin() != '\\')
			src += "\\";
		
		FILETIME  ftBefore7Day = {0};
		GetSystemTimeAsFileTime(&ftBefore7Day);
		__int64* p = (__int64*)&ftBefore7Day;
		*p -= (7LL * 24 * 60 * 60 * 10000000);

		WIN32_FIND_DATA wfd = {0};
		i8desk::CAutoFindFile hFinder = FindFirstFile((src + filter).c_str(), &wfd);
		if ( hFinder.IsValid() )
			return ;

		while (FindNextFile(hFinder, &wfd) && WAIT_OBJECT_0 != WaitForSingleObject(m_hExit, 0))
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				CompareFileTime(&wfd.ftLastWriteTime, &ftBefore7Day) == -1)
			{
				tstring file = src + wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(file.c_str());
			}
		}
	}

	void CLogRptPlug::DeleteMoreLogFile(HANDLE m_hExit)
	{
		TCHAR szLogDir[MAX_PATH] = {0};
		_stprintf(szLogDir, "%sLog\\", GetAppPath().c_str());

		DeleteMoreLogFileImp(tstring(szLogDir), tstring("*.log"), m_hExit);
		DeleteMoreLogFileImp(tstring(szLogDir), tstring("*.zip"), m_hExit);
	}

	IPlug* WINAPI CreatePlug()
	{
		IPlug* pPlug = NULL;
		try
		{
			pPlug = new CLogRptPlug;
		}
		catch (...) { pPlug = NULL; }
		
		return pPlug;
	}
}