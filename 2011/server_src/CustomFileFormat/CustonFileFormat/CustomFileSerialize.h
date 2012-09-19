#ifndef __CUSTOM_FILE_SERIALIZE_HPP
#define __CUSTOM_FILE_SERIALIZE_HPP

#include <sstream>
#include <algorithm>
#include <atlbase.h>
#include <atlconv.h>

#include "../../../include/Extend STL/UnicodeStl.h"
#include "../../../include/Utility/utility.h"


namespace custom_file
{


	namespace detail
	{
		struct FileCount
		{
			template<typename T>
			bool operator()(const T &file) const
			{
				return file.second->used_ == 1;
			}
		};

		template < typename T >
		inline T *Allocator(size_t size)
		{
			return static_cast<T *>(::operator new(size * sizeof(T)));
		}

		struct Deletor
		{
			template < typename T >
			void operator()(T *val)
			{		
				::operator delete(val);
			}
		};
	}

	// ----------------------------------------------------
	// class TaskSerialize

	// 任务序列化
	template<typename ImplT>
	class FileInfoSerialize
	{
	private:
		stdex::tString filePath_;
		std::fstream stream_;

	public:
		explicit FileInfoSerialize(LPCTSTR filePath)
			: filePath_(filePath)
		{
			if( !::PathFileExists(filePath_.c_str()) )
			{
                utility::MakeSureDirectoryPathExists(filePath_.c_str());
				std::ofstream out(filePath_.c_str());
				out.close();
			}

			stream_.open(filePath_.c_str(), std::ios::binary | std::ios::in | std::ios::out);
			if( !stream_ )
			{
				std::stringstream error;
				error << "打开文件 (" << CT2A(filePath_.c_str()) << ") 进行序列化失败 ";
				throw std::runtime_error(error.str());
			}
		}
		~FileInfoSerialize()
		{

		}

	public:
		template<typename FileInfoT>
		void Update(const FileInfoT &info)
		{
			typedef typename FileInfoT value_type;
			size_t offset = info.offset_ - offsetof(value_type, data_);

			if( !stream_ )
				throw std::runtime_error("文件句柄无效!");

			stream_.seekp(offset);
			stream_ << info;
			stream_.flush();
		}

		template<typename FileInfoT>
		void UpdateIndex(const FileInfoT &info)
		{
			typedef typename FileInfoT value_type;
			size_t offset = info.offset_ - offsetof(value_type, data_);

			if( !stream_ )
				throw std::runtime_error("文件句柄无效!");

			stream_.seekp(offset);
			stream_.write(reinterpret_cast<const char *>(&info), offsetof(value_type, data_));
			stream_.flush();
		}

		// 从文件读取
		void LoadFromFile()
		{
			_LoadImpl(stream_);
		}

		// 写向文件
		void SaveToFile()
		{
			stream_.clear();
			if( !stream_ )
			{
				std::stringstream error;
				error << "打开文件 (" << CT2A(filePath_.c_str()) << ") 进行序列化失败 ";
				throw std::runtime_error(error.str());
			}

			_SaveImpl(stream_, static_cast<ImplT *>(this)->GetFilesInfo());
		}

	private:
		template<typename StreamT, typename ContainerT>
		void _SaveImpl(StreamT &os, ContainerT &container)
		{
			for(typename ContainerT::const_iterator iter = container.begin(); 
				iter != container.end(); ++iter)
			{
				const typename ContainerT::value_type::second_type::element_type &fileInfo = *((*iter).second);
				if( !fileInfo.used_ )
					continue;
				
				Update(fileInfo);
				UpdateIndex(fileInfo);
			}

			os.seekp(0, std::ios::beg);
			size_t count = std::count_if(container.begin(), container.end(), detail::FileCount());
			os.write((const char *)&count, sizeof(count));

			os.flush();
		}

		
		template<typename StreamT>
		void _LoadImpl(StreamT &in)
		{
			size_t n = 0;
			in.read((char *)&n, sizeof(n));
			
			for(int i = 0; i != n; ++i)
			{
				FileInfo info;
				in >> info;

				static_cast<ImplT *>(this)->Load(info);
			}
		}
	};
}





#endif