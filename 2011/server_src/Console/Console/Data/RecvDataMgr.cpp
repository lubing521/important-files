#include "stdafx.h"
#include "RecvDataMgr.h"

#include "../../../../include/frame.h"
#include "../Network/IOService.h"
#include "../Misc.h"
#include "Exception.h"


namespace i8desk
{
	namespace manager
	{

		template < typename UDPT, typename BufferT, typename ErrorCallbackT, typename ComplateCallbackT >
		struct UdpHelper
			: public std::tr1::enable_shared_from_this<UdpHelper<UDPT, BufferT, ErrorCallbackT, ComplateCallbackT>>
		{
			UDPT &udp_;
			BufferT &buf_;

			typedef std::map<size_t, stdex::tString> Msg;
			Msg msg_;

			DWORD group_;
			DWORD totalLen_;

			const ErrorCallbackT &errorCallback_;
			const ComplateCallbackT &complateCallback_;

			UdpHelper(UDPT &udp, BufferT &buf, const ErrorCallbackT &error, const ComplateCallbackT &complate)
				: udp_(udp)
				, buf_(buf)
				, errorCallback_(error)
				, complateCallback_(complate)
				, group_(0)
				, totalLen_(0)
			{}

			void Handle()
			{
				CPkgHelper in(&buf_[0]);

				try
				{
					DWORD serial = 0;
					in >> group_ >> totalLen_;

					stdex::tString text;
					in >> serial >> text;
					msg_.insert(std::make_pair(serial, text));

					size_t recvLen = 0;
					recvLen += text.length();
					
		
					while( totalLen_ > recvLen )
					{
						sockaddr_in addr ={0};
						size_t size = udp_.RecvFrom(Buffer(buf_), &addr);

						CPkgHelper in(&buf_[0]);
						if( in.GetHeader()->Cmd != CMD_GAME_CLIENTTOCON )
							continue;

						DWORD totalLen = 0;
						DWORD groupno = 0; 
						stdex::tString tmp;
						in >> groupno >> totalLen >> serial >> tmp;
						assert(in.GetHeader()->Cmd == CMD_GAME_CLIENTTOCON);
						assert(groupno == group_);
						assert(totalLen_ == totalLen);

						if( group_ != groupno )
							continue;

						msg_.insert(std::make_pair(serial, tmp));

						recvLen += text.length();
					}

					
					stdex::tString allText;
					for(Msg::const_iterator iter = msg_.begin(); iter != msg_.end(); ++iter)
						allText += iter->second;

					exception::Excute excute;
					exception::ExceptHandle(
						std::tr1::bind(&exception::Excute::Run<ComplateCallbackT, stdex::tString>, &excute, complateCallback_, 
						std::tr1::cref(text)), errorCallback_);
	
				}
				catch(...)
				{

				}
				
			}
		};
		
		template < typename UDPT, typename BufT, typename ErrorCallbackT, typename ComplateCallbackT >
		std::tr1::shared_ptr<UdpHelper<UDPT, BufT, ErrorCallbackT, ComplateCallbackT>> MakeUDPHelper(UDPT &udp, BufT &buf, const ErrorCallbackT &error, const ComplateCallbackT &complate)
		{
			typedef UdpHelper<UDPT, BufT, ErrorCallbackT, ComplateCallbackT> UDPHelperType;
			std::tr1::shared_ptr<UDPHelperType> udpHelper(new UDPHelperType(udp, buf, error, complate));
	
			return udpHelper;
		}


		RecvDataMgr::RecvDataMgr()
			: udp_(io::GetIODispatcher())
			, taskStateCallback_(0)
			, logCallbacks_(0)
			, errorCallback_(0)
			, clientCallback_(0)
		{
			buf_.resize(1024 * 1024);

			thread_.RegisterFunc(std::tr1::bind(&RecvDataMgr::_AsyncRecv, this));
		}

		RecvDataMgr::~RecvDataMgr()
		{}

		void RecvDataMgr::Start(u_short port)
		{
			Stop();
			
			
			if( !udp_.IsOpen() )
				udp_.Open(Udp::V4());
			else
				return;


			udp_.Bind(Udp::V4().Family(), port, INADDR_ANY);

			thread_.Start();
		}

