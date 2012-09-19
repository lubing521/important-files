#ifndef _i8desk_netlayer_h_
#define _i8desk_netlayer_h_

#include <wtypes.h>
#include <tchar.h>
#include <winsock2.h>
#include <stdexcept>
#include <comdef.h>
#include <assert.h>
#include "define.h"

namespace i8desk
{
	//数据包协议头
	#pragma warning(push)
	#pragma warning(disable:4200)
	#define START_FLAG	0x5E7D
	struct _packageheader
	{
		WORD  StartFlag;
		WORD  Cmd;
		DWORD Length;
		WORD  Version;
		BYTE  Resv[6];
		BYTE  data[0];
	};
	#pragma warning(pop)

	//内存池组件，使用INetLayer组件，必须通过该内存池来分配接收，发送缓冲区
	struct IMemPool
	{
		virtual void  Release() = 0;

		//初始池中分配的节点数,每个结点块的大小,增长数。
		virtual void  SetDefault(DWORD dwdefPoolSize, DWORD dwdefBlockSize, DWORD dwdefIncrease) = 0;
		virtual DWORD GetDefPoolSize() = 0;		
		virtual DWORD GetDefBlockSize() = 0;
		virtual DWORD GetDefIncrease() = 0;

		//分配，释放，释放所有内存池中节点
		virtual char* Alloc(DWORD dwSize=0) = 0;	//如果分配大于默认的节点块大小时，需要告诉需要分配的大小
		virtual void  Free(void* pBlock) = 0;
		virtual void  FreeAll() = 0;
	
		//得到分配的一个节点的大小
		virtual DWORD GetBufSize(void* pBlock) = 0; 
		//检查内存节点是否从内存池中分配
		virtual BOOL  IsAllocFrom(void* pBlock) = 0;
	};

	struct INetLayerEvent;

	//数据包头解析组件。用于分包处理。如果没有向INetLayer设置一个实例，组件在内部有一个默认实现。
	struct IPackageParse
	{
		virtual void  Release() = 0;
		virtual DWORD ParasePackage(char* package, DWORD length) = 0;
	};

	//网络层组件
	struct INetLayer
	{
		virtual void Release() = 0;

		//设置，取得网络层事件处理接口指针
		virtual void SetINetLayerEvent(INetLayerEvent* pINetLayerEvent) = 0;
		virtual INetLayerEvent* GetINetLayerEvent() = 0;

		//设置，取得内存池接口指针
		virtual void SetIMemPool(IMemPool* pIMemPool) = 0;
		virtual IMemPool* GetIMemPool() = 0;

		//设置，取得包解析接口指针
		virtual void SetIPackageParse(IPackageParse* pPackageParse) = 0;
		virtual IPackageParse* GetIPackageParse() = 0;

		//启动网络层，停止网络层组件
		virtual DWORD Start(WORD wPort = 7918) = 0; //参数：需要监听的端口
		virtual void  Stop() = 0;
		virtual WORD  GetPort() = 0;

		//发送给客户端一个数据包
		virtual void  SendPackage(SOCKET sck, LPSTR pPackage, DWORD length) = 0;

		//服务端主动断开一个客户端
		virtual void  DisConnect(SOCKET sck) = 0;
	};

	struct INetLayerEvent
	{
		virtual void Release() = 0;

		//当客户端连接上来时，产生该事件
		virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param) = 0;

