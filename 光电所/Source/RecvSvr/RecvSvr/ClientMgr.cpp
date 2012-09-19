#include "stdafx.h"
#include "ClientMgr.hpp"

#include <array>

using namespace async;

namespace recvSvr
{

	class Session
		: public std::tr1::enable_shared_from_this<Session>
	{
		iocp::IODispatcher &io_;
		network::Tcp::Socket socket_;
		typedef std::tr1::function<void(const network::SocketPtr &)> DisConnect;
		DisConnect disConnect_;

	public:
		Session(iocp::IODispatcher &io, const network::SocketPtr &sock, const DisConnect &disConnect)
			: io_(io)
			, socket_(sock)
			, disConnect_(disConnect)
		{

		}
		~Session()
		{
			Stop();
		}


		void Start()
		{
			socket_.SetOption(network::SendBufSize(2 * common::MAX_BMP_BUFFER_SIZE));
		}

		void Stop()
		{
			socket_.Close();
		}

		void Run(const common::BmpInfo &buf)
		{	
			try
			{
				common::Buffer header = common::MakeBuffer(sizeof(common::TCPPKHeader));
				common::TCPPKHeader *headerBuf = reinterpret_cast<common::TCPPKHeader *>(header.first.get());

				headerBuf->startFlag_ = common::StartFlag;
				headerBuf->cmd_ = common::CMD_VIEW_DATA;
				_tcscpy(headerBuf->fileName_, buf.second.c_str());
				headerBuf->length_ = buf.first.second;

				//size_t headerlen = iocp::Write(socket_, iocp::Buffer(reinterpret_cast<const char *>(header.first.get()), header.second));
				//assert(headerlen == sizeof(common::TCPPKHeader));

				//size_t contentLen = iocp::Write(socket_, iocp::Buffer(buf.first.first.get(), buf.first.second));
				//assert(contentLen == buf.first.second);

				
				iocp::AsyncWrite(socket_, iocp::Buffer(header.first.get(), header.second), iocp::TransferAll(), 
					std::tr1::bind(&Session::_HandleWriteHeader, shared_from_this(), iocp::_Size, iocp::_Error, header, buf.first));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				_DisConnect();
			}
		}

	private:
		void _HandleRead(u_long bytes, u_long error)
		{
			
		}

		void _HandleWriteHeader(u_long bytes, u_long error, const common::Buffer &header, const common::Buffer &content)
		{
			if( bytes == 0 || error != 0 )
			{
				_DisConnect();
				return;
			}

			assert(bytes == header.second);

			try
			{		
				iocp::AsyncWrite(socket_, iocp::Buffer(content.first.get(), content.second), iocp::TransferAll(), 
					std::tr1::bind(&Session::_HandleWriteContent, shared_from_this(), iocp::_Size, iocp::_Error, content));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				_DisConnect();
			}
		}

		void _HandleWriteContent(u_long bytes, u_long error, const common::Buffer &content)
		{
			if( bytes == 0 || error != 0 )
			{
				_DisConnect();
				return;
			}

			assert(bytes == content.second);
		}

		void _Dispatch()
		{
			disConnect_(socket_.Get());
		}
		void _DisConnect()
		{
			io_.Post(std::tr1::bind(&Session::_Dispatch, shared_from_this()));
		}
	};


	void SessionMgr::Start()
	{
		try
		{
			acceptor_.AsyncAccept(0, 
				std::tr1::bind(&SessionMgr::_OnAccept, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
		} 
		catch(const std::exception &e)
		{
			::OutputDebugStringA(e.what());
		}
	}

	void SessionMgr::Stop()
	{
		acceptor_.Close();

		struct OP
		{
			void operator()(const SessionContainer::value_type& val)
			{
				val.second->Stop(); 
			}
		};
		sessions_.for_each(OP());
		sessions_.clear();
	}


	void SessionMgr::Run(const common::BmpInfo &buffer) const
	{
		if( ::InterlockedExchangeAdd(&isPaused_, 0) == 1 )
			return;

		struct OP
		{
			const common::BmpInfo &buffer_;
			OP(const common::BmpInfo &buffer)
				: buffer_(buffer)
			{}
			void operator()(const SessionContainer::value_type& val) const
			{ 
				val.second->Run(std::tr1::cref(buffer_));
			}
		};
		sessions_.for_each(OP(buffer));
	}

	void SessionMgr::_OnAccept(u_long error, const network::SocketPtr &acceptSocket)
	{
		using namespace std::tr1::placeholders;

		try
		{
			acceptor_.AsyncAccept(0, 
				std::tr1::bind(&SessionMgr::_OnAccept, this, _1, _2));


			SessionPtr session(new Session(io_, acceptSocket, 
				std::tr1::bind(&SessionMgr::_OnDisconnect, this, _1)));
			sessions_.insert(acceptSocket->GetHandle(), session);
			session->Start();

			SOCKADDR_IN addr = {0};
			int addrLen = sizeof(addr);
			::getsockname(acceptSocket->GetHandle(), (sockaddr *)&addr, &addrLen);
			std::string ip = network::IPAddress::Parse(addr.sin_addr.S_un.S_addr);
			acceptCallback_(std::tr1::cref(ip));
		}
		catch(const std::exception &e)
		{
			::OutputDebugStringA(e.what());
		}

		
	}

	void SessionMgr::_OnDisconnect(const network::SocketPtr &sck)
	{
		sessions_.erase(sck->GetHandle());
	}
}