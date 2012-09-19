#include "stdafx.h"
#include "DataMgr.hpp"

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/UDP.hpp"
#include "../../../include/Win32/Network/INet.h"
#include "../../../include/Zip/ZipHelper.hpp"

#include <functional>
#include <vector>
#include <ctime>

#include <atlbase.h>
#include <atlconv.h>


using namespace async;


namespace sendsvr
{
	class DataMgr::RecvImpl
	{
		iocp::IODispatcher &io_;
		network::Udp::Socket recvSck_;

		u_int curTotalLen_;
		std::vector<common::Buffer> bufIndex_;

	public:
		RecvImpl(iocp::IODispatcher &io, u_short recvPort)
			: io_(io)
			, recvSck_(io_, network::Udp::V4(), recvPort)
			, curTotalLen_(0)
		{
			recvSck_.SetOption(network::RecvTimeOut(3000));
		}

		~RecvImpl()
		{

		}

		void Start()
		{

		}

		void Stop()
		{
			recvSck_.Close();
		}


		common::BmpInfo Run()
		{
			try
			{
				
				while(1)
				{
					common::Buffer udpBuf(common::MakeBuffer(common::MAX_UDP_PACKAGE));

					SOCKADDR_IN addr = {0};
					size_t len = recvSck_.RecvFrom(iocp::Buffer(udpBuf.first.get(), udpBuf.second), &addr);
					common::UDPPKHeader *header = reinterpret_cast<common::UDPPKHeader *>(udpBuf.first.get());
					
					if( len == 0 ||
						header->startFlag_ != common::StartFlag )
						throw std::runtime_error("data Invalid");
					
					char retBuf[4] = {0};
					recvSck_.SendTo(iocp::Buffer(retBuf), &addr);

					udpBuf.second = header->curLength_;

					// construct
					if( header->seqenceID_ == 0 )
					{
						bufIndex_.clear();
						bufIndex_.resize(header->totalID_);

						std::cout << header->fileName_ << "--" << ::time(0) << std::endl;
						curTotalLen_ = 0;
					}

					// Error !!!
					if( header->seqenceID_ >= bufIndex_.size() )
						throw std::runtime_error("data seqence id invalid");

					bufIndex_[header->seqenceID_] = udpBuf;
					curTotalLen_ += header->curLength_;


					// Success
					if( header->seqenceID_ == header->totalID_ - 1 &&
						curTotalLen_ == header->totalLength_ )
					{
						common::Buffer bmpBuffer = common::MakeBMPBuffer(curTotalLen_);

						size_t pos = 0;
						for(size_t i = 0; i != bufIndex_.size(); ++i)
						{
							const char *bmpUDPBuf = bufIndex_[i].first.get() + sizeof(common::UDPPKHeader);
							const size_t bmpUDPLen = bufIndex_[i].second;

							::memmove(bmpBuffer.first.get() + pos, bmpUDPBuf, bmpUDPLen);

							pos += bmpUDPLen;
						}
						
						return std::make_pair(bmpBuffer, (LPCWSTR)CA2W(header->fileName_));
					}
				}
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				static common::Buffer invalidBuffer;
				static std::wstring invalidFileName;
				return std::make_pair(invalidBuffer, invalidFileName);
			}
		}
	};

	class DataMgr::SendImpl
	{
		std::auto_ptr<WinInet::CInternetSession> session_;
		std::auto_ptr<WinInet::CFtpConnection> connection_;

		u_short ftpPort_;
		stdex::tString ftpIP_;
		stdex::tString ftpUserName_;
		stdex::tString ftpPassword_;
		stdex::tString ftpTmpPath_;

		

	public:
		SendImpl(u_short ftpPort, const stdex::tString &ftpIP, const stdex::tString &ftpUserName, const stdex::tString &ftpPassword, const stdex::tString &ftpTmpPath)
			: ftpPort_(ftpPort)
			, ftpIP_(ftpIP)
			, ftpUserName_(ftpUserName)
			, ftpPassword_(ftpPassword)
		{
			TCHAR path[MAX_PATH] = {0};
			_tcscpy(path, ftpTmpPath.c_str());
			::PathAddBackslash(path);
			ftpTmpPath_ = path;

			ftpTmpPath_ += _T("tmp.zip");
		}
		~SendImpl()
		{

		}


