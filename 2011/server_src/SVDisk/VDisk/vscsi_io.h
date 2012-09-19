#pragma once


//Created By ZR @ 2010-08-26
//Version 1
//this file is for virtual scsi disk network comunication protocol
//all Ver fileds are based on value 1 


// 当前版本号
enum { VER = 1 };


typedef enum _OP_CODE
{
	OP_READ=1,
	OP_WRITE,
	OP_GET_DISK_SIZE
}OP_CODE;

typedef enum _REQ_STATUS
{
	STATUS_OK=1,
	STATUS_ERR 
}REQ_STATUS;

typedef struct _OP_PARAM_STRUCT
{
	UCHAR  Ver;   //版本号,根据版本号解析后面Param的所有数据结构，可以和I8_SCSI__NET_PROTO_REQ的Ver不一样 
	union Param
	{
		struct _READ
		{
			LARGE_INTEGER offset;
			LARGE_INTEGER len;
		};
		struct _WRITE
		{
			LARGE_INTEGER offset;
			LARGE_INTEGER len;
			UINT          offset_to_write;//表示要写的数据从本数据结构开始位置的偏移 
			                              //例如，本数据结构的偏移（即I8_SCSI__NET_PROTO_REQ.op_param_offset
										  //的值）为482，offset_to_write为5,则实际要写的数据在整个数据包的位置为
										  // 482+5=487
		};
	};
}OP_PARAM_STRUCT;



typedef struct  _I8_SCSI__NET_PROTO_REQ
{
	UCHAR       ver;     //版本号,根据版本号解析后面的所有数据结构 
	REQ_STATUS  status;  //对客户端发出去的请求来说无意义，客户端接收服务端数据包时，此字段代表之前的这个请求是否成功      
	OP_CODE		op_code;
	UINT		packetsize; //sizeof(I8_SCSI__NET_PROTO)+op_param_len
	UINT		op_param_offset;
	UINT		op_param_len;
}I8_SCSI__NET_PROTO,*PI8_SCSI__NET_PROTO;
