// SafeCenterConfigDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "SafeCenterConfigDlg.h"
#include "Console.h"
#include "ConsoleDlg.h"

#include "SCBaseConfigDlg.h"
#include "SCArpProtectDlg.h"
#include "SCProgramBlacklistDlg.h"
#include "SCWidgetBlacklistDlg.h"

using namespace MSXML2;

class ContentHandler : public ISAXContentHandler
{
public:
	ContentHandler(	
		CSCBaseConfigDlg *pSCBaseConfigDlg,
		CSCArpProtectDlg *pSCArpProtectDlg,
		CSCProgramBlacklistDlg *pSCProgramBlacklistDlg,
		CSCWidgetBlacklistDlg *pSCWidgetBlacklistDlg)
		: m_pSCBaseConfigDlg(pSCBaseConfigDlg)
		, m_pSCArpProtectDlg(pSCArpProtectDlg)
		, m_pSCProgramBlacklistDlg(pSCProgramBlacklistDlg)
		, m_pSCWidgetBlacklistDlg(pSCWidgetBlacklistDlg)
	{
	}

	HRESULT __stdcall QueryInterface(const IID &, void **){return S_OK;}
	ULONG __stdcall AddRef(void){return 1;}
	ULONG __stdcall Release(void){return 1;}
	HRESULT __stdcall raw_putDocumentLocator (struct ISAXLocator * pLocator ) {return S_OK;}
	HRESULT __stdcall raw_startDocument ( ) {return S_OK;}
	HRESULT __stdcall raw_endDocument ( ) {return S_OK;}
	HRESULT __stdcall raw_startPrefixMapping (unsigned short * pwchPrefix,int cchPrefix,unsigned short * pwchUri,int cchUri ) {return S_OK;}
	HRESULT __stdcall raw_endPrefixMapping (unsigned short * pwchPrefix,int cchPrefix ) {return S_OK;}
	HRESULT __stdcall raw_characters (unsigned short * pwchChars,int cchChars ) {return S_OK;}
	HRESULT __stdcall raw_ignorableWhitespace (unsigned short * pwchChars,int cchChars ) {return S_OK;}
	HRESULT __stdcall raw_processingInstruction (unsigned short * pwchTarget,int cchTarget,unsigned short * pwchData,int cchData ) {return S_OK;}
	HRESULT __stdcall raw_skippedEntity (unsigned short * pwchName,int cchName ) {return S_OK;}

	HRESULT __stdcall raw_startElement (
		unsigned short * pwchNamespaceUri,
		int cchNamespaceUri,
		unsigned short * pwchLocalName,
		int cchLocalName,
		unsigned short * pwchQName,
		int cchQName,
		struct ISAXAttributes * pAttributes ) 
	{
		unsigned short * pwchValue;
		int cchValue;
		HRESULT hr;

		if (::wcscmp((wchar_t *)pwchLocalName, L"WindowInfo") == 0)
		{
			//窗口禁止列表
			std::wstring wstrName, wstrTime, wstrDesc;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"Name", 4, &pwchValue, &cchValue);
			if (hr != S_OK)
				return S_OK;
			wstrName = std::wstring((wchar_t *)pwchValue, cchValue);

			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"AddTime", 7, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrTime = std::wstring((wchar_t *)pwchValue, cchValue);

			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"Description", 11, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrDesc = std::wstring((wchar_t *)pwchValue, cchValue);

			m_pSCWidgetBlacklistDlg->InsertItem(i8desk::WCToMB(wstrTime).c_str(), 
				i8desk::WCToMB(wstrName).c_str(), i8desk::WCToMB(wstrDesc).c_str());
		}
		else if (::wcscmp((wchar_t *)pwchLocalName, L"ProccessInfo") == 0)
		{
			//程序禁止列表
			std::wstring wstrName, wstrTime, wstrDesc;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"Name", 4, &pwchValue, &cchValue);
			if (hr != S_OK)
				return S_OK;
			wstrName = std::wstring((wchar_t *)pwchValue, cchValue);

			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"AddTime", 7, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrTime = std::wstring((wchar_t *)pwchValue, cchValue);

			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"Description", 11, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrDesc = std::wstring((wchar_t *)pwchValue, cchValue);

