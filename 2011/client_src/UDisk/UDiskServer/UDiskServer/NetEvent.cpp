#include "StdAfx.h"
#include "NetEvent.h"
#include "UDiskServer.h"

extern  CUDiskServerApp theApp;

DWORD MakeTimeFromFileTime(const FILETIME& ft)
{
	SYSTEMTIME st = {0};
	 
	FileTimeToSystemTime(&ft, &st);
	struct tm stm = {0};
	stm.tm_year = st.wYear - 1900;
	stm.tm_mon  = st.wMonth - 1;
	stm.tm_mday = st.wDay;
	stm.tm_hour = st.wHour;
	stm.tm_min  = st.wMinute;
	stm.tm_sec  = st.wSecond;
	DWORD dwTime =  (DWORD)mktime(&stm);
	return dwTime;

}
CNetEvent::CNetEvent(void)
{

	m_pConfigInfo = theApp.m_pConfigInfo;
}

CNetEvent::~CNetEvent(void)
{
}
void CNetEvent::Release()
{
delete this;
}

//当客户端连接上来时，产生该事件
void CNetEvent::OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param) 
{

}
//当服务端发客户端发送完成一个数据包时产生该事件
void CNetEvent::OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) 
{

}

//当服务端接收到客户端一个完整命令包时，产生该事件给应用层处理一个命令
void CNetEvent::OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) 
{

	CPackageHelper inpack((LPSTR)lpPackage);
	if(((_packageheader*)lpPackage)->StartFlag != 0x5e7d)
		return ;
	switch(((_packageheader*)lpPackage)->Cmd)
	{
	case NET_CMD_REGISTCLIENT:
		{
			GetDiskFreeSize();
			char buf[MAX_PATH] = {0};
			inpack.popString(buf);
			char* ptr = strstr(buf,"|");
			if(ptr == NULL)return;
			*ptr = '\0';
			char* password = ++ptr;
			if(m_pConfigInfo->bRegist)
			{
				string str;
				RegisteClient(buf,password,str);
				SendData(sck,pNetLayer,NET_CMD_SUCESS,str.c_str(),str.size());
			}
			else
			{
				SendData(sck,pNetLayer,NET_CMD_FAIL,m_pConfigInfo->szStopRigTitle.c_str(),m_pConfigInfo->szStopRigTitle.size());
			}
			break;
		}
	case NET_CMD_POSTBLOCKFILE:
		{	
			
			WriteBlockFile((LPSTR)lpPackage);
			break;
		}
	case NET_CMD_LOG:
		{
			char buf[MAX_PATH]={0};
			inpack.popString(buf);
			if(!LogVaild(buf))
			{
				string str("用户名密码不对");
				SendData(sck,pNetLayer,NET_CMD_FAIL,str.c_str(),str.size());
			}
			else
			{
				SendData(sck,pNetLayer,NET_CMD_SUCESS);
			}
			break;
		}
	case NET_CMD_GETUDISKINFO://得到upan上文件列表
		{
			char path[MAX_PATH] = {0};
			char buf[64] = {0};
			inpack.popString(buf);
			sprintf_s(path,"%s%s\\u盘\\",m_pConfigInfo->szSavePath.c_str(),buf);
			vector<FileInfo> vecfileLst;
			ScanFile(path,vecfileLst);
			DWORD dwsize = (vecfileLst.size()*64);
			char*pData = pNetLayer->GetIMemPool()->Alloc(dwsize);
			CPackageHelper outpack(pData);
			outpack.pushDWORD(vecfileLst.size());
			for(size_t i =0; i<vecfileLst.size();i++)
			{
				outpack.pushString(vecfileLst[i].filename.c_str(),vecfileLst[i].filename.size());
				outpack.pushDWORD(vecfileLst[i].filesize);
				outpack.pushDWORD(vecfileLst[i].dwTime);
			}
			_packageheader header ={0};
			header.Cmd = NET_CMD_GETUDISKINFO;
			header.StartFlag = 0x5e7d;
			header.Length = outpack.GetOffset();
			outpack.pushPackageHeader(header);
			pNetLayer->SendPackage(sck,pData,header.Length);
			break;
		}

	case MET_CMD_GETBLCOKFILE:
		{
			SendBlcokFile(pNetLayer,sck,lpPackage);
			break;
		}
	case NET_CMD_DELETEFILE:
		{
			char buf[MAX_PATH] = {0};
			inpack.popString(buf);
			string file = m_pConfigInfo->szSavePath + string(buf);
			SetFileAttributes(file.c_str(),FILE_ATTRIBUTE_NORMAL);
			DeleteFile(file.c_str());
			break;
		}
	case NET_CMD_GETCOMMENTRES:
		{

			char path[MAX_PATH] = {0};
		
			if(m_pConfigInfo->bShowCommres && m_pConfigInfo->szCommResPath.size())
				sprintf_s(path,"%s\\",m_pConfigInfo->szCommResPath.c_str());
			else
				ZeroMemory(path,MAX_PATH);

			vector<FileInfo> vecfileLst;
			ScanFile(path,vecfileLst);
			DWORD dwsize = (vecfileLst.size()*64);
			char*pData = pNetLayer->GetIMemPool()->Alloc(dwsize);
			CPackageHelper outpack(pData);
			outpack.pushDWORD(vecfileLst.size());
			for(size_t i =0; i<vecfileLst.size();i++)
			{
				outpack.pushString(vecfileLst[i].filename.c_str(),vecfileLst[i].filename.size());
				outpack.pushDWORD(vecfileLst[i].filesize);
				outpack.pushDWORD(vecfileLst[i].dwTime);
			}
		
			_packageheader header ={0};
			header.Cmd = m_pConfigInfo->bShowCommres?NET_CMD_GETUDISKINFO:NET_CMD_FAIL;
			header.StartFlag = 0x5e7d;
			header.Length = outpack.GetOffset();
			outpack.pushPackageHeader(header);
			pNetLayer->SendPackage(sck,pData,header.Length);
			break;
		}
	case NET_CMD_RESETPASSWORD:
		{
			char buf[MAX_PATH] = {0};
			inpack.popString(buf);
			if(ReSetPassword(buf))
			{
				string str("密码修改成功");
				SendData(sck,pNetLayer,NET_CMD_SUCESS,str.c_str(),str.length());
			}
			else
			{
				string str("密码修改失败");
				SendData(sck,pNetLayer,NET_CMD_FAIL,str.c_str(),str.length());
			}
			break;
		}
	case NET_CMD_GETCONFIGINFO:
		{
			char* pData = pNetLayer->GetIMemPool()->Alloc();
			CPackageHelper inpack(pData);
			inpack.pushString(m_pConfigInfo->sztitle.c_str(),m_pConfigInfo->sztitle.size());
			inpack.pushString(m_pConfigInfo->szSavePath.c_str(),m_pConfigInfo->szSavePath.size());
			inpack.pushDWORD(m_pConfigInfo->dwAllocSize);
			inpack.pushDWORD(m_pConfigInfo->bShowPersion?1:0);
			inpack.pushString(m_pConfigInfo->szCustomInfo.c_str(),m_pConfigInfo->szCustomInfo.size());
			inpack.pushDWORD(m_pConfigInfo->bAutoStart?1:0);
			inpack.pushDWORD(m_pConfigInfo->bRegist?1:0);
			inpack.pushString(m_pConfigInfo->szStopRigTitle.c_str(),m_pConfigInfo->szStopRigTitle.size());
			inpack.pushDWORD(m_pConfigInfo->dwDay);
			inpack.pushDWORD(m_pConfigInfo->dwSpace);
			inpack.pushString(m_pConfigInfo->szCommResPath.c_str(),m_pConfigInfo->szCommResPath.size());
			inpack.pushDWORD(m_pConfigInfo->bShowCommres?1:0);

			_packageheader header ={0};
			header.Cmd = NET_CMD_GETCONFIGINFO;
			header.StartFlag = 0x5e7d;
			header.Length = inpack.GetOffset();
			inpack.pushPackageHeader(header);
			pNetLayer->SendPackage(sck,pData,header.Length);
/*
			string			sztitle;		//显示标题
			string			szSavePath;		//个人存档路径
			DWORD			dwAllocSize;	//个人分配空间大小
			bool			bShowPersion;	//是否显示个人存储区
			string			szCustomInfo;	//用户自定义说明。
			bool			bAutoStart;		//是否随系统启动
			bool			bRegist;		//是否可以注册
			string			szStopRigTitle;	//停止注册显示信息
			DWORD			dwDay;			// 删除几天不登陆用户
			DWORD			dwSpace;			//可用空间小于好多停止注册
			string			szCommResPath;	//公共资源路劲
			bool			bShowCommres;  //是否显示公共资源。
*/
			break;
		}
	case NET_CMD_GETSAVEGAMELST:
		{
			char buf[64] = {0};
			char szPath[MAX_PATH] = {0};
			inpack.popString(buf);
			sprintf_s(szPath,"%s%s\\game\\",m_pConfigInfo->szSavePath.c_str(),buf);
			map<DWORD,FILETIME> maplst;
			ScanGameGid(szPath,maplst);
			char* pData = pNetLayer->GetIMemPool()->Alloc(maplst.size()*10);
			CPackageHelper outpack(pData);
			outpack.pushDWORD(maplst.size());
			map<DWORD,FILETIME>::iterator it  = maplst.begin();
			for(; it != maplst.end();it ++)
			{
				outpack.pushDWORD(it->first);
				outpack.pushDWORD(MakeTimeFromFileTime(it->second));
			}
			_packageheader header ={0};
			header.Cmd = NET_CMD_GETSAVEGAMELST;
			header.StartFlag = 0x5e7d;
			header.Length = outpack.GetOffset();
			outpack.pushPackageHeader(header);
			pNetLayer->SendPackage(sck,pData,header.Length);
			break;
		}
	case NET_CMD_DELETEGAMERECORD:
		{
			char buf[64] = {0};
			char szPath[MAX_PATH] = {0};
			inpack.popString(buf);
			sprintf_s(szPath,"%s%spath.zip",m_pConfigInfo->szSavePath.c_str(),buf);
// 			SHFILEOPSTRUCT FileOp; 
// 			ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
// 			FileOp.fFlags = FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION; 
// 			FileOp.hNameMappings = NULL; 
// 			FileOp.hwnd =NULL; 
// 			FileOp.lpszProgressTitle = NULL; 
// 			FileOp.pFrom = szPath; 
// 			FileOp.pTo =NULL; 
// 			FileOp.wFunc = FO_DELETE; 
// 			int ret = SHFileOperation(&FileOp);
			DeleteFile(szPath);
			sprintf_s(szPath,"%s%sApplication.zip",m_pConfigInfo->szSavePath.c_str(),buf);
			DeleteFile(szPath);
			sprintf_s(szPath,"%s%sDocuments.zip",m_pConfigInfo->szSavePath.c_str(),buf);
			DeleteFile(szPath);
			PathRemoveFileSpec(szPath);
			RemoveDirectory(szPath);
			_packageheader header ={0};
			header.Cmd = NET_CMD_DELETEGAMERECORD;
			header.StartFlag = 0x5e7d;
			header.Length = sizeof(_packageheader);
			pNetLayer->SendPackage(sck,(char*)&header,header.Length);
			break;
		}
	case NET_CMD_GETGAMERECORD:
		{
			DWORD ByteWrite;
			char buf[MAX_PATH] = {0};
			char path[MAX_PATH] = {0};
			inpack.popString(buf);
			sprintf_s(path,"%s%s",m_pConfigInfo->szSavePath.c_str(),buf);
			HANDLE hFile = CreateFile(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			DWORD dwSize = 0;
			if(hFile != INVALID_HANDLE_VALUE)
			{
				 dwSize = GetFileSize(hFile,NULL);
			}
			char*pData = pNetLayer->GetIMemPool()->Alloc(dwSize+30);
			CPackageHelper outpack(pData);
			outpack.pushDWORD(dwSize);
			BOOL b = ReadFile(hFile,pData+outpack.GetOffset(),dwSize,&ByteWrite,NULL);
			_packageheader header = {0};
			header.Cmd = b? NET_CMD_GETGAMERECORD:NET_CMD_FAIL;
			header.StartFlag = 0x5e7d;
			header.Length = outpack.GetOffset()+dwSize;
			outpack.pushPackageHeader(header);
			pNetLayer->SendPackage(sck,pData,header.Length);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFile);
			}
			break;
		}

	}


}

