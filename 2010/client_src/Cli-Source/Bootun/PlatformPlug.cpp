#include "StdAfx.h"
#include <process.h>
#include "..\Include\plug.h"
#include "PlatformPlug.h"
#include "Tool.h"
#include <Userenv.h>

namespace i8desk
{


	CPlatformPlug::CPlatformPlug(void)
	{	
		m_hUpdateModule = NULL;
	}

	CPlatformPlug::~CPlatformPlug(void)
	{
		if (m_hUpdateModule)
		{
			FreeLibrary(m_hUpdateModule);
		}
	}

	BOOL CPlatformPlug::Start( ILogger* pLogger, IPlugMgr* pPlugMgr, HANDLE hExit, CHyOperator* pRestore )
	{
		m_pLogger = pLogger;
		m_pPlugMgr = pPlugMgr;
		m_hExit = hExit;
		m_pRestore = pRestore;

		char szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		PathAddBackslash(szPath);
		m_strCurrentDir = szPath;

		//加载更新组件
		string strUpdateDll = m_strCurrentDir + TEXT("UpdateGameCli.dll");
		m_hUpdateModule = LoadLibrary(strUpdateDll.c_str());
		if (m_hUpdateModule)
		{
			FastCopy_Start = (FASTCOPY_START)GetProcAddress(m_hUpdateModule, (char*)1);
			FastCopy_End = (FASTCOPY_END)GetProcAddress(m_hUpdateModule, (char*)2);
			FastCopy_GetSpeedEx = (FASTCOPY_GETSPEEDEX)GetProcAddress(m_hUpdateModule, (char*)4);
		}

		if (m_hUpdateModule == NULL
			|| FastCopy_Start == NULL
			|| FastCopy_End == NULL
			|| FastCopy_GetSpeedEx == NULL)
		{
			return FALSE;
		}

		CHAR    szFileName[MAX_PATH] = {0};
		SDG_GetAppPath(szFileName, TRUE);
		lstrcat(szFileName, "gameclient.ini");
		char ip[MAX_PATH] = {0};
		GetPrivateProfileString("SystemSet", "ServerAddr", "", ip, MAX_PATH, szFileName);
		m_strMajorSvrIp = ip;

		m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, _WorkThread, this, 0, NULL);
		return TRUE;
	}


	UINT __stdcall CPlatformPlug::_WorkThread( PVOID lpVoid )
	{
		CPlatformPlug* pPlatformPlug = (CPlatformPlug*)lpVoid;
		return pPlatformPlug->WorkThread();
	}

	UINT CPlatformPlug::WorkThread()
	{
		if (!DownloadConfig() || !ParserConfig())
			return 0;

		int nIdx = 0, nPlugCount = m_vtPlugInfo.size();
		while (nIdx < nPlugCount)
		{
			if (WaitForSingleObject(m_hExit, 0) == WAIT_OBJECT_0)
				return TRUE;

			if (!m_vtPlugInfo[nIdx].bEnable)
			{
				//删除
				DeleteDirectory(m_vtPlugInfo[nIdx].szClientDir);
				nIdx++;
				continue;
			}
			//下载插件
			DWORD hUpdate = FastCopy_Start(m_vtPlugInfo[nIdx].szServerDir, m_vtPlugInfo[nIdx].szClientDir, m_strMajorSvrIp.c_str(), NULL,m_vtPlugInfo[nIdx].dwGameId,  ::FC_COPY_VERSION, 0, 0);
			while (TRUE)
			{
				if (WaitForSingleObject(m_hExit, 1000) == WAIT_OBJECT_0)
					return TRUE;

				FastCopyStatus Status = {0};
				FastCopy_GetSpeedEx(hUpdate, &Status);

				if (FCS_FINISH == Status.Progress)
				{
					FastCopy_End(hUpdate);	
					ExecClient(m_vtPlugInfo[nIdx]);
					break;
				}
				else if (FCS_ERROR == Status.Progress || FCS_NOROOM == Status.Progress)
				{
					FastCopy_End(hUpdate);
					m_pLogger->WriteLog(LM_INFO, TEXT("插件目录%s下载失败，失败原因：%s"), m_vtPlugInfo[nIdx].szServerDir, Status.szCurFile);
					break;
				}
				else continue;
			}
			nIdx++;
		}

		return 1;
	}

	BOOL CPlatformPlug::DownloadConfig()
	{
		string strSeverPath, strClientPath;
		strSeverPath  = string(TEXT("plugin_tools\\")) + PLATFORMPLUG_CONFIG_FILENAME;
		strClientPath = m_strCurrentDir + PLATFORMPLUG_CONFIG_FILENAME;
		//判断是否需要更新
		BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)strSeverPath.c_str(), (char*)strClientPath.c_str());
		if (bIsNeedUpdate)
		{
			DWORD bufersize = 0;
			byte* bufer = NULL;
			if (DownloadFile(1, fprelPath, strSeverPath, (char**)&bufer, bufersize) == FALSE)
			{
				return false;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("配置文件下载成功:%s.\r\n"), strClientPath.c_str());
			char DataFolder[MAX_PATH] = {0};
			lstrcpy(DataFolder, strClientPath.c_str());
			PathRemoveFileSpec(DataFolder);
			PathAddBackslash(DataFolder);
			CreateDirectory(DataFolder,NULL);
			WriteDataFile(strClientPath,(char*)bufer,false,bufersize);
			delete[]bufer;
		}
		return TRUE;
	}

	BOOL CPlatformPlug::ParserConfig()
	{
		string strXmlPath = m_strCurrentDir + PLATFORMPLUG_CONFIG_FILENAME;
		try
		{
			CMarkup xml;
			if (!xml.Load(strXmlPath.c_str()))
			{
				return FALSE;
			}

			xml.ResetPos();
			if (!xml.FindElem(TEXT("main"))
				|| !xml.IntoElem()
				|| !xml.FindElem(TEXT("products")))
			{
				return FALSE;
			}
			xml.IntoElem();

			while (xml.FindElem(TEXT("product")))
			{
				STPlugInfo stPlugInfo;
				string strText = xml.GetAttrib(TEXT("gid"));
				
				stPlugInfo.dwGameId = _ttoi(strText.c_str());
				xml.IntoElem();

				xml.FindElem(TEXT("base"));
				xml.IntoElem();		
				
				xml.FindElem(TEXT("name"));
				strText = m_strCurrentDir + TEXT("plugin_tools\\");
				strText += xml.GetData();
				strText += TEXT("\\");
				strcpy_s(stPlugInfo.szClientDir, MAX_PATH, strText.c_str());

				xml.FindElem(TEXT("status"));		
				strText = xml.GetData();
				stPlugInfo.bEnable = _ttoi(strText.c_str())==1?TRUE:FALSE;
				xml.OutOfElem();

				xml.FindElem(TEXT("client"));
				xml.IntoElem();

				xml.FindElem(TEXT("exe"));
				strcpy_s(stPlugInfo.szExeName, MAX_PATH, xml.GetData().c_str());

				xml.FindElem(TEXT("param"));
				strcpy_s(stPlugInfo.szExeParam, MAX_PATH, xml.GetData().c_str());

				xml.FindElem(TEXT("path"));
				strcpy_s(stPlugInfo.szServerDir, MAX_PATH,  xml.GetData().c_str()); 

				xml.FindElem(TEXT("runtype"));
				stPlugInfo.dwRunType = _ttoi(xml.GetData().c_str());    

				//退出client
				xml.OutOfElem();

				m_vtPlugInfo.push_back(stPlugInfo);

				//退出product
				xml.OutOfElem();
			}
		}
		catch (...)
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL CPlatformPlug::ExecClient( STPlugInfo stPlugInfo )
	{
		char szRunGamePath[MAX_PATH] = {0};
		szRunGamePath[0] = '\"';

		GetModuleFileName(NULL, &szRunGamePath[1], MAX_PATH);
		PathRemoveFileSpec(szRunGamePath);
		PathAddBackslash(szRunGamePath);
		lstrcat(szRunGamePath, "RunGame.exe\" |");
	    lstrcat(szRunGamePath, stPlugInfo.szClientDir);
		lstrcat(szRunGamePath, stPlugInfo.szExeName);
		lstrcat(szRunGamePath, "|");
		if (lstrlen(stPlugInfo.szExeParam) > 0)
		{			
			lstrcat(szRunGamePath, stPlugInfo.szExeParam);
		}

		RunProcess(szRunGamePath, szRunGamePath);
		return TRUE;
	}

	BOOL CPlatformPlug::DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen )
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

	void CPlatformPlug::WriteDataFile( string FileName, char* data,bool bInprotect, DWORD datalength /*= 0*/ )
	{
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
		else
		{
			if (bInprotect)
			{
				bool bNeedInProtUpdate = m_pRestore->MountDosChar((char*)(FileName.c_str()));
				if (bNeedInProtUpdate)
				{				
					tstring strTmp("\\\\.\\");
					strTmp += m_pRestore->GetDevicePath();
					tstring device= FileName;
					device.replace(0, 2, strTmp);

					BOOL ret = CopyFileA(FileName.c_str(), device.c_str(), FALSE);
					if (!ret)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("copy %s :%d."), device.c_str(), GetLastError());
					}				
					m_pRestore->UMountDosChar((char*)(FileName.c_str()));
				}
			}			
		}

		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}

	BOOL CPlatformPlug::FileIsNeedUpdate( DWORD dwProgramType, DWORD dwPathType, LPSTR lpRemoteFileName, LPSTR lpLocalFilePath /*= NULL*/ )
	{
		return TRUE;
		//定义变量
		BOOL	bNeedUpdate = FALSE;
		char    szSendBuffer[1024] = {0};
		char*   pRecvBuffer = NULL;		
		DWORD   dwRemoteFileSize = 0;
		DWORD   dwLocalFileSize  = 0;
		DWORD   dwRemoteFileCRC  = 0;
		DWORD   dwLocalFileCRC   = 0;
		HANDLE  hLocalFile  = INVALID_HANDLE_VALUE;
		int     nRecvSize   = 0;
		_packageheader* pPacketHeader = NULL;
		//本地文件不存在，必然需要更新
		if(lpLocalFilePath == NULL) return TRUE;
		//获取本地文件信息
		//打开文件			
		hLocalFile = ::CreateFile(lpLocalFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLocalFile != INVALID_HANDLE_VALUE)
		{
			//获取文件大小
			dwLocalFileSize = GetFileSize(hLocalFile, NULL);
			if(dwLocalFileSize == INVALID_FILE_SIZE)
			{
				//文件大小获取失败
				CloseHandle(hLocalFile);
				return TRUE;
			}
			CloseHandle(hLocalFile);
			//获取文件CRC
			dwLocalFileCRC = GetFileCRC32(lpLocalFilePath);
		}
		else return TRUE;
		//构建获取文件索引命令
		_packageheader* pheader = (_packageheader*)szSendBuffer;
		ZeroMemory(pheader, sizeof(_packageheader));
		pheader->StartFlag = START_FLAG;
		pheader->Cmd       = CMD_CLIUPT_DOWNFILEINFO;
		pheader->Length    = sizeof(_packageheader) ;
		pheader->Version   = PRO_VERSION;
		pheader->Resv[0]   = OBJECT_CLIENTSVI;
		CPackageHelper SendPacket(szSendBuffer);
		SendPacket.pushDWORD(dwProgramType);
		SendPacket.pushDWORD(dwPathType);
		SendPacket.pushString(lpRemoteFileName, strlen(lpRemoteFileName));
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!m_pPlugMgr->GetCommunication()->ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取文件索引信息失败\r\n");
			return TRUE;
		}
		//解析命令
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
			dwRemoteFileSize = RecvPacket.popDWORD();
			dwRemoteFileCRC  = RecvPacket.popDWORD();
			//比对信息
			if (dwRemoteFileSize != dwLocalFileSize || dwRemoteFileCRC != dwLocalFileCRC)
			{
				bNeedUpdate = TRUE;
			}
		}
		CoTaskMemFree(pRecvBuffer);
		return bNeedUpdate;
	}

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
	DWORD CPlatformPlug::GetFileCRC32( const std::string& szFile )
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

	BOOL CPlatformPlug::DeleteDirectory( char *pszDir )
	{
		char szDriver[MAX_PATH] = {0};
		szDriver[0] = pszDir[0];

		__int64 iDelSize = 0;
		SDG_DeleteDirectory(pszDir, &iDelSize);

		if (m_pRestore->MountDosChar(szDriver))
		{
			string dir(pszDir);
			__int64 iVDelSize = 0;
			dir.replace(0, 2, TEXT("I8Desk"));
			dir.insert(0, "\\\\.\\");
			SDG_DeleteDirectory(pszDir, &iVDelSize);
			m_pRestore->UMountDosChar(szDriver);
		}

		return TRUE;
	}

	BOOL CPlatformPlug::GetTokenByName(HANDLE &hToken,LPSTR lpName)
	{
		if(!lpName)
		{
			return FALSE;
		}
		HANDLE hProcessSnap = NULL; 
		BOOL bRet = FALSE; 
		PROCESSENTRY32 pe32 = {0}; 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
		if (hProcessSnap == INVALID_HANDLE_VALUE) 
			return (FALSE); 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		if (Process32First(hProcessSnap, &pe32)) 
		{ 
			do 
			{
				if(strcmpi(pe32.szExeFile, lpName) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
						FALSE,pe32.th32ProcessID);
					BOOL bRet = FALSE;
					if (hProcess != NULL)
					{
						if (OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken))
							bRet = TRUE;
						CloseHandle (hProcessSnap); 
					}
					return (bRet);
				}
			} 
			while (Process32Next(hProcessSnap, &pe32)); 
			bRet = TRUE; 
		} 
		else 
			bRet = FALSE;
		CloseHandle (hProcessSnap); 
		return (bRet);
	}

	BOOL CPlatformPlug::RunProcess( LPCSTR lpImage,LPCSTR lpPara )
	{
		if(!lpImage)
		{
			return FALSE;
		}
		HANDLE hToken;
		if(!GetTokenByName(hToken,"explorer.exe"))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("OpenProcessToken Faile!"));
			return FALSE;
		}
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		si.cb= sizeof(STARTUPINFO);
		si.lpDesktop = TEXT("winsta0\\default");
		LPVOID lpEnv = NULL;
		CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
		BOOL bResult = CreateProcessAsUser(hToken,NULL,(LPSTR)lpPara,NULL,NULL,
			FALSE,CREATE_UNICODE_ENVIRONMENT,lpEnv, NULL, &si, &pi);
		CloseHandle(hToken);
		if(bResult)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser ok:%s"), lpPara);
		}
		else
		{
			DWORD dwError = GetLastError();
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser false:%d:%s"), dwError, lpPara);
		}
		if (lpEnv)
			DestroyEnvironmentBlock(lpEnv);
		return bResult;
	}












}