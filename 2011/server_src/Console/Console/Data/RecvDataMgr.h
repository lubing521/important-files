#ifndef __CONSOLE_RECV_DATA_MANAGER_HPP
#define __CONSOLE_RECV_DATA_MANAGER_HPP

#include "../../../../include/AsyncIO/Network/UDP.hpp"
#include "../../../../include/MultiThread/Thread.hpp"
#include "../../../../include/TaskInfo.h"
#include "../../../../include/I8Type.hpp"
#include "../../../../include/tablestruct.h"

#include "../LogRecord.h"
#include <array>
#include <map>
#include <tuple>
#include <list>

namespace i8desk
{
	namespace manager
	{

		using namespace async;
		using namespace async::network;
		// ----------------------------
		// class RecvDataMgr

		class RecvDataMgr
		{
		public:
			typedef std::tr1::function<void(TaskNotify, DWORD, DWORD)>	TaskStatusCallback;
			typedef std::tr1::function<void(TaskNotify, DWORD)>			PlugStatucCallback;
			typedef std::tr1::function<void(LogRecordPtr)>				LogCallback;
			typedef std::tr1::function<void(const std::tr1::tuple<db::TClientPtr, DWORD> &)>		ClientExceptCallback;
			typedef std::tr1::function<void()>							ConsoleCallback;						
			typedef std::tr1::function<void(const stdex::tString &)>	ClientCallback;
			typedef std::tr1::function<void(const stdex::tString &)>	ErrorCallback;
			typedef std::tr1::function<void(bool)>						CheckUserCallback;
			typedef std::tr1::function<void()>							NewServerCallback;
			typedef std::tr1::function<void()>							RefreshPLCallback;
			
		private:
			async::network::Udp::Socket udp_;
			std::vector<char>		buf_;

			TaskStatusCallback		taskStateCallback_;
			PlugStatucCallback		plugStateCallback_;
			ClientCallback			clientCallback_;
			std::list<LogCallback>	logCallbacks_;
			ClientExceptCallback	cliExceptCallback_;
			ErrorCallback			errorCallback_;
			ConsoleCallback			consoleCallback_;
			CheckUserCallback		checkUserCallback_;
			NewServerCallback		newServerCallback_;
			RefreshPLCallback		refreshPLCallback_;

			async::thread::ThreadImplEx thread_;
		public:
			RecvDataMgr();
			~RecvDataMgr();

		public:
			ErrorCallback		RegisterError(const ErrorCallback &error);
			TaskStatusCallback	RegisterTaskStatus(const TaskStatusCallback &taskStatus);
			PlugStatucCallback	RegisterPlugStatus(const PlugStatucCallback &plugStatus);
			ClientCallback		RegisterClient(const ClientCallback &callback);
			void				RegisterLog(const LogCallback &logCallback);
			ClientExceptCallback RegisterCliExcept(const ClientExceptCallback &exceptCallback);
			ConsoleCallback		RegisterConsole(const ConsoleCallback &consoleCallback);
			CheckUserCallback	RegisterCheckUser(const CheckUserCallback &checkUserCallback);
			NewServerCallback   RegisterNewServer(const NewServerCallback &newServerCallback);
			RefreshPLCallback	RegisterRefreshPL(const RefreshPLCallback &refreshPL);

			void Start(u_short port);
			void Stop();


		private:
			RecvDataMgr(const RecvDataMgr &);
			RecvDataMgr &operator=(const RecvDataMgr &);

		private:
			DWORD _AsyncRecv();
			void _OnRecv(u_long error, u_long size);
			void _ClientToConsole();
			void _PlugReportLog();
			void _TaskStateChange();
			void _NotifyCheckUser();
			void _PlugComplate();
			void _ConsoleMsg();
			void _CliExcept();
			void _NewServer();
			void _RefreshPL();
		};
	}
}




#endif