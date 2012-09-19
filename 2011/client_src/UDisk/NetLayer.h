#ifndef _i8desk_netlayer_h_
#define _i8desk_netlayer_h_

#include <wtypes.h>
#include <tchar.h>
#include <winsock2.h>
#include <stdexcept>
#include <comdef.h>


class CLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	CLock()			{ InitializeCriticalSection(&m_lock);	}
	~CLock()		{ DeleteCriticalSection(&m_lock);		}
	void Lock()		{ EnterCriticalSection(&m_lock);		}
	void UnLock()	{ LeaveCriticalSection(&m_lock);		}
};

//同步guard工具类  ex:CLock m_lock; CAutoLock<CLock> lock(m_lock);
template <class T>
struct CAutoLock		
{
private:
	T* m_pT;
public:
	CAutoLock(T* pt): m_pT(pt) 
	{
		if (m_pT != NULL)
			m_pT->Lock();
	}
	~CAutoLock()
	{
		if (m_pT != NULL)
			m_pT->UnLock();
	}
};

template <class T>
void Release_Interface(T &pt)
{
	if (pt != NULL)
		pt->Release();
	pt = NULL;
}

template <typename T>
void Delete_Pointer(T* & p)
{
	if (p != NULL)
	{
		delete p;
		p = NULL;
	}
}
template <typename T>
void Delete_Array(T* &p)
{
	if (p != NULL)
	{
		delete []p;
		p = NULL;
	}
}

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
		CPackageHelper(LPSTR lpPackage);
		~CPackageHelper();
	public:	
		void  SetPackageLength(DWORD dwLength);
		DWORD GetPackageLength();
		void  SetOffset(DWORD dwOffset);
		DWORD GetOffset();	

		void  pushPackageHeader(_packageheader& header);
		void  popPackageHeader(_packageheader& header);

		void  pushByte(BYTE bValue);
		void  pushWORD(WORD wValue);
		void  pushDWORD(DWORD dwValue);
		void  pushString(LPCSTR lpString, DWORD dwLength, BOOL bHaveZeor = TRUE);
		void  pushData(LPBYTE lpData, DWORD dwLength);
		void  pushIntFromVariant(const _variant_t& var, DWORD byteNum = 4);
		void  pushStringFromVariant(const _variant_t& var, BOOL bHaveZero = TRUE);

		BYTE  popByte();
		WORD  popWORD();
		DWORD popDWORD();
        DWORD popString(LPSTR lpString, BOOL bHaveZero = TRUE);
        DWORD popStringEx(LPSTR lpString);
		DWORD popData(LPBYTE lpData);
	private:
		LPSTR m_lpPackage;
		DWORD m_dwPosition;
	};

	inline CPackageHelper::CPackageHelper(LPSTR lpPackage)
	{
		m_lpPackage = lpPackage;
		m_dwPosition = sizeof(_packageheader);
	}

	inline CPackageHelper::~CPackageHelper()
	{
	}

	inline void  CPackageHelper::SetPackageLength(DWORD dwLength)
	{	
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
		m_lpPackage[m_dwPosition++] = bValue;
	}

	inline void  CPackageHelper::pushWORD(WORD wValue)
	{
		*(WORD*)(&m_lpPackage[m_dwPosition]) = wValue;
		m_dwPosition += 2;
	}

	inline void  CPackageHelper::pushDWORD(DWORD dwValue)
	{
		*(DWORD*)(&m_lpPackage[m_dwPosition]) = dwValue;
		m_dwPosition += 4;
	}

	inline void  CPackageHelper::pushString(LPCSTR lpString, DWORD dwLength, BOOL bHaveZeor)
	{
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

	inline DWORD CPackageHelper::popString(LPSTR lpString, BOOL bHaveZero)
	{
		DWORD dwLength = popDWORD();

		_packageheader* header = (_packageheader*)m_lpPackage;
		DWORD dwCursor = m_dwPosition + dwLength;
		if (bHaveZero) dwCursor ++;
		if (header->Length < dwCursor)
		{
			throw std::out_of_range("package out of range");
		}

		memcpy(lpString, &m_lpPackage[m_dwPosition], dwLength);
		lpString[dwLength] = 0;
		m_dwPosition += dwLength;
		if (bHaveZero)	m_dwPosition++;
		return dwLength;
	}

    inline DWORD CPackageHelper::popStringEx(LPSTR lpString)
    {
        DWORD dwLength = popDWORD() + 1;

        _packageheader* header = (_packageheader*)m_lpPackage;
        if (header->Length < m_dwPosition + dwLength * sizeof(WCHAR))
        {
            throw std::out_of_range("package out of range");
        }
        WCHAR awString[5000];

        memcpy(awString, &m_lpPackage[m_dwPosition], sizeof(WCHAR) * dwLength);
        m_dwPosition += dwLength * sizeof(WCHAR);
        CStringA sString = CW2A(awString);
        memcpy(lpString, LPCSTR(sString), sString.GetLength() + 1);
        return dwLength;
    }

	inline DWORD CPackageHelper::popData(LPBYTE lpData)
	{
		return popString((LPSTR)lpData, FALSE);
	}
}

#endif
