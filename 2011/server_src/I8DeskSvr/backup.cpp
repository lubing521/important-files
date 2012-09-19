#include "stdafx.h"
#include "backup.h"
#include <process.h>
#include <shellapi.h>
#include <ShlObj.h>
#include "../../include/Win32/FileSystem/FindFile.hpp"
#include "../../include/Win32/FileSystem/FileOperator.hpp"
#include "../../include/Extend STL/StringAlgorithm.h"

namespace i8desk
{
	struct CheckBackNumT
	{
		static const __int64 PerSecDay = (__int64)10000000 * 24 * 60 * 60;

		bool operator()(const filesystem::CFindFile &filefind, int backupNum)
		{
			// 文件最后写时间
			ULARGE_INTEGER time = {0};
			::memcpy(&time, &filefind.m_fd.ftLastWriteTime, sizeof(ULARGE_INTEGER));

			// 系统当前时间
			ULARGE_INTEGER now = {0};
			SYSTEMTIME st = {0};
			::GetSystemTime(&st);
			FILETIME f = {0};
			::SystemTimeToFileTime(&st, &f);
			::memcpy(&now, &f, sizeof(now));

			// 时间差
			__int64 span = now.QuadPart - time.QuadPart;

			if( span <= PerSecDay * backupNum )
				return true;

			return false;
		}
	};

	bool CBackup::StartBackup()
	{
		if ((m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL ||
			(m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL)))
		{
			return false;
		}
		return true;
	}

	void CBackup::StopBackup()
	{
		if (m_hExited != NULL)
		{
			SetEvent(m_hExited);
			if (m_hThread != NULL)
			{
				WaitForSingleObject(m_hThread, INFINITE);
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
			CloseHandle(m_hExited);
			m_hThread = m_hExited;
		}
	}

	DWORD CopyDir(LPCTSTR szSrcDir, LPCTSTR szDstDir)
	{
		SHCreateDirectory(NULL, _bstr_t(szDstDir));

		TCHAR path[MAX_PATH] = {0};
		SHFILEOPSTRUCT info = {0};
		ZeroMemory(&info, sizeof(info));
		_stprintf(path, TEXT("%s*.*"), szSrcDir);
		info.pFrom = path;
		info.pTo = szDstDir;
		info.wFunc = FO_COPY;
		info.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
		return SHFileOperation(&info);
	}

	void DeleteDir(LPCTSTR szDir)
	{
		TCHAR path[MAX_PATH] = {0};
		SHFILEOPSTRUCT info = {0};
		ZeroMemory(&info, sizeof(info));
		_stprintf(path, TEXT("%s*.*"), szDir);
		info.pFrom = path;
		info.wFunc = FO_DELETE;
		info.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
		SHFileOperation(&info);
		RemoveDirectory(szDir);
	}

	bool CBackup::ExecBackup()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("Start execute backup data..."));

		TCHAR root[MAX_PATH]		= {0};
 		TCHAR configfile[MAX_PATH]	= {0};
		TCHAR backupdir[MAX_PATH]	= {0};
		TCHAR backupfile[MAX_PATH]	= {0};
		TCHAR backuptmpdir[MAX_PATH]	= {0};

		GetModuleFileName(NULL, root, _countof(root));
		PathRemoveFileSpec(root);
		PathAddBackslash(root);
		_stprintf(configfile, TEXT("%sData\\backup.ini"), root);

