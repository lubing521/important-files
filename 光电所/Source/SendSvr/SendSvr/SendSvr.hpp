#ifndef __SEND_SVR_MANAGER_HPP
#define __SEND_SVR_MANAGER_HPP


#include <tuple>

#include "DataMgr.hpp"
#include "ClientMgr.hpp"
#include "StateMgr.hpp"
#include "HistoryMonitor.hpp"

#include "../../../include/Utility/utility.h"
#include "../../../include/Win32/WinService/win32svr.h"

namespace sendsvr
{
	enum { IP, PORT, USERNAME, PASSWORD, PATH};
	typedef std::tr1::tuple<std::wstring, u_short, std::wstring, std::wstring, std::wstring> HistoryInfo;


	class SendSvrMgr
		: public win32::CWin32Svr
	{
	private:
		async::iocp::IODispatcher io_;

		std::auto_ptr<DataMgr> dataMgrI_;
		std::auto_ptr<DataMgr> dataMgrJ_;

		std::auto_ptr<SessionMgr> sessionMgrI_;
		std::auto_ptr<SessionMgr> sessionMgrJ_;

		std::auto_ptr<StateMgr> stateMgr_;
		std::auto_ptr<HistoryMonitor> historyMonitor_;

		std::wstring localIP_;
		u_short udpRecvport_;

	public:
		SendSvrMgr()
			: win32::CWin32Svr(L"SendSvr", L"Send Data Server", (common::GetAppPath() + L"SendSvr.exe").c_str())
			, io_()
		{
			u_short udpRecvPortI	= common::GetIniConfig<u_short>(L"SendSvr.ini", L"CONFIG", L"UDP_RECV_PORT_I");
			u_short udpRecvPortJ	= common::GetIniConfig<u_short>(L"SendSvr.ini", L"CONFIG", L"UDP_RECV_PORT_J");

			u_short tcpViewPortI	= common::GetIniConfig<u_short>(L"SendSvr.ini", L"CONFIG", L"TCP_VIEW_PORT_I");
			u_short tcpViewPortJ	= common::GetIniConfig<u_short>(L"SendSvr.ini", L"CONFIG", L"TCP_VIEW_PORT_J");

			u_short tcpControlPort	= common::GetIniConfig<u_short>(L"SendSvr.ini", L"CONFIG", L"TCP_CONTROL_PORT");
			u_short ftpPort			= common::GetIniConfig<u_short>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_PORT");
			
			stdex::tString ftpIP = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_IP");
			stdex::tString userName = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_USERNAME");
			stdex::tString password = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_PASSWORD");
			stdex::tString ftpTmpPathI = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_TMP_PATH_I");
			stdex::tString ftpTmpPathJ = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"REAL_DATA_FTP", L"FTP_TMP_PATH_J");

			u_short historyftpPort = common::GetIniConfig<u_short>(L"SendSvr.ini", L"HISTORY_DATA_FTP", L"FTP_PORT");
			stdex::tString historyftpIP = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"HISTORY_DATA_FTP", L"FTP_IP");
			stdex::tString historyuserName = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"HISTORY_DATA_FTP", L"FTP_USERNAME");
			stdex::tString historypassword = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"HISTORY_DATA_FTP", L"FTP_PASSWORD");
			stdex::tString historymonitorPath = common::GetIniConfig<stdex::tString>(L"SendSvr.ini", L"HISTORY_DATA_FTP", L"MONITOR_PATH");


			dataMgrI_.reset(new DataMgr(io_, udpRecvPortI, ftpPort, ftpIP, userName, password, ftpTmpPathI));
			dataMgrJ_.reset(new DataMgr(io_, udpRecvPortJ, ftpPort, ftpIP, userName, password, ftpTmpPathJ));

			sessionMgrI_.reset(new SessionMgr(io_, tcpViewPortI));
			sessionMgrJ_.reset(new SessionMgr(io_, tcpViewPortJ));

			stateMgr_.reset(new StateMgr(io_, tcpControlPort));

			const HistoryInfo &history = std::tr1::make_tuple(historyftpIP, historyftpPort, historyuserName, historypassword, historymonitorPath);
			historyMonitor_.reset(new HistoryMonitor(io_, 
				std::tr1::get<PATH>(history), std::tr1::get<IP>(history), 
				std::tr1::get<PORT>(history), std::tr1::get<USERNAME>(history), 
				std::tr1::get<PASSWORD>(history)));

			
			localIP_ = utility::GetLocalIP();
			udpRecvport_ = udpRecvPortI;
		}
		~SendSvrMgr()
		{

		}

	public:
		virtual DWORD OnStart()
		{
			using namespace std::tr1::placeholders;

			dataMgrI_->Start();
			dataMgrJ_->Start();

			sessionMgrI_->Start();
			sessionMgrJ_->Start();

			stateMgr_->Start(std::tr1::bind(&SendSvrMgr::_OnRead, this, _1, _2));
			historyMonitor_->Start();

			dataMgrI_->RegisterSendCallback(std::tr1::bind(&SessionMgr::Run, sessionMgrI_.get(), _1));
			dataMgrJ_->RegisterSendCallback(std::tr1::bind(&SessionMgr::Run, sessionMgrJ_.get(), _1));

			return ERROR_SUCCESS;
		}

		virtual DWORD OnStop()
		{
			historyMonitor_->Stop();
			stateMgr_->Stop();

			sessionMgrI_->Stop();
			sessionMgrJ_->Stop();

			dataMgrI_->Stop();
			dataMgrJ_->Stop();

			return ERROR_SUCCESS;
		}

	private:
		void _OnRead(const char *buf, size_t len)
		{
			const common::TCPPKHeader *inHeader = reinterpret_cast<const common::TCPPKHeader *>(buf);
			common::TCPPKHelper out(stateMgr_->Buffer().data(), stateMgr_->Buffer().size());
			common::TCPPKHeader *outHeader = reinterpret_cast<common::TCPPKHeader *>(stateMgr_->Buffer().data());

			common::TCPPKHeader header = {common::StartFlag, inHeader->cmd_, _T(""), 0};
			out << header;

			switch(inHeader->cmd_)
			{
			case common::CMD_HERT_BEAT:
				out << localIP_ << udpRecvport_ << dataMgrI_->IsOK() << dataMgrI_->IsPaused();
				break;
			case common::CMD_PAUSED_VIEW:
				dataMgrI_->Pause();
				dataMgrJ_->Pause();
				out << 1L;
				break;
			case common::CMD_RESUME_VIEW:
				dataMgrI_->Resume();
				dataMgrJ_->Resume();
				out << 1L;
				break;
			default:
				break;
			}

		
			outHeader->length_ = out.Length() - sizeof(common::TCPPKHeader);
			stateMgr_->Send(out.Length());
		}
	};
}




#endif