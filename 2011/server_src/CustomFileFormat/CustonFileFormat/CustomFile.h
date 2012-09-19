#ifndef __CUSTOM_FILE_FORMAT_HPP
#define __CUSTOM_FILE_FORMAT_HPP

#include <memory>



namespace custom_file
{


	typedef unsigned long DWORD;
	typedef std::tr1::shared_ptr<char> DataType;


	struct Deletor
	{
		template<typename T>
		void operator()(T *ptr)
		{
			::operator delete(ptr);
		}
	};



	struct FileInfo
	{
		long gid_;				// gid
		size_t offset_;			// 数据区偏移量
		size_t curSize_;		// 正在使用数据实际大小
		size_t realSize_;		// 分配空间大小
		size_t used_;			// 是否被使用
		DWORD crc_;				// CRC
		DataType data_;			// 实际数据内容

		FileInfo()
			: gid_(0), offset_(0), curSize_(0)
			, realSize_(0), used_(0), crc_(0)
		{}

		FileInfo(const FileInfo &rhs)
			: gid_(rhs.gid_), offset_(rhs.offset_), curSize_(rhs.curSize_)
			, realSize_(rhs.realSize_), used_(rhs.used_), crc_(rhs.crc_)
			, data_(rhs.data_)
		{}

		FileInfo &operator=(const FileInfo &rhs)
		{
			if( &rhs != this )
			{
				gid_		= rhs.gid_; 
				offset_		= rhs.offset_;
				curSize_	= rhs.curSize_;
				realSize_	= rhs.realSize_;
				used_		= rhs.used_; 
				crc_		= rhs.crc_;
				data_		= rhs.data_;
			}

			return *this;
		}
	};

	typedef std::tr1::shared_ptr<FileInfo>	FileInfoPtr;

	// 工厂函数
	inline FileInfoPtr MakeFileInfo()
	{
		return FileInfoPtr(new FileInfo);
	}
	inline FileInfoPtr MakeFileInfo(const FileInfo &fileInfo)
	{
		return FileInfoPtr(new FileInfo(fileInfo));
	}

	inline DataType MakeData(size_t size)
	{
		return DataType(static_cast<char *>(::operator new(size)), Deletor());
	}

	// 支持序列化
	template<typename CharT>
	std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &out, const FileInfo &file)
	{
		const size_t size = offsetof(FileInfo, data_);
		out.write(reinterpret_cast<const char *>(&file), size);
		out.write(file.data_.get(), file.realSize_);

		return out;
	}

	template<typename CharT>
	std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &in, FileInfo &file)
	{
		const size_t size = offsetof(FileInfo, data_);
		in.read(reinterpret_cast<char *>(&file), size);
		file.data_ = MakeData(file.realSize_);

		in.read(file.data_.get(), file.realSize_);

		return in;
	}


}

#endif