		void Start()
		{
			// FTP
			session_.reset(new WinInet::CInternetSession(L"SendSvr"));
			if( *session_ == 0 )
				return;

			connection_.reset(new WinInet::CFtpConnection(*session_, ftpIP_.c_str(), 
				ftpPort_, ftpUserName_.c_str(), ftpPassword_.c_str()));
			if( *connection_ == 0 )
				return;

			connection_->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 1 * 60 * 1000);
			connection_->SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 1 * 60 * 1000);
		}

		void Stop()
		{
			connection_.reset();
			session_.reset();
		}

		void Run(const common::BmpInfo &buffer)
		{
			if( *connection_ == 0 )
			{
				Start();
				return;
			}

			const std::wstring &bmp = buffer.second;

			try
			{
				// Zip
				zip::Zip(ftpTmpPath_, bmp, buffer.first);
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				::DeleteFile(ftpTmpPath_.c_str());
				return;
			}

			{
				// Ftp
				if( !connection_->Upload(ftpTmpPath_.c_str(), bmp.c_str(), FTP_TRANSFER_TYPE_BINARY) )
				{
					Start();
					return;
				}

				::DeleteFile(ftpTmpPath_.c_str());
			}

			{
				// Flag
				static stdex::tString flagPath = L"tmp.flag";
				if( !::PathFileExists(flagPath.c_str()) )
				{
					std::ofstream flag(flagPath.c_str(), std::ios::out);
					flag << 1;
				}

				std::wstring flagFile = bmp.substr(0, bmp.length() - 3) + L"flag";
				connection_->Upload(flagPath.c_str(), flagFile.c_str(), FTP_TRANSFER_TYPE_BINARY);
			}
		}
	};


	DataMgr::DataMgr(iocp::IODispatcher &io, u_short udpRecvPort, u_short ftpPort, const stdex::tString &ftpIP,
		const stdex::tString &ftpUserName, const stdex::tString &ftpPassword, const stdex::tString &ftpTmpPath)
		: recvImpl_(new RecvImpl(io, udpRecvPort))
		, sendImpl_(new SendImpl(ftpPort, ftpIP, ftpUserName, ftpPassword, ftpTmpPath))
		, isPaused_(0)
		, isOK_(1)
	{

	}
	DataMgr::~DataMgr()
	{

	}

	void DataMgr::Start()
	{
		exit_.Create(NULL, TRUE);

		recvThread_.RegisterFunc(std::tr1::bind(&DataMgr::_RecvThread, this));
		sendThread_.RegisterFunc(std::tr1::bind(&DataMgr::_SendThread, this));

		recvThread_.Start();
		sendThread_.Start();

		recvImpl_->Start();
		sendImpl_->Start();

		RegisterSendCallback(std::tr1::bind(&SendImpl::Run, sendImpl_.get(), std::tr1::placeholders::_1));

	}

	void DataMgr::Stop()
	{
		exit_.SetEvent();


		recvImpl_->Stop();
		sendImpl_->Stop();

		recvThread_.Stop();
		sendThread_.Stop();
	}

	void DataMgr::RegisterSendCallback(const SendCallback &callback)
	{
		sendCallbacks_.push_back(callback);
	}

	DWORD DataMgr::_RecvThread()
	{
		while(!recvThread_.IsAborted())
		{
			if( exit_.WaitForEvent(0) )
				break;

			common::BmpInfo bmpBuffer = recvImpl_->Run();

			// valid data
			if( bmpBuffer.first.second > common::MAX_BMP_BUFFER_SIZE )
			{
				::InterlockedExchange(&isOK_, 0);
				::OutputDebugString(L"Invalid data");
			}

			if( !IsPaused() )
			{
				if( queue_.Size() <= 2 )
					queue_.Put(bmpBuffer);
			}
		}
		
		return 0;
	}


	DWORD DataMgr::_SendThread()
	{
		while(!sendThread_.IsAborted())
		{
			common::BmpInfo buf = queue_.Get();
			if( exit_.WaitForEvent(0) )
				break;

			if( buf.first.second == 0 )
			{
				::InterlockedExchange(&isOK_, 0);
				continue;
			}

			struct OP
			{
				const common::BmpInfo &buffer_;
				OP(const common::BmpInfo &buffer)
					: buffer_(buffer)
				{}
				void operator()(const SendCallback &callback) const
				{ callback(std::tr1::cref(buffer_)); }
			};

			::InterlockedExchange(&isOK_, 1);
			std::for_each(sendCallbacks_.begin(), sendCallbacks_.end(), OP(buf));
		}

		return 0;
	}
}