//当客户端断开时产生该事件.
void CNetEvent::OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)
{

}
void CNetEvent::ScanGameGid(const string& szfile,map<DWORD,FILETIME>& maplst)
{
	WIN32_FIND_DATA wfd ={0};
	string strfile(szfile);
	strfile+="*.*";
	HANDLE hFile = FindFirstFile(strfile.c_str(),&wfd);
	if(hFile == INVALID_HANDLE_VALUE)
		return ;
	do 
	{
		if(wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wfd.cFileName[0] != '.')
			{
				char path[MAX_PATH] ={0};
				sprintf_s(path,"%s%s\\*.zip",szfile.c_str(),wfd.cFileName);
				WIN32_FIND_DATA FatData ={0};
				HANDLE handle = FindFirstFile(path,&FatData);
				FindClose(handle);
				DWORD gid = atol(wfd.cFileName);
				maplst.insert(make_pair(gid,FatData.ftLastWriteTime));
			}
		}
	} while(FindNextFile(hFile,&wfd));
	FindClose(hFile);

}
void CNetEvent::SendBlcokFile(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage)
{

	DWORD dwByteRead = 0;
	char buf[sizeof(FileHeader)+1] = {0};
	FileHeader* pHeader = (FileHeader*)buf;
	CPackageHelper outpack((LPSTR)lpPackage);
	outpack.popString(buf);

	DWORD dwsize = sizeof(FileHeader)+sizeof(_packageheader)+pHeader->DataSize +MAX_PATH;
	char* pData = pNetLayer->GetIMemPool()->Alloc(dwsize);
	CPackageHelper inpcak(pData);
	DWORD dwMove = pHeader->Num*BLOCK;
	char file[MAX_PATH] = {0};
	if(pHeader->IsPersion)
		sprintf_s(file,"%s%s\\u盘\\%s",m_pConfigInfo->szSavePath.c_str(),pHeader->FilePath,pHeader->filename);
	else
	sprintf_s(file,"%s\\%s",m_pConfigInfo->szCommResPath.c_str(),pHeader->filename);
	HANDLE hFile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		pHeader->DataSize = 0;
	}
	else
	{
		inpcak.pushString(buf,sizeof(FileHeader));
		SetFilePointer(hFile,dwMove,NULL,FILE_BEGIN);
		ReadFile(hFile,pData+inpcak.GetOffset(),pHeader->DataSize,&dwByteRead,NULL);
		CloseHandle(hFile);
	}
	_packageheader header ={0};
	header.Cmd = MET_CMD_GETBLCOKFILE;
	header.StartFlag = 0x5e7d;
	header.Length = outpack.GetOffset()+pHeader->DataSize;
	inpcak.pushPackageHeader(header);
	pNetLayer->SendPackage(sck,pData,header.Length);
}
void CNetEvent::GetFilepath()
{

	char szpath[MAX_PATH] = {0};
	GetModuleFileName(NULL,szpath,MAX_PATH);
	PathRemoveFileSpec(szpath);
	PathRemoveFileSpec(szpath);
	lstrcat(szpath,"\\u盘存档\\");
	m_pConfigInfo->szSavePath  = string(szpath);
	MakeSureDirectoryPathExists(szpath);

}
bool    CNetEvent::GetDiskFreeSize()
{
	ULARGE_INTEGER roomSize;
	roomSize.QuadPart = 0;
	char Driver[] = {"x:\\"};
	Driver[0] = *m_pConfigInfo->szSavePath.begin();
	GetDiskFreeSpaceEx(Driver, &roomSize, NULL, NULL);
	DWORD64 dwtmp = roomSize.QuadPart/(1024*1024);
	if(dwtmp < m_pConfigInfo->dwSpace)
	{
		m_pConfigInfo->bRegist = false;
		m_pConfigInfo->szStopRigTitle = "服务器磁盘空间不够,停止注册";
		return false;
	}
	return true;
}
BOOL CNetEvent::RegisteClient(char* username,char* password,string& str)
{


	string szfile = m_pConfigInfo->szSavePath  + string(username)+_T("\\");
	WIN32_FILE_ATTRIBUTE_DATA wfd = {0};
	BOOL bExist = GetFileAttributesEx(szfile.c_str(),GetFileExInfoStandard,&wfd);
	if(bExist && (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY))
	{
		str = _T("注册失败，此用户名已经存在");
		return FALSE;
	}
	MakeSureDirectoryPathExists(szfile.c_str());
	char buf[MAX_PATH] = {0};
	
	ZeroMemory(buf,MAX_PATH);
	sprintf_s(buf,"%s%s.ini",szfile.c_str(),username);
	WritePrivateProfileString(_T("system"),_T("password"),password,buf);

	sprintf_s(buf,"%su盘\\",szfile.c_str());
	BOOL b = MakeSureDirectoryPathExists(buf);
	str = b? "注册成功":"注册失败，服务器创建目录错";
	return b;
}
bool   CNetEvent:: LogVaild(char* pbuf)
{
	char* ptr = strstr(pbuf,"|");
	if(ptr == NULL)
	{
		return false;
	}
	*ptr = '\0';
	ptr++;
	char file[MAX_PATH]= {0};
	char password[32] = {0};
	sprintf_s(file,"%s%s\\%s.ini",m_pConfigInfo->szSavePath.c_str(),pbuf,pbuf);
	GetPrivateProfileString("system","password","",password,MAX_PATH,file);
	if(strcmp(ptr,password) == 0)
	{

		CTime t= CTime::GetCurrentTime();
		time_t time = t.GetTime();
		char buf[64 ] = {0};
		sprintf_s(buf,"%d",time);
		WritePrivateProfileString("system","LogTime",buf,file);
		return true;
	}
	return false;

}
int CNetEvent::WriteBlockFile(LPSTR lpackage)
{
	DWORD ByteWrite;
	CPackageHelper inpack(lpackage);
	char buf[sizeof(FileHeader)+1] = {0};
	inpack.popString(buf);
	FileHeader* pheader = (FileHeader*)buf;
	char file[MAX_PATH] = {0};
	sprintf_s(file,"%s%s%s",m_pConfigInfo->szSavePath.c_str(),pheader->FilePath,pheader->filename);
	MakeSureDirectoryPathExists(file);
	map<string,HANDLE>::iterator it = m_mapHandle.find(string(file));
	if(it == m_mapHandle.end())
		{
			HANDLE hFile = CreateFile(file,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile == INVALID_HANDLE_VALUE)
				return GetLastError();
			if(!WriteFile(hFile,lpackage+inpack.GetOffset(),pheader->DataSize,&ByteWrite,NULL))
			{
				CloseHandle(hFile);
				return GetLastError();
			}
			if(!pheader->bEnd)
			{
				CAutoLock<CLock> lock(&m_optex);
				m_mapHandle.insert(make_pair(string(file),hFile));
			}
			else
			{
				CloseHandle(hFile);
			}
		}
		else
		{
			if(!SetFilePointer(it->second,pheader->Num*BLOCK,NULL,FILE_BEGIN) ||
				(!WriteFile(it->second,lpackage+inpack.GetOffset(),pheader->DataSize,&ByteWrite,NULL)))
			{
				CloseHandle(it->second);
				CAutoLock<CLock> lock(&m_optex);
				m_mapHandle.erase(it);
				return GetLastError();
			}
			if(pheader->bEnd)
			{
				CloseHandle(it->second);
				CAutoLock<CLock> lock(&m_optex);
				m_mapHandle.erase(it);
			}
		}
	return 0;
}

