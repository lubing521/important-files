#include "stdafx.h"

#include <time.h>
#include <process.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <atlenc.h>
#include <shellapi.h>
#include <atlenc.h>

#include "Tool.h"
#include "update.h"
#include "Markup.h"


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

#define PATH_ROOT		"%root%"			//表示应用程序安装目录
#define PATH_SYSTEM		"%system%"			//表示系统目录
#define PATH_PROGRAM	"%program%"			//表示C:\program file\\目录


#define _RUN_TMP_FILE_		".~tmp"			//更新正在占用的文件时，先改成一个临时文件.
#define _UPDATE_TMP_DIR_	"UpdateTemp"	//更新过程中，所有需要下载的文件的临时目录


namespace i8desk
{
	static DWORD Crc32Table[256] = {
		0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
		0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
		0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
		0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
		0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
		0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
		0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
		0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
		0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
		0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
		0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
		0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
		0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
		0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
		0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
		0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
		0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
		0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
		0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
		0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
		0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
		0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
		0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
		0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
		0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
		0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
		0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
		0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
		0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
		0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
		0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
		0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
		0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
		0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
		0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
		0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
		0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
		0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
		0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
		0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
		0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
		0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
		0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
		0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
		0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
		0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
		0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
		0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
		0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
		0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
		0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
		0x2d02ef8dL
	};

	static DWORD CalBufCRC32(BYTE* buffer, DWORD dwSize)
	{
		ULONG  crc(0xffffffff);
		int len;
		len = dwSize;
		while(len--)
			crc = (crc >> 8) ^ Crc32Table[(crc & 0xFF) ^ *buffer++];
		return crc^0xffffffff;
	}

	std::string _GetRoorDir()
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

	CAutoUpdate::CAutoUpdate(ILogger* pLogger, IPlugMgr* pPlugMgr,CHyOperator* pHy) : 
		m_pLogger(pLogger), m_pPlugMgr(pPlugMgr),m_pHy(pHy), m_hThread(NULL), m_hExited(NULL)
	{

	}
	
	CAutoUpdate::~CAutoUpdate()
	{
	}
	
