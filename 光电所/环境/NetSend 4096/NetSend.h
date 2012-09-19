// NetSend.h : main header file for the NETSEND application
//

#if !defined(AFX_NETSEND_H__E30CFE87_7867_489A_B166_FE402883FBB0__INCLUDED_)
#define AFX_NETSEND_H__E30CFE87_7867_489A_B166_FE402883FBB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNetSendApp:
// See NetSend.cpp for the implementation of this class
//

class CNetSendApp : public CWinApp
{
public:
	CNetSendApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetSendApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNetSendApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETSEND_H__E30CFE87_7867_489A_B166_FE402883FBB0__INCLUDED_)
