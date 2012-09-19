#ifndef __ASYNC_DATA_HELPER_HPP
#define __ASYNC_DATA_HELPER_HPP

#include <functional>
#include "Network/IOService.h"


namespace i8desk
{
	// ------------------------------------------
	// 异步获取游戏数据

	template < typename R = void >
	struct AsyncCallbackHelper
	{
		typedef std::tr1::function<R()> CallbackType;
	};
	typedef AsyncCallbackHelper<>::CallbackType AsyncCallback;


	template < typename R >
	struct AsyncData
		: public std::tr1::enable_shared_from_this<AsyncData<R>>
	{
		typedef typename AsyncCallbackHelper<R>::CallbackType CallbackType;
		CallbackType callback_;

		explicit AsyncData(const CallbackType &callback)
			: callback_(callback)
		{}

		void Run()
		{
			i8desk::io::GetIODispatcher().Post(
				std::tr1::bind(&AsyncData::_RunImpl, shared_from_this()));
		}

		void _RunImpl()
		{
			callback_();
		}
	};
	

	template < typename R >
	inline void AsyncDataHelper(const std::tr1::function<R()> &handler)
	{
		std::tr1::shared_ptr<AsyncData<R>> async(new AsyncData<R>(handler));
		async->Run();
	}


	inline void AsyncDataHelper(const std::tr1::function<void()> &handler)
	{
		std::tr1::shared_ptr<AsyncData<void>> async(new AsyncData<void>(handler));
		async->Run();
	}
}

#endif