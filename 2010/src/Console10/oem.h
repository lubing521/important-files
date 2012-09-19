#ifndef _i8desk_oem_inc_
#define _i8desk_oem_inc_

#include <shlwapi.h>
#include <gdiplus.h>
#include <atlcoll.h>
#include <atlenc.h>


namespace i8desk
{
	//oem属性
	class COemWrapper 
	{
	public:

		CString GetString(LPCTSTR lpszKey, LPCTSTR lpszDefault)
		{
			ASSERT(lpszDefault);
			if( !m_bOk )
				return lpszDefault;

			TCHAR szValue[BUFSIZ] = {0};
			DWORD dwLen = ::GetPrivateProfileString(_T("OEM"), lpszKey, 
				lpszDefault, szValue, sizeof(szValue), m_szFileTemp);

			if (dwLen > 0) {
				return szValue;
			}

			return lpszDefault;
		}

		CString GetUrl(LPCTSTR lpszKey, LPCTSTR lpszDefault)
		{
			ASSERT(lpszDefault);
			if( !m_bOk )
				return lpszDefault;

			TCHAR szValue[BUFSIZ] = {0};
			DWORD dwLen = ::GetPrivateProfileString(_T("url"), lpszKey, 
				lpszDefault, szValue, sizeof(szValue), m_szFileTemp);

			if (dwLen > 0) 
			{
				return szValue;
			}

			return lpszDefault;
		}

		int IsAdsShow(int nDefault)
		{
			if( !m_bOk )
				return nDefault;

			nDefault = ::GetPrivateProfileInt(_T("OEM"), _T("ShowAds"), 
				nDefault, m_szFileTemp);

			return nDefault;
		}
		
		//设置控制台主窗口标题字符串
		bool LoadString(CString& str, LPCTSTR lpszProg = _T("console"))
		{
			ASSERT(str.IsEmpty());

			if( !m_bOk )
				return false;


			TCHAR szAppName[BUFSIZ];
			DWORD dwLen = ::GetPrivateProfileString(_T("OEM"), lpszProg, 
				_T(""), szAppName, sizeof(szAppName), m_szFileTemp);

			if (dwLen > 0) {
				str = szAppName;
			}

			return !str.IsEmpty();
		}
		
		//设置控制台主窗口图标
		bool LoadIcon(HICON &hIcon, LPCTSTR lpszProg = _T("console"))
		{
			ASSERT (hIcon == 0);

			if( !m_bOk )
				return false;

			TCHAR szAppIcon[MAX_PATH];
			DWORD dwLen = ::GetPrivateProfileString(_T("OEM"), _T("icon"), 
				_T(""), szAppIcon, sizeof(szAppIcon), m_szFileTemp);
			
			if (dwLen > 0) {
				hIcon = (HICON)LoadImage(0, szAppIcon, IMAGE_ICON, 
					0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE);
			}

			return hIcon != 0;
		}

		//
		bool LoadExternalTool(CString& strText, CString& strProg, HICON &hIcon, LPCTSTR lpszId)
		{
			ASSERT (hIcon == 0);
			if( !m_bOk )
				return false;

			TCHAR szText[BUFSIZ];
			DWORD dwLen = ::GetPrivateProfileString(lpszId, _T("text"), 
				_T(""), szText, sizeof(szText), m_szFileTemp);
			if (dwLen == 0) {
				return false;
			}

			TCHAR szProg[MAX_PATH];
			dwLen = ::GetPrivateProfileString(lpszId, _T("prog"), 
				_T(""), szProg, sizeof(szProg), m_szFileTemp);
			if (dwLen == 0) {
				//return false;
			}

			TCHAR szAppIcon[MAX_PATH];
			dwLen = ::GetPrivateProfileString(lpszId, _T("icon"), 
				_T(""), szAppIcon, sizeof(szAppIcon), m_szFileTemp);
			if (dwLen == 0) {
				return false;
			}

			hIcon = (HICON)LoadImage(0, szAppIcon, IMAGE_ICON,
				0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE);

			if (hIcon) {
				strText = szText;
				strProg = szProg;
			}

			return hIcon != 0;
		}

