#include "stdafx.h"
#include "../Console.h"
#include "DlgSyncTaskAddorCopy.h"

#include "../Business/SyncTaskBusiness.h"
#include "../ManagerInstance.h"
#include "../AsyncDataHelper.h"
#include "../MessageBox.h"

#include "../ui/Skin/SkinMgr.h"

#include "../../../../include/UpdateGame.h"
#include "../../../../include/Extend STL/UnicodeStl.h"
#include "DlgSyncTaskInfoEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	struct AddCombo
	{
		CComboBox &combo_;
		AddCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(LPCTSTR name, LPCTSTR sid)
		{
			int index = combo_.AddString(name);
			combo_.SetItemData(index, reinterpret_cast<DWORD_PTR>(sid));
		}
	};
}
// CDlgSyncTaskAddorCopy 对话框

IMPLEMENT_DYNAMIC(CDlgSyncTaskAddorCopy, CNonFrameChildDlg)

CDlgSyncTaskAddorCopy::CDlgSyncTaskAddorCopy(CWnd* pParent /*=NULL*/)
: CNonFrameChildDlg(CDlgSyncTaskAddorCopy::IDD, pParent)
{

}

CDlgSyncTaskAddorCopy::~CDlgSyncTaskAddorCopy()
{
}

void CDlgSyncTaskAddorCopy::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SYNCTASK_ADDORCOPY_NEW, wndRadioSyncNew_);
	DDX_Control(pDX, IDC_RADIO_SYNCTASK_ADDORCOPY_COPY, wndRadioSyncCopy_);
	DDX_Control(pDX, IDC_COMBO_SYNCTASK_ADDORCOPY_COPYTASK, wndComboSyncCopyTask_);

	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_ADDORCOPY_OK, wndBtnOk_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_ADDORCOPY_CANCEL, wndBtnCancel_);
}


BEGIN_MESSAGE_MAP(CDlgSyncTaskAddorCopy, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_ADDORCOPY_OK, &CDlgSyncTaskAddorCopy::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_ADDORCOPY_CANCEL, &CDlgSyncTaskAddorCopy::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_SYNCTASK_ADDORCOPY_COPY, &CDlgSyncTaskAddorCopy::OnBnClickedRadioCopy)
	ON_BN_CLICKED(IDC_RADIO_SYNCTASK_ADDORCOPY_NEW, &CDlgSyncTaskAddorCopy::OnBnClickedRadioNew)

END_MESSAGE_MAP()


// CDlgSyncTaskAddorCopy 消息处理程序

BOOL CDlgSyncTaskAddorCopy::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	i8desk::data_helper::SyncTaskTraits::MapType &synctasks = i8desk::GetDataMgr().GetSyncTasks();; 

	for(i8desk::data_helper::SyncTaskTraits::MapConstIterator iter= synctasks.begin(); 
		iter != synctasks.end(); ++iter )
	{
		int nItem = wndComboSyncCopyTask_.AddString(iter->second->Name);
		wndComboSyncCopyTask_.SetItemData(nItem, reinterpret_cast<DWORD_PTR>((iter->second).get()));
	}

	wndRadioSyncNew_.SetCheck(TRUE);
	wndComboSyncCopyTask_.ShowWindow(SW_HIDE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgSyncTaskAddorCopy::OnBnClickedRadioCopy()
{
	wndComboSyncCopyTask_.ShowWindow(SW_SHOW);
	wndComboSyncCopyTask_.SetCurSel(0);
	wndRadioSyncNew_.SetCheck(FALSE);
}

void CDlgSyncTaskAddorCopy::OnBnClickedRadioNew()
{
	wndComboSyncCopyTask_.ShowWindow(SW_HIDE);
	wndRadioSyncCopy_.SetCheck(FALSE);
}

void CDlgSyncTaskAddorCopy::OnBnClickedOk()
{
	UpdateData(TRUE);

	if( wndRadioSyncNew_.GetCheck() == BST_CHECKED )
	{
		CDlgSyncTaskInfoEx info(isadd, 0);
		info.DoModal();
	}
	else
	{
		int nsel = wndComboSyncCopyTask_.GetCurSel();
		typedef i8desk::data_helper::SyncTaskTraits::ElementType SyncTaskType;
		SyncTaskType *val = reinterpret_cast<SyncTaskType *>(wndComboSyncCopyTask_.GetItemData(nsel));

		CDlgSyncTaskInfoEx info(iscopy, val);
		info.DoModal();
	}

	OnOK();
}

void CDlgSyncTaskAddorCopy::OnBnClickedCancel()
{
	OnCancel();
}