		GetPrivateProfileString(TEXT("System"), TEXT("backupdir"), TEXT(""), backupdir, MAX_PATH, configfile);
		if (lstrlen(backupdir) == 0)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("exit backup because backupdir is empty."));
			return false;
		}
		PathAddBackslash(backupdir);
		SHCreateDirectory(NULL, _bstr_t(backupdir));

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		_stprintf(backupfile, TEXT("%sI8DeskBackup-%04d%02d%02d%02d%02d.zip"), 
			backupdir, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		_stprintf(backupdir,	TEXT("%sData\\"), root);
		_stprintf(backuptmpdir, TEXT("%sBackupTempDir\\"), root);
		DWORD dwRet = CopyDir(backupdir, backuptmpdir);
		if (dwRet)
		{
			_com_error Error(dwRet);
			m_pLogger->WriteLog(LM_INFO, TEXT("copy data to temp dir fail:%d:%s."), dwRet, Error.ErrorMessage());
			return false;
		}

		HZIP hZip = CreateZip(backupfile, 0, ZIP_FILENAME);
		if (hZip == 0)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("exit backup because open backup file fail:%s"), backupfile);
			return false;
		}

		if (!ZipData(hZip, backuptmpdir, lstrlen(backuptmpdir)))
		{
			CloseZip(hZip);
			m_pLogger->WriteLog(LM_INFO, TEXT("exit backup data..."));
			DeleteFile(backupfile);
			DeleteDir(backuptmpdir);
			return false;
		}
		CloseZip(hZip);
		DeleteDir(backuptmpdir);

		m_pLogger->WriteLog(LM_INFO, TEXT("backup data success."));
		return true;
	}

	bool CBackup::ZipData(HZIP hZip, LPCTSTR szDir, int nBase)
	{
		bool ret = true;
		TCHAR path[MAX_PATH] = {0};
		_stprintf(path, TEXT("%s*"), szDir);
		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(path, &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return false;

		while (FindNextFile(hFinder, &wfd) != 0)
		{
			if (lstrcmp(wfd.cFileName, TEXT(".")) == 0 || lstrcmp(wfd.cFileName, TEXT("..")) == 0)
				continue ;

			if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
				return false;

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				_stprintf(path, TEXT("%s%s"), szDir, wfd.cFileName);
				ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FOLDER);
				if (zr != ZR_OK)
				{
					TCHAR log[1024] = {0};
					FormatZipMessage(zr, log, _countof(log));
					m_pLogger->WriteLog(LM_INFO, TEXT("add data file fail.%s:%s"), &path[nBase], log);
					return false;
				}
				lstrcat(path, TEXT("\\"));
				ret = ZipData(hZip, path, nBase);
			}
			else
			{
				_stprintf(path, TEXT("%s%s"), szDir, wfd.cFileName);
				ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FILENAME);
				if (zr != ZR_OK)
				{
					TCHAR log[1024] = {0};
					FormatZipMessage(zr, log, _countof(log));
					m_pLogger->WriteLog(LM_INFO, TEXT("add data file fail.%s:%s"), &path[nBase], log);
					return false;
				}
			}
		}
		FindClose(hFinder);
		return ret;
	}

	bool CBackup::Judg(int hour, int minute)
	{
		TCHAR root[MAX_PATH]		= {0};
		TCHAR configfile[MAX_PATH]	= {0};
		TCHAR backupdir[MAX_PATH]	= {0};
		TCHAR backupfile[MAX_PATH]	= {0};

		GetModuleFileName(NULL, root, _countof(root));
		PathRemoveFileSpec(root);
		PathAddBackslash(root);
		_stprintf(configfile, TEXT("%sData\\backup.ini"), root);

		GetPrivateProfileString(TEXT("System"), TEXT("backupdir"), TEXT(""), backupdir, MAX_PATH, configfile);

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		_stprintf(backupfile, TEXT("%sI8DeskBackup-%04d%02d%02d%02d%02d.zip"), 
			backupdir, st.wYear, st.wMonth, st.wDay, hour, minute);

		if(PathFileExists(backupfile))
			return true;

		return false;
	}


	void CBackup::DeleteLog(int Day)
	{

		// 删除目标临时文件
		win32::file::detail::CheckFileTimeT checker;
		win32::file::detail::DeleteFileT deletor;

		stdex::tOstringstream Path;
		Path << utility::GetAppPath() << _T("Log\\*") ;


		filesystem::CFindFile file;
		BOOL suc = file.FindFile(Path.str().c_str());
		while(suc)
		{
			suc = file.FindNextFile();

			if( file.IsDots() )
				continue;
			
			if( checker(file) )
			{
				deletor(file.GetFilePath());
			}
		}

	}

	void CBackup::DeleteBack()
	{

		TCHAR root[MAX_PATH]		= {0};
		TCHAR configfile[MAX_PATH]	= {0};
		TCHAR backupdir[MAX_PATH]	= {0};
		TCHAR backupnum[MAX_PATH]	= {0};

		GetModuleFileName(NULL, root, _countof(root));
		PathRemoveFileSpec(root);
		PathAddBackslash(root);
		_stprintf(configfile, TEXT("%sData\\backup.ini"), root);

		GetPrivateProfileString(TEXT("System"), TEXT("backupdir"), TEXT(""), backupdir, MAX_PATH, configfile);

		filesystem::CFindFile file;

		stdex::tString path = backupdir;
		path +=  _T("\\I8DeskBackup-*.zip");
		BOOL bSuc = file.FindFile(path.c_str());

		std::vector<stdex::tString> vecfiles;
		
		while( bSuc )
		{
			stdex::tString filepath = file.GetFilePath();
			vecfiles.push_back(filepath);
			bSuc = file.FindNextFile();
		}

		GetPrivateProfileString(TEXT("System"), TEXT("backupnum"), TEXT(""), backupnum, MAX_PATH, configfile);
		size_t num = 0;
		stdex::ToNumber(num, backupnum);

		if(vecfiles.size() <= num)
			return;

		win32::file::detail::DeleteFileT deletor;
		
		for(std::vector<stdex::tString>::const_reverse_iterator iter = vecfiles.rbegin() + num; 
			iter != vecfiles.rend(); ++iter)
		{
			deletor(*iter);
		}
		


	}


	UINT __stdcall CBackup::WorkThread(LPVOID lpVoid)
	{
		CBackup* pThis = reinterpret_cast<CBackup*>(lpVoid);
		TCHAR configfile[MAX_PATH] = {0};
		GetModuleFileName(NULL, configfile, MAX_PATH);
		PathRemoveFileSpec(configfile);
		PathAddBackslash(configfile);
		lstrcat(configfile, TEXT("Data\\backup.ini"));
		TCHAR szDate[MAX_PATH] = {0};

		while (1)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 1000 * 30 ))
				break;

			GetPrivateProfileString(TEXT("System"), TEXT("backupdate"), TEXT(""), szDate, MAX_PATH, configfile);
			stdex::tString str(szDate);
			if (str.size() == 5 && str[2] == TEXT(':'))
			{
				int dwHour	 = _ttoi(str.substr(0, 2).c_str());
				int dwMinute = _ttoi(str.substr(3, 2).c_str());

				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				if (st.wHour == dwHour && st.wMinute == dwMinute )
				{
					if(!pThis->Judg(dwHour, dwMinute))
					{
						pThis->ExecBackup();

						pThis->DeleteBack();
					}

					pThis->DeleteLog(st.wDay);
				}
			}
		}

		return 0;
	}
}