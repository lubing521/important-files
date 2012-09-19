#include "stdafx.h"
#include "backup.h"
#include <process.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <time.h>
#include <ShellAPI.h>
#include "XUnzip.h"

#include "../../include/Utility/FileOperator.hpp"


namespace i8desk
{
	bool CBackup::StartBackup()
	{
		if( !backupTimer_.Call(backupDelayTime, backupPeriodTime) )
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_ERROR, TEXT("Create backup data thread erorr:%d:%s"), dwError, Error.ErrorMessage());
		
			return false;
		}

		/*if( !cleanTimer_.Call(cleanDelayTime, cleanPeriodTime) )
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_ERROR, TEXT("Create clean data thread erorr:%d:%s"), dwError, Error.ErrorMessage());

			return false;
		}*/

		return true;
	}

	void CBackup::StopBackup()
	{
		backupTimer_.Cancel();
		//cleanTimer_.Cancel();
	}

	DWORD CopyDir(LPCSTR szSrcDir, LPCSTR szDstDir)
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

	void DeleteDir(LPCSTR szDir)
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
		if (m_pLogger)
			m_pLogger->WriteLog(LM_INFO, "Start execute backup data...");

		char root[MAX_PATH]			= {0};
 		char configfile[MAX_PATH]	= {0};
		char backupdir[MAX_PATH]	= {0};
		char backupfile[MAX_PATH]	= {0};
		char backuptmpdir[MAX_PATH]	= {0};

		GetModuleFileName(NULL, root, MAX_PATH);
		PathRemoveFileSpec(root);
		PathAddBackslash(root);
		sprintf_s(configfile, "%sData\\backup.ini", root);

		GetPrivateProfileString("System", "backupdir", "", backupdir, MAX_PATH, configfile);
		if (lstrlen(backupdir) == 0)
		{
			if (m_pLogger)
				m_pLogger->WriteLog(LM_INFO, "exit backup because backupdir is empty.");
			return false;
		}
		PathAddBackslash(backupdir);
		SHCreateDirectory(NULL, _bstr_t(backupdir));

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		sprintf_s(backupfile, "%sI8DeskBackup-%04d%02d%02d%02d%02d%02d.zip", 
			backupdir, st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond);
		sprintf_s(backupdir, "%sData\\", root);
		sprintf_s(backuptmpdir, "%sBackupTempDir\\", root);
		DWORD dwRet = CopyDir(backupdir, backuptmpdir);
		if (dwRet)
		{
			_com_error Error(dwRet);
			if (m_pLogger)
				m_pLogger->WriteLog(LM_INFO, "copy data to temp dir fail:%d:%s.", dwRet, Error.ErrorMessage());
			return false;
		}

		HZIP hZip = CreateZip(backupfile, 0, ZIP_FILENAME);
		if (hZip == 0)
		{
			if (m_pLogger)
				m_pLogger->WriteLog(LM_INFO, "exit backup because open backup file fail:%s", backupfile);
			return false;
		}

		if (!ZipData(hZip, backuptmpdir, lstrlen(backuptmpdir)))
		{
			CloseZip(hZip);
			if (m_pLogger)
				m_pLogger->WriteLog(LM_INFO, "exit backup data...");
			DeleteFile(backupfile);
			DeleteDir(backuptmpdir);
			return false;
		}
		CloseZip(hZip);
		DeleteDir(backuptmpdir);

		if (m_pLogger)
			m_pLogger->WriteLog(LM_INFO, "backup data success.");
		return true;
	}

	bool CBackup::ZipData(HZIP hZip, LPCSTR szDir, int nBase)
	{
		bool ret = true;
		char path[MAX_PATH] = {0};
		sprintf_s(path, "%s*", szDir);
		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(path, &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return false;

		while (FindNextFile(hFinder, &wfd) != 0)
		{
			if (lstrcmp(wfd.cFileName, ".") == 0 || lstrcmp(wfd.cFileName, "..") == 0)
				continue ;

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sprintf_s(path, "%s%s", szDir, wfd.cFileName);
				ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FOLDER);
				if (zr != ZR_OK)
				{
					char log[1024] = {0};
					FormatZipMessage(zr, log, sizeof(log));
					m_pLogger->WriteLog(LM_INFO, "add data file fail.%s:%s", &path[nBase], log);
					return false;
				}
				lstrcat(path, "\\");
				ret = ZipData(hZip, path, nBase);
			}
			else
			{
				sprintf_s(path, "%s%s", szDir, wfd.cFileName);
				ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FILENAME);
				if (zr != ZR_OK)
				{
					char log[1024] = {0};
					FormatZipMessage(zr, log, sizeof(log));
					m_pLogger->WriteLog(LM_INFO, "add data file fail.%s:%s", &path[nBase], log);
					return false;
				}
			}
		}
		FindClose(hFinder);
		return ret;
	}

	void CBackup::_Backup(BOOLEAN outTime)
	{
		char configfile[MAX_PATH] = {0};
		GetModuleFileName(NULL, configfile, MAX_PATH);
		PathRemoveFileSpec(configfile);
		PathAddBackslash(configfile);
		lstrcat(configfile, "Data\\backup.ini");
		char szDate[MAX_PATH] = {0};

		if( outTime )
		{
			GetPrivateProfileString("System", "backupdate", "", szDate, MAX_PATH, configfile);
			std::string str(szDate);
			if (str.size() == 5 && str[2] == ':')
			{
				int dwHour = atoi(str.substr(0, 2).c_str());
				int dwMinute = atoi(str.substr(3, 2).c_str());

				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				if (st.wHour == dwHour && st.wMinute == dwMinute )
				{
					m_pLogger->WriteLog(LM_DEBUG, _T("正在数据备份..."));
					if( ExecBackup() )
					{
						char szValue[20] = {0};
						sprintf_s(szValue, sizeof(szValue), "%d", _time32(NULL));
						WritePrivateProfileString("System", "LastBackupDate", szValue, configfile);
					}
				}
			}
		}
	}

	void CBackup::_Clean(BOOLEAN outTime)
	{
		if( !outTime )
			return;

		m_pLogger->WriteLog(LM_DEBUG, _T("正在清除过期数据备份..."));
		std_string path = GetAppPath() + _T("Data\\backup.ini");

		TCHAR backupTmp[MAX_PATH] = {0};
		::GetPrivateProfileString(_T("System"), _T("backupdir"), _T(""), backupTmp, MAX_PATH, path.c_str());
		std_string backupDir(backupTmp);

		using namespace utility;
		static FilesOperator<CheckFileTimeT, DeleteFileT> op;
		op(backupDir);

		//m_pLogger->WriteLog(LM_INFO, _T("已成功删除备份文件"));
	}
}