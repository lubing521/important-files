#include "stdafx.h"
#include "httpfile.h"
#include "I8Type.h"
#include "../../include/i8mp.h"

#include <shlwapi.h>
#include <process.h>
#include <winhttp.h>

#include <sstream>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winhttp.lib")

namespace HTTPFILE
{
	class CHttpFile
	{
	public:
		CHttpFile();
		~CHttpFile();
	public:
		DWORD Start(const std_string& url, const std_string& file, BOOL async, PFNDOWNCALLBACK pfnCallBack, LPVOID dwUser);
		DWORD Stop();
		DWORD Terminate();

	private:
		static DWORD __stdcall DownThreadProc(LPVOID lpVoid);
	private:
		HANDLE m_hThread;
		HANDLE m_hExited;
		HANDLE m_hFile;
		BOOL   m_async;
		PFNDOWNCALLBACK m_pfnCallBack;
		LPVOID m_dwUser;
		
		wchar_t _szHost[MAX_PATH];
		wchar_t _szReq [MAX_PATH];
		WORD    _wPort;
		DWORD   _dwLastError;

		DECLARE_I8_NEW_DELETE_OP()
	};

	CHttpFile::CHttpFile() :
		m_hThread (NULL),
		m_hFile(INVALID_HANDLE_VALUE), 
		m_async(FALSE),
		m_pfnCallBack(NULL),
		m_dwUser(NULL), 
		_wPort(80),
		_dwLastError(0)
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		ZeroMemory(_szHost, sizeof(_szHost));
		ZeroMemory(_szReq,  sizeof(_szReq));
	}

	CHttpFile::~CHttpFile()
	{
		Stop();
		CloseHandle(m_hExited);
	}

	DWORD CHttpFile::Start(const std_string& url, const std_string& file, BOOL async, PFNDOWNCALLBACK pfnCallBack, LPVOID dwUser)
	{
		m_dwUser = dwUser;
		m_async  = async;
		m_pfnCallBack = pfnCallBack;

		wchar_t szUrl [MAX_PATH] = {0};
		//Parse url
		{
#ifdef _UNICODE
			lstrcpy(szUrl, url.c_str());
#else
			MultiByteToWideChar(GetACP(), 0, url.c_str(), -1, szUrl, MAX_PATH);
#endif
			URL_COMPONENTS urlComp = {0};
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.lpszHostName = _szHost;
			urlComp.dwHostNameLength  = MAX_PATH;
			urlComp.lpszUrlPath = _szReq;
			urlComp.dwUrlPathLength   = MAX_PATH;
			if (!WinHttpCrackUrl(szUrl, (DWORD)wcslen(szUrl), 0, &urlComp))
			{
				return GetLastError();
			}
			_wPort = urlComp.nPort;
		}

		//delete local file and create file.
		if (PathFileExists(file.c_str()))
		{
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
			if (!DeleteFile(file.c_str()))
				return GetLastError();
		}
		m_hFile = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
			return GetLastError();

		//start download thread.
		_dwLastError = 0;
		ResetEvent(m_hExited);
		m_hThread=CreateThread(NULL,0,DownThreadProc,this,0,NULL);
		//m_hThread = (HANDLE)_beginthreadex(NULL, 0, DownThreadProc, this, 0, NULL);
		if (m_hThread == NULL)
		{
			return GetLastError();
		}
		if (m_async == FALSE)
		{
			WaitForSingleObject(m_hThread, INFINITE);
			return _dwLastError;
		}

		return 0;
	}

	DWORD CHttpFile::Stop()
	{
		if (m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		return 0;
	}

	DWORD __stdcall CHttpFile::DownThreadProc(LPVOID lpVoid)
	{
		CHttpFile* pHttpFile = (CHttpFile*)lpVoid;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

		I8_TRY
		{
			if ((hSession = WinHttpOpen(L"A WinHTTP I8Desk download Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)) == NULL)
			{
				throw GetLastError();
			}

			if ((hConnect = WinHttpConnect( hSession, pHttpFile->_szHost, pHttpFile->_wPort, 0)) == NULL)
			{
				throw GetLastError();
			}

			if ((hRequest = WinHttpOpenRequest( hConnect, L"GET", pHttpFile->_szReq, NULL, WINHTTP_NO_REFERER, 
				WINHTTP_DEFAULT_ACCEPT_TYPES, 0)) == NULL)
			{
				throw GetLastError();
			}
			
			if (!WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
				WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
			{
				throw GetLastError();
			}

			if (!WinHttpReceiveResponse( hRequest, NULL))
			{
				throw GetLastError();
			}
			
			DWORD dwTotal = 0;
			DWORD dwSize = sizeof(DWORD);
			if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER ,
				NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX) || dwTotal != 200)
			{
				throw GetLastError();
			}
			dwSize = sizeof(DWORD);
			if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH|WINHTTP_QUERY_FLAG_NUMBER,
				NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX) )
			{
				throw GetLastError();
			}
			
			DWORD dwReadBytes = 0;
			char* pszOutBuffer = NULL;
			DWORD dwLeft = dwTotal;
			
			while (dwLeft > 0 && WaitForSingleObject(pHttpFile->m_hExited, 0) != WAIT_OBJECT_0) 
			{
				dwSize = 0;
				if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
					throw GetLastError();

				//pszOutBuffer = new char[dwSize+1];
				pszOutBuffer = I8MP.alloc(dwSize+1);
                ZeroMemory(pszOutBuffer, dwSize+1);

				if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwReadBytes))
				{
					//delete []pszOutBuffer;
					I8MP.free(pszOutBuffer);
					throw GetLastError();
				}
                if (!WriteFile(pHttpFile->m_hFile, pszOutBuffer, dwReadBytes, &dwSize, NULL)
					|| dwSize == 0 )
				{
					//delete []pszOutBuffer;
					I8MP.free(pszOutBuffer);
					throw GetLastError();
				}

				//delete [] pszOutBuffer;
				I8MP.free(pszOutBuffer);
				dwLeft -= dwReadBytes;

				if (pHttpFile->m_async && pHttpFile->m_pfnCallBack != NULL)
				{
					pHttpFile->m_pfnCallBack(0, (dwTotal-dwLeft) *100/ dwTotal, pHttpFile->m_dwUser);
				}
			}
		}
		I8_CATCHALL 
		{
			pHttpFile->_dwLastError = GetLastError();
			if (pHttpFile->_dwLastError == 0)
				pHttpFile->_dwLastError = 1;	//unkown.
			
			if (pHttpFile->m_async && pHttpFile->m_pfnCallBack != NULL)
			{
				pHttpFile->m_pfnCallBack(pHttpFile->_dwLastError, 100, pHttpFile->m_dwUser);
			}
		}
		I8_ENDTRY;

		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		if (pHttpFile->m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(pHttpFile->m_hFile);
			pHttpFile->m_hFile = INVALID_HANDLE_VALUE;
		}
		return 0;
	}

	DWORD CreateHttpFile(PULONGLONG handle, LPCTSTR url, LPCTSTR file, BOOL async, PFNDOWNCALLBACK pfnCallBack, LPVOID dwUser)
	{
		CHttpFile* pHttpFile = NULL;
		
		DWORD dwError = 0;
		I8_TRY
		{
			*handle = 0;
			pHttpFile = new CHttpFile();
			dwError = pHttpFile->Start(url, file, async, pfnCallBack, dwUser);
			*handle = reinterpret_cast<ULONGLONG>(pHttpFile);
		}
		I8_CATCHALL {
			dwError = 8;
		}
		I8_ENDTRY;

		return dwError;
	}
	
	void  DeleteHttpFile(ULONGLONG handle)
	{
		I8_TRY
		{
			CHttpFile* pHttpFile = reinterpret_cast<CHttpFile*>(handle);
			if (pHttpFile != NULL)
			{
				pHttpFile->Stop();
				delete pHttpFile;
			}
		}
		I8_CATCHALL	{	}	I8_ENDTRY;

	}

	void ReclaimHttpObjectPool(void)
	{
		CHttpFile::ReclaimObjectPool();
	}


	IMPLMENT_I8_NEW_DELETE_OP(CHttpFile)

}