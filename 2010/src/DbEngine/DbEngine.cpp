#include "stdafx.h"
#include <tchar.h>
#include <Shlwapi.h>
#include <ComDef.h>
#include <WinSock2.h>
#include <time.h>
#include <sstream>
#include <iostream>

#include "../../include/plug.h"


#import "C:\PROGRAM FILES\COMMON FILES\System\ado\MSJRO.DLL" no_namespace

#pragma warning(disable:4996)
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define ERR_OPENDATABASE	TEXT("数据库连接己经断开")

namespace i8desk
{
	class CDatabase : public IDatabase 
	{
	private:
		_ConnectionPtr m_pConn;
		CLock m_lock;
		TCHAR m_szErrInfo[4096];
		long m_hErrCode;
	public:
		CDatabase()
		{
			CoInitialize(NULL);
			m_pConn.CreateInstance(__uuidof(Connection));
			ZeroMemory(m_szErrInfo, sizeof(m_szErrInfo));
			m_hErrCode = 0;
		}
		virtual ~CDatabase()
		{
			Close();
			m_pConn.Release();
			CoUninitialize();
		}

		virtual void Release()	{ delete this; }

		virtual bool Open(LPCTSTR lpszConnString, LPCTSTR lpszUser, LPCTSTR lpszPassword)
		{
			m_szErrInfo[0] = 0;
			m_hErrCode = 0;
			try
			{
				m_pConn->CursorLocation = adUseClient;
				m_pConn->Open(_bstr_t(lpszConnString), _bstr_t(lpszUser), 
					_bstr_t(lpszPassword), adModeUnknown);
				if (m_pConn->State == adStateOpen)
					return true;
				return false;
			}
			catch (_com_error& e)
			{
				// 在此，GetItem、GetNativeError均会再次抛出异常，需要再次捕获
				try
				{
					ErrorsPtr pErrors = m_pConn->Errors;
					ErrorPtr  pError  = pErrors != NULL ? pErrors->GetItem(0) : NULL;
					if (pError != NULL && pError->GetNativeError())
						m_hErrCode = pError->GetNativeError();
					if (m_hErrCode == 0)
						m_hErrCode = GetLastError();
				}
				catch(...)
				{
					m_hErrCode = GetLastError();
				}

				lstrcpy(m_szErrInfo,  (LPCTSTR)e.Description());
				return false;
			}
			return true;
		}

		virtual void Close()
		{
			if (m_pConn != NULL && m_pConn->State == adStateOpen)
				m_pConn->Close();
		}
		virtual LPCTSTR GetErrInfo()	{ return m_szErrInfo;   }
		virtual long	GetErrCode()	{ return m_hErrCode;	}

		virtual void Lock()				{ m_lock.Lock();		}
		virtual void UnLock()			{ m_lock.UnLock();		}

		virtual void BeginTrans()		{ m_pConn->BeginTrans();	}
		virtual void CommitTrans()		{ m_pConn->CommitTrans();	}
		virtual void RollbackTrans()	{ m_pConn->RollbackTrans(); }

		virtual bool ExecSql(LPCTSTR lpszSql, PLONG RecordsAffected)
		{
			m_hErrCode = 0;
			m_szErrInfo[0] = 0;
			if (m_pConn == NULL || m_pConn->State != adStateOpen)
			{		
				lstrcpy(m_szErrInfo, ERR_OPENDATABASE);
				return false;
			}

			try
			{
				_variant_t v;
				m_pConn->Execute(_bstr_t(lpszSql), &v, 0);

				if (RecordsAffected != NULL)
				{
					*RecordsAffected = (long)v;
				}
				return true;
			}
			catch (_com_error& e)
			{
				// 在此，GetItem、GetNativeError均会再次抛出异常，需要再次捕获
				try
				{
					ErrorsPtr pErrors = m_pConn->Errors;
					ErrorPtr  pError  = pErrors != NULL ? pErrors->GetItem(0) : NULL;
					if (pError != NULL && pError->GetNativeError())
						m_hErrCode = pError->GetNativeError();
					if (m_hErrCode == 0)
						m_hErrCode = GetLastError();
				}
				catch(...)
				{
					m_hErrCode = GetLastError();
				}
				
				_stprintf(m_szErrInfo, TEXT("%s:%s"), (LPCTSTR)e.Description(), e.ErrorMessage());
				return false;
			}

			return true;
		}