			m_pSCProgramBlacklistDlg->InsertItem(i8desk::WCToMB(wstrTime).c_str(), 
				i8desk::WCToMB(wstrName).c_str(), i8desk::WCToMB(wstrDesc).c_str());
		}
		/*
		else if (::wcscmp((wchar_t *)pwchLocalName, L"Status") == 0)
		{
			//ARP保护设置
			BOOL bIsOpen = FALSE;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"IsOpen", 6, &pwchValue, &cchValue);
			if (hr == S_OK)
				bIsOpen = ::_wtoi(std::wstring((wchar_t *)pwchValue, cchValue).c_str());
			
			BOOL bIsAutoGetGatewayAddr = FALSE;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"IsAutoGetGatewayAddr", 20, &pwchValue, &cchValue);
			if (hr == S_OK)
				bIsAutoGetGatewayAddr = ::_wtoi(std::wstring((wchar_t *)pwchValue, cchValue).c_str());

			std::wstring wstrIP;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"GatewayIp", 9, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrIP = std::wstring((wchar_t*)pwchValue, cchValue);

			std::wstring wstrMAC;
			hr = pAttributes->raw_getValueFromQName(
				(USHORT*)L"GatewayMac", 10, &pwchValue, &cchValue);
			if (hr == S_OK)
				wstrMAC = std::wstring((wchar_t*)pwchValue, cchValue);

			m_pSCArpProtectDlg->SetConfig(bIsOpen, bIsAutoGetGatewayAddr,
				i8desk::WCToMB(wstrIP).c_str(), i8desk::WCToMB(wstrMAC).c_str());
		}//*/

		return S_OK;
	}

	HRESULT __stdcall raw_endElement (
		/*[in]*/ unsigned short * pwchNamespaceUri,
		/*[in]*/ int cchNamespaceUri,
		/*[in]*/ unsigned short * pwchLocalName,
		/*[in]*/ int cchLocalName,
		/*[in]*/ unsigned short * pwchQName,
		/*[in]*/ int cchQName ) 
	{
		return S_OK;
	}

private:
	CSCBaseConfigDlg *m_pSCBaseConfigDlg;
	CSCArpProtectDlg *m_pSCArpProtectDlg;
	CSCProgramBlacklistDlg *m_pSCProgramBlacklistDlg;
	CSCWidgetBlacklistDlg *m_pSCWidgetBlacklistDlg;
};

IMPLEMENT_DYNAMIC(CSafeCenterConfigDlg, CDialog)

CSafeCenterConfigDlg::CSafeCenterConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSafeCenterConfigDlg::IDD, pParent)
{
	m_pLastDlg = NULL;
}

CSafeCenterConfigDlg::~CSafeCenterConfigDlg()
{
}

void CSafeCenterConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstBox);
	DDX_Control(pDX, IDC_OPTICON, m_btnIcon);
	DDX_Control(pDX, IDC_DESCRIPTION, m_btnDescription);
}

BEGIN_MESSAGE_MAP(CSafeCenterConfigDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST, &CSafeCenterConfigDlg::OnLbnSelchangeList)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CSafeCenterConfigDlg::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CSafeCenterConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWnd*  pWnd = GetDlgItem(IDC_AREA);

	/*
	CSCBaseConfigDlg *pSCBaseConfigDlg = new CSCBaseConfigDlg;
	{
		pSCBaseConfigDlg->Create(CSCBaseConfigDlg::IDD, pWnd);		
		m_OptList.push_back(tagItem(IDI_OPTION,	"常规配置", "选择安全中心及常规设置", pSCBaseConfigDlg));
	}
	//*/

	/*
	CSCArpProtectDlg *pSCArpProtectDlg = new CSCArpProtectDlg;
	{
		pSCArpProtectDlg->Create(CSCArpProtectDlg::IDD, pWnd);		
		m_OptList.push_back(tagItem(IDI_OPTION,	"ARP保护", "ARP实时保护设置", pSCArpProtectDlg));
	}
	//*/

	CSCProgramBlacklistDlg *pSCProgramBlacklistDlg = new CSCProgramBlacklistDlg;
	{
		pSCProgramBlacklistDlg->Create(CSCProgramBlacklistDlg::IDD, pWnd);		
		m_OptList.push_back(tagItem(IDI_OPTION, "程序禁止", 
			"设置不允许运行的程序", pSCProgramBlacklistDlg));
	}

	CSCWidgetBlacklistDlg *pSCWidgetBlacklistDlg = new CSCWidgetBlacklistDlg;
	{
		pSCWidgetBlacklistDlg->Create(CSCWidgetBlacklistDlg::IDD, pWnd);		
		m_OptList.push_back(tagItem(IDI_OPTION,	"窗口禁止",
			"设置不允许运行包含相应窗口的程序", pSCWidgetBlacklistDlg));
	}

	//初始化各窗口
	try {
		ISAXXMLReader* pRdr = 0;
		HRESULT hr = CoCreateInstance(__uuidof(SAXXMLReader), 0,
			CLSCTX_ALL, __uuidof(ISAXXMLReader), (void**)&pRdr);
		if (hr == S_OK && pRdr) {
			ContentHandler ContentHandler(
				0, //pSCBaseConfigDlg, 
				0, //pSCArpProtectDlg, 
				pSCProgramBlacklistDlg, 
				pSCWidgetBlacklistDlg);
			pRdr->raw_putContentHandler(&ContentHandler);

			std::string filename = i8desk::GetAppPath() + "data\\config\\i8safe.xml";
			pRdr->raw_parseURL((USHORT*)i8desk::MBToWC(filename).c_str());
			pRdr->Release();
		}
	} catch(...) {
		AfxMessageBox("初始化安全中心设置数据失败\n配置文件不存在或格式错误！");
	}

	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		m_lstBox.AddItem(m_OptList[idx].IconID,	m_OptList[idx].Caption);
	}
	
	m_lstBox.SetCurSel(0);
	OnLbnSelchangeList();
	CenterWindow();

	return TRUE;
}