		//设置oem的logo图片
		bool LoadLogo(HBITMAP &hBitmap, COLORREF crBkgrd = RGB(0, 0, 0))
		{
			ASSERT (hBitmap == 0);
			if( !m_bOk )
				return false;

			TCHAR szLogoName[BUFSIZ];
			DWORD dwLen = ::GetPrivateProfileString(_T("OEM"), _T("logo"), 
				_T(""), szLogoName, sizeof(szLogoName), m_szFileTemp);

			if (dwLen > 0) {
#ifndef _UNICODE
				WCHAR szWideCharStr[MAX_PATH] = {0};
				MultiByteToWideChar (CP_ACP, 0, szLogoName, ::lstrlen(szLogoName), 
					szWideCharStr, sizeof(szWideCharStr));
				if (Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(szWideCharStr)) {
#else
				if (Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(szLogoName)) {
#endif
					pBitmap->GetHBITMAP(crBkgrd, &hBitmap);
					delete pBitmap;
				}
			}

			return hBitmap != 0;
		}

		static COemWrapper *instance()
		{
			static COemWrapper oem;
			return &oem;
		}

	private:
		bool m_bOk;
		TCHAR m_szFileName[MAX_PATH];
		TCHAR m_szFileTemp[MAX_PATH];
		
		ATL::CAutoVectorPtr<TCHAR> m_encodeContent;
		HANDLE m_hFileTmp;

		COemWrapper()
			: m_hFileTmp(INVALID_HANDLE_VALUE)
			, m_bOk(false)
		{
			memset(m_szFileName, 0, sizeof(m_szFileName));

			TCHAR szPath[MAX_PATH] = {0};
			::GetModuleFileName(0, szPath, MAX_PATH);
			::PathRemoveFileSpec(szPath);

			_tcscpy(m_szFileName, szPath);

			::lstrcat(m_szFileName, _T("\\oem\\oem.ini"));


			memset(szPath, 0, MAX_PATH);
			::GetSystemDirectory(szPath, MAX_PATH);      

			//创建临时文件 
			_tcscpy(m_szFileTemp, szPath);
			_tcscat(m_szFileTemp, _T("\\oemTmp.ini"));
			::DeleteFile(m_szFileTemp);

			if( _Decode() )
			{
				_ReadConfigContent();
			}
		}

		~COemWrapper()
		{
			if( m_hFileTmp != INVALID_HANDLE_VALUE )
			{
				::CloseHandle(m_hFileTmp);
				::DeleteFile(m_szFileTemp);
			}
		}


	private:
		bool _Decode()
		{
			// 读取配置文件
			HANDLE hFile = ::CreateFile(m_szFileName, GENERIC_READ | GENERIC_WRITE, 
				0, NULL, OPEN_EXISTING, 0, NULL);
			if( hFile == INVALID_HANDLE_VALUE )
			{
				//AfxMessageBox(_T("读取OEM配置文件错误"));
				return false;
			}

			// 解密
			DWORD dwFileSize = ::GetFileSize(hFile, NULL);
			int nDecLen = ATL::Base64DecodeGetRequiredLength(dwFileSize);

			
			m_encodeContent.Allocate(dwFileSize);

			i8desk::ReadFileContent(hFile, (BYTE *)&(*m_encodeContent), dwFileSize);
			::CloseHandle(hFile);

			CString strOrig;
			Base64Decode(m_encodeContent, dwFileSize, (BYTE*)strOrig.GetBuffer(nDecLen), &nDecLen);
			strOrig.ReleaseBuffer(nDecLen);


			// 写入文件
			m_hFileTmp = ::CreateFile(m_szFileTemp, GENERIC_WRITE, FILE_SHARE_READ
				, NULL, CREATE_ALWAYS, 0, NULL);
			if( m_hFileTmp == INVALID_HANDLE_VALUE )
			{
				//AfxMessageBox(_T("读取OEM读取OEM配置文件错误"));
				return false;
			}
			i8desk::WriteFileContent(m_hFileTmp, (BYTE *)(LPCTSTR)strOrig, strOrig.GetLength());
			::FlushFileBuffers(m_hFileTmp);

			m_bOk = true;
			return true;
		}


		void _ReadConfigContent()
		{

		}
	};
}

#define I8DESK_OEM i8desk::COemWrapper::instance() 

#endif //#ifndef _i8desk_oem_inc_