#ifndef __CONTROL_MANAGER_HPP
#define __CONTROL_MANAGER_HPP

#include <tuple>
#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../Common/PackageHelper.hpp"


using namespace async;


namespace ctrl
{
	enum { STATUS_OK, STATUS_PAUSED, STATUS_IP, STATUS_PORT };
	typedef std::tr1::tuple<bool, bool, std::wstring, u_short> StatusInfo;

	typedef std::pair<std::string, u_short> RemoteInfo;

	class ControlMgr
	{
	private:
		class MgrImpl;
		
		std::auto_ptr<MgrImpl> proxyMgr_;
		std::auto_ptr<MgrImpl> sendMgr_;
		std::auto_ptr<MgrImpl> recvMgr_;

		iocp::IODispatcher io_;

	public:
		ControlMgr();
		~ControlMgr();

	public:
		void Start(const RemoteInfo &proxy, const RemoteInfo &sendSvr, const RemoteInfo &recvSvr);
		void Stop();

	public:
		void Query();

		void ChangeUDPProxyState(bool isOK);
		void ChangeSendSvrState(bool isOK);
		void ChangeRecvSvrState(bool isOK);
		
		StatusInfo GetUDPProxyStatus() const;
		StatusInfo GetSendSvrStatus() const;
		StatusInfo GetRecvSvrStatus() const;

		common::RemoteClients GetRecvSvrClients() const;
		void PlayHistory(const std::wstring &i_path, const std::wstring &j_path, size_t i_freq, size_t j_freq);
		void StopHistory();
	};


	inline std::auto_ptr<ControlMgr> &ControlMgrInstance()
	{
		static std::auto_ptr<ControlMgr> mgr(new ControlMgr);
		return mgr;
	}
}




#endif