void CSafeCenterConfigDlg::OnLbnSelchangeList()
{
	int nSel = m_lstBox.GetCurSel();
	if (nSel == -1)
		return ;

	CDialog* pDlg = m_OptList[nSel].pDlg;
	ASSERT(pDlg);

	if (m_pLastDlg && m_pLastDlg != pDlg)
	{
		m_pLastDlg->ShowWindow(SW_HIDE);
	}

	HICON hIcon = m_btnIcon.GetIcon();
	if (hIcon != NULL)
		DestroyIcon(hIcon);
	m_btnIcon.SetIcon(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_OptList[nSel].IconID)));
	m_btnDescription.SetWindowText(m_OptList[nSel].Description);

	if (pDlg && m_pLastDlg != pDlg)
	{
		CRect rc;
		CWnd*  pWnd = GetDlgItem(IDC_AREA);
		pWnd->GetWindowRect(&rc);
		pWnd->ScreenToClient(&rc);
		rc.DeflateRect(1, 1, 1, 1);
		pDlg->MoveWindow(&rc);
		pDlg->ShowWindow(SW_SHOW);
	}
	m_OptList[nSel].pDlg = pDlg;
	m_pLastDlg = pDlg;
	m_lstBox.SetFocus();
}

BOOL CSafeCenterConfigDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CSafeCenterConfigDlg::OnDestroy()
{
	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		if (m_OptList[idx].pDlg != NULL)
		{
			if (m_OptList[idx].pDlg->m_hWnd != NULL)
				m_OptList[idx].pDlg->DestroyWindow();
			delete m_OptList[idx].pDlg;
		}
	}
	CDialog::OnDestroy();
}

void CSafeCenterConfigDlg::OnBnClickedOk()
{
	CWaitCursor wc;

	MSXML2::IXMLDOMDocumentPtr XmlDoc;
	XmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	XmlDoc->appendChild(XmlDoc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"gb2312\""));
	MSXML2::IXMLDOMElementPtr RootElem = XmlDoc->createElement(_T("Config"));
	XmlDoc->appendChild(RootElem);

	MSXML2::IXMLDOMElementPtr SafeCenterSel = XmlDoc->createElement(_T("SafeCenterSel"));
	SafeCenterSel->setAttribute(_T("Index"), 1L);
	RootElem->appendChild(SafeCenterSel);

	WPARAM wparam = reinterpret_cast<WPARAM>(XmlDoc.GetInterfacePtr());
	LPARAM lparam = reinterpret_cast<LPARAM>(RootElem.GetInterfacePtr());
	
	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		ASSERT(m_OptList[idx].pDlg);
		if (HWND hwnd = m_OptList[idx].pDlg->GetSafeHwnd())
		{
			if (!::SendMessage(hwnd, i8desk::g_nOptApplyMsg, wparam, lparam))
			{
				m_lstBox.SetCurSel(idx);
				OnLbnSelchangeList();
				return ;
			}
		}
	}

	std::string filename = i8desk::GetAppPath() + "data\\config\\";
	if (!i8desk::mkpath(filename))
	{
		CString str("创建文件夹失败:");
		str += filename.c_str();
		AfxMessageBox(str);
	}
	filename += "i8safe.xml";

	try {
		XmlDoc->save(filename.c_str());

		CConsoleDlg *pMainWnd = static_cast<CConsoleDlg *>(AfxGetMainWnd());
		std::string ErrInfo;
		if (!pMainWnd->m_pDbMgr->ReloadCacheFile(filename.c_str(), ErrInfo))
		{
			wc.Restore();
			AfxMessageBox(ErrInfo.c_str());
		}
	} catch(...) {
		CString str("保存文件失败:");
		str += filename.c_str();
		AfxMessageBox(filename.c_str());
	}

	OnOK();
}