	bool CAutoUpdate::StartUpdate()
	{
		if (m_pPlugMgr == NULL)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("IPlugMgr Pointer is null."));
			return false;
		}
		//ensure update temp dir exist.and clear all temp file.
		{
			std::string dir = _GetRoorDir() + "\\"_UPDATE_TMP_DIR_"\\";
			CreateDirectoryA(dir.c_str(), NULL);

// 			//删除一次更新临时目录下的所有的文件.
			tstring path= dir +"*.*";
			DelUpdateTmpFile(path);
			//删除原来被占用时，更新产生的临时文件.
			path= _GetRoorDir() +"\\*"+_RUN_TMP_FILE_;
			DelUpdateTmpFile(path);
		}

		m_pLogger->WriteLog(LM_INFO, TEXT("Start AutoUpdate...."));
		if (!GetFileList())
			return false;
		if (DownAllFile())
			return ExecUpdate();
		else
			return false;

		return true;
	}
	
	void CAutoUpdate::StopUpdate()
	{

	}
	
	UINT __stdcall CAutoUpdate::WorkThread(LPVOID lpVoid)
	{
		CAutoUpdate* pThis = reinterpret_cast<CAutoUpdate*>(lpVoid);

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
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Start AutoUpdate...."));
				pThis->GetFileList();
				if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;
 
				pThis->GetNeedUpdateList();
				if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;

				pThis->DownAllFile();
				if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;

				pThis->ExecUpdate();
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Stop AutoUpdate"));
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("WaitForMultipleObjects Error:%d:%d:%s"), dwRet, dwError, Error.ErrorMessage());
			}
		}

		pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Exit Update WorkThread."));
		return 0;
	}

	bool CAutoUpdate::GetFileList()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("Update I8Client Files."));
		tstring	strFilList = "\\BarOnlineUpdate\\Update.idx";
		DWORD bufersize = 0;
		byte* bufer = NULL;
 		if (DownloadFile(1, fprelPath, strFilList, (char**)&bufer, bufersize) == FALSE)
		{
			return false;
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("获取文件更新列表成功:%s.\r\n"), strFilList.c_str());

		tagIdxFile* pIdxFile = (tagIdxFile*)bufer ;		
		for (DWORD i=0;i<pIdxFile->dwFileCount;i++)
		{
			tagFileInfo FileInfo;

			tagFileItem* pFileItem= (tagFileItem*)(&pIdxFile->FileList[i]);		
			FileInfo.szFileName= tstring(pFileItem->FileName);
			ProccessFilePath(FileInfo.szFileName,FileInfo.szRelPath);

			DWORD dwCltFileCrc = GetFileCRC32(FileInfo.szFileName);
			if (pFileItem->dwFileCrc32 == dwCltFileCrc)
			{
				//CRC相同，如果这个文件是驱动，则进一步比较driver目录下的驱动文件的CRC是否相符
				char szFile[MAX_PATH] = {0};
				lstrcpy(szFile, FileInfo.szFileName.c_str());
				char* pFileName = PathFindFileName(szFile);
				char* pExtName  = PathFindExtension(szFile);
				if (pExtName == NULL || strcmpi(pExtName, ".sys") != 0)
					continue ; //不是驱动文件

				char szSys[MAX_PATH] = {0};
				GetSystemDirectory(szSys, MAX_PATH);
				PathAddBackslash(szSys);
				lstrcat(szSys, "drivers\\");
				lstrcat(szSys, pFileName);

				if (pFileItem->dwFileCrc32 == GetFileCRC32(std::string(szSys)))
					continue ;  //CRC也相同
			}
			//如果CRC不同，则需要更新，则处理相应的其它值。			
			{
				FileInfo.NeedUpdate = true;
				char   fname[MAX_PATH];
				char fext[MAX_PATH] ;
				_splitpath(pFileItem->FileName,NULL,NULL,fname,fext);
				_stprintf(fname,"%s%s",fname,fext);
				FileInfo.szTmpName = _GetRoorDir() + "\\"_UPDATE_TMP_DIR_"\\"+tstring(FileInfo.szRelPath);//生成每个文件下载的临时文件名.（先下载到临时目录，文件名不变）
				FileInfo.szRelPath = "\\BarOnlineUpdate\\"+ FileInfo.szRelPath;//服务端文件的相对路径
				m_lstFileList.push_back(FileInfo);
			}
		}
		//释放内存
		delete []bufer;
		return true;
	}
	
	void CAutoUpdate::GetNeedUpdateList()
	{

// 		m_pLogger->WriteLog(LM_INFO, TEXT("Get Need Update File List:"));
// 		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
// 		for (; it != m_lstFileList.end(); it ++)
// 		{
// 			if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
// 				break;
// 
// 			if (it->dwCmpType == CMP_FILE_CRC32)
// 			{
// 				TCHAR buf[20] = {0};
// 				sprintf(buf, "%X", GetFileCRC32(it->szFileName));				
// 				it->NeedUpdate = lstrcmpi(it->szParam1.c_str(), buf) != 0;
// 			}
// 			else if (it->dwCmpType == CMP_FILE_VERSION)
// 			{
// 				it->NeedUpdate = (GetFileVersion(it->szFileName) != it->szParam1);				
// 			}
// 			else if (it->dwCmpType == CMP_FILE_SIZE_MODTIME)
// 			{
// 				DWORD dwModTime = 0;
// 				DWORD dwSize = GetFileSizeModifyTime(it->szFileName, dwModTime);
// 				it->NeedUpdate = (atoi(it->szParam1.c_str()) != dwSize || atoi(it->szParam2.c_str()) != dwModTime);
// 			}
// 			
// 			if (it->NeedUpdate)
// 			{
// 				m_pLogger->WriteLog(LM_INFO, TEXT("File:%s"), it->szFileName.c_str());
// 			}
// 		}
// 		m_pLogger->WriteLog(LM_INFO, TEXT("End of Need Update File:"));
	}

	bool CAutoUpdate::DownAllFile()
	{
		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
		for (; it != m_lstFileList.end(); it ++)
		{
			if (!it->NeedUpdate)
				continue;
			m_pLogger->WriteLog(LM_INFO, TEXT("Start Download file:%s"), (LPCSTR)_bstr_t(it->szFileName.c_str()));
			if (!DownLoadFile(&(*it)))
				return false;
		}
		return true;

	}

	bool CAutoUpdate::ExecUpdate()
	{	
		if (m_lstFileList.size() == 0)
			return true;

		std::list<tagFileInfo>::iterator it = m_lstFileList.begin();
		for (; it != m_lstFileList.end(); it ++)
		{
			if (!it->NeedUpdate)
				continue ;
			if (!PathFileExists(it->szTmpName.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("download's tmp file is not exist:%s."), it->szTmpName.c_str());
				return false;
			}
			EnsureFileDirExist(it->szFileName);
			SetFileAttributes(it->szFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
			RemoveDirectory(it->szFileName.c_str());
			
			//如果是驱动文件，则拷贝一份到driver目录。
			if (it->szFileName.size()>4 && strcmpi(it->szFileName.substr(it->szFileName.size()-4, 4).c_str(), ".sys") == 0)
			{
				char szSysPath[MAX_PATH] = {0};
				GetSystemDirectory(szSysPath, MAX_PATH);
				_strupr(szSysPath);
				PathAddBackslash(szSysPath);
				lstrcat(szSysPath, "drivers");
				lstrcat(szSysPath, it->szTmpName.substr(it->szTmpName.rfind('\\')).c_str());
				CopyFileA(it->szTmpName.c_str(), szSysPath, FALSE);
				m_pLogger->WriteLog(LM_INFO, "Copy Sys to Drivers:%d", GetLastError());
				szSysPath[1] = 0;
				if (m_pHy->MountDosChar(szSysPath)) //如果加载虚拟设备成功才穿透。
				{
					szSysPath[1] = ':';
					std::string strSysPath = (szSysPath);
					char szTmp[MAX_PATH] = {0};
					_stprintf(szTmp, "\\\\.\\I8Desk%C", szSysPath[0]);
					strSysPath.replace(0, 2, szTmp);
					CopyFileA(it->szTmpName.c_str(), strSysPath.c_str(), FALSE);
					m_pLogger->WriteLog(LM_INFO, "Copy Sys to virtual device:%d", GetLastError());
					szSysPath[1] = 0;
					m_pHy->UMountDosChar(szSysPath);
				}
				else
				{
					m_pLogger->WriteLog(LM_INFO, "加载系统盘的虚拟设备失败.");
				}
			}

			char szDrv[MAX_PATH] = {0};
			sprintf(szDrv, "%c", toupper(it->szFileName[0]));
			bool bNeedInProtUpdate = m_pHy->MountDosChar(szDrv);
			if (bNeedInProtUpdate)
			{
				tstring strTmp("\\\\.\\");
				strTmp += m_pHy->GetDevicePath();
				tstring device= it->szFileName;
				device.replace(0, 2, strTmp);
				EnsureFileDirExist(device,strTmp.size());
				SetFileAttributes(it->szFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
				RemoveDirectory(device.c_str());
				BOOL ret = CopyFileA(it->szTmpName.c_str(), device.c_str(), FALSE);
				m_pLogger->WriteLog(LM_INFO, TEXT("copy %s :%d."), device.c_str(), GetLastError());
				m_pHy->UMountDosChar(szDrv);
				if (!ret)//拷贝虚拟设备失败，立即中止更新;
				{
					//过滤bho文件.他升级不成功，不当成失败
					std::string sub = it->szFileName.substr(it->szFileName.rfind('\\') + 1);
					if (strcmpi(sub.c_str(), "BHO_Goyoo.dll") != 0 &&
						strcmpi(sub.c_str(), "BBN_Goyoo.dll") != 0)
						return false;
				}
			}
			if (CopyFileA(it->szTmpName.c_str(), it->szFileName.c_str(), FALSE))
			{
//				nSucessUpdateFile++;
				m_pLogger->WriteLog(LM_INFO, TEXT("Copy File Success:%s."), it->szFileName.c_str());
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Copy File Fail:%s:%d:%s"), it->szFileName.c_str(), dwError, Error.ErrorMessage());

				std::string tmp = it->szFileName + ".~tmp";
				SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFileA(tmp.c_str());
				if (!MoveFileExA(it->szFileName.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING))
				{
					dwError = GetLastError();
					Error = _com_error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("Rename File:%s:%d:%s"), 
						it->szFileName.c_str(), dwError, Error.ErrorMessage());
				}
				if (!CopyFileA(it->szTmpName.c_str(), it->szFileName.c_str(), FALSE))
				{
					dwError = GetLastError();
					Error = _com_error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("ReCopy File::%s:%d:%s"), it->szFileName.c_str(), dwError, Error.ErrorMessage());
					
					//过滤bho文件.他升级不成功，不当成失败
					std::string sub = it->szFileName.substr(it->szFileName.rfind('\\') + 1);
					if (strcmpi(sub.c_str(), "BHO_Goyoo.dll") != 0 &&
						strcmpi(sub.c_str(), "BBN_Goyoo.dll") != 0)
						return false;
				}
				else
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("ReCopy File:%s:Sucess"), it->szFileName.c_str());
				}
			}
		}
		//re start self (by normal dos exe) to restart service.
