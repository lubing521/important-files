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


		// ----------------------------------

		SystemExcept::SystemExcept(const stdex::tString &msg, DWORD err/* = ::GetLastError()*/)
			: Base(msg)
		{
			stdex::tOstringstream oss;
			TCHAR *buffer = 0;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				0, err, 0, (LPTSTR)&buffer, 0, 0);
			oss << _T("Win32 Error(") << err << _T(": ") << buffer << _T(")--") ;

			::LocalFree(buffer);
			msg_ = oss.str() + msg;
		}


		// -----------------------------------

		RuntimeExcept::RuntimeExcept(const stdex::tString &msg)
			: Base(msg)
		{}

		RuntimeExcept::RuntimeExcept(LPCSTR msg)
			: Base(_T(""))
		{
			msg_ = CA2T(msg);
		}
	}
}