#include "stdafx.h"
#include <time.h>
#include <process.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <atlenc.h>
#include <Winhttp.h>
#include <shellapi.h>
#include <atlenc.h>
#include <sstream>

#include "update.h"
#include "Markup.h"

#import "../../lib/winhttp.dll"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

#define PATH_ROOT		"%root%"			//表示应用程序安装目录
#define PATH_SYSTEM		"%system%"			//表示系统目录
#define PATH_PROGRAM	"%program%"			//表示C:\program file\目录


#define _RUN_TMP_FILE_			".~tmp"				//更新正在占用的文件时，先改成一个临时文件.
#define _UPDATE_TMP_DIR_		"UpdateTemp"		//更新过程中，所有需要下载的文件的临时目录
#define _UPDATE_CLIENT_DIR_		"BarOnlineUpdate"	//客户端升级的目录.

namespace i8desk
{

	std::string _GetRootDir()
	{
		char path[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, path, MAX_PATH);
		PathRemoveFileSpecA(path);
		PathRemoveBackslashA(path);
		return std::string(path);
	}

	std::string _GetSystemDir()
	{
		char path[MAX_PATH] = {0};
		GetSystemDirectoryA(path, MAX_PATH);
		PathRemoveBackslashA(path);
		return std::string(path);
	}

	std::string _GetProgramDir()
	{
		char path[MAX_PATH] = {0};

		SHGetSpecialFolderPathA(NULL, path, CSIDL_PROGRAM_FILES, TRUE);
		PathRemoveBackslashA(path);
		return std::string(path);
	}

	CAutoUpate::CAutoUpate(ILogger* pLogger, CPlugMgr* pPlugMgr) : 
		m_pLogger(pLogger), m_pPlugMgr(pPlugMgr), m_hThread(NULL), m_hExited(NULL), m_bNeedReStartService(false)
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	
	CAutoUpate::~CAutoUpate()
	{
		StopUpdate();
		if (m_hExited)
			CloseHandle(m_hExited);
	}
	
