#ifndef __COMM_ASYNCCOMM_HPP
#define __COMM_ASYNCCOMM_HPP

#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/Buffer.hpp"







namespace async
{
	
	namespace comm
	{

		using namespace async::IOCP;

		// forward declare
		class AsyncComm;
		typedef pointer<AsyncComm> FilePtr;


		// class FileBuffer
		typedef BufferT<char, MemoryMgr::DEFAULT_SIZE, async::memory::MemAllocator<char, MemoryMgr::MemoryPool> > Buffer;
		//typedef BufferT<char, MemoryMgr::DEFAULT_FILE_SIZE, std::allocator<char> > FileBuffer;
		typedef pointer<Buffer> BufferPtr;



		//--------------------------------------------------------------------------------
		// class File

		class AsyncComm
			: public Object
		{
		private:
			HANDLE asyncComm_;
			IODispatcher &io_;

		public:
			AsyncComm(IODispatcher &, LPCTSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE hTemplate = NULL);
			virtual ~AsyncComm();

			// non-copyable
		private:
			AsyncComm(const AsyncComm &);
			AsyncComm &operator=(const AsyncComm &);


		public:
			HANDLE operator()()
			{
				return asyncComm_;
			}
			const HANDLE operator()() const
			{
				return asyncComm_;
			}

			HANDLE GetHandle()
			{
				return asyncComm_;
			}
			HANDLE GetHandle() const
			{
				return asyncComm_;
			}

		public:
			// Read
			AsyncResultPtr BeginRead(const BufferPtr &buf, size_t nOffset, size_t nBufSize, PLARGE_INTEGER fileOffset
				, AsyncCallbackFunc callback = 0, const ObjectPtr &asyncState = nothing, const ObjectPtr &internalState = nothing);
			size_t EndRead(const AsyncResultPtr &asynResult);

			// Write
			AsyncResultPtr BeginWrite(const BufferPtr &buf, size_t nOffset, size_t nBufSize, PLARGE_INTEGER fileOffset
				, AsyncCallbackFunc callback = 0, const ObjectPtr &asyncState = nothing, const ObjectPtr &internalState = nothing);

			size_t EndWrite(const AsyncResultPtr &asynResult);


			void Close();

		private:
			size_t _EndAsyncOperation(const AsyncResultPtr &asyncResult);
		};
	}





}




#endif