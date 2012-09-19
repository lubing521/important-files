#include "stdafx.h"
#include "Exception.h"

#include <sstream>


namespace i8desk
{
	namespace exception
	{

		
		void ExceptStack::OnBegin()
		{

		}

		void ExceptStack::OnEntry(void *pvAddress, size_t lineNum, LPCSTR fileName, LPCSTR szModule, LPCSTR szSymbol)
		{
			std::stringstream os;
			os << fileName << "(" << lineNum << "): " << szSymbol << std::endl;
			
			::OutputDebugStringA(os.str().c_str());
		}
		
		void ExceptStack::OnError(LPCSTR szError)
		{

		}
		
		void ExceptStack::OnEnd()
		{

		}





		// -----------------------------------------

		Base::Base(LPCTSTR msg, ExceptStack &stack)
			: msg_(msg)
		{
			utility::DumpStack(&stack);
		}
		Base::Base(const stdex::tString &msg, ExceptStack &stack)
			: msg_(msg)
		{}

		Base::~Base()
		{}

		const stdex::tString &Base::What() const
		{
			return msg_;
		}


		// -----------------------------------------------
		// class Network

		Network::Network(LPCTSTR msg)
			: Base(msg)
		{
			msg_ = _T("Exception Network :");
			msg_ += msg;
		}
		Network::Network(const stdex::tString &msg)
			: Base(msg)
		{
			msg_ = _T("Exception Network: ");
			msg_ += msg;
		}

		Network::~Network()
		{}



		// -----------------------------------------------
		// class UnConnect

		UnConnect::UnConnect(LPCTSTR msg)
			: Network(msg)
		{
			msg_ = _T("UnConnect Server:");
			msg_ += msg;
		}
		UnConnect::UnConnect(const stdex::tString &msg)
			: Network(msg)
		{ 
			msg_ = _T("UnConnect Server: ");
			msg_ += msg;
		}

		UnConnect::~UnConnect()
		{}

		// -----------------------------------------------
		// class Businuss


		Businuss::Businuss(LPCTSTR msg)
			: Base(msg)
		{
			msg_ = _T("Exception Businuss :");
			msg_ += msg;
		}
		Businuss::Businuss(const stdex::tString &msg)
			: Base(msg)
		{
			msg_ = _T("Exception Businuss :");
			msg_ += msg;
		}

		Businuss::~Businuss()
		{}
	}
}