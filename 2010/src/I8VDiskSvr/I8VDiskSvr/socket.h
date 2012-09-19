#ifndef __socket_inc__
#define __socket_inc__

#include <Shlwapi.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <string>
#include <map>
#include <set>
#include <tuple>

#include "../VirtualDisk/VirtualDisk.h"



#define SERVER_PORT				17918
#define CONFIG_FILE				"Data\\I8SyncSvr.ini"


namespace i8desk
{

	namespace utility
	{


		inline std::string MakeIpString(DWORD dwIP)
		{
			if (dwIP == 0)
				return std::string("0.0.0.0");
			in_addr addr;
			addr.s_addr = dwIP;
			char *p = inet_ntoa(addr);
			if (p != NULL)
				return std::string(inet_ntoa(addr));
			return std::string("0.0.0.0");
		}

	}




	typedef struct tagVDInfoEx : public i8desk::tagVDInfo
	{
		char  ID[40];
		char  SID[40];
		DWORD Serial;
		DWORD Type;
		DWORD LoadType;
		BOOL  IsLocalMachine;
		HANDLE handle;
	}tagVDInfoEx;



	class CSocket
	{
	private:
		SOCKET m_Socket;
		i8desk::IPlug  * m_pPlug;	//虚拟盘插件
		i8desk::ILogger* m_pLogger;
		HANDLE   m_hExited;
		std::string m_SvrIp;

	public:
		CSocket(i8desk::IPlug* pPlug, i8desk::ILogger* pLogger, HANDLE hExited);
		~CSocket();

	public:
		bool IsConnect() { return m_Socket != INVALID_SOCKET; }

		bool CreateSocket();
		void  CloseSocket();

		void FreeVDisk(std::map<DWORD, tagVDInfoEx*>& VDList);
		bool GetVDiskList(std::map<DWORD, tagVDInfoEx*>& VDList);
		bool ReportVDiskStatus(std::map<DWORD, tagVDInfoEx*>& VDisk);
		bool ReportServerStatus(std::map<DWORD, tagVDInfoEx*>& VDisks, DWORD bInnerUpdate, IPlug* pPlug);

		void ProcessVDisk(std::map<DWORD, tagVDInfoEx*>& VDList, std::map<DWORD, tagVDInfoEx*>& newVDList);

		template<typename ContainerT>
		void GetFileList(ContainerT &val);

		std::tr1::tuple<DWORD, DWORD> GetFileInfo(const i8desk::std_string &file);
		void GetFileData(const i8desk::std_string &file, char *&fileDataBuf, DWORD &recvFileSize);

	private:
		inline void InitPackage(i8desk::_packageheader* pHeader, WORD cmd)
		{
			ZeroMemory(pHeader, sizeof(i8desk::_packageheader));
			pHeader->StartFlag = START_FLAG;
			pHeader->Cmd       = cmd;								//命令字
			pHeader->Length    = sizeof(i8desk::_packageheader);	//整个包的长度。初始化为包头的长度
			pHeader->Version   = PRO_VERSION;						//表示版本号
			pHeader->Resv[0]   = OBJECT_VDISKSVI;					//表示虚拟盘的的命令
		}

	private:
		bool _SendCmd(char* pOutData, int outlength, char*& pInData, int& inlength);
		bool _SendData(char* pOutData, int outlength);
		bool _RecvData(char*& pInData, int& inlength);
		BOOL _IsLocalMachineVDisk(tagVDInfoEx* pVDisk);
	};

	
	// -----------------------------------
	// implement

	template<typename ContainerT>
	void CSocket::GetFileList(ContainerT &val)
	{
		m_pLogger->WriteLog(LM_INFO, "Start GetFileList");

		CAutoArrayPtr<char>::AutoPtr pSendData(new char[4 * 1024]);
		_packageheader* pheader = (_packageheader*)pSendData.m_Handle;
		InitPackage(pheader, CMD_VDSVR_GETUPDATE_FILELIST);


		CPackageHelper out(pSendData);
		pheader->Length = out.GetOffset();
		out.pushPackageHeader(*pheader);

		char *outData = NULL;
		int outLen = 0;
		if( !_SendCmd(pSendData, pheader->Length, outData, outLen) )
			throw std::logic_error("获取更新文件列表出错!");


		m_pLogger->WriteLog(LM_INFO, "GetFileList ing...");
		i8desk::CAutoArrayPtr<char>::AutoPtr data(outData);
		CPackageHelper in(data);
		DWORD dwStatus = in.popDWORD();

		// Error
		if( dwStatus == 0 )
		{	
			throw std::runtime_error("获取更新文件列表数据错误!");
		}

		

		// 得到升级文件个数
		DWORD dwSize = in.popDWORD();
		m_pLogger->WriteLog(LM_INFO, "需要更新的文件个数 %d", dwSize);

		// 得到升级文件列表
		std_string file;
		for(size_t i = 0; i != dwSize; ++i)
		{
			in >> file;

			if( !file.empty() )
				val.push_back(file);
			
			m_pLogger->WriteLog(LM_INFO, file.c_str());
		}

		m_pLogger->WriteLog(LM_INFO, "GetFileList end...");
	}
}


#endif