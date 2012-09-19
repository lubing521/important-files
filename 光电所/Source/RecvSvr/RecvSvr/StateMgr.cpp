#include "stdafx.h"
#include "StateMgr.hpp"


namespace recvSvr
{

	void StateMgr::Start(const OnReadCallback &readCallback)
	{	
		readCallback_ = readCallback;

		using namespace std::tr1::placeholders;
		try
		{
			acceptor_.AsyncAccept(0, std::tr1::bind(&StateMgr::_AcceptImpl, this, _1, _2));
		}
		catch(std::exception &e)
		{
			::OutputDebugStringA(e.what());
		}

	}

	void StateMgr::Stop()
	{
		sck_.reset();
	}

	void StateMgr::Send(size_t len)
	{
		try
		{
			iocp::AsyncWrite(*sck_, iocp::Buffer(buf_, len), iocp::TransferAll(), 
				std::tr1::bind(&StateMgr::_WriteImpl, this, iocp::_Size, iocp::_Error));
		}
		catch(std::exception &e)
		{
			::OutputDebugStringA(e.what());
			_ReStart();
		}
	}


	void StateMgr::_ReStart()
	{
		Stop();
		Start(readCallback_);
	}

	void StateMgr::_AcceptImpl(u_long error, const network::SocketPtr &acceptSocket)
	{
		try
		{
			if( error == 0 )
			{
				sck_.reset(new network::Tcp::Socket(acceptSocket));

				iocp::AsyncRead(*sck_, iocp::Buffer(buf_), iocp::TransferAtLeast(1/*sizeof(common::TCPPKHeader)*/),
					std::tr1::bind(&StateMgr::_ReadImpl, this, iocp::_Size, iocp::_Error));
			}
		}
		catch(const std::exception &e)
		{
			::OutputDebugStringA(e.what());
			_ReStart();
		}
	}

	void StateMgr::_ReadImpl(u_long size, u_long error)
	{
		if( error != 0 || size == 0 )
		{
			_ReStart();
			return;
		}

		readCallback_(buf_.data(), size);
	}

	void StateMgr::_WriteImpl(u_long size, u_long error)
	{
		common::TCPPKHeader *header = reinterpret_cast<common::TCPPKHeader *>(buf_.data());

		sck_->Close();
		sck_.reset();

		_ReStart();
	}


}