bool CNetEvent::SendData(SOCKET sck,INetLayer*pNetLayer,WORD cmd,const char* pdata,DWORD length)
{
	char* pbuf = pNetLayer->GetIMemPool()->Alloc(length+MAX_PATH);
	CPackageHelper inpack(pbuf);
	if(pdata && length)
	{
		inpack.pushString(pdata,length);
	}
	_packageheader header = {0};
	header.Cmd = cmd;
	header.StartFlag = 0x5e7d;
	header.Length = inpack.GetOffset();
	inpack.pushPackageHeader(header);
	pNetLayer->SendPackage(sck,pbuf,header.Length);
	return true;
}
bool    CNetEvent::ScanFile(char* path,vector<FileInfo>& vecfileLst)
{
	if(lstrlen(path) == 0)
		return false;
	string Dstpath(path);
	Dstpath+="*.*";
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = FindFirstFile(Dstpath.c_str(),&wfd);
	if(hFind == INVALID_HANDLE_VALUE)
		return false;
	do 
	{
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wfd.cFileName[0] != '.')
			{
			}
		}
		else
		{
			FileInfo finfo;
			finfo.filename = string(wfd.cFileName);
			finfo.filesize = wfd.nFileSizeLow;
			finfo.dwTime = MakeTimeFromFileTime(wfd.ftLastWriteTime);
			vecfileLst.push_back(finfo);
		}
	
	} while(FindNextFile(hFind,&wfd) != 0);
	FindClose(hFind);
	return true;
}
bool CNetEvent::ReSetPassword(char* buf)
{

	char* ptr = strstr(buf,"|");
	if(ptr == NULL)return false;
	char* UserName = buf;
	*ptr = '\0';
	char* oldPassword = ++ptr;
	ptr = strstr(oldPassword,"|");
	if(ptr ==NULL) return false;
	*ptr = '\0';
	char* newPassword = ++ptr;
	char szpath[MAX_PATH] = {0};
	sprintf_s(szpath,"%s%s\\%s.ini",m_pConfigInfo->szSavePath.c_str(),UserName,UserName);
	char sztmp[64] = {0};
	GetPrivateProfileString("system","password","",sztmp,64,szpath);
	if(strcmp(sztmp,oldPassword) ==0)
	{
		WritePrivateProfileString("system","password",newPassword,szpath);
		return true;
	}




	return false;
}