		virtual bool ExecSql(LPCTSTR lpszSql, _RecordsetPtr& prcd)
		{
			m_hErrCode = 0;
			m_szErrInfo[0] = 0;
			if (m_pConn == NULL || m_pConn->State != adStateOpen)
			{		
				lstrcpy(m_szErrInfo, ERR_OPENDATABASE);
				return false;
			}

			try
			{
				prcd.CreateInstance(__uuidof(Recordset));
				HRESULT hr = prcd->Open(_bstr_t(lpszSql), m_pConn.GetInterfacePtr(), 
					adOpenForwardOnly, adLockReadOnly, adCmdText);
				return SUCCEEDED(hr);
			}
			catch (_com_error& e)
			{
				// 在此，GetItem、GetNativeError均会再次抛出异常，需要再次捕获
				try
				{
					ErrorsPtr pErrors = m_pConn->Errors;
					ErrorPtr  pError  = pErrors != NULL ? pErrors->GetItem(0) : NULL;
					if (pError != NULL && pError->GetNativeError())
						m_hErrCode = pError->GetNativeError();
					if (m_hErrCode == 0)
						m_hErrCode = GetLastError();
				}
				catch(...)
				{
					m_hErrCode = GetLastError();
				}

				_stprintf(m_szErrInfo, TEXT("%s:%s"), (LPCTSTR)e.Description(), e.ErrorMessage());

				try
				{
					if (prcd != NULL && prcd->State == adStateOpen)
						prcd->Close();
					if (prcd != NULL)
						prcd.Release();
				}
				catch (...) {	}
				return false;
			}

			return true;
		}

		virtual bool CompactDatabase(LPCTSTR lpszDbFile)
		{
			m_szErrInfo[0] = 0;
			tstring dbsrc = lpszDbFile;
			tstring dbbak = lpszDbFile + tstring(TEXT(".bak"));
			tstring szConnString1 = TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=") + dbsrc;
			tstring szConnString2 = TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=") + dbbak;

			//压缩数据库.
			try
			{
				IJetEnginePtr JetEngine(__uuidof(JetEngine));

				SetFileAttributes(dbbak.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(dbbak.c_str());

				szConnString2 += TEXT(";Jet OLEDB:Engine Type=5");
				JetEngine->CompactDatabase(szConnString1.c_str(), szConnString2.c_str());
				Sleep(100);
				CopyFile(dbbak.c_str(), dbsrc.c_str(), FALSE);
				DeleteFile(dbbak.c_str());
			}
			catch (_com_error& e)
			{
				_stprintf(m_szErrInfo, TEXT("压缩数据库错误:%s."), (LPCTSTR)e.Description());
				return false;
			}
			return true;
		}
	};

	class CSysOpt : public ISysOpt
	{
	private:
		ISysOptTable*	m_pDatabase;
	public:
		CSysOpt() : m_pDatabase(NULL)	{}
		virtual ~CSysOpt()				{}

		virtual void Release()			{ delete this; }
		virtual void SetIDatabase(void* pDatabase)	{ m_pDatabase = reinterpret_cast<ISysOptTable*>(pDatabase); }

		virtual bool GetOpt(LPCTSTR optName, bool defValue)
		{
			return (1 == GetOpt(optName, defValue ? 1L : 0L));
		}

		virtual long GetOpt(LPCTSTR optName, long defValue)
		{
			TCHAR szDefValue[MAX_SYSOPT_LEN] = {0}, szValue[MAX_SYSOPT_LEN] = {0};
			_stprintf(szDefValue, TEXT("%d"), defValue);
			GetOpt(optName, szValue, szDefValue);
			return _ttoi(szValue);
		}

		virtual __int64 GetOpt(LPCTSTR optName, __int64 defValue)
		{
			TCHAR szDefValue[MAX_SYSOPT_LEN] = {0}, szValue[MAX_SYSOPT_LEN] = {0};
			_stprintf(szDefValue, TEXT("%I64d"), defValue);
			GetOpt(optName, szValue, szDefValue);
			return _ttoi64(szValue);
		}

