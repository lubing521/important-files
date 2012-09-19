#pragma once

#include <process.h>

#include <functional>

// CWaitMessageDlg 对话框

class CWaitMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitMessageDlg)

public:
	CWaitMessageDlg(LPCTSTR lpstr, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWaitMessageDlg();

// 对话框数据
	enum { IDD = IDD_WAIT_MESSAGE };
	
	//方法模板:exec
	//执行操作并显示提示框，操作完成提示框自动消失
	//操作完成前将阻止其他操作，即本提示框是模态的
	//模板参数: Op: 操作的函数对象类型，与void(void)兼容
	//参数：op：操作的函数对象，常见的是使用std::tr1::bind得到的
	//注意：op将在一个单独的临时线程中执行
	//		如果线程创建失败则将退化为直接调用，提示框也不会显示
	//
	template<typename Op> void exec(Op op)
	{
		m_op = op;

		if (HANDLE h = (HANDLE)_beginthreadex(0, 0, work, this, 0, 0)) {
			 //使用模态框以用户的禁止其他操作，直到op完成
			this->DoModal();
			
 			//等待调用op的工作线程结束
			::WaitForSingleObject(h, INFINITE);
			::CloseHandle(h);
		} else {
			//创建线程失败时则退化为直接调用
			CWaitCursor wc;
			op(); 
		}
	}

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	static const int MAX_DOT_NUM = 8;
	int m_progress;
	CString m_strMsg;					//提示消息
	std::tr1::function<void()> m_op;	//要做的操作
	CBrush m_brush;

	static UINT __stdcall work(void *p)
	{
		CWaitMessageDlg *pThis = reinterpret_cast<CWaitMessageDlg *>(p);

		::CoInitialize(0);
		pThis->m_op();
		::CoUninitialize();
		
		pThis->WaitForInitialComplete();
		return pThis->PostMessage(WM_CLOSE) ? 0 : -1;
	}

	void WaitForInitialComplete(void)
	{
		::WaitForSingleObject((HANDLE)s_event, INFINITE);
		::Sleep(50);
	}

	static CEvent s_event;
};