		//当服务端发客户端发送完成一个数据包时产生该事件
		virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) = 0; 

		//当服务端接收到客户端一个完整命令包时，产生该事件给应用层处理一个命令
		virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) = 0;

		//当客户端断开时产生该事件.
		virtual void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param) = 0;
	};

	BOOL __stdcall CreateIMemPool(IMemPool** pIMemPool);
	BOOL __stdcall CreateINetLayer(INetLayer** pINetLayer);

	//数据包缓存辅助管理类
	class CPackageHelper
	{
	public:
		CPackageHelper() 
			: m_lpPackage(0) 
		{
		}
		CPackageHelper(LPSTR lpPackage)
		{
			m_lpPackage = lpPackage;
			m_dwPosition = sizeof(_packageheader);
		}
		template<typename CharT, size_t N>
		CPackageHelper(CharT (&arr)[N])
			: m_lpPackage(arr)
			, m_nBufSiz(N)
			, m_dwPosition(sizeof(_packageheader))
		{
			assert(m_dwPosition + 4 <= m_nBufSiz);
			GetHeader()->StartFlag	= START_FLAG;
			GetHeader()->Version	= PRO_VERSION;
		}

		CPackageHelper(LPSTR lpPackage, size_t nBufSiz)
			: m_lpPackage(lpPackage)
			, m_nBufSiz(nBufSiz)
			, m_dwPosition(sizeof(_packageheader))
		{
			assert(m_dwPosition + 4 <= m_nBufSiz);
			GetHeader()->StartFlag	= START_FLAG;
			GetHeader()->Version	= PRO_VERSION;
		}

		~CPackageHelper()
		{
		}

	public:	
		void ResetBuffer(char *pbuf, size_t nBufSiz);
		char *GetBuffer(void) const { return m_lpPackage; }
		void  SetPackageLength(DWORD dwLength);
		DWORD GetLength() { return GetPackageLength(); }
		DWORD GetPackageLength();
		void  SetOffset(DWORD dwOffset);
		DWORD GetOffset();	
		
		_packageheader *GetHeader()
		{
			return (_packageheader*)m_lpPackage;
		}

		void SetCmd(WORD cmd) 
		{
			((_packageheader*)m_lpPackage)->Cmd = cmd; 
		}

		void  pushPackageHeader(_packageheader& header);
		void  popPackageHeader(_packageheader& header);

		void  pushByte(BYTE bValue);
		void  pushWORD(WORD wValue);
		void  pushDWORD(DWORD dwValue);
		void  pushQWORD(unsigned long long val);
		void  pushString(LPCSTR lpString, DWORD dwLength, BOOL bHaveZeor = TRUE);
		void  pushData(LPBYTE lpData, DWORD dwLength);
		void  pushIntFromVariant(const _variant_t& var, DWORD byteNum = 4);
		void  pushStringFromVariant(const _variant_t& var, BOOL bHaveZero = TRUE);
		
		template<typename T>
		inline void pushStruct(const T& value)
		{
			assert(m_dwPosition + sizeof(T) <= m_nBufSiz);
			memcpy(m_lpPackage + m_dwPosition, &value, sizeof(T));
			m_dwPosition += sizeof(T);
		}

		template<typename T>
		inline void popStruct(T& value)
		{
			if (GetHeader()->Length < (m_dwPosition + sizeof(T)))
				throw std::out_of_range("package out of range");

			memcpy(&value, m_lpPackage + m_dwPosition, sizeof(T));
			m_dwPosition += sizeof(T);
		}

		BYTE  popByte();
		WORD  popWORD();
		unsigned long long popQWORD();
		DWORD popDWORD();

		inline DWORD popString(char *_Dest, size_t _Size, BOOL bHaveZero = TRUE);
		template<size_t _Size> 
		inline DWORD popString(char (&_Dest)[_Size], BOOL bHaveZero = TRUE);
		
		DWORD popData(BYTE *_Dest, size_t _Size);

		template<size_t _Size>
		DWORD popData(BYTE (&_Dest)[_Size]);

	private:
		LPSTR m_lpPackage;
		size_t m_nBufSiz;
		DWORD m_dwPosition;
	};

	inline void CPackageHelper::ResetBuffer(char * pbuf, size_t nBufSiz)
	{
		m_lpPackage = pbuf;
		m_nBufSiz = nBufSiz;
		m_dwPosition = sizeof(_packageheader);
	
		assert(m_dwPosition + 4 <= m_nBufSiz);
		SetCmd(0);
	}

	inline void  CPackageHelper::SetPackageLength(DWORD dwLength)
	{	
		assert(dwLength <= m_nBufSiz);
		_packageheader* pheader = (_packageheader*)m_lpPackage;
		pheader->Length = dwLength;
	}

	inline DWORD CPackageHelper::GetPackageLength()
	{
		_packageheader* pheader = (_packageheader*)m_lpPackage;
		return pheader->Length;
	}

	inline void  CPackageHelper::SetOffset(DWORD dwOffset)
	{
		m_dwPosition = dwOffset;
	}

	inline DWORD CPackageHelper::GetOffset()
	{
		return m_dwPosition;
	}

	inline void  CPackageHelper::pushPackageHeader(_packageheader& header)
	{
		_packageheader* pheader = (_packageheader*)m_lpPackage;
		*pheader = header;
	}

	inline void  CPackageHelper::popPackageHeader(_packageheader& header)
	{
		_packageheader* pheader = (_packageheader*)m_lpPackage;
		header = *pheader;
	}

	inline void  CPackageHelper::pushByte(BYTE bValue)
	{
		assert(m_dwPosition + sizeof(BYTE) <= m_nBufSiz);
		m_lpPackage[m_dwPosition++] = bValue;
	}

	inline void  CPackageHelper::pushWORD(WORD wValue)
	{
		assert(m_dwPosition + sizeof(WORD) <= m_nBufSiz);
		*(WORD*)(&m_lpPackage[m_dwPosition]) = wValue;
		m_dwPosition += 2;
	}

	inline void  CPackageHelper::pushDWORD(DWORD dwValue)
	{
		assert(m_dwPosition + sizeof(DWORD) <= m_nBufSiz);
		*(DWORD*)(&m_lpPackage[m_dwPosition]) = dwValue;
		m_dwPosition += 4;
	}
	inline void  CPackageHelper::pushQWORD(unsigned long long val)
	{
		assert(m_dwPosition + sizeof(val) <= m_nBufSiz);
		*(unsigned long long*)(&m_lpPackage[m_dwPosition]) = val;
		m_dwPosition += 8;
	}

	inline void  CPackageHelper::pushString(LPCSTR lpString, DWORD dwLength, BOOL bHaveZeor)
	{
		assert(m_dwPosition + dwLength + 4 + bHaveZeor <= m_nBufSiz);
		pushDWORD(dwLength);
		memcpy(&m_lpPackage[m_dwPosition], lpString, dwLength);
		m_dwPosition += dwLength;
		if (bHaveZeor)
		{
			m_lpPackage[m_dwPosition] = 0;
			m_dwPosition ++;
		}
	}

	inline void  CPackageHelper::pushData(LPBYTE lpData, DWORD dwLength)
	{
		pushString((LPSTR)lpData, dwLength, FALSE);
	}

	inline void  CPackageHelper::pushIntFromVariant(const _variant_t& var, DWORD byteNum)
	{
		if (var.vt == VT_NULL)
		{
			if (byteNum == 1)
				pushByte(0);
			else if (byteNum == 2)
				pushWORD(0);
			else if (byteNum == 4)
				pushDWORD(0);
		}
		else
		{
			if (byteNum == 1)
				pushByte((BYTE)var);
			else if (byteNum == 2)
				pushWORD((short)var);
			else if (byteNum == 4)
				pushDWORD((long)var);
		}			
	}

	inline void  CPackageHelper::pushStringFromVariant(const _variant_t& var, BOOL bHaveZero)
	{
		if (var.vt == VT_NULL)
		{
			pushString("", 0);
		}
		else
		{
			_bstr_t bstr = (_bstr_t)var;
			pushString(bstr, lstrlenA(bstr), bHaveZero);
		}
	}

	inline BYTE  CPackageHelper::popByte()
	{
		_packageheader* header = (_packageheader*)m_lpPackage;
		if (header->Length < (m_dwPosition + 1))
		{
			throw std::out_of_range("package out of range");
		}

		BYTE b = m_lpPackage[m_dwPosition++];
		return b;
	}

	inline WORD  CPackageHelper::popWORD()
	{
		_packageheader* header = (_packageheader*)m_lpPackage;
		if (header->Length < (m_dwPosition + 2))
		{
			throw std::out_of_range("package out of range");
		}

		WORD w = *(WORD*)(&m_lpPackage[m_dwPosition]);
		m_dwPosition += 2;
		return w;
	}

	inline unsigned long long CPackageHelper::popQWORD()
	{
		_packageheader* header = (_packageheader*)m_lpPackage;
		if (header->Length < (m_dwPosition+8))
		{
			throw std::out_of_range("package out of range");
		}

		unsigned long long val = *(unsigned long long*)(&m_lpPackage[m_dwPosition]);
		m_dwPosition += 8;
		return val;
	}
	inline DWORD CPackageHelper::popDWORD()
	{
		_packageheader* header = (_packageheader*)m_lpPackage;
		if (header->Length < (m_dwPosition+4))
		{
			throw std::out_of_range("package out of range");
		}

		DWORD dw = *(DWORD*)(&m_lpPackage[m_dwPosition]);
		m_dwPosition += 4;
		return dw;
	}

	inline DWORD CPackageHelper::popString(char *_Dest, size_t _Size, BOOL bHaveZero)
	{
		DWORD dwLength = popDWORD();

		_packageheader* header = (_packageheader*)m_lpPackage;
		DWORD dwCursor = m_dwPosition + dwLength;
		if (bHaveZero) 
			dwCursor++;
		
		if (header->Length < dwCursor) {
			throw std::out_of_range("package out of range");
		}

		if (dwLength >= _Size) {
			throw std::out_of_range("buffer size is too small or string too long");
		}

		memcpy(_Dest, &m_lpPackage[m_dwPosition], dwLength);
		_Dest[dwLength] = 0;
		m_dwPosition += dwLength;

		if (bHaveZero)	
			m_dwPosition++;
		
		return dwLength;
	}

	template<size_t _Size>
	inline DWORD CPackageHelper::popString(char (&_Dest)[_Size], BOOL bHaveZero)
	{
		DWORD dwLength = popDWORD();

		DWORD dwCursor = m_dwPosition + dwLength;
		if (bHaveZero) 
			dwCursor ++;

		_packageheader* header = (_packageheader*)m_lpPackage;
		if (header->Length < dwCursor) {
			throw std::out_of_range("package out of range");
		}

		if (dwLength >= _Size) {
			throw std::out_of_range("buffer size is too small or string too long");
		}

		memcpy(_Dest, &m_lpPackage[m_dwPosition], dwLength);
		_Dest[dwLength] = 0;
		m_dwPosition += dwLength;
		if (bHaveZero)	
			m_dwPosition++;

		return dwLength;
	}

	inline DWORD CPackageHelper::popData(BYTE *_Dest, size_t _Size)
	{
		DWORD dwLength = popDWORD();
		if (_Size < dwLength) {
			throw std::out_of_range("buffer too small");
		}

		_packageheader* header = (_packageheader*)m_lpPackage;
		DWORD dwCursor = m_dwPosition + dwLength;
		if (header->Length < dwCursor) {
			throw std::out_of_range("package out of range");
		}

		memcpy(_Dest, &m_lpPackage[m_dwPosition], dwLength);
		m_dwPosition += dwLength;

		return dwLength;
	}

	template<size_t _Size>
	inline DWORD CPackageHelper::popData(BYTE (&_Dest)[_Size])
	{
		return popData(_Dest, _Size);
	}

	inline CPackageHelper& operator<<(CPackageHelper& package, long long out)
	{
		package.pushQWORD(out);
		return package;
	}	
	inline CPackageHelper& operator<<(CPackageHelper& package, unsigned long long out)
	{
		package.pushQWORD(out);
		return package;
	}	
	inline CPackageHelper& operator<<(CPackageHelper& package, DWORD out)
	{
		package.pushDWORD(out);
		return package;
	}
	inline CPackageHelper& operator<<(CPackageHelper& package, WORD out)
	{
		package.pushWORD(out);
		return package;
	}
	inline CPackageHelper& operator<<(CPackageHelper& package, long out)
	{
		package.pushDWORD(out);
		return package;
	}
	inline CPackageHelper& operator<<(CPackageHelper& package, int out)
	{
		package.pushDWORD(out);
		return package;
	}	
	inline CPackageHelper& operator<<(CPackageHelper& package, BYTE out)
	{
		package.pushByte(out);
		return package;
	}

	inline CPackageHelper& operator<<(CPackageHelper& package, char *out)
	{
		package.pushString(out, strlen(out));
		return package;
	}

	inline CPackageHelper& operator<<(CPackageHelper& package, const char *out)
	{
		package.pushString(out, strlen(out));
		return package;
	}
	inline CPackageHelper& operator<<(CPackageHelper& package, const std::string& out)
	{
		package.pushString(out.c_str(), out.size());
		return package;
	}

	template<typename T>
	inline CPackageHelper& operator<<(CPackageHelper& package, const T& out)
	{
		package.pushStruct(out);
		return package;
	}

	inline CPackageHelper& operator>>(CPackageHelper& package, long long& out)
	{
		out = package.popQWORD();
		return package;
	}	
	inline CPackageHelper& operator>>(CPackageHelper& package, unsigned long long& out)
	{
		out = package.popQWORD();
		return package;
	}	
	inline CPackageHelper& operator>>(CPackageHelper& package, DWORD& out)
	{
		out = package.popDWORD();
		return package;
	}
	inline CPackageHelper& operator>>(CPackageHelper& package, WORD& out)
	{
		out = package.popWORD();
		return package;
	}

	inline CPackageHelper& operator>>(CPackageHelper& package, long& out)
	{
		out = package.popDWORD();
		return package;
	}
	inline CPackageHelper& operator>>(CPackageHelper& package, int& out)
	{
		out = package.popDWORD();
		return package;
	}

	inline CPackageHelper& operator>>(CPackageHelper& package, BYTE& out)
	{
		out = package.popByte();
		return package;
	}

	inline CPackageHelper& operator>>(CPackageHelper& package, std::string& out)
	{
		DWORD dwLength = package.popDWORD();
		DWORD dwCursor = package.GetOffset() + dwLength + 1;
		if (package.GetHeader()->Length < dwCursor) {
			throw std::out_of_range("package out of range");
		}
		out.assign(package.GetBuffer() + package.GetOffset(), dwLength);
		package.SetOffset(dwCursor);

		return package;
	}

	template<typename T>
	inline CPackageHelper& operator>>(CPackageHelper& package, T& out)
	{
		package.popStruct(out);
		return package;
	}

	template<size_t _Size>
	inline CPackageHelper& operator>>(CPackageHelper& package, char (&_Desc)[_Size])
	{
		package.popString(_Desc, 1);
		return package;
	}
	
	template<size_t _Size>
	inline CPackageHelper& operator>>(CPackageHelper& package, BYTE (&_Desc)[_Size])
	{
		package.popData(_Desc);
		return package;
	}
	


}

#endif
