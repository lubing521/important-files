#ifndef _i8desk_netlayer_h_
#define _i8desk_netlayer_h_

#include <wtypes.h>
#include <winsock2.h>
#include <stdexcept>
#include <comdef.h>
//#include "plug.h"

namespace i8desk
{

struct _File_Info :public WIN32_FIND_DATA
{
	char path[MAX_PATH];

};
	struct FileObj
	{
		HANDLE hMap ;
		char* MapAddr; //影射内存地址
		bool IsBlock; //是否是文件数据
		bool IsEnd;		//是否是文件为尾
		__int64 remain;	//剩余文件大小。
		__int64 offest;	//文件偏移。
	};

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

	struct IMemPool
	{
		virtual void  Release() = 0;

		virtual void  SetDefault(DWORD dwdefPoolSize, DWORD dwdefBlockSize, DWORD dwdefIncrease) = 0;
		virtual DWORD GetDefPoolSize() = 0;
		virtual DWORD GetDefBlockSize() = 0;
		virtual DWORD GetDefIncrease() = 0;

		virtual char* Alloc(DWORD dwSize=0) = 0;
		virtual void  Free(void* pBlock) = 0;
		virtual void  FreeAll() = 0;
	
		virtual DWORD GetBufSize(void* pBlock) = 0; 
		virtual BOOL  IsAllocFrom(void* pBlock) = 0;
	};
	struct INetLayer;
	struct INetLayerEvent
	{
		virtual void Release() = 0;
		
		virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck) = 0;
		virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,FileObj& obj) = 0; 
		virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage) = 0;
		virtual void OnClose(INetLayer*pNetLayer, SOCKET sck) = 0;
	};

	struct IPackageParse
	{
		virtual void Release() = 0;
		virtual DWORD ParasePackage(char* package, DWORD length) = 0;
	};

	


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

// 	void  CPackageHelper::pushIntFromVariant(const _variant_t& var, DWORD byteNum)
// 	{
// 		if (var.vt == VT_NULL)
// 		{
// 			if (byteNum == 1)
// 				pushByte(0);
// 			else if (byteNum == 2)
// 				pushWORD(0);
// 			else if (byteNum == 4)
// 				pushDWORD(0);
// 		}
// 		else
// 		{
// 			if (byteNum == 1)
// 				pushByte((BYTE)var);
// 			else if (byteNum == 2)
// 				pushWORD((short)var);
// 			else if (byteNum == 4)
// 				pushDWORD((long)var);
// 		}			
// 	}

// 	void  CPackageHelper::pushStringFromVariant(const _variant_t& var, BOOL bHaveZero)
// 	{
// 		if (var.vt == VT_NULL)
// 		{
// 			pushString("", 0);
// 		}
// 		else
// 		{
// 			_bstr_t bstr = (_bstr_t)var;
// 			pushString(bstr, lstrlenA(bstr), bHaveZero);
// 		}
// 	}

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

	inline DWORD CPackageHelper::popData(LPBYTE lpData)
	{
		return popString((LPSTR)lpData, FALSE);
	}
}

#endif
