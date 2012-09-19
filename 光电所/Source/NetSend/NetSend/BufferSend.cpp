#include "stdafx.h"
#include "BufferSend.hpp"


namespace oc
{

	void BufferSend::SetBuffer(const char *buf, size_t len, const char *fileName)
	{
		buf_ = buf;
		buflen_ = len;
		fileName_ = fileName;
	}

	void BufferSend::Run()
	{
		_Subcontract();
	}

	void BufferSend::_Subcontract()
	{
		char subBuf[common::MAX_UDP_PACKAGE] = {0};

		const size_t dataLen = common::MAX_UDP_PACKAGE - sizeof(common::UDPPKHeader);
		size_t cnt = buflen_ / dataLen + 1;
		size_t left = buflen_ % dataLen;

		size_t index = 0;
		do
		{
			common::UDPPkHelper os(subBuf);

			size_t sendLen = 0;
			if( index == cnt - 1 )
				sendLen = left;
			else
				sendLen = dataLen;

			common::UDPPKHeader header = {common::StartFlag, buflen_, sendLen, cnt, index};
			strcpy_s(header.fileName_, _countof(header.fileName_), fileName_.c_str());

			os << header;
			os.PushPointer(buf_ + index * dataLen, sendLen);

			assert(os.Length() == sizeof(common::UDPPKHeader) + sendLen);

			++index;

			_Send(subBuf, os.Length());
		}while(index != cnt);
	}

	void BufferSend::_Send(const char *buf, size_t len)
	{
		try
		{
			size_t sendLen = sck_.SendTo(Buffer(buf, len), &addr_);
			assert(sendLen == len);

			char retBuf[4] = {0};
			SOCKADDR_IN senderAddr = {0};
			int senderLen = sizeof(senderAddr);
			size_t retLen = sck_.RecvFrom(Buffer(retBuf), &senderAddr);
			assert(retLen == sizeof(retBuf));
		}
		catch(std::exception &e)
		{
			::OutputDebugStringA(e.what());
		}

	}

	void BufferSend::_SendToImpl(size_t err, size_t len)
	{
	
	}
}