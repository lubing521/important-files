#ifndef __FILESYSTEM_FILE_CHANGE_HPP
#define __FILESYSTEM_FILE_CHANGE_HPP

#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/Buffer.hpp"
#include "detail/FileChangeHook.hpp"

namespace async
{
	using namespace iocp;

	


	namespace iocp
	{
		// forward declare 
		typedef std::tr1::function<void(u_long, u_long, FILE_NOTIFY_INFORMATION *)> FileChangeCallbackType;

		struct FileChangeCallback;


		// 
		template < >
		struct IOAsyncCallback< FileChangeCallback >
		{
			void operator()(FileChangeCallback *p)
			{
				ObjectDeallocate<FileChangeCallback>(p);
			}	
		};

		typedef Pointer<FileChangeCallback, IOAsyncCallback<FileChangeCallback>> FileChangeCallbackPtr;

		inline FileChangeCallback *MakeFileChangeCallback(const FileChangeCallbackType &handler)
		{
			return ObjectAllocate<FileChangeCallback>(handler);
		}



		struct FileChangeCallback
			: public iocp::AsyncCallbackBase
		{
			static const size_t BUFFER_LEN	= 64 * 1024;
			static const size_t PADDING		= sizeof(u_long);

			FileChangeCallbackType handler_;
			std::tr1::aligned_storage<BUFFER_LEN, PADDING>::type buffer_;

			FileChangeCallback(const FileChangeCallbackType &handler)
				: handler_(handler)
			{
			}

			virtual ~FileChangeCallback()
			{}

			virtual void Invoke(AsyncCallbackBase *p, u_long size, u_long error)
			{
				FILE_NOTIFY_INFORMATION *notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&buffer_);
				
				handler_(size, error, notifyInfo);
				
				FileChangeCallbackPtr ptr(static_cast<FileChangeCallback *>(p));
			}
		};

		template < >
		struct ObjectFactory< FileChangeCallback >
		{
			typedef memory::FixedMemoryPool<true, sizeof(FileChangeCallback)>	PoolType;
			typedef ObjectPool<PoolType>										ObjectPoolType;
		};


	}



	namespace filesystem
	{
	

		// -----------------------------------------
		// class FileChange

		class FileSystemMonitor
		{
		private:
			// File Handle
			HANDLE file_;
			DWORD filter_;

			// IO服务
			iocp::IODispatcher &io_;

		public:
			explicit FileSystemMonitor(iocp::IODispatcher &, 
				DWORD filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY);
			FileSystemMonitor(iocp::IODispatcher &, LPCTSTR,
				DWORD filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY);
			~FileSystemMonitor();

			// non-copyable
		private:
			FileSystemMonitor(const FileSystemMonitor &);
			FileSystemMonitor &operator=(const FileSystemMonitor &);


		public:
			// explicit转换
			operator HANDLE()					{ return file_; }
			operator const HANDLE () const		{ return file_; }

			// 显示获取
			HANDLE GetHandle()					{ return file_; }
			const HANDLE GetHandle() const		{ return file_; }

		public:
			void Open(LPCTSTR path);

			void Close();

			bool IsValid() const
			{
				return file_ != INVALID_HANDLE_VALUE;
			}

		public:
			template < typename HandlerT >
			void Monitor(const HandlerT &handler)
			{
				if( !IsValid() )
					throw std::logic_error("File Path Not Valid");

				typedef detail::FileChangeHandle<HandlerT> FileChangeHandle;
				iocp::FileChangeCallbackPtr asynResult(iocp::MakeFileChangeCallback(FileChangeHandle(*this, handler)));
				
				DWORD ret = 0;
				BOOL suc = ::ReadDirectoryChangesW(file_, &asynResult->buffer_, FileChangeCallback::BUFFER_LEN, TRUE, 
					filter_, &ret, asynResult.Get(), 0);
				
				if( !suc )
					throw Win32Exception("ReadDirectoryChangesW");

				asynResult.Release();
			}
		};
	}
}




#endif