		virtual void GetOpt(LPCTSTR optName, LPTSTR lpszValue, LPCTSTR defValue)
		{
			db::tSysOpt Opt = {0};
			if (0 == m_pDatabase->GetData(optName, &Opt, NULL))
			{
				strcpy(lpszValue, Opt.value);
				return ;
			}
			
			lstrcpy(Opt.key, optName);
			lstrcpy(Opt.value, defValue);
			m_pDatabase->Insert(&Opt);
			strcpy(lpszValue, defValue);
			return ;

			/*
			TCHAR szSql[4096] = {0};
			_stprintf(szSql, TEXT("select OptValue from tSysOpt where OptName = \'%s\'"), optName);

			CAutoLock<IDatabase> lock(m_pDatabase);
			try
			{
				_RecordsetPtr prcd;
				if (m_pDatabase->ExecSql(szSql, prcd))
				{
					if (prcd->adoEOF != VARIANT_TRUE)
					{
						_variant_t var = prcd->GetCollect(0L);
						prcd->Close();

						if (var.vt != VT_NULL)
						{
							lstrcpy(lpszValue, (LPCTSTR)(_bstr_t)(var));
							return ;
						}
						else
						{
							_stprintf(szSql, TEXT("update tSysOpt set OptValue = \'%s\' where OptName = \'%s\'"), defValue, optName);
							m_pDatabase->ExecSql(szSql);
						}
					}
					else
					{
						prcd->Close();
						prcd.Release();
						_stprintf(szSql, TEXT("insert into tSysOpt (OptName, OptValue) values (\'%s\', \'%s\')"), optName, defValue);
						m_pDatabase->ExecSql(szSql);
					}
				}
			}
			catch (...) {}
			//return default value.
			lstrcpy(lpszValue, defValue);
			*/
		}

		virtual void SetOpt(LPCTSTR optName, bool optValue)
		{
			SetOpt(optName, optValue ? 1L : 0L);
		}

		virtual void SetOpt(LPCTSTR optName, long optValue)
		{
			TCHAR szValue[MAX_SYSOPT_LEN] = {0};
			_stprintf(szValue, TEXT("%d"), optValue);
			SetOpt(optName, szValue);
		}

		virtual void SetOpt(LPCTSTR optName, __int64 optValue)
		{
			TCHAR szValue[MAX_SYSOPT_LEN] = {0};
			_stprintf(szValue, TEXT("%I64d"), optValue);
			SetOpt(optName, szValue);
		}

		virtual void SetOpt(LPCTSTR optName, LPCTSTR optValue)
		{
			db::tSysOpt Opt = {0};
			lstrcpy(Opt.key, optName);
			lstrcpy(Opt.value, optValue);
			m_pDatabase->Ensure(&Opt, MASK_TSYSOPT_VALUE);

			/*TCHAR szSql[4096] = {0};
			_stprintf(szSql, TEXT("update tSysOpt  set OptValue = \'%s\' where OptName = \'%s\'"), optValue, optName);
			LONG line = 0;
			CAutoLock<IDatabase> lock(m_pDatabase);
			try
			{
				if (m_pDatabase->ExecSql(szSql, &line))
				{
					if (line != 1)
					{
						_stprintf(szSql, TEXT("insert into tSysOpt (OptName, OptValue) values (\'%s\', \'%s\')"), optName, optValue);
						m_pDatabase->ExecSql(szSql);
					}
				}
			}
			catch (...) {}
			*/
		}
	};

	class CUdpLog
	{
	public:
		CUdpLog() : m_dwIp(0), m_dwPort(0)
		{
			WSADATA wsa = {0};
			WSAStartup(0x0202, &wsa);
			m_sckUdp = socket(AF_INET, SOCK_DGRAM, 0);
		}
		~CUdpLog()
		{
			WSACleanup();
		}
		void SetConsoleInfo(DWORD dwIp, DWORD dwPort)
		{
			m_dwIp = dwIp;
			m_dwPort = dwPort;
		}
		void SendLogToConsole(DWORD level, LPCTSTR lpszMod, LPCTSTR lpszLog, int nLen = -1)
		{
			if (m_dwIp != 0 && m_dwPort != 0 && m_sckUdp != INVALID_SOCKET && lpszMod != NULL && lpszLog != NULL)
			{
				char xbuf[8192] = {0};
				CPackageHelper out(xbuf);
				out.pushDWORD(_time32(NULL));
				out.pushDWORD(level);
				if (nLen == -1)
					nLen = strlen(lpszLog);
				out.pushString(lpszLog, nLen);
				out.pushString(lpszMod, strlen(lpszMod));

				_packageheader* pheader = (_packageheader*)xbuf;
				pheader->StartFlag = START_FLAG;
				pheader->Cmd = CMD_PLUGIN_LOG_REPORT;
				pheader->Version = PRO_VERSION;
				pheader->Length = out.GetOffset();

				sockaddr_in addr = {0};
				addr.sin_family = AF_INET;
				addr.sin_port   = (WORD)m_dwPort;
				addr.sin_addr.s_addr = m_dwIp;
				sendto(m_sckUdp, xbuf, pheader->Length, 0, (PSOCKADDR)&addr, sizeof(addr));
			}
		}
	private:
		DWORD m_dwIp;
		DWORD m_dwPort;
		SOCKET m_sckUdp;
	};

