#ifndef __PACKAGE_PARSER_IMPL_HPP
#define __PACKAGE_PARSER_IMPL_HPP


#include "../../vscsi_io.h"
#include <tuple>

namespace i8desk
{

	namespace networkimpl
	{
		// -------------------------------------------------
		// class ProtocolV1

		struct ProtocolV1
		{
			// Op Code, Start Address, Length
			typedef std::tr1::tuple<OP_CODE, size_t, size_t>	PacketInfo;
			typedef I8_SCSI__NET_PROTO							HeaderType;

			ProtocolV1(const HeaderType *const header)
				: header_(header)
			{}

			unsigned char Ver() const
			{
				return header_->ver;
			}
			OP_CODE OP() const
			{
				return header_->op_code;
			}
			size_t Offset() const
			{
				return header_->op_param_offset;
			}
			size_t Length() const
			{
				return header_->op_param_len;
			}

			PacketInfo GetPacketInfo() const
			{
				std::tr1::make_tuple(header_->op_code, header_->op_param_offset, header_->op_param_len);
			}

		public:
			// 构造头
			static size_t ConstructHeader(char *buffer, OP_CODE op, size_t size, REQ_STATUS status = STATUS_OK)
			{
				HeaderType *tmp = reinterpret_cast<HeaderType *>(buffer);

				tmp->ver				= VER;
				tmp->status				= status;
				tmp->op_code			= op;
				tmp->op_param_len		= size;
				tmp->op_param_offset	= 0;
				tmp->packetsize			= sizeof(HeaderType) + size;

				return sizeof(HeaderType);
			}

			// 构造数据包
			static size_t ConstructPackage(char *buffer, size_t bufLen, char *data, size_t dataLen, OP_CODE op, REQ_STATUS status = STATUS_OK)
			{
				const size_t packageLen = sizeof(HeaderType) + dataLen;
				assert(bufLen >= packageLen);

				ConstructHeader(buffer, op, dataLen, status);
				std::copy(data, data + dataLen, buffer + sizeof(HeaderType));

				return packageLen;
			}

			template<typename ParamT>
			static size_t ConstructPackage(char *buffer, size_t bufLen, const ParamT &var, OP_CODE op, REQ_STATUS status = STATUS_OK)
			{
				const size_t packageLen = sizeof(HeaderType) + sizeof(ParamT);
				assert(bufLen >= packageLen);

				ConstructHeader(buffer, op, sizeof(ParamT), status);
				::memcpy(buffer + sizeof(HeaderType), &var, sizeof(ParamT));

				return packageLen;
			}

		private:
			const HeaderType *const header_;
		};

		
	}


}



#endif