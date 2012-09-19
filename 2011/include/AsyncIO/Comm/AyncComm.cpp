#include "stdafx.h"

#include "File.hpp"

#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/WinException.hpp"


namespace  async
{



	namespace comm
	{
		AsyncComm::AsyncComm(IODispatcher &io, LPCTSTR lpszFilePath, DWORD dwAccess, DWORD dwShareMode, 
			LPSECURITY_ATTRIBUTES lpAttributes, DWORD dwCreatePosition, DWORD dwFlag, HANDLE hTemplate/* = NULL*/)
			: asyncComm_(INVALID_HANDLE_VALUE)
			, io_(io)
		{
			// 创建文件句柄
			asyncComm_ = ::CreateFile(lpszFilePath, dwAccess, dwShareMode, lpAttributes, dwCreatePosition, dwFlag, hTemplate);
			if( asyncComm_ == INVALID_HANDLE_VALUE )
				throw Win32Exception("CreateFile");

			// 不触发文件对象 Vista
			//::SetFileCompletionNotificationModes(file_, FILE_SKIP_EVENT_ON_HANDLE);

			io_.Bind(asyncComm_);
		}	

		AsyncComm::~AsyncComm()
		{
			Close();
		}



		// Read
		AsyncResultPtr AsyncComm::BeginRead(const BufferPtr &buf, size_t nOffset, size_t nBufSize, PLARGE_INTEGER offset
			, AsyncCallbackFunc callback/* = 0*/, const ObjectPtr &asyncState/* = nothing*/ , const ObjectPtr &internalState/* = nothing*/)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, buf, asyncState, internalState, callback));
			asynResult->AddRef();

			// 设置偏移
			if( offset != NULL )
			{
				(static_cast<OVERLAPPED *>(asynResult.Get()))->Offset		= offset->LowPart;
				(static_cast<OVERLAPPED *>(asynResult.Get()))->OffsetHigh	= offset->HighPart;
			}	

			if( !::ReadFile(asyncComm_, buf->data(nOffset), nBufSize, NULL, asynResult.Get()) &&
				::GetLastError() != ERROR_IO_PENDING )
			{
				asynResult->Release();
				throw Win32Exception("ReadFile");
			}

			return asynResult;
		}

		size_t AsyncComm::EndRead(const AsyncResultPtr &asynResult)
		{
			return _EndAsyncOperation(asynResult);
		}


		// Write
		AsyncResultPtr AsyncComm::BeginWrite(const BufferPtr &buf, size_t nOffset, size_t nBufSize, PLARGE_INTEGER offset
			, AsyncCallbackFunc callback/* = 0*/, const ObjectPtr &asyncState/* = nothing*/, const ObjectPtr &internalState/* = nothing*/)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, buf, asyncState, internalState, callback));
			asynResult->AddRef();

			// 设置偏移
			if( offset != NULL )
			{
				(static_cast<OVERLAPPED *>(asynResult.Get()))->Offset = offset->LowPart;
				(static_cast<OVERLAPPED *>(asynResult.Get()))->OffsetHigh = offset->HighPart;
			}		


			if( !::WriteFile(asyncComm_, buf->data(nOffset), nBufSize, NULL, asynResult.Get()) &&
				::GetLastError() != ERROR_IO_PENDING )
			{
				asynResult->Release();
				throw Win32Exception("WriteFile");
			}

			return asynResult;
		}

		size_t AsyncComm::EndWrite(const AsyncResultPtr &asynResult)
		{
			return _EndAsyncOperation(asynResult);
		}


		void AsyncComm::Close()
		{
			if( asyncComm_ != INVALID_HANDLE_VALUE )
			{
				::CloseHandle(asyncComm_);
				asyncComm_ = INVALID_HANDLE_VALUE;
			}
		}


		size_t AsyncComm::_EndAsyncOperation(const AsyncResultPtr &asyncResult)
		{
			DWORD size = 0;

			if( 0 == ::GetOverlappedResult(asyncComm_, asyncResult.Get(), &size, TRUE) )
				throw Win32Exception("GetOverlappedResult");

			return size;
		}
	}

}