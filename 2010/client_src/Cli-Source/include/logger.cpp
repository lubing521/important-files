namespace i8desk
{
	class CLogger : public ILogger
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
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
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

		virtual LPCTSTR GetLogFileName()	{ return m_logFile;		}
		virtual DWORD	GetMaxLogSize()		{ return m_dwMaxSize;	}
		virtual bool	GetAddCrLfAfter()	{ return m_bAddCrLf;	}
		virtual bool	GetAddDateBefore()	{ return m_bAddDate;	}

		virtual void WriteLog(LPCTSTR lpszFormater, ...)
		{
			CAutoLock<CLock> lock(&m_lock);
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
					return ;
				SetFilePointer(m_hFile, 0, 0, FILE_END);
			}
			if (GetFileSize(m_hFile, NULL) > m_dwMaxSize)
			{
				SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
				SetEndOfFile(m_hFile);
			}

			TCHAR xbuf[4096] = {0};
			int nLen = 0;
			if (m_bAddDate)
			{
				GetLocalTime(&st);
				nLen += _stprintf(xbuf, TEXT("%04d-%02d-%02d %02d:%02d:%02d "), 
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
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
			WriteFile(m_hFile, xbuf, nLen, &dwWriteBytes, NULL);
		}

		virtual void CleanLog()
		{
			CAutoLock<CLock> lock(&m_lock);
			WriteLog(TEXT(""));//保证句柄文件被打开，防止没有程序启动后没有写过日志，就来删除，实际上就没有删除到，因为m_hFile==INVALID_HANDLE_VALUE.
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