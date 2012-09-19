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


namespace utility
{


	inline stdex::tString MakeIpString(DWORD dwIP)
	{
		if (dwIP == 0)
			return stdex::tString(_T("0.0.0.0"));
		in_addr addr;
		addr.s_addr = dwIP;
		char *p = inet_ntoa(addr);
		if (p != NULL)
			return stdex::tString(CA2T(::inet_ntoa(addr)));
		return stdex::tString(_T("0.0.0.0"));
	}

}



namespace i8desk
{


	struct tagVDInfoEx : public i8desk::tagVDInfo
	{
		DWORD Serial;
		DWORD Type;
		DWORD LoadType;
		BOOL  IsLocalMachine;
		HANDLE handle;
	};



	class CSocket
	{
	private:
		SOCKET m_Socket;
		i8desk::ISvrPlug  * m_pPlug;	//虚拟盘插件
		i8desk::ILogger* m_pLogger;
		HANDLE   m_hExited;
		stdex::tString m_SvrIp;

	public:
		CSocket(i8desk::ISvrPlug* pPlug, i8desk::ILogger* pLogger, HANDLE hExited);
		~CSocket();

	public:
		const stdex::tString &GetSvrIP() const
		{ return m_SvrIp; }
		bool IsConnect() const { return m_Socket != INVALID_SOCKET; }

		bool CreateSocket();
		void CloseSocket();

		void FreeVDisk(std::map<DWORD, tagVDInfoEx*>& VDList);
		bool GetVDiskList(std::map<DWORD, tagVDInfoEx*>& VDList);
		bool ReportVDiskStatus(std::map<DWORD, tagVDInfoEx*>& VDisk);
		bool ReportServerStatus(std::map<DWORD, tagVDInfoEx*>& VDisks, DWORD bInnerUpdate, ISvrPlug* pPlug);
		bool ReportVDiskClientStatus(std::map<DWORD, tagVDInfoEx*>& VDisks);

		void ProcessVDisk(std::map<DWORD, tagVDInfoEx*>& VDList, std::map<DWORD, tagVDInfoEx*>& newVDList);

		template<typename ContainerT>
		void GetFileList(ContainerT &val);

		std::tr1::tuple<DWORD, DWORD> GetFileInfo(const stdex::tString &file);
		void GetFileData(const stdex::tString &file, char *&fileDataBuf, DWORD &recvFileSize);

	private:
		bool _SendCmd(const char* pOutData, int outlength, char*& pInData, int& inlength);
		bool _SendData(char* pOutData, int outlength);
		bool _RecvData(char*& pInData, int& inlength);
		BOOL _IsLocalMachineVDisk(tagVDInfoEx* pVDisk);
	};


	class VDiskPkHelper
		: public CPkgHelper
	{
	public:
		VDiskPkHelper(LPSTR lpBuf, size_t len, WORD cmd)
			: CPkgHelper(lpBuf, len, cmd, OBJECT_VDISKSVR)
		{
		}

		template<size_t N>
		VDiskPkHelper(char (&arr)[N], WORD cmd)
			: CPkgHelper(arr, cmd, OBJECT_VDISKSVR)
		{
		}
	};

	// -----------------------------------
	// implement

	template<typename ContainerT>
	void CSocket::GetFileList(ContainerT &val)
	{
		m_pLogger->WriteLog(LM_INFO, _T("Start GetFileList"));

		static const size_t len = 4 * 1024;
		utility::CArrayCharPtr pSendData(new char[len]);

		VDiskPkHelper out(pSendData, len, CMD_VDSVR_GETUPDATE_FILELIST);
		char *outData = NULL;
		int outLen = 0;
		if( !_SendCmd(pSendData, out.GetLength(), outData, outLen) )
			throw std::logic_error("获取更新文件列表出错!");


		m_pLogger->WriteLog(LM_INFO, _T("GetFileList ing..."));
		utility::CArrayCharPtr data(outData);
		CPkgHelper in(data);
		DWORD dwStatus = 0;
		in.Pop(dwStatus);

		// Error
		if( dwStatus == 0 )
		{	
			throw std::runtime_error("获取更新文件列表数据错误!");
		}



		// 得到升级文件个数
		DWORD dwSize = 0;
		in.Pop(dwSize);
		m_pLogger->WriteLog(LM_INFO, _T("需要更新的文件个数 %d"), dwSize);

		// 得到升级文件列表
		stdex::tString file;
		for(size_t i = 0; i != dwSize; ++i)
		{
			in >> file;

			if( !file.empty() )
				val.push_back(file);

			m_pLogger->WriteLog(LM_INFO, file.c_str());
		}

		m_pLogger->WriteLog(LM_INFO, _T("GetFileList end..."));
	}


}
#endif