	static CUdpLog g_logConsole;

	class CLogger 
		: public ILogger
	{
	private:
		HANDLE m_hFile;	
		DWORD m_dwMaxSize;
		bool  m_bAddDate;
		bool  m_bAddCrLf;
		CLock  m_lock;
		TCHAR m_logFile[MAX_PATH];
		SYSTEMTIME m_SysTime;
	public:
		CLogger():m_hFile(INVALID_HANDLE_VALUE), m_dwMaxSize(5*1024*1024), m_bAddDate(true),m_bAddCrLf(true)
		{
			lstrcpy(m_logFile, TEXT("log"));
			ZeroMemory(&m_SysTime, sizeof(m_SysTime));
		}
		virtual ~CLogger()		
		{ 
			if (m_hFile != INVALID_HANDLE_VALUE) 
			{
				CloseHandle(m_hFile); 
				m_hFile = INVALID_HANDLE_VALUE;
			}
		}
		virtual void Release()	{ delete this; }

		virtual void SetLogFileName(LPCTSTR lpszLogFile)	
		{
			if (lpszLogFile != NULL && lstrlen(lpszLogFile))
			{
				lstrcpy(m_logFile, lpszLogFile);
			}
		}
		
		virtual void SetMaxLogSize(DWORD dwMaxSize)
		{
			m_dwMaxSize = dwMaxSize;
		}

		virtual void SetAddDateBefore(bool bAddDate)
		{
			m_bAddDate = bAddDate;
		}

		virtual void SetAddCrLfAfter(bool bAddCrLf)
		{
			m_bAddCrLf = bAddCrLf;
		}

		virtual void SetProptery(LPCTSTR lpszLogFile, DWORD dwMaxLogSize, bool bAddDate)
		{
			SetLogFileName(lpszLogFile);
			m_dwMaxSize = dwMaxLogSize;
			m_bAddDate  = bAddDate;
		}

		void SetConsole(DWORD ip, DWORD port)
		{
			g_logConsole.SetConsoleInfo(ip, port);
		}

		virtual LPCTSTR GetLogFileName()	{ return m_logFile;		}
		virtual DWORD	GetMaxLogSize()		{ return m_dwMaxSize;	}
		virtual bool	GetAddCrLfAfter()	{ return m_bAddCrLf;	}
		virtual bool	GetAddDateBefore()	{ return m_bAddDate;	}

		bool EnsureOpenFile()
		{
			SYSTEMTIME st = {0};
			GetLocalTime(&st);
			if (st.wYear != m_SysTime.wYear || st.wMonth != m_SysTime.wMonth ||
				st.wDay  != m_SysTime.wDay)
			{
				if (m_hFile != INVALID_HANDLE_VALUE)
				{
					CloseHandle(m_hFile);
					m_hFile = INVALID_HANDLE_VALUE;
				}
				m_SysTime = st;
			}
			if (m_hFile == INVALID_HANDLE_VALUE)
			{
				TCHAR szFileName[MAX_PATH] = {0};
				TCHAR szLogPath[MAX_PATH] = {0};
				GetModuleFileName(NULL, szLogPath, MAX_PATH);
				PathRemoveFileSpec(szLogPath);
				PathAddBackslash(szLogPath);
				lstrcat(szLogPath, TEXT("Log\\"));
				CreateDirectory(szLogPath, NULL);
				_stprintf(szFileName, TEXT("%s%s-%04d%02d%02d.log"), szLogPath, m_logFile, 
					st.wYear, st.wMonth, st.wDay);
				m_hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
				if (m_hFile == INVALID_HANDLE_VALUE)
					return false;
				SetFilePointer(m_hFile, 0, 0, FILE_END);
			}
			if (GetFileSize(m_hFile, NULL) > m_dwMaxSize)
			{
				SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
				SetEndOfFile(m_hFile);
			}
			return true;
		}

		virtual void WriteLog(DWORD level, LPCTSTR lpszFormater, ...)
		{
			CAutoLock<CLock> lock(&m_lock);
			if (!EnsureOpenFile())
				return ;			

			TCHAR xbuf[8192] = {0};
			int nLen = 0, nOffset = 0;
			if (m_bAddDate)
			{
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				nOffset = _stprintf(xbuf, TEXT("%04d-%02d-%02d %02d:%02d:%02d "), 
								st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				nLen += nOffset;
			}

			va_list marker;
			va_start(marker, lpszFormater);
			nLen += _vstprintf(&xbuf[nLen], lpszFormater, marker);
			va_end(marker);
			if (m_bAddCrLf)
			{
				xbuf[nLen++] += TEXT('\r');
				xbuf[nLen++] += TEXT('\n');
				xbuf[nLen] = 0;
			}
			nLen *= sizeof(TCHAR);

			DWORD dwWriteBytes = 0;
			if (level > LM_DEBUG)
				WriteFile(m_hFile, xbuf, nLen, &dwWriteBytes, NULL);
#ifdef _DEBUG
			std::cout << xbuf ;
#endif // _DEBUG
			g_logConsole.SendLogToConsole(level, m_logFile, &xbuf[nOffset], nLen - nOffset);
		}

		virtual void WriteLog2(DWORD level, LPCTSTR lpszLog, int nLen = -1)
		{
			CAutoLock<CLock> lock(&m_lock);
			if (!EnsureOpenFile())
				return ;

			std::stringstream out;
			if (m_bAddDate)
			{
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				TCHAR xbuf[MAX_PATH] = {0};
				_stprintf(xbuf, TEXT("%04d-%02d-%02d %02d:%02d:%02d "), 
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				out << xbuf;
			}

			out << lpszLog;

			if (m_bAddCrLf)
			{
				out << "\r\n";
			}
			std::string str = out.str();

			DWORD dwWriteBytes = 0;
			if (level > LM_DEBUG)
				WriteFile(m_hFile, str.c_str(), str.size(), &dwWriteBytes, NULL);
#ifdef _DEBUG
			std::cout << str;
#endif // _DEBUG
			g_logConsole.SendLogToConsole(level, m_logFile, lpszLog, nLen);
		}

		virtual void CleanLog()
		{
			CAutoLock<CLock> lock(&m_lock);
			WriteLog2(LM_TRACE, TEXT(""));//保证句柄文件被打开，防止没有程序启动后没有写过日志，就来删除，实际上就没有删除到，因为m_hFile==INVALID_HANDLE_VALUE.
			if (m_hFile == INVALID_HANDLE_VALUE)
				return ;
			SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
			SetEndOfFile(m_hFile);
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	};

	BOOL WINAPI CreateDatabase(IDatabase** ppIDatabase)
	{
		if (ppIDatabase == NULL)
			return FALSE;
		*ppIDatabase = NULL;
		try
		{
			*ppIDatabase = new CDatabase;
		}
		catch (...) { *ppIDatabase = NULL; }
		if (*ppIDatabase == NULL)
			return FALSE;

		return TRUE;
	}

	BOOL WINAPI CreateSysOpt(ISysOpt** ppISysOpt)
	{
		if (ppISysOpt == NULL)
			return FALSE;
		*ppISysOpt = NULL;
		try
		{
			*ppISysOpt = new CSysOpt;
		}
		catch (...) { *ppISysOpt = NULL; }
		if (*ppISysOpt == NULL)
			return FALSE;

		return TRUE;
	}

	BOOL WINAPI CreateLogger(ILogger** ppILogger)
	{
		if (ppILogger == NULL)
			return FALSE;
		*ppILogger = NULL;
		try
		{
			*ppILogger = new CLogger;
		}
		catch (...) { *ppILogger = NULL; }
		if (*ppILogger == NULL)
			return FALSE;

		return TRUE;
	}
}