	bool CAutoUpate::StartUpdate()
	{
		if (m_pPlugMgr == NULL)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("IPlugMgr Pointer is null."));
			return false;
		}
		//clear all temp file.
		{
			std::string dir = _GetRootDir() + "\\"_UPDATE_TMP_DIR_"\\";

			//删除原来被占用时，更新产生的临时文件.
			TCHAR path[MAX_PATH] = {0};
			SHFILEOPSTRUCT info = {0};
			ZeroMemory(&info, sizeof(info));
			_stprintf(path, TEXT("%s\\*")_RUN_TMP_FILE_, _GetRootDir().c_str());
			info.pFrom = path;
			info.wFunc = FO_DELETE;
			info.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
			SHFileOperation(&info);

			//删除上次更新临时目录下的所有的文件.(上次可能没有删除成功)
			ZeroMemory(&info, sizeof(info));
			_stprintf(path, TEXT("%s*.*"), dir.c_str());
			info.pFrom = path;
			info.wFunc = FO_DELETE;
			info.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
			SHFileOperation(&info);
		}

		m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
		m_pLogger->WriteLog(LM_INFO, TEXT("Create Update WorkThread:%s"), m_hThread != NULL ? TEXT("success.") : TEXT("Fail."));
		if (m_hThread == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Start Update Thread Error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		return true;
	}
	
	void CAutoUpate::StopUpdate()
	{
		if (m_hThread != NULL)
		{
			if (m_hExited != NULL)
			{
				SetEvent(m_hExited);
				WaitForSingleObject(m_hThread, INFINITE);
			}
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
	
	UINT __stdcall CAutoUpate::WorkThread(LPVOID lpVoid)
	{
		CoInitialize(NULL);
		CAutoUpate* pThis = reinterpret_cast<CAutoUpate*>(lpVoid);

		//__asm { int 3 }
		HANDLE hEvent[2];
		hEvent[0] = pThis->m_hExited;
		hEvent[1] = CreateEvent(NULL, FALSE, TRUE, NULL);

		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(2, hEvent, FALSE, 2 * 60 * 60 * 1000);
			if (dwRet == WAIT_OBJECT_0)
				break;
			else if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
			{
				try
				{
					pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Start AutoUpdate...."));
					pThis->m_bNeedReStartService = false;
					pThis->GetFileList();

					if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
						break;

					if (pThis->DownAllFile())
					{
						if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
							break;
						pThis->ExecUpdate();
						pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Stop AutoUpdate"));
					}
					else
					{
						pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Update Fail."));
					}
				}
				catch (...) { pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Update Thread Unknow.")); }
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("WaitForMultipleObjects Error:%d:%d:%s"), dwRet, dwError, Error.ErrorMessage());
				break;
			}
		}

		pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Exit Update WorkThread."));
		return 0;
	}

	void CAutoUpate::GetFileList()
	{
		using namespace WinHttp;
		m_lstFileList.clear();
		IWinHttpRequestPtr http;
		_bstr_t url;
_start:
		try
		{
			http.CreateInstance(__uuidof(WinHttpRequest));
			url = GetOpt(TEXT("server"), TEXT("update.domainurl"),
				TEXT("http://update.i8desk.com/update2/u2.do")).c_str();
			//nid,sver,smac
			std::stringstream out;
			TCHAR buf[MAX_PATH] = {0};
			out << "?nid=" << (DWORD)m_pPlugMgr->GetSysOpt()->GetOpt(OPT_U_NID, 0L);
			m_pPlugMgr->GetSysOpt()->GetOpt(OPT_U_SMAC, buf, TEXT(""));
			out << "&smac=" << uri_encode(buf);
			out << "&sver=" << uri_encode(GetFileVersion().c_str());
			_bstr_t url2 = url + _bstr_t(out.str().c_str());

			http->Open(_bstr_t("GET"), url2, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("close"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded"));

			http->Send();
			if (200 != http->Status)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Check StatusCode Error Result:%d."), http->Status);
				_com_raise_error(0);
			}
			else
			{
				std::string strXml = (LPCSTR)http->ResponseText;
				int dwSize = strXml.size();
				BYTE* pDst = new BYTE[dwSize];
				ZeroMemory(pDst, dwSize);
				ATL::Base64Decode(strXml.c_str(), strXml.size(), pDst, &dwSize);
				
				try
				{
					CMarkup xml;
					//if (xml.Load("E:\\I8Desk2009\\bin\\setting.xml"))
					if (!xml.SetDoc((LPCSTR)pDst))
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("parase update xml file error:%s"), xml.GetError().c_str());
					}
					else
					{
						xml.ResetPos();
						if (xml.FindElem("filelist") && xml.IntoElem())
						{
							m_pLogger->WriteLog(LM_INFO, TEXT("Get All File list:"));
							while (xml.FindElem("file"))
							{
								if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
									break;
								tagFileInfo FileInfo;
								FileInfo.szFileName  = xml.GetAttrib("name");
								FileInfo.szUrl		 = xml.GetAttrib("url");
								FileInfo.dwCrc		 = StringToInt(xml.GetAttrib("crc").c_str(), 16);
								FileInfo.dwModifyTime= StringToInt(xml.GetAttrib("modifytime").c_str(), 10);

								ProccessFilePath(&FileInfo);

								if (GetFileCRC32(FileInfo.szParseFile) != FileInfo.dwCrc)
								{
									m_lstFileList.push_back(FileInfo);
									m_pLogger->WriteLog(LM_INFO, "Need Update File:%s", FileInfo.szParseFile.c_str());
								}
								else
								{
									//m_pLogger->WriteLog(LM_INFO, "Not Need Update File:%s", FileInfo.szParseFile.c_str());
								}
							}
							m_pLogger->WriteLog(LM_INFO, TEXT("End All File list:"));
						}
						else m_pLogger->WriteLog(LM_INFO, "Xml File Format Error.");
					}
				}
				catch (...) { m_pLogger->WriteLog(LM_INFO, "GetFileList Have ErrorX."); }
				delete []pDst;
			}
		}
		catch (_com_error& Error) 
		{
			if (Error.Error() != 0)
				m_pLogger->WriteLog(LM_INFO, TEXT("Update:GetFileList Error:%s"), (LPCTSTR)Error.Description());
			_bstr_t newurl = GetOpt(TEXT("server"), TEXT("update.ipurl"),
				TEXT("http://update.i8desk.com/update2/u2.do")).c_str();			          
			if (newurl != url) 
			{
				url= newurl;
				goto _start;
			}
		}
	}

	bool CAutoUpate::DownAllFile()
	{
		//创建更新临时目录和客户端更新目录(保证目录一定有).
		std::string dir = _GetRootDir() + "\\"_UPDATE_TMP_DIR_"\\";
		CreateDirectoryA(dir.c_str(), NULL);
		dir = _GetRootDir() +  "\\"_UPDATE_CLIENT_DIR_"\\";
		CreateDirectoryA(dir.c_str(), NULL);

		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
		for (; it != m_lstFileList.end(); it ++)
		{	
			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				break;

			m_pLogger->WriteLog(LM_INFO, TEXT("Start Download file:%s"), (LPCSTR)_bstr_t(it->szParseFile.c_str()));
			if (!DownLoadFile(&(*it)))
				return false;
		}
		return true;
	}

	void CAutoUpate::ExecUpdate()
	{
		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
		for (; it != m_lstFileList.end(); it ++)
		{
			if (!PathFileExists(it->szTmpName.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("download's tmp file is not exist:%s."), it->szTmpName.c_str());
				continue;
			}
			EnsureFileDirExist(it->szParseFile);
			if (CopyFileA(it->szTmpName.c_str(), it->szParseFile.c_str(), FALSE))
			{
				HANDLE hFile = CreateFileA(it->szParseFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);
				
				if (hFile != INVALID_HANDLE_VALUE)
				{
					struct tm* ptm = _gmtime32((const __time32_t *)&it->dwModifyTime);
					if (ptm != NULL)
					{
						SYSTEMTIME st = {0};
						st.wYear = ptm->tm_year + 1900;
						st.wMonth = ptm->tm_mon + 1;
						st.wDay = ptm->tm_mday;
						st.wHour = ptm->tm_hour;
						st.wMinute = ptm->tm_min;
						st.wSecond = ptm->tm_sec;
						FILETIME ft = {0};
						SystemTimeToFileTime(&st, &ft);
						SetFileTime(hFile, NULL, NULL, &ft);
					}
					CloseHandle(hFile);
				}
				m_pLogger->WriteLog(LM_INFO, TEXT("Copy File Success:%s."), it->szParseFile.c_str());
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Copy File Fail:%s:%d:%s"), it->szParseFile.c_str(), dwError, Error.ErrorMessage());

				std::string tmp = it->szParseFile + ".~tmp";
				SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFileA(tmp.c_str());
				if (!MoveFileExA(it->szParseFile.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING))
				{
					dwError = GetLastError();
					Error = _com_error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("Rename File:%s:%d:%s"), 
						it->szFileName.c_str(), dwError, Error.ErrorMessage());
					return ;
				}
				if (!CopyFileA(it->szTmpName.c_str(), it->szParseFile.c_str(), FALSE))
				{
					dwError = GetLastError();
					Error = _com_error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("ReCopy File::%s:%d:%s"), it->szParseFile.c_str(), dwError, Error.ErrorMessage());
					return ;
				}
				else
				{
					m_bNeedReStartService = true;
					m_pLogger->WriteLog(LM_INFO, TEXT("ReCopy File:%s:Sucess"), it->szParseFile.c_str());
				}
			}
		}
		
		TCHAR path[MAX_PATH] = {0};
		SHFILEOPSTRUCT info = {0};
		ZeroMemory(&info, sizeof(info));
		_stprintf(path, TEXT("%s\\")_UPDATE_TMP_DIR_ TEXT("\\*.*"), _GetRootDir().c_str());
		info.pFrom = path;
		info.wFunc = FO_DELETE;
		info.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
		SHFileOperation(&info);

		_stprintf(path, TEXT("%s\\")_UPDATE_TMP_DIR_ TEXT("\\"), _GetRootDir().c_str());
		RemoveDirectory(path);

		//re start self (by normal dos exe) to restart service.
		if (m_bNeedReStartService)
		{
			GetModuleFileName(NULL, path, MAX_PATH);
			lstrcat(path, TEXT(" -AutoUpdate"));
			Sleep(200);
			STARTUPINFO si = {sizeof(si)};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi = {0};
			BOOL ret = CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			if (ret)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Run Self by AutoUpdate Success."));
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Run Self by AutoUpdate Fail:%d:%s"), dwError, Error.ErrorMessage());
			}
		}
	}

	DWORD CAutoUpate::GetFileCRC32(const std::string&  szFile)
	{
		HANDLE hFile = CreateFileA(szFile.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
// 			DWORD dwError = GetLastError();
// 			_com_error Error(dwError);
// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileCRC32 CreateFile Error:%d:%s"), dwError, Error.ErrorMessage());
			return 0;
		}

		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pData = new BYTE[dwSize+1];
		DWORD dwReadBytes = 0;
		if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwSize != dwReadBytes)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileCRC32 ReadFile Error:%d:%s"), dwError, Error.ErrorMessage());
 			CloseHandle(hFile);
			delete []pData;
			return 0;
		}
		
		DWORD dwCRC32 = CalBufCRC32(pData, dwSize);
		CloseHandle(hFile);
		delete []pData;
		return dwCRC32;
	}
	
	void CAutoUpate::ProccessFilePath(tagFileInfo* pFileInfo)
	{
		typedef std::string (*XFUNC)(void);
		struct _list
		{
			char*	find;
			XFUNC   xFunc;
		};
		_list xlist[] = 
		{
			{PATH_ROOT,		_GetRootDir		},
			{PATH_SYSTEM,	_GetSystemDir	},
			{PATH_PROGRAM,	_GetProgramDir	}
		};
		
		//得到文件下载后需要拷贝的目的文件
		pFileInfo->szParseFile = pFileInfo->szFileName;
		for (int idx=0; idx<_countof(xlist); idx++)
		{
			std::string find = xlist[idx].find;
			int pos = 0;
			if ( (pos = pFileInfo->szParseFile.find(find, pos)) != pFileInfo->szParseFile.npos)
			{
				pFileInfo->szParseFile.replace(pos, find.size(), xlist[idx].xFunc());
				break;
			}
		}
		
		pFileInfo->szTmpName = pFileInfo->szFileName;
		std::string tmpDir = _GetRootDir() + TEXT("\\") _UPDATE_TMP_DIR_;
		for (int idx=0; idx<_countof(xlist); idx++)
		{
			std::string find = xlist[idx].find;
			int pos = 0;
			if ( (pos = pFileInfo->szTmpName.find(find, pos)) != pFileInfo->szTmpName.npos)
			{
				pFileInfo->szTmpName.replace(pos, find.size(), tmpDir);
				break;
			}
		}

		//生成每个文件下载的临时文件名.（先下载到临时目录，文件名不变）
// 		pFileInfo->szTmpName = _GetRootDir() + TEXT("\\") _UPDATE_TMP_DIR_;
// 		pFileInfo->szTmpName += pFileInfo->szFileName.substr(pFileInfo->szFileName.rfind('\\'));
	}

	bool CAutoUpate::DownLoadFile(tagFileInfo* pFileInfo)
	{
		std::string szFile = pFileInfo->szTmpName;

		EnsureFileDirExist(szFile);
		if (PathFileExistsA(szFile.c_str()))
		{
			SetFileAttributesA(szFile.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFileA(szFile.c_str());
		}
		HANDLE hFile = CreateFileA(szFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Download file Error:%s:%d:%s"), 
				szFile.c_str(), dwError, Error.ErrorMessage());
			return false;
		}

		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

		bool flag = true;
		try
		{
			wchar_t host[MAX_PATH] = {0};
			wchar_t req [MAX_PATH] = {0};
			URL_COMPONENTS urlComp = {0};
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.lpszHostName = host;
			urlComp.dwHostNameLength  = MAX_PATH;
			urlComp.lpszUrlPath = req;
			urlComp.dwUrlPathLength   = MAX_PATH;
			std::wstring str = _bstr_t(pFileInfo->szUrl.c_str());
			WinHttpCrackUrl(str.c_str(), str.size(), 0, &urlComp);

			if ((hSession = WinHttpOpen(L"A WinHTTP I8Desk download Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)) == NULL)
			{
				throw GetLastError();
			}

			if ((hConnect = WinHttpConnect( hSession, host, urlComp.nPort, 0)) == NULL)
			{
				throw GetLastError();
			}

			if ((hRequest = WinHttpOpenRequest( hConnect, L"GET", req, NULL, WINHTTP_NO_REFERER, 
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
				NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX))
			{
				throw GetLastError();
			}
			if (dwTotal != 200)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("http reponse erorr:%d"), dwTotal);
				throw (DWORD)0;
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

			while (dwLeft > 0 && WaitForSingleObject(m_hExited, 0) != WAIT_OBJECT_0) 
			{
				dwSize = 0;
				if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
					throw GetLastError();

				pszOutBuffer = new char[dwSize+1];
				ZeroMemory(pszOutBuffer, dwSize+1);

				if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwReadBytes))
				{
					DWORD dwError = GetLastError();
					delete []pszOutBuffer;
					throw dwError;
				}
				if (!WriteFile(hFile, pszOutBuffer, dwReadBytes, &dwSize, NULL))
				{
					DWORD dwError = GetLastError();
					delete []pszOutBuffer;
					throw dwError;
				}

				delete [] pszOutBuffer;
				dwLeft -= dwReadBytes;
			}
			if (dwLeft > 0)
			{
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
				SetFileAttributesA(szFile.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFileA((szFile.c_str()));
				m_pLogger->WriteLog(LM_INFO, TEXT("Delete file of unfinish downing."));
			}
		}
		catch (DWORD dwError)
		{
			if (dwError)
			{
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Download file Error:%s:%d:%s"), 
					pFileInfo->szParseFile.c_str(), dwError, (LPCTSTR)Error.Description());
			}
			flag = false;
		}

		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		if (hFile != INVALID_HANDLE_VALUE) 
			CloseHandle(hFile);
		if (flag == false)
		{
			DeleteFile(szFile.c_str());
		}
		else	//如果下载成功再进行一次ＣＲＣ对比。看下载的文件内容是否正确。
		{
			if (GetFileCRC32(szFile) != pFileInfo->dwCrc)
			{
				flag = false;
				m_pLogger->WriteLog(LM_INFO, TEXT("Download file fail(crc error):%s"), pFileInfo->szParseFile.c_str());
			}
		}

		return flag;
	}

	tstring CAutoUpate::GetOpt(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue)
	{
		tstring szSysConfig = _GetRootDir() + TEXT("\\i8desksys.config");
		tstring szDestFile  = _GetRootDir() + TEXT("\\Data\\i8desksys.config.ini");

		SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szDestFile.c_str());

		HANDLE hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			try
			{
				DWORD dwSize = GetFileSize(hFile, NULL);
				if (dwSize == INVALID_FILE_SIZE)
					throw 1;
				char *pSrc = new char[dwSize];
				DWORD dwReadBytes = 0;
				if (!ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL))
				{
					delete[]pSrc;
					CloseHandle(hFile);
					return tstring(szDefValue);
				}
				CloseHandle(hFile);

				int dwNewSize = dwSize;
				BYTE* pDst = new BYTE[dwNewSize];
				ATL::Base64Decode(pSrc, dwSize, pDst, &dwNewSize);
				hFile = CreateFile(szDestFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SYSTEM);
					WriteFile(hFile, pDst, dwNewSize, &dwReadBytes, NULL);
					CloseHandle(hFile);

					TCHAR buf[4096] = {0};
					::GetPrivateProfileString(szSection, szKeyName, szDefValue, buf, 4096, szDestFile.c_str());
					DeleteFile(szDestFile.c_str());
					delete []pSrc;
					delete []pDst;
					return tstring(buf);
				}
				delete []pSrc;
				delete []pDst;
			}
			catch (...) { m_pLogger->WriteLog(LM_INFO, "Read System Option Error."); }
		}
		return tstring(szDefValue);
	}

	void CAutoUpate::EnsureFileDirExist(const std::string& file, int offset /* = 0 */)
	{
		int end = file.rfind('\\');
		int pt  = file.find('\\', offset);

		while(pt != std::string::npos && pt<=end)		
		{		
			std::string path = file.substr(0, pt+1);
			DWORD dwRet = GetFileAttributes(path.c_str());
			if(dwRet == INVALID_FILE_ATTRIBUTES)			
				::CreateDirectory(path.c_str(), NULL);
			pt = file.find('\\', pt+1);
		}
	}

	DWORD CAutoUpate::StringToInt(LPCSTR pString, int K /* = 16 */)
	{
		int nLen = pString ? lstrlen(pString) : 0;
		DWORD result = 0;
		DWORD number = 1;
		for (int idx=nLen-1; nLen && idx>=0; idx--)
		{
			if (isdigit(pString[idx]))
			{
				result += ((pString[idx]-'0') * number);
			}
			else if (isalpha(pString[idx]))
			{
				result += ((tolower(pString[idx]) - 'a' + 10) * number);
			}
			number *= K;
		}
		return result;
	}

	tstring CAutoUpate::GetFileVersion()
	{
		tstring szFile = GetAppPath() + TEXT("I8DeskSvr.exe");
		TCHAR version[MAX_PATH] = TEXT("0.0.0.0");
		DWORD dwSize = GetFileVersionInfoSize(szFile.c_str(), NULL);
		if(dwSize)
		{
			LPTSTR pblock = new TCHAR[dwSize+1];
			GetFileVersionInfo(szFile.c_str(), 0, dwSize, pblock);
			UINT nQuerySize;
			DWORD* pTransTable = NULL;
			VerQueryValue(pblock, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
			LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			TCHAR SubBlock[MAX_PATH] = {0};
			_stprintf(SubBlock, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
			LPTSTR lpData;
			VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize);
			lstrcpy(version, lpData);
			delete[] pblock;
		}
		tstring str(version);
		//replace ',' to '.'
		size_t pos = 0;
		while ( (pos = str.find(',', pos)) != std::string::npos)
		{
			str[pos] = '.';
		}
		//delete [SP]
		pos = 0;
		while ( (pos = str.find(' ', pos)) != std::string::npos)
		{
			str.erase(pos, 1);
		}
		return str;
	}

	std::string CAutoUpate::uri_encode(LPCTSTR lpszSouce)
	{
		std::string source((LPCSTR)_bstr_t(lpszSouce));
		char element[4];
		unsigned char chr;
		std::string res;
		for ( unsigned int i=0; i<source.length(); i++ )
		{
			chr = source[i];
			if (chr > 127)		//汉字
			{
				sprintf(element, "%%%02X", chr);				
				res += element;
				chr = source[++i];
				sprintf(element, "%%%02X", chr);
				res += element;
			}
			else if ( (chr >= '0' && chr <= '9') || 
				(chr >= 'a' && chr <= 'z') ||
				(chr >= 'A' && chr <= 'Z') )
			{
				res += chr;
			}
			//else if ( strchr(reserved,chr) || strchr(unsafe,chr) || strchr(other,chr) )
			else
			{
				sprintf( element, "%%%02X", chr); 
				res += element;
			}
		}	

		return res;
	}
}