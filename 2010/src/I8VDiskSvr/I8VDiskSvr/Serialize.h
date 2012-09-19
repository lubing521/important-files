#ifndef __VDISK_SERIALIZE_HPP
#define __VDISK_SERIALIZE_HPP

#include "../VirtualDisk/VirtualDisk.h"



namespace i8desk
{


	// 支持序列化
	template<typename T>
	inline std::ostream &operator<<(std::ostream &os, T &t)
	{
		os.write(reinterpret_cast<const char *>(&t), sizeof(T));

		return os;
	}
	template<typename T>
	inline std::ostream &operator<<(std::ostream &os, T *t)
	{
		return os << *t;
	}

	template<typename T>
	inline std::istream &operator>>(std::istream &in, T &t)
	{
		in.read(reinterpret_cast<char *>(&t), sizeof(T));

		return in;
	}
	template<typename T>
	inline std::istream &operator>>(std::istream &in, T *t)
	{
		return in >> *t;
	}

	// 支持对map的序列化
	template<typename KeyT, typename ValueT>
	std::ostream &operator<<(std::ostream &os, std::map<KeyT, ValueT> &container)
	{
		os << container.size() << std::endl;

		for(std::map<KeyT, ValueT>::iterator iter = container.begin(); 
			iter != container.end(); ++iter)
			os << iter->first << iter->second << std::endl;

		return os;
	}

	template<typename KeyT, typename ValueT>
	std::istream &operator>>(std::istream &in, std::map<KeyT, ValueT *> &container)
	{
		size_t n = 0;
		in >> n;

		for(int i = 0; i != n; ++i)
		{
			KeyT nKey = 0;
			ValueT *value = new ValueT;
			in >> nKey >> value;

			container.insert(std::make_pair(nKey, value));
		}

		return in;
	}

}

#endif