#include "stdafx.h"
#include "DataMgr.hpp"

#include "../../../include/Extend STL/StringAlgorithm.h"


namespace view
{
	DataMgr::DataMgr(u_short port, const std::string &ip, const Callback &callback)
		: io_()
		, sck_(io_, network::Tcp::V4())
		, callback_(callback)
		, port_(port)
		, ip_(ip)
	{

	}

	DataMgr::~DataMgr()
	{

	}


	void DataMgr::Start()
	{
		try
		{
			if( !sck_.IsOpen() )
				sck_.Open();

			sck_.AsyncConnect(network::IPAddress::Parse(ip_), port_, 
				std::tr1::bind(&DataMgr::_OnConnect, shared_from_this(), iocp::_Size, iocp::_Error));
		}
		catch(std::exception &e)
		{
			::OutputDebugStringA(e.what());
		}
	}

	void DataMgr::Stop()
	{
		sck_.Close();
		io_.Stop();
	}

	BmpInfo DataMgr::GetBuffer()
	{
		return buffer_.Get();
	}

	const std::wstring &DataMgr::GetTaskID() const
	{
		return taskID_;
	}
	
	std::wstring DataMgr::GetTaskTime() const
	{
		if( taskTime_.empty() )
			return taskTime_;

		std::wstring date_time = taskTime_.substr(0, taskTime_.length() - 4);
		std::wstring m_second = taskTime_.substr(taskTime_.length() - 4, 3);

		time_t tmp = stdex::ToNumber<time_t>(date_time);
		CTime time(tmp);

		CString out_time = time.Format(L"%Y%m%d%H%M%S");
		out_time += m_second.c_str();
		return out_time;
	}

	void DataMgr::_OnConnect(u_long size, u_long error)
	{
		if( error != 0 )
		{
			_DisConnect();
			return;
		}

		try
		{
			sck_.SetOption(network::RecvBufSize(2 * common::MAX_BMP_BUFFER_SIZE));

			while(1)
			{
				common::Buffer header = common::MakeBuffer(sizeof(common::TCPPKHeader));
				size_t headerLen = iocp::Read(sck_, iocp::Buffer(header.first.get(), header.second));
				assert(headerLen == header.second);

				common::TCPPKHeader *headerBuf = reinterpret_cast<common::TCPPKHeader *>(header.first.get());
				if( headerBuf->startFlag_ != common::StartFlag )
				{
					_DisConnect();
					break;
				}

				std::vector<std::wstring> params;
				std::wstring fileName(headerBuf->fileName_);
				stdex::Split(params, fileName, L'_');
				taskTime_ = params[0];
				taskID_ = params[1];


				common::Buffer tcpBuf = common::MakeBMPBuffer(headerBuf->length_);
				size_t bufferLen = iocp::Read(sck_, iocp::Buffer(tcpBuf.first.get(), tcpBuf.second));

				assert(bufferLen == headerBuf->length_);

				BmpInfo info;
				info.buf_ = tcpBuf;
				info.width_ = stdex::ToNumber<size_t>(params[4]);
				info.height_ = stdex::ToNumber<size_t>(params[5]);
				buffer_.Put(info);
				callback_();
			}
			
			//iocp::AsyncRead(sck_, iocp::Buffer(header.get(), sizeof(*header)), iocp::TransferAll(),
			//	std::tr1::bind(&DataMgr::_OnReadHeader, shared_from_this(), iocp::_Size, iocp::_Error, header));

		}
		catch(std::exception &e)
		{
			//_DisConnect();
			::OutputDebugStringA(e.what());
		}
	}

	void DataMgr::_OnReadHeader(u_long size, u_long error, const std::tr1::shared_ptr<common::TCPPKHeader> &header)
	{
		if( error != 0 || size == 0 || 
			header->startFlag_ != common::StartFlag )
		{
			_DisConnect();
			return;
		}

		try
		{
			common::Buffer tcpBuf = common::MakeBMPBuffer(header->length_);
			
			size_t len = iocp::Read(sck_, iocp::Buffer(tcpBuf.first.get(), tcpBuf.second), iocp::TransferAll());
			_OnReadContent(len, 0, tcpBuf);
			//iocp::AsyncRead(sck_, iocp::Buffer(tcpBuf.first.get(), tcpBuf.second), iocp::TransferAll(),
			//	std::tr1::bind(&DataMgr::_OnReadContent, shared_from_this(), iocp::_Size, iocp::_Error, tcpBuf));
		}
		catch(std::exception &e)
		{
			_DisConnect();
			::OutputDebugStringA(e.what());
		}
	}

	void DataMgr::_OnReadContent(u_long size, u_long error, common::Buffer tcpBuf)
	{
		if( error != 0 || size == 0 || 
			size != tcpBuf.second )
		{
			_DisConnect();
			return;
		}

		try
		{
			//buffer_.Put(tcpBuf);

			callback_();

			std::tr1::shared_ptr<common::TCPPKHeader> header(new common::TCPPKHeader);
			size_t len = iocp::Read(sck_, iocp::Buffer(reinterpret_cast<char *>(header.get()), sizeof(*header)));
			_OnReadHeader(len, 0, header);

			//std::tr1::shared_ptr<common::TCPPKHeader> header(new common::TCPPKHeader);
			//char *buf = reinterpret_cast<char *>(header.get());
			//iocp::AsyncRead(sck_, iocp::Buffer(buf, sizeof(*header)), iocp::TransferAll(),
			//	std::tr1::bind(&DataMgr::_OnReadHeader, shared_from_this(), iocp::_Size, iocp::_Error, header));
		}
		catch(std::exception &e)
		{
			::OutputDebugStringA(e.what());
			_DisConnect();
		}
	}

	void DataMgr::_DisConnect()
	{
		taskID_.clear();
		taskTime_.clear();

		sck_.Close();
		
		Start();
	}
}