		void RecvDataMgr::Stop()
		{
			try
			{
				//udp_.Cancel();
				udp_.Close();
				thread_.Stop();
			}
			catch(std::exception &e)
			{
				if( errorCallback_ != 0 )
				{
					stdex::tString err(CA2T(e.what()));
					errorCallback_(std::tr1::cref(err));
				}
			}
		}

		RecvDataMgr::ErrorCallback RecvDataMgr::RegisterError(const ErrorCallback &error)
		{
			ErrorCallback tmp = errorCallback_;
			errorCallback_ = error;
			return tmp;
		}

		RecvDataMgr::ClientCallback RecvDataMgr::RegisterClient(const ClientCallback &callbackstr)
		{
			ClientCallback tmp = clientCallback_;
			clientCallback_	= callbackstr;
			return tmp;
		}

		RecvDataMgr::TaskStatusCallback RecvDataMgr::RegisterTaskStatus(const TaskStatusCallback &taskStatus)
		{
			TaskStatusCallback tmp = taskStateCallback_;
			taskStateCallback_ = taskStatus;
			return tmp;
		}

		RecvDataMgr::PlugStatucCallback RecvDataMgr::RegisterPlugStatus(const PlugStatucCallback &plugStatus)
		{
			PlugStatucCallback tmp = plugStateCallback_;
			plugStateCallback_ = plugStatus;
			return tmp;
		}

		void RecvDataMgr::RegisterLog(const LogCallback &logCallback)
		{
			logCallbacks_.push_back(logCallback);
		}
		
		RecvDataMgr::ClientExceptCallback RecvDataMgr::RegisterCliExcept(const ClientExceptCallback &exceptCallback)
		{
			ClientExceptCallback tmp = cliExceptCallback_;
			cliExceptCallback_ = exceptCallback;
			return tmp;
		}

		RecvDataMgr::ConsoleCallback RecvDataMgr::RegisterConsole(const ConsoleCallback &consoleCallback)
		{
			ConsoleCallback tmp = consoleCallback_;
			consoleCallback_ = consoleCallback;
			return tmp;
		}

		RecvDataMgr::CheckUserCallback RecvDataMgr::RegisterCheckUser(const CheckUserCallback &checkUserCallback)
		{
			CheckUserCallback tmp = checkUserCallback_;
			checkUserCallback_ = checkUserCallback;
			return tmp;
		}

		RecvDataMgr::NewServerCallback RecvDataMgr::RegisterNewServer(const NewServerCallback &newServerCallback)
		{
			NewServerCallback tmp = newServerCallback_;
			newServerCallback_ = newServerCallback;
			return tmp;
		}

		RecvDataMgr::RefreshPLCallback RecvDataMgr::RegisterRefreshPL(const RefreshPLCallback &refreshPLCallback)
		{
			RefreshPLCallback tmp = refreshPLCallback_;
			refreshPLCallback_ = refreshPLCallback;
			return tmp;
		}

		DWORD RecvDataMgr::_AsyncRecv()
		{
			if ( !udp_.IsOpen() )
				return 0;

			while( !thread_.IsAborted() )
			{
				try
				{
					sockaddr_in addr = {0};
					size_t size = udp_.RecvFrom(Buffer(buf_), &addr);
					_OnRecv(::GetLastError(), size);
				}
				catch(std::exception &e)
				{
					if( errorCallback_ != 0 )
					{
						stdex::tString err(CA2T(e.what()));
						errorCallback_(std::tr1::cref(err));
					}
					break;
				}
			}
		

			return 0;
		}


