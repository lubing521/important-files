#pragma once

#include <afxhtml.h>
#include <map>
#include <string>

//////////////////
// This struct defines one entry in the command map that maps text strings to
// command IDs. If your command map has an entry "about" that maps to
// ID_APP_ABOUT, and your HTML has a link <A HREF="app:about">, then clicking
// the link will invoke the ID_APP_ABOUT command. To set the map, call
// CHtmlCtrl::SetCmdMap.
//
struct HTMLCMDMAP 
{
	TCHAR name[MAX_PATH];		// command name used in "app:name" HREF in <A
	UINT nID;
};

//////////////////
// Class to turn CHtmlView into ordinary control that can live in dialog as
// well as frame.
//
class CHtmlCtrl 
	: public CHtmlView 
{
protected:
	std::map<int, std::wstring> m_cmdmap;	// command map
	BOOL m_bHideMenu;			// hide context menu

public:
	CHtmlCtrl() : m_bHideMenu(FALSE){ }
	~CHtmlCtrl() { }

	// get/set HideContextMenu property
	BOOL GetHideContextMenu()			 { return m_bHideMenu; }
	void SetHideContextMenu(BOOL val) { m_bHideMenu=val; }

	// Set doc contents from string
	HRESULT SetHTML(LPCTSTR strHTML);

	// set command map
	void RegisterCmdMap(int ID, const std::wstring &name)	 { m_cmdmap.insert(std::make_pair(ID, name)); }

	// create control in same place as static control
	BOOL CreateFromStatic(UINT nID, CWnd* pParent);

	// craete control from scratch
	BOOL Create(const RECT& rc, CWnd* pParent, UINT nID,
		DWORD dwStyle = WS_CHILD|WS_VISIBLE, CCreateContext* pContext = NULL)
	{
		return CHtmlView::Create(NULL, NULL, dwStyle, rc, pParent, nID, pContext);
	}

	// override to intercept child window messages to disable context menu
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);

	// Normally, CHtmlView destroys itself in PostNcDestroy, but we don't want
	// to do that for a control since a control is usually implemented as a
	// member of another window object.
	//
	virtual void PostNcDestroy() {  }

	// Overrides to bypass MFC doc/view frame dependencies. These are
	// the only places CHtmView depends on livining inside a frame.
	afx_msg void OnDestroy();
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg);

	// override to trap "app:" pseudo protocol
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
		DWORD nFlags,
		LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData,
		LPCTSTR lpszHeaders,
		BOOL* pbCancel );

	// You can override this to handle "app:" commmands.
	// Only necescary if you don't use a command map.
	virtual void OnAppCmd(LPCTSTR lpszCmd);

	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC(CHtmlCtrl)
};