//		if (nNeedUpdateFile && nNeedUpdateFile == nSucessUpdateFile)
		{
			std::string dir = _GetRoorDir() + "\\"_UPDATE_TMP_DIR_"\\*";
			DelUpdateTmpFile(dir);
			TCHAR path[MAX_PATH] = {0};
			path[0] = '"';
			GetModuleFileName(NULL, &path[1], MAX_PATH);
			lstrcat(path, TEXT("\" -AutoUpdate"));
			Sleep(200);
			STARTUPINFO si = {sizeof(si)};
			PROCESS_INFORMATION pi = {0};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
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
		return true;
	}

	std::string CAutoUpdate::GetFileVersion(const std::string& szFile)
	{
		char version[MAX_PATH] = "0.0.0.0";
		DWORD dwSize = GetFileVersionInfoSizeA(szFile.c_str(), NULL);
		if(dwSize)
		{
			char* pblock = new char[dwSize+1];
			GetFileVersionInfoA(szFile.c_str(), 0, dwSize, pblock);
			UINT nQuerySize;
			DWORD* pTransTable = NULL;
			VerQueryValueA(pblock, "\\VarFileInfo\\Translation", (void **)&pTransTable, &nQuerySize);
			LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			char SubBlock[MAX_PATH] = {0};
			sprintf(SubBlock, "\\StringFileInfo\\%08lx\\FileVersion", m_dwLangCharset);	
			LPSTR lpData;
			VerQueryValueA(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize);
			lstrcpyA(version, lpData);
			delete[] pblock;
		}
		std::string str(version);
		//replace ',' to '.'
		size_t pos = 0;
		while ( (pos = str.find(',', pos)) != std::string::npos)
		{
			str[pos] = '.';
		}

		return str;
	}
	
	DWORD CAutoUpdate::GetFileSizeModifyTime(const std::string& szFile, DWORD& dwModTime)
	{
		dwModTime = 0;
		HANDLE hFile = CreateFileA(szFile.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
// 			DWORD dwError = GetLastError();
// 			_com_error Error(dwError);
// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileSizeModifyTime CreateFile Error:%d:%s"), dwError, Error.ErrorMessage());
			return 0;
		}
		DWORD dwSize = GetFileSize(hFile, NULL);

		FILETIME ft = {0};
		if (!GetFileTime(hFile, NULL, NULL, &ft))
		{
// 			DWORD dwError = GetLastError();
// 			_com_error Error(dwError);
// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileSizeModifyTime GetFileTime Error:%d:%s"), dwError, Error.ErrorMessage());
			CloseHandle(hFile);
			return dwSize;
		}
		CloseHandle(hFile);

		//convert filetime to time_t
		SYSTEMTIME st = {0};
		FileTimeToSystemTime(&ft, &st);
		struct tm fTm = {0};
		fTm.tm_year = st.wYear - 1900;
		fTm.tm_mon = st.wMonth;
		fTm.tm_mday = st.wDay;
		fTm.tm_hour = st.wHour;
		fTm.tm_min = st.wMinute;
		fTm.tm_sec = st.wSecond;
		dwModTime = _mktime32(&fTm);
		return dwSize;
	}

	DWORD CAutoUpdate::GetFileCRC32(const std::string&  szFile)
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
// 			DWORD dwError = GetLastError();
// 			_com_error Error(dwError);
// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileCRC32 ReadFile Error:%d:%s"), dwError, Error.ErrorMessage());
 			CloseHandle(hFile);
			delete []pData;
			return 0;
		}
		
		DWORD dwCRC32 = CalBufCRC32(pData, dwSize);
		CloseHandle(hFile);
		delete []pData;
		return dwCRC32;
	}
	
	void CAutoUpdate::ProccessFilePath(std::string& src,std::string& relPath)
	{
		typedef std::string (*XFUNC)(void);
		struct _list
		{
			char*	find;
			XFUNC   xFunc;
		};
		_list xlist[] = 
		{
			{PATH_ROOT,		_GetRoorDir		},
			{PATH_SYSTEM,	_GetSystemDir	},
			{PATH_PROGRAM,	_GetProgramDir	}
		};
		
		for (int idx=0; idx<_countof(xlist); idx++)
		{
			std::string find = xlist[idx].find;
			int pos = 0;
			while ( (pos = src.find(find, pos)) != src.npos)
			{
				relPath = src.substr(pos+find.size()+1,src.size());				
				src.replace(pos, find.size(), xlist[idx].xFunc());				
			}
		}
	}

	bool CAutoUpdate::DownLoadFile(tagFileInfo* pFileInfo)
	{
//		return false;
		std::string szFile = pFileInfo->szTmpName;
		if (PathFileExistsA(szFile.c_str()))
		{
			SetFileAttributesA(szFile.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFileA(szFile.c_str());
		}
    	DWORD bufersize = 0;
		byte* bufer = NULL;
		if (DownloadFile(1, fprelPath, pFileInfo->szRelPath, (char**)&bufer, bufersize) == FALSE)
		{
			return false;
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("客户端更新文件下载成功:%s.\r\n"), pFileInfo->szRelPath.c_str());

		TCHAR path[MAX_PATH] = {0};
		lstrcpy(path,pFileInfo->szTmpName.c_str());
		PathRemoveFileSpec(path);
		PathAddBackslash(path);
		EnsureFileDirExist(path);
//		CreateDirectory(path,NULL);		
		WriteDataFile(pFileInfo->szTmpName, (char*)bufer, bufersize);
		delete []bufer;
		return true;
	}

	tstring CAutoUpdate::GetOpt(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue)
	{
		tstring szSysConfig = _GetRoorDir() + TEXT("\\i8desksys.config");
		tstring szDestFile  = _GetRoorDir() + TEXT("\\Data\\i8desksys.config.ini");

		SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szDestFile.c_str());

		HANDLE hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = GetFileSize(hFile, NULL);
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
			hFile = CreateFile(szDestFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SYSTEM);
				WriteFile(hFile, pDst, dwNewSize, &dwReadBytes, NULL);
				CloseHandle(hFile);

				TCHAR buf[4096] = {0};
				::GetPrivateProfileString(szSection, szKeyName, szDefValue, buf, 4096, szDestFile.c_str());
				DeleteFile(szDestFile.c_str());
				return tstring(buf);
			}
			delete []pSrc;
			delete []pDst;
		}
		return tstring(szDefValue);
	}

	void CAutoUpdate::EnsureFileDirExist(const std::string& file, int offset /* = 0 */)
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
	void CAutoUpdate::WriteDataFile(tstring FileName, char* data, DWORD datalength)
	{
		m_pLogger->WriteLog(LM_INFO, "Write offline file:%s", FileName.c_str());
		if (datalength == 0)
		{
			datalength = ((_packageheader*)data)->Length;
		}

		HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, "CreateFile Fail:%d:%s!", dwError, Error.ErrorMessage());
			return ;
		}

		DWORD dwByteWrite = 0;
		if (!WriteFile(hFile, data, datalength, &dwByteWrite, NULL) || dwByteWrite != datalength)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("WriteFile Fail:%d:%s!"), dwError, Error.ErrorMessage());
		}
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	bool CAutoUpdate::NeedInProtUpdate(tstring path)
	{
// 		if (m_pHy->IsInitialize())
// 		{
// 			if (m_pHy->IsInstallHy())
// 			{
// 				char device[MAX_PATH] = {0};
// 				path[1]= 0;
// 				sprintf(device, "I8Desk%c", path.c_str());
// 				if (m_pHy->IsDriverProtected((char*)path.c_str()) == 0)
// 				{	
// 					if (m_pHy->MountDosChar((char*)path.c_str(), device) == 0)
// 					{
// 						m_pLogger->WriteLog(LM_INFO, TEXT("load virtual device success:%s-%s."), path.c_str(), device);
// 						return true;
// 					}
// 					else
// 					{
// 						m_pLogger->WriteLog(LM_INFO, TEXT("load virtual device fail."));
// 					}
// 				}
// 				else
// 				{
// 					m_pLogger->WriteLog(LM_INFO, TEXT("driver is not protected."));
// 				}
// 			}
// 		}
		return false;
	}
	void CAutoUpdate::DelUpdateTmpFile(std::string& FilePath)
	{
		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(FilePath.c_str(), &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return ;
		TCHAR path[MAX_PATH] = {0};
		lstrcpy(path, FilePath.c_str());
		PathRemoveFileSpec(path);
		PathAddBackslash(path);	
		do 
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
			
				tstring file =tstring(path);
				file+= wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);				
				DeleteFile(file.c_str());
			}

		} while(FindNextFile(hFinder, &wfd) != 0);
		FindClose(hFinder);
		return ;
	}

	BOOL CAutoUpdate::DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen )
	{
		char buf[1024] = {0};
		DWORD dwIndex  = 0;
		DWORD dwFileSize = 0;
		DWORD dwLastFlag = 0;
		DWORD dwCurrentSize = 0;
		char *pData = NULL;
		int nSize = 0;
		_packageheader* pheader = (_packageheader*)buf;
		ZeroMemory(pheader, sizeof(_packageheader));
		pheader->StartFlag = START_FLAG;
		pheader->Cmd       = CMD_CLIUPT_DOWNFILESIZE;
		pheader->Length    = sizeof(_packageheader) ;
		pheader->Version   = PRO_VERSION;
		pheader->Resv[0]   = OBJECT_CLIENTSVI;
		CPackageHelper outpackage(buf);
		outpackage.pushDWORD(dwProgramType);
		outpackage.pushDWORD(dwPathType);
		outpackage.pushString((char*)strRemoteFileName.c_str(), strRemoteFileName.length());
		pheader->Length = outpackage.GetOffset();

		if (!m_pPlugMgr->GetCommunication()->ExecCmd(buf, pheader->Length, pData, nSize))
		{
			m_pLogger->WriteLog(LM_INFO, "请求获取下载文件大小失败\r\n");
			return FALSE;
		}       
		//获取文件大小
		CPackageHelper inpackage(pData);
		if (inpackage.popDWORD() != CMD_RET_SUCCESS)
		{
			return FALSE;
		}
		dwFileSize = inpackage.popDWORD();
		CoTaskMemFree(pData);
		*pRecvBuffer = new char[dwFileSize + 1];
		//下载文件数据
		while (dwIndex < dwFileSize)
		{
			pheader = (_packageheader*)buf;
			ZeroMemory(pheader, sizeof(_packageheader));
			pheader->StartFlag = START_FLAG;
			pheader->Cmd       = CMD_CLIUPT_DOWNFILECONTENT;
			pheader->Length    = sizeof(_packageheader) ;
			pheader->Version   = PRO_VERSION;
			pheader->Resv[0]   = OBJECT_CLIENTSVI;
			CPackageHelper outpackage(buf);
			pheader->Length = outpackage.GetOffset();

			if (!m_pPlugMgr->GetCommunication()->ExecCmd(buf, pheader->Length, pData, nSize))
			{
				m_pLogger->WriteLog(LM_INFO, "请求获取下载文件内容失败\r\n");
				delete [](*pRecvBuffer);
				return FALSE;
			}  
			CPackageHelper inpackage(pData);
			if (inpackage.popDWORD() != CMD_RET_SUCCESS)
			{
				delete [](*pRecvBuffer);
				return FALSE;
			}
			dwLastFlag = inpackage.popDWORD();
			dwCurrentSize = inpackage.popString(*pRecvBuffer + dwIndex, FALSE);
			dwIndex += dwCurrentSize;
			CoTaskMemFree(pData);
			//最后一包，结束。
			if (dwLastFlag == 1) break;
			//延时处理
			Sleep(50);
		}

		//正确性校验
		if (dwIndex != dwFileSize)
		{
			m_pLogger->WriteLog(LM_INFO, "下载文件出现错误，服务器返回文件大小:%ld字节，客户端接收到%ld字节", dwFileSize, dwIndex);
			delete [](*pRecvBuffer);
			return FALSE;
		}
		dwRecvLen = dwFileSize;
		return TRUE;
	}
}