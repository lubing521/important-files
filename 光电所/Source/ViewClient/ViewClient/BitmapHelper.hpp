#ifndef _CBITMAPFILE_H_
#define _CBITMAPFILE_H_

class CBitmapFile 
	: public CGdiObject
{
public:

	static CBitmapFile* PASCAL FromHandle(HBITMAP hBitmap);

	// Constructors

	CBitmapFile();
	~CBitmapFile();

	BOOL LoadBitmap(LPCTSTR lpszFileName);

	BOOL CreateBitmap(int nWidth, int nHeight, UINT nBitCount, const void* lpBits);

	BOOL CreateBitmapIndirect(LPBITMAPINFO lpBitmapInfo, const void* lpBits);

	// Attributes

	operator HBITMAP() const;

	int GetBitmap(BITMAP* pBitMap);

protected:

	// Attributes

	int GetColorNumber(WORD wBitCount);

public:

	// Operations

	DWORD SetBitmapBits(DWORD dwCount, const void* lpBits);

	DWORD GetBitmapBits(DWORD dwCount, LPVOID lpBits);

	// Implementation

};

#endif

