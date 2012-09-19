#include "stdafx.h"
#include "update.h"
#include <time.h>
#include <process.h>
#include <atlenc.h>
#include <shlobj.h>

#undef  _WS2DEF_
#include <winhttp.h>

//#import "C:\WINDOWS\WinSxS\x86_Microsoft.Windows.WinHTTP_6595b64144ccf1df_5.1.3790.4427_x-ww_FDB042FC\WinHttp.dll"
#import "C:\WINDOWS\System32\WinHttp.dll"
#import "msxml3.dll"

#include "../../include/utility/utility.h"


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

#define PATH_ROOT				TEXT("%root%")				//表示应用程序安装目录
#define PATH_SYSTEM				TEXT("%system%")			//表示系统目录
#define PATH_PROGRAM			TEXT("%program%")			//表示C:\program file\目录


#define _RUN_TMP_FILE_			TEXT(".~tmp")				//更新正在占用的文件时，先改成一个临时文件.
#define _UPDATE_TMP_DIR_		TEXT("UpdateTemp")			//更新过程中，所有需要下载的文件的临时目录
#define _UPDATE_CLIENT_DIR_		TEXT("BarOnlineUpdate")		//客户端升级的目录.


#define UPDATE_PARAMETER		TEXT(" -AutoUpdate")		//主服务通过该参数来重启应用程序
#define UPDATE_URL				TEXT("http://update.i8desk.com/update2/u2.do")