		void RecvDataMgr::_ClientToConsole()
		{
			MakeUDPHelper(udp_, buf_, errorCallback_, clientCallback_)->Handle();
		}

		
		void RecvDataMgr::_TaskStateChange()
		{
			// 三层下载任务状态通知
			CPkgHelper in(&buf_[0]);

			DWORD ret = 0, notify = 0, gid = 0, status = 0;
			in >> ret >> notify >> gid >> status;

			
			if( taskStateCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<TaskStatusCallback, TaskNotify, DWORD, DWORD>, &excute, taskStateCallback_, (TaskNotify)notify, gid, status), errorCallback_);
			}
		}

		void RecvDataMgr::_ConsoleMsg()
		{
			if( consoleCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<ConsoleCallback>, &excute, consoleCallback_), errorCallback_);
			}
		}

		void RecvDataMgr::_PlugReportLog()
		{
			// 服务组件日志消息
			LogRecordPtr p(new LogRecord);
			CPkgHelper in(&buf_[0]);
			in >> p->time >> p->type >> p->plugin >> p->data;

			if( !logCallbacks_.empty() )
			{
				exception::Excute excute;

				for(std::list<LogCallback>::const_iterator iter = logCallbacks_.begin();
					iter != logCallbacks_.end(); ++iter)
				{
					exception::ExceptHandle(
						std::tr1::bind(&exception::Excute::Run<LogCallback, LogRecordPtr>, &excute, *iter, p), errorCallback_);
				}
				
			}
		}

		void RecvDataMgr::_CliExcept()
		{
			// 客户机硬件错误报告
			db::TClientPtr client(new db::tClient);
			DWORD ret = 0;
			DWORD uMask = 0;

			CPkgHelper in(&buf_[0]);
			in >> ret >> *client >> uMask;

			std::tr1::tuple<db::TClientPtr, DWORD> exceptInfo = std::tr1::make_tuple(client, uMask);
			if( cliExceptCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<ClientExceptCallback, std::tr1::tuple<db::TClientPtr, DWORD>>, &excute, cliExceptCallback_, exceptInfo), errorCallback_);
			}
		}
		void RecvDataMgr::_NotifyCheckUser()
		{
			// 验证用户信息通知
			CPkgHelper in(&buf_[0]);
			DWORD ret = 0;
			in >> ret;

			if( checkUserCallback_ != 0 )
				checkUserCallback_(ret == 1 ? true : false);
		}

		void RecvDataMgr::_PlugComplate()
		{
			// 插件下载任务状态通知
			CPkgHelper in(&buf_[0]);

			DWORD ret = 0, notify = 0, gid = 0, status = 0;
			in >> ret >> notify >> gid >> status;

			if( taskStateCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<PlugStatucCallback, TaskNotify, DWORD>, &excute, plugStateCallback_, (TaskNotify)notify, gid), errorCallback_);
			}
		}

		void RecvDataMgr::_NewServer()
		{
			// 新增从服务器
			CPkgHelper in(&buf_[0]);

			if( newServerCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<NewServerCallback>, &excute, newServerCallback_), errorCallback_);
			}
		}

		void RecvDataMgr::_RefreshPL()
		{
			// 刷新PL
			CPkgHelper in(&buf_[0]);

			if( refreshPLCallback_ != 0 )
			{
				exception::Excute excute;
				exception::ExceptHandle(
					std::tr1::bind(&exception::Excute::Run<RefreshPLCallback>, &excute, refreshPLCallback_), errorCallback_);
			}
		}



		void RecvDataMgr::_OnRecv(u_long error, u_long size)
		{
			if( error == ERROR_OPERATION_ABORTED )
				return;
			else if( error != 0 )
			{
				assert(error);
			}

			pkgheader *header = reinterpret_cast<pkgheader *>(&buf_[0]);
			switch(header->Cmd)
			{
				case CMD_GAME_CLIENTTOCON:
					_ClientToConsole();
					break;
				case CMD_TASK_STATE_CHANGED:
					_TaskStateChange();
					break;
				case CMD_PLUGIN_LOG_REPORT:
					_PlugReportLog();
					break;
				case CMD_CON_OFFLINE_NOTIFY:
					_ConsoleMsg();
					break;
				case CMD_PLUG_TASK_COMPLATE:
					_PlugComplate();
					break;
				case CMD_CLIENT_HARDWARECHANGE:
					_CliExcept();
					break;
				case CMD_CHECKUSER_NOTIFY:
					_NotifyCheckUser();
					break;
				case CMD_ADD_NEW_SERVER:
					_NewServer();
					break;

				case CMD_REFRESHPL:
					_RefreshPL();
					break;
				default:
					assert(0);
					break;
			}
		}
	}
}