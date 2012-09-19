#define START_FLAG	0x5E7D


#pragma warning(push)
#pragma warning(disable:4200)
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

inline DWORD CPackageHelper::popData(LPBYTE lpData)
{
	return popString((LPSTR)lpData, FALSE);
}