namespace i8desk
{
	stdex::tString _GetRootDir()
	{
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL, path, MAX_PATH);
		PathRemoveFileSpec(path);
		PathRemoveBackslash(path);
		return stdex::tString(path);
	}

	stdex::tString _GetSystemDir()
	{
		TCHAR path[MAX_PATH] = {0};
		GetSystemDirectory(path, MAX_PATH);
		PathRemoveBackslash(path);
		return stdex::tString(path);
	}

	stdex::tString _GetProgramDir()
	{
		TCHAR path[MAX_PATH] = {0};

		SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES, TRUE);
		PathRemoveBackslash(path);
		return stdex::tString(path);
	}

	CAutoUpate::CAutoUpate(CPlugMgr* pPlugMgr, ILogger* pLogger) : m_pPlugMgr(pPlugMgr), m_pLogger(pLogger), 
		m_hThread(NULL), m_hExited(NULL), m_bNeedReStartService(false)
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	
	CAutoUpate::~CAutoUpate()
	{
		StopUpdate();
		if (m_hExited)
		{
			CloseHandle(m_hExited);
			m_hExited = NULL;
		}
	}
	
	void DeleteAllFile(const stdex::tString& dir, const stdex::tString& filter)
	{
		WIN32_FIND_DATA wfd = {0};
		HANDLE hFile = FindFirstFile((dir + filter).c_str(), &wfd);
		if (hFile == INVALID_HANDLE_VALUE)
			return ;

		do 
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmpi(wfd.cFileName, TEXT(".")) != 0 && lstrcmpi(wfd.cFileName, TEXT("..")) != 0)
				{
					stdex::tString subdir = dir + wfd.cFileName + TEXT("\\");
					DeleteAllFile(subdir, filter);
				}
			}
			else
			{
				stdex::tString file = dir +wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(file.c_str());
			}
		} while (FindNextFile(hFile, &wfd));

		FindClose(hFile);
	}

	bool CAutoUpate::StartUpdate()
	{
		if (m_hExited == NULL)
			return false;

		DeleteAllFile(utility::GetAppPath(), TEXT("*")_RUN_TMP_FILE_);
		DeleteAllFile(utility::GetAppPath() + _UPDATE_TMP_DIR_ + TEXT("\\"), TEXT("*"));
	
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
		if (m_hThread == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create Update Thread Error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		return true;
	}
	
	void CAutoUpate::StopUpdate()
	{
		if (m_hThread != NULL && m_hExited != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
	
	UINT __stdcall CAutoUpate::WorkThread(LPVOID lpVoid)
	{
		CoInitialize(NULL);
		CAutoUpate* pThis = reinterpret_cast<CAutoUpate*>(lpVoid);

		HANDLE hEvent[2] = {0};
		hEvent[0] = pThis->m_hExited;
		hEvent[1] = CreateEvent(NULL, FALSE, TRUE, NULL);

		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(2, hEvent, FALSE, 2 * 60 * 60 * 1000);
			if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
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
			else break;
		}
		return 0;
	}

	inline _bstr_t GetNodeAttrValue(const MSXML2::IXMLDOMNodePtr& node, LPCTSTR name)
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attr = node->Getattributes();
		if (!attr)
			return _bstr_t(TEXT(""));
		MSXML2::IXMLDOMNodePtr node2 = attr->getNamedItem(_bstr_t(name));
		if (!node2)
			return _bstr_t(TEXT(""));
		return node2->text;
	}

	void CAutoUpate::GetFileList()
	{
		using namespace WinHttp;
		m_lstFileList.clear();
		
		try
		{
			//nid,sver,smac
			DWORD nid = m_pPlugMgr->GetSysOpt()->GetOpt(OPT_U_NID, 0L);

			TCHAR szExe[MAX_PATH] ={0};
			GetModuleFileName(NULL, szExe, _countof(szExe));
			stdex::tString sver = utility::GetFileVersion(szExe);

			TCHAR smac[MAX_PATH] = {0};
			m_pPlugMgr->GetSysOpt()->GetOpt(OPT_U_SMAC, smac, TEXT(""));

			TCHAR szParameter[1024] = {0};
			_stprintf(szParameter, TEXT("?nid=%u&smac=%s&sver=%s"), nid, 
				uri_encode(smac).c_str(), uri_encode(sver.c_str()).c_str());

			_bstr_t url = _bstr_t(UPDATE_URL) + szParameter;
			IWinHttpRequestPtr http(__uuidof(WinHttpRequest));
			http->Open(_bstr_t("GET"), url, VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t(TEXT("Connection")), _bstr_t(TEXT("close")));
			http->SetRequestHeader(_bstr_t(TEXT("Content-Type")), _bstr_t(TEXT("application/x-www-form-urlencoded")));
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
				char* pDst = new char[dwSize];
				ZeroMemory(pDst, dwSize);
				ATL::Base64Decode(strXml.c_str(), strXml.size(), (BYTE*)pDst, &dwSize);
				
				MSXML2::IXMLDOMDocumentPtr xml(__uuidof(DOMDocument));
				if (VARIANT_TRUE == xml->loadXML(_bstr_t(pDst)))
				{
					MSXML2::IXMLDOMNodeListPtr files = xml->selectNodes(TEXT("/filelist/file"));
					for (long idx=0; idx<files->length; idx++)
					{
						MSXML2::IXMLDOMNodePtr file = files->Getitem(idx);
						tagFileInfo FileInfo;
						FileInfo.szFileName  = GetNodeAttrValue(file, TEXT("name"));
						FileInfo.szUrl		 = GetNodeAttrValue(file, TEXT("url"));
						FileInfo.dwCrc		 = StringToInt(GetNodeAttrValue(file, TEXT("crc")), 16);
						FileInfo.dwModifyTime= StringToInt(GetNodeAttrValue(file, TEXT("modifytime")), 10);
						
						ProccessFilePath(&FileInfo);
						if (GetFileCRC32(FileInfo.szParseFile) != FileInfo.dwCrc)
						{
							m_lstFileList.push_back(FileInfo);
							m_pLogger->WriteLog(LM_INFO, TEXT("Need Update File:%s"), FileInfo.szParseFile.c_str());
						}
					}
				}
				delete []pDst;
			}
		}
		catch (_com_error& Error) 
		{
			if (Error.Error() != 0)
				m_pLogger->WriteLog(LM_INFO, TEXT("Update:GetFileList Error:%s"), (LPCTSTR)Error.Description());
		}
	}

	bool CAutoUpate::DownAllFile()
	{
		stdex::tString dir = utility::GetAppPath() + _UPDATE_TMP_DIR_;
		CreateDirectory(dir.c_str(), NULL);
		dir = utility::GetAppPath() + _UPDATE_CLIENT_DIR_;
		CreateDirectory(dir.c_str(), NULL);

		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
		for (; it != m_lstFileList.end(); it ++)
		{
			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				break;

			m_pLogger->WriteLog(LM_INFO, TEXT("Start Download file:%s"), it->szParseFile.c_str());
#ifdef DEBUG
			return false;
#else
			if (!DownLoadFile(&(*it)))
				return false;
#endif
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
			if (CopyFile(it->szTmpName.c_str(), it->szParseFile.c_str(), FALSE))
			{
				HANDLE hFile = CreateFile(it->szParseFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
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
				m_pLogger->WriteLog(LM_INFO, TEXT("Copy File Fail:%s:%d:%s"), it->szParseFile.c_str(), 
					dwError, Error.ErrorMessage());

				stdex::tString tmp = it->szParseFile + _RUN_TMP_FILE_;
				SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(tmp.c_str());
				if (!MoveFileEx(it->szParseFile.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING))
				{
					dwError = GetLastError();
					Error = _com_error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("Rename File:%s:%d:%s"), 
						it->szFileName.c_str(), dwError, Error.ErrorMessage());
					return ;
				}
				if (!CopyFile(it->szTmpName.c_str(), it->szParseFile.c_str(), FALSE))
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
		DeleteAllFile(utility::GetAppPath() + _UPDATE_TMP_DIR_ + TEXT("\\"), TEXT("*"));
		RemoveDirectory((utility::GetAppPath() + _UPDATE_TMP_DIR_).c_str());

		//re start self (by normal dos exe) to restart service.
		if (m_bNeedReStartService)
		{
			TCHAR path[MAX_PATH] = {0};
			GetModuleFileName(NULL, path, MAX_PATH);
			lstrcat(path, UPDATE_PARAMETER);
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

	DWORD CAutoUpate::GetFileCRC32(const stdex::tString&  szFile)
	{
		HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return 0;

		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pData = new BYTE[dwSize+1];
		DWORD dwReadBytes = 0;
		if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwSize != dwReadBytes)
		{
 			CloseHandle(hFile);
			delete []pData;
			return 0;
		}
		
		DWORD dwCRC32 = utility::CalBufCRC32(pData, dwSize);
		CloseHandle(hFile);
		delete []pData;
		return dwCRC32;
	}
	
	void CAutoUpate::ProccessFilePath(tagFileInfo* pFileInfo)
	{
		typedef stdex::tString (*XFUNC)(void);
		struct _list
		{
			LPCTSTR  find;
			XFUNC   xFunc;
		};
		_list xlist[] = 
		{
			{PATH_ROOT,		_GetRootDir		},
			{PATH_SYSTEM,	_GetSystemDir	},
			{PATH_PROGRAM,	_GetProgramDir	}
		};
		
		pFileInfo->szParseFile = pFileInfo->szFileName;
		for (int idx=0; idx<_countof(xlist); idx++)
		{
			stdex::tString find = xlist[idx].find;
			int pos = 0;
			if ( (pos = pFileInfo->szParseFile.find(find, pos)) != pFileInfo->szParseFile.npos)
			{
				pFileInfo->szParseFile.replace(pos, find.size(), xlist[idx].xFunc());
				break;
			}
		}
		
		pFileInfo->szTmpName = pFileInfo->szFileName;
		stdex::tString tmpDir = _GetRootDir() + TEXT("\\") _UPDATE_TMP_DIR_;
		for (int idx=0; idx<_countof(xlist); idx++)
		{
			stdex::tString find = xlist[idx].find;
			int pos = 0;
			if ( (pos = pFileInfo->szTmpName.find(find, pos)) != pFileInfo->szTmpName.npos)
			{
				pFileInfo->szTmpName.replace(pos, find.size(), tmpDir);
				break;
			}
		}
	}

	bool CAutoUpate::DownLoadFile(tagFileInfo* pFileInfo)
	{
		stdex::tString szFile = pFileInfo->szTmpName;

		EnsureFileDirExist(szFile);
		if (PathFileExists(szFile.c_str()))
		{
			SetFileAttributes(szFile.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szFile.c_str());
		}
		HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
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
				SetFileAttributes(szFile.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile((szFile.c_str()));
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
		else
		{
			if (GetFileCRC32(szFile) != pFileInfo->dwCrc)
			{
				flag = false;
				m_pLogger->WriteLog(LM_INFO, TEXT("Download file fail(crc error):%s"), pFileInfo->szParseFile.c_str());
			}
		}

		return flag;
	}

	void CAutoUpate::EnsureFileDirExist(const stdex::tString& file, int offset /* = 0 */)
	{
		int end = file.rfind(TEXT('\\'));
		int pt  = file.find(TEXT('\\'), offset);

		while(pt != stdex::tString::npos && pt<=end)		
		{		
			stdex::tString path = file.substr(0, pt+1);
			DWORD dwRet = GetFileAttributes(path.c_str());
			if(dwRet == INVALID_FILE_ATTRIBUTES)			
				::CreateDirectory(path.c_str(), NULL);
			pt = file.find(TEXT('\\'), pt+1);
		}
	}

	DWORD CAutoUpate::StringToInt(LPCTSTR pString, int K /* = 16 */)
	{
		int nLen = pString ? lstrlen(pString) : 0;
		DWORD result = 0;
		DWORD number = 1;
		for (int idx=nLen-1; nLen && idx>=0; idx--)
		{
			if (_istdigit(pString[idx]))
			{
				result += ((pString[idx]- TEXT('0')) * number);
			}
			else if (_istalpha(pString[idx]))
			{
				result += ((_totlower(pString[idx]) - TEXT('a') + 10) * number);
			}
			number *= K;
		}
		return result;
	}

	stdex::tString CAutoUpate::uri_encode(LPCTSTR lpszSouce)
	{
		stdex::tString result;
		stdex::tString source(lpszSouce);
		for (size_t i=0; i<source.length(); i++ )
		{
			TCHAR element[4] = {0};
			TCHAR chr = source[i];
			if (chr > 127)		//汉字
			{
#ifdef _UNICODE
				_stprintf(element, TEXT("%%%02X"), chr & 0xf);				
				result += element;
				_stprintf(element, TEXT("%%%02X"), (chr >> 8) & 0xf);				
				result += element;					
#else
				_stprintf(element, TEXT("%%%02X"), chr);				
				result += element;
				chr = source[++i];
				_stprintf(element, TEXT("%%%02X"), chr);
				result += element;
#endif
			}
			else if (_istalpha(chr) || _istdigit(chr))	//alpha and digit.
			{
				result += chr;
			}
			else										//control char.
			{
				_stprintf( element, TEXT("%%%02X"), chr);
				result += element;
			}
		}
		return result;
	}
}