#ifndef __RECV_SERVER_HPP
#define __RECV_SERVER_HPP

#include <atltime.h>

#include "../../../include/Container/BlockingQueue.hpp"
#include "../../../include/Win32/WinService/win32svr.h"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include "../../../include/AsyncIO/Timer/Timer.hpp"

#include "../../Common/MemoryPool.hpp"
#include "../../Common/Utility.hpp"
#include "../../Common/DBMgr.hpp"

#include "MonitorMgr.hpp"
#include "SendMgr.hpp"
#include "ClientMgr.hpp"
#include "StateMgr.hpp"
#include "WriteTaskFile.hpp"
#include "HistoryPlay.hpp"
#include "HistoryMonitor.hpp"

namespace recvSvr
{
	using namespace async;


	class RecvSvr
		: public win32::CWin32Svr
	{
		iocp::IODispatcher io_;
		
		async::timer::Timer timer_;
		typedef async::container::BlockingQueue<common::BmpInfo> FileBuffers;
		FileBuffers fileBuffersI_;
		FileBuffers fileBuffersJ_;

		std::auto_ptr<MonitorMgr> monitor_;

		std::auto_ptr<SendMgr> sendMgrI_;
		std::auto_ptr<SendMgr> sendMgrJ_;

		std::auto_ptr<SessionMgr> sessionMgrI_;
		std::auto_ptr<SessionMgr> sessionMgrJ_;
		std::auto_ptr<StateMgr> stateMgr_;
		std::auto_ptr<HistoryMonitor> historyMonitor_;

		std::pair<size_t, std::tr1::shared_ptr<WriteTaskFile>> writeTaskI_;
		std::pair<size_t, std::tr1::shared_ptr<WriteTaskFile>> writeTaskJ_;

		std::wstring localIP_;
		u_short ftpPort_;
		std::wstring taskPath_;
		std::wstring historyPath_;


		std::wstring task_id_;
		volatile long j_cnt_;
		volatile long i_cnt_;

	public:
		RecvSvr()
			: CWin32Svr(L"RecvSvr", L"Recv Data Server", (common::GetAppPath() + L"RecvSvr.exe").c_str())
			, io_(10)
			, j_cnt_(0)
			, i_cnt_(0)
			, timer_(io_, 5 * 60 * 1000, 5 * 60 * 1000, std::tr1::bind(&RecvSvr::_TaskComplate, this))
		{
			localIP_ = utility::GetLocalIP();
			timer_.AsyncWait();
		}
		~RecvSvr()
		{

		}

	public:
		virtual DWORD OnStart()
		{	
			using namespace std::tr1::placeholders;

			try
			{
				u_short controlPort = common::GetIniConfig<u_short>(L"RecvSvr.ini", L"CONFIG", L"TCP_CONTROL_PORT");
				u_short viewPortI = common::GetIniConfig<u_short>(L"RecvSvr.ini", L"CONFIG", L"TCP_VIEW_PORT_I");
				u_short viewPortJ = common::GetIniConfig<u_short>(L"RecvSvr.ini", L"CONFIG", L"TCP_VIEW_PORT_J");
				taskPath_ = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"CONFIG", L"TASK_PATH");
				historyPath_ = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"CONFIG", L"HISTORY_PATH");

				ftpPort_ = common::GetIniConfig<u_short>(L"RecvSvr.ini", L"FTP", L"FTP_PORT");
				std::wstring monitorPath = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"FTP", L"MONITOR_PATH");
				std::wstring historyPath = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"FTP", L"MONITOR_HISTORY_PATH");

				std::wstring dbIP = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"DB", L"DB_IP");
				std::wstring dbName = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"DB", L"DB_NAME");
				std::wstring dbUserName = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"DB", L"DB_USERNAME");
				std::wstring dbPassword = common::GetIniConfig<std::wstring>(L"RecvSvr.ini", L"DB", L"DB_PASSWORD");

				db::DBMgrInstance().Init(dbIP, dbName, dbUserName, dbPassword);
				//db::DBMgrInstance().ExecuteSQL(L"set names 'utf-8'", adCmdUnspecified);
				
				sendMgrI_.reset(new SendMgr(io_, std::tr1::bind(&RecvSvr::GetFileBuffer, this, std::tr1::ref(fileBuffersI_))));
				sendMgrI_->Start();
				sendMgrJ_.reset(new SendMgr(io_, std::tr1::bind(&RecvSvr::GetFileBuffer, this, std::tr1::ref(fileBuffersJ_))));
				sendMgrJ_->Start();

				monitor_.reset(new MonitorMgr(io_, monitorPath, 
					std::tr1::bind(&RecvSvr::PutFileBuffer, this, _1)));
				monitor_->Start();



				historyMonitor_.reset(new HistoryMonitor(io_, historyPath,
					std::tr1::bind(&RecvSvr::_HandleHistory, this, _1)));
				historyMonitor_->Start();

				sessionMgrI_.reset(new SessionMgr(io_, viewPortI,
					std::tr1::bind(&RecvSvr::_OnAcceptSession, this, std::tr1::placeholders::_1)));
				sessionMgrI_->Start();
				sessionMgrJ_.reset(new SessionMgr(io_, viewPortJ,
					std::tr1::bind(&RecvSvr::_OnAcceptSession, this, std::tr1::placeholders::_1)));
				sessionMgrJ_->Start();

				stateMgr_.reset(new StateMgr(io_, controlPort));
				stateMgr_->Start(std::tr1::bind(&RecvSvr::_HandleMsg, this, _1, _2));

				sendMgrI_->Register(std::tr1::bind(&SessionMgr::Run, sessionMgrI_.get(), _1));
				sendMgrJ_->Register(std::tr1::bind(&SessionMgr::Run, sessionMgrJ_.get(), _1));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				exit(-1);
			}

			return ERROR_SUCCESS;
			
		}

		virtual DWORD OnStop()
		{
			stateMgr_->Stop();
			sessionMgrI_->Stop();
			sessionMgrJ_->Stop();

			fileBuffersI_.Put(common::BmpInfo());
			fileBuffersJ_.Put(common::BmpInfo());

			historyMonitor_->Stop();
			monitor_->Stop();

			sendMgrI_->Stop();
			sendMgrJ_->Stop();
	
			db::DBMgrInstance().UnInit();
			io_.Stop();

			return ERROR_SUCCESS;
		}

		void PutFileBuffer(const common::BmpInfo &buffer)
		{
			static std::wstring i_freq, j_freq;

			// 判断是否为起始图片
			std::vector<stdex::tString> params;
			stdex::Split(params, buffer.second, L'_');

			const TCHAR flag = params[6][5];
			const TCHAR port = *(params[2].rbegin());

			if( ::InterlockedExchangeAdd(&i_cnt_, 0) == 0 || 
				::InterlockedExchangeAdd(&j_cnt_, 0) == 0 )
			{
				const std::wstring &taskID = params[1];
				const std::wstring &freq = params[3];

				task_id_ = taskID;
				
				if( port == L'I' )
				{
					i_freq = freq;

					CTime time = CTime::GetCurrentTime();
					std::wstring i_path = taskPath_ + taskID + L"_" + (LPCTSTR)time.Format(L"%Y%m%d_%H%M") + _T("/I/");
					sendMgrI_->UnRegister(writeTaskI_.first);

					writeTaskI_.second.reset(new WriteTaskFile(io_, i_path));
					writeTaskI_.first = sendMgrI_->Register(std::tr1::bind(&WriteTaskFile::Handle, writeTaskI_.second.get(), std::tr1::placeholders::_1));
				
					::InterlockedIncrement(&i_cnt_);
				}
				else if( port == 'J' )
				{
					j_freq = freq;

					CTime time = CTime::GetCurrentTime();
					std::wstring j_path = taskPath_ + taskID + L"_" + (LPCTSTR)time.Format(L"%Y%m%d_%H%M") + _T("/J/");
					sendMgrJ_->UnRegister(writeTaskJ_.first);

					writeTaskJ_.second.reset(new WriteTaskFile(io_, j_path));
					writeTaskJ_.first = sendMgrJ_->Register(std::tr1::bind(&WriteTaskFile::Handle, writeTaskJ_.second.get(), std::tr1::placeholders::_1));
				
					::InterlockedIncrement(&j_cnt_);
				}

				if( !i_freq.empty() && 
					!j_freq.empty() )
				{
					std::wstringstream os;
					os << L"INSERT INTO task_t (task_id, type, date_added, path, i_freq, j_freq) " 
						<< L"VALUES("
						<< L"'" << taskID << L"', " << common::REAL_DATA << L", now(), '" << taskPath_ << "', " << i_freq << ", " << j_freq
						<< L")";
						if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
							::OutputDebugString(L"INSERT INTO task_t ERROR!");

					os.str(L"");
					os << L"INSERT INTO log_t (date_added, message) "
						<< L"VALUES("
						<< L"now(), " << L"'实时任务[" << taskID << L"]开始'"
						<< L")";
					if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
						::OutputDebugString(L"INSERT INTO log_t ERROR!");

					i_freq.clear();
					j_freq.clear();
				}
			}
			else
			{
				if( port == L'I' )
				{
					::InterlockedIncrement(&i_cnt_);
					fileBuffersI_.Put(buffer);
				}
				else if( port == L'J' )
				{
					::InterlockedIncrement(&j_cnt_);
					fileBuffersJ_.Put(buffer);
				}
			}

			timer_.Cancel();
			timer_.SetTimer(5 * 60 * 1000, 5 * 60 * 1000);
		}			

		common::BmpInfo GetFileBuffer(FileBuffers &fileBuffers)
		{
			return fileBuffers.Get();
		}

	private:
		void _TaskComplate()
		{
			if( i_cnt_ == 0 || j_cnt_ == 0 || task_id_.empty() )
				return;

			std::wstringstream os;
			os << L"UPDATE task_t SET date_end = now(), "
				<< L"i_total = " << i_cnt_ << L", "
				<< L"j_total = " << j_cnt_ << L" "
				<< L"WHERE task_id = '"
				<< task_id_ << '\'';
			if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
				::OutputDebugString(L"UPDATE task_t ERROR!");

			os.str(L"");
			os << L"INSERT INTO log_t (date_added, message) "
				<< L"VALUES("
				<< L"now(), " << L"'实时任务[" << task_id_ << L"]结束'"
				<< L")";
			if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
				::OutputDebugString(L"INSERT INTO log_t ERROR!");

			::InterlockedExchange(&i_cnt_, 0);
			::InterlockedExchange(&j_cnt_, 0);
			task_id_.clear();
		}

		void _HandleHistory(const common::BmpInfo &buffer)
		{
			const std::wstring folderPath = taskPath_ + buffer.second;

			std::tr1::shared_ptr<WriteTaskFile> moveHistoryFile(new WriteTaskFile(io_, folderPath));
			moveHistoryFile->Handle(buffer);

			std::wstringstream os;
			os << L"INSERT INTO task_t (task_id, type, date_added, path) "
				<< L"VALUES("
				<< L"'" << buffer.second << L"', " << common::HISTORY_DATA << L", now(), '" << folderPath << "'"
				<< L")";
			if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
				::OutputDebugString(L"INSERT INTO ERROR!");

			os.str(L"");
			os << L"INSERT INTO log_t (date_added, message) "
				<< L"VALUES("
				<< L"now(), " << L"'处理历史任务[" << buffer.second << L"]'"
				<< L")";
			if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
				::OutputDebugString(L"INSERT INTO log_t ERROR!");
		}

		void _OnAcceptSession(const std::string &ip)
		{
			std::wstringstream os;
			os << L"INSERT INTO log_t (date_added, message) "
				<< L"VALUES("
				<< L"now(), " << L"'查看客户端(" << ip.c_str() << L")连接到接收服务器"
				<< utility::GetLocalIP() << L")'"
				<< L")";
			if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
				::OutputDebugString(L"INSERT INTO log_t ERROR!");
		}

		void _HandleMsg(const char *msg, size_t len)
		{
			const common::TCPPKHeader *inHeader = reinterpret_cast<const common::TCPPKHeader *>(msg);

			common::TCPPKHelper out(stateMgr_->Buffer().data(), stateMgr_->Buffer().size());
			common::TCPPKHeader *outHeader = reinterpret_cast<common::TCPPKHeader *>(stateMgr_->Buffer().data());
			
			common::TCPPKHeader header = {common::StartFlag, inHeader->cmd_, _T(""), 0};
			out << header;

			switch(inHeader->cmd_)
			{
			case common::CMD_HERT_BEAT:
				out << localIP_ << ftpPort_ << monitor_->IsOK() << monitor_->IsPaused();
				break;
			case common::CMD_PAUSED_VIEW:
				monitor_->Pause();
				out << 1L;
				break;
			case common::CMD_RESUME_VIEW:
				monitor_->Resume();
				out << 1L;
				break;
			case common::CMD_GET_CLIENTS:
				out << sessionMgrI_->GetClients();
				break;
			case common::CMD_HISTORY_START:
				{
					common::TCPPKHelper in((char *)msg, len);
					common::TCPPKHeader header = {0};
					std::wstring i_path, j_path;
					size_t i_freq = 0, j_freq = 0;
					in >> header >> i_path >> j_path >> i_freq >> j_freq;
					
					std::tr1::shared_ptr<HistoryPlay> history(new HistoryPlay(io_, i_path, j_path, i_freq, j_freq,
						std::tr1::bind(&FileBuffers::Put, std::tr1::ref(fileBuffersI_), std::tr1::placeholders::_1),
						std::tr1::bind(&FileBuffers::Put, std::tr1::ref(fileBuffersJ_), std::tr1::placeholders::_1)));
					history->Start();
				}
				break;
			case common::CMD_HISTORY_STOP:
				{
					HistoryPlay::Stop();
				}
				break;
			default:
				out << 0L;
				break;
			}

			outHeader->length_ = out.Length() - sizeof(common::TCPPKHeader);
			stateMgr_->Send(out.Length());
		}
	};
}



#endif