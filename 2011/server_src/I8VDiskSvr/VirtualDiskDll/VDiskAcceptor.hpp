#ifndef __VDISK_ACCEPTOR_HPP
#define __VDISK_ACCEPTOR_HPP


#include "../../../include/MultiThread/Thread.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"

namespace i8desk
{


	class VDiskService;

	// --------------------------
	// class VDiskAcceptor

	class VDiskAcceptor
	{

	private:
		async::iocp::IODispatcher &io_;
		VDiskService &svr_;

		// Socket Acceptor
		async::network::Tcp::Accpetor acceptor_;
		// Socket Acceptor Thread
		async::thread::ThreadImplEx thread_;

	public:
		explicit VDiskAcceptor(VDiskService &svr, async::iocp::IODispatcher &io);

	public:
		// Æô¶¯
		void Start(LPCTSTR ip, WORD port);
		// ½áÊø
		void Stop();

	private:
		void _OnAccept(u_long error, const async::network::SocketPtr &remoteSocket);

	private:
		DWORD _Thread();
	};
}

#endif