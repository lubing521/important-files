#pragma once


#include <functional>

// CDlgWaitMsg 对话框

class CDlgWaitMsg : public CDialog
{
	DECLARE_DYNAMIC(CDlgWaitMsg)

public:
	CDlgWaitMsg(size_t maxTime = -1, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWaitMsg();

// 对话框数据
	enum { IDD = IDD_DIALOG_WAIT_MSG };

private:
	typedef std::tr1::function<void()> Callback;

	size_t progress_;
	size_t maxTime_;
	static const size_t MAX_DOT = 8;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	//方法模板:exec
	//执行操作并显示提示框，操作完成提示框自动消失
	//操作完成前将阻止其他操作，即本提示框是模态的
	//模板参数: Op: 操作的函数对象类型，与void(void)兼容
	//参数：op：操作的函数对象，常见的是使用std::tr1::bind得到的
	//
	void Exec(const Callback &op);

public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
