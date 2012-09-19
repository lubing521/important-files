#ifndef __CLIENT_EXCEPTION_HPP
#define __CLIENT_EXCEPTION_HPP


#include "../../../include/Utility/StackWalker.h"



namespace i8desk
{
	namespace exception
	{


		// ----------------------------------------------
		// Stack Walker

		class ExceptStack
			: public utility::IStackDumpHandler
		{
			virtual void OnBegin();
			virtual void OnEntry(void *pvAddress, size_t lineNum, LPCSTR fileName, LPCSTR szModule, LPCSTR szSymbol);
			virtual void OnError(LPCSTR szError);
			virtual void OnEnd();
		};



		// ----------------------------------------------
		// class Base

		class Base
		{
		protected:
			stdex::tString msg_;

		public:
			explicit Base(LPCTSTR msg, ExceptStack &stack = ExceptStack());
			explicit Base(const stdex::tString &msg, ExceptStack &stack = ExceptStack());

			virtual ~Base();

		public:
			virtual const stdex::tString &What() const;
		};



		// -----------------------------------------
		// class SystemExcept

		class SystemExcept
			: public Base
		{
		public:
			explicit SystemExcept(const stdex::tString &msg, DWORD err = ::GetLastError());
		};



		// -----------------------------------------
		// class RuntimeExcept

		class RuntimeExcept
			: public Base
		{
		public:
			explicit RuntimeExcept(const stdex::tString &msg);
			explicit RuntimeExcept(LPCSTR msg);
		};



		// ================================================
		// 处理回调

		struct Excute
		{
			template < typename CallbackT >
			void Run(const CallbackT &callback)
			{
				callback();
			}

			template < typename CallbackT, typename Arg1 >
			void Run(const CallbackT &callback, const Arg1 &arg1)
			{
				callback(arg1);
			}

			template < typename CallbackT, typename Arg1, typename Arg2 >
			void Run(const CallbackT &callback, const Arg1 &arg1, const Arg2 &arg2)
			{
				callback(arg1, arg2);
			}

			template < typename CallbackT, typename Arg1, typename Arg2, typename Arg3 >
			void Run(const CallbackT &callback, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
			{
				callback(arg1, arg2, arg3);
			}

			template < typename CallbackT, typename Arg1, typename Arg2, typename Arg3, typename Arg4 >
			void Run(const CallbackT &callback, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
			{
				callback(arg1, arg2, arg3, arg4);
			}

			template < typename CallbackT, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
			void Run(const CallbackT &callback, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
			{
				callback(arg1, arg2, arg3, arg4, arg5);
			}
		};



		// Exception Handle

		namespace detail
		{
			template <typename CallbackT, typename ExceptT>
			inline void Translate(const CallbackT &callback, const ExceptT &e)
			{
				stdex::tString msg(e.What());

				callback(std::tr1::cref(msg));
			}
		}



		template <typename OpT, typename CallbackT >
		inline bool ExceptHandle(const OpT &op, const CallbackT &callback)
		{
			try
			{
				op();
			}
			catch(Base &e)
			{
				detail::Translate(callback, e);

				return false;
			}

			return true;
		}


		template < typename BufferT, typename OpT, typename CallbackT >
		inline bool ExceptHandle(BufferT &buffer, const OpT &op, const CallbackT &callback)
		{
			try
			{
				buffer = op();
			}
			catch(Base &e)
			{
				detail::Translate(callback, e);

				return false;
			}

			return true;
		}

	}
}




#endif