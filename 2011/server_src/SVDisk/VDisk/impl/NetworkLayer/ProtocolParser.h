#ifndef __PROTOCOL_PARSER_HPP
#define __PROTOCOL_PARSER_HPP



#include <exception>

namespace i8desk
{
	
	namespace networkimpl
	{
		// ----------------------------------------
		// class ProtocolParser

		template<typename ParserT>
		class ProtocolParser
			: public ParserT
		{
			typedef ParserT							ParserType;
			typedef typename ParserType::PacketInfo	PacketInfoType;
			typedef typename ParserType::HeaderType	HeaderType;

		public:
			ProtocolParser(const char *buffer, size_t bufLen)
				: ParserType(reinterpret_cast<const HeaderType *>(buffer))
			{
				assert(bufLen >= sizeof(HeaderType));

				if( bufLen < sizeof(HeaderType) )
					throw std::out_of_range("bufLen < sizeof(I8_SCSI__NET_PROTO");
			}

		public:


		};
	}
	

}

#endif