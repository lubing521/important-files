// DlgTools.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgTools.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/PluginInfo.h"

#include "../ManagerInstance.h"
#include "../Business/PlugToolBusiness.h"
#include "../MessageBox.h"

#include "UIHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgTools

IMPLEMENT_DYNCREATE(CDlgTools, CDialog)

CDlgTools::CDlgTools(CWnd* pParent)
	: i8desk::ui::BaseWnd(CDlgTools::IDD, pParent)
{

}

CDlgTools::~CDlgTools()
{
}

void CDlgTools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TOOLS, wndList_);
}


BEGIN_EASYSIZE_MAP(CDlgTools)
	EASYSIZE(IDC_LIST_TOOLS, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)

END_EASYSIZE_MAP 


BEGIN_MESSAGE_MAP(CDlgTools, i8desk::ui::BaseWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_TOOLS, &CDlgTools::OnLvnGetdispinfoListTools)
	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgTools::OnApplySelect)
END_MESSAGE_MAP()

namespace 
{

	CCustomColumnTraitPlugStatus *statusTrait = 0;
	CCustomColumnTraitPlugButton *btnTrait = 0;

	namespace detail
	{
		template < typename PlugsT >
		CCustomColumnTraitPlugName *GetNameTraits(PlugsT &plugs)
		{
			CCustomColumnTraitPlugName *rowTrait = new CCustomColumnTraitPlugName(plugs);
			rowTrait->SetBkImg(

				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
	
			rowTrait->SetIconBK(i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Default.png")));
			return rowTrait;
		}


		CCustomColumnTrait *GetTextTrait()
		{
			CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
			rowTrait->SetBkImg(

				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return rowTrait;
		}

		template < typename PlugsT >
		CCustomColumnTraitPlugStatus *GetStatusTraits(PlugsT &plugs)
		{
			statusTrait = new CCustomColumnTraitPlugStatus(plugs);
			statusTrait->SetBkImg(

				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			statusTrait->SetProgressImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue.png")));

			return statusTrait;
		}

		template < typename PlugsT >
		CCustomColumnTraitPlugRate *GetRateTraits(PlugsT &plugs)
		{
			CCustomColumnTraitPlugRate *rowTrait = new CCustomColumnTraitPlugRate(plugs);
			rowTrait->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return rowTrait;
		}


		template < typename ParentT >
		CCustomColumnTraitPlugButton *GetButtonTrait(ParentT *parent)
		{
			btnTrait = new CCustomColumnTraitPlugButton;
			btnTrait->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			using namespace std::tr1::placeholders;
			btnTrait->SetButtonCallback(
				std::tr1::bind(&ParentT::OnBtnClick, parent, _1, _2),
				std::tr1::bind(&ParentT::OnInitCallback, parent, _1));
			btnTrait->SetButton(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Dis.png")));
			return btnTrait;
		}

	}


	HBITMAP* GetScroll()
	{
		static HBITMAP scroll[] = 
		{
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Top.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Btm.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Top.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Btm.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Mid.png"))
		};

		return scroll;
	}

	// 下载任务进度
	typedef std::map<long, std::pair<size_t, size_t>> ProgressType;
	ProgressType Progress;

	enum { MASK_NAME = 1, MASK_SIZE, MASK_RATING, MASK_STATUS, MASK_OPERATE };

	template < typename ListCtrlT, typename ParentT, typename PlugsT >
	void InitList(ListCtrlT &listCtrl, ParentT *parent, PlugsT &plugs)
	{
		listCtrl.InsertHiddenLabelColumn();
		listCtrl.SetItemHeight(64);

		listCtrl.SetScrollImage(GetScroll());
		listCtrl.InsertColumnTrait(MASK_NAME,	_T("插件名称"),	LVCFMT_LEFT, 400, MASK_NAME,	detail::GetNameTraits(plugs));
		listCtrl.InsertColumnTrait(MASK_SIZE,	_T("插件大小"),	LVCFMT_LEFT, 100, MASK_SIZE,	detail::GetTextTrait());
		listCtrl.InsertColumnTrait(MASK_RATING,	_T("评分"),		LVCFMT_LEFT, 95, MASK_RATING,	detail::GetRateTraits(plugs));
		listCtrl.InsertColumnTrait(MASK_STATUS,	_T("状态"),		LVCFMT_LEFT, 100, MASK_STATUS,	detail::GetStatusTraits(plugs));
		listCtrl.InsertColumnTrait(MASK_OPERATE,_T("插件操作"),	LVCFMT_LEFT, 295, MASK_OPERATE,	detail::GetButtonTrait(parent));
	}
}


void CDlgTools::Register()
{
	using namespace std::tr1::placeholders;
	i8desk::GetRecvDataMgr().RegisterPlugStatus(std::tr1::bind(&CDlgTools::_PlugComplate, this, _1, _2));
}

void CDlgTools::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_PLUGTOOL);
}

void CDlgTools::OnRealDataUpdate()
{
	Progress.clear();
	i8desk::business::plug::GetProgress(Progress);

	const i8desk::data_helper::PlugToolTraits::VectorType &plugs = i8desk::GetDataMgr().GetPlugTools();

	for(ProgressType::const_iterator iter = Progress.begin(); iter != Progress.end(); ++iter)
	{
		struct GIDEqual
		{
			long gid_;
			GIDEqual(long gid)
				: gid_(gid)
			{}

			bool operator()(const i8desk::data_helper::PlugToolTraits::ValueType &val) const
			{
				return gid_ == val->PID;
			}
		};
		i8desk::data_helper::PlugToolTraits::VecConstIterator vIter = std::find_if(
			plugs.begin(), plugs.end(), GIDEqual(iter->first));
		if( vIter == plugs.end() )
			return;
		// 已下载完成
		if( iter->second.second < 95 )
			statusTrait->SetProgress(iter->second.first, iter->second.second, true);
	}
	wndList_.Invalidate();
}

void CDlgTools::OnReConnect()
{
	i8desk::GetDataMgr().GetPlugTools().clear();
}

void CDlgTools::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetPlugTools());
}


void CDlgTools::OnDataComplate()
{
	_Init();
}

void CDlgTools::_Init()
{
	i8desk::data_helper::PlugToolTraits::VectorType &plugTools = i8desk::GetDataMgr().GetPlugTools();
	i8desk::business::plug::SetNameBmp(plugTools);
	i8desk::business::plug::SetStatusBmp(plugTools);
	i8desk::business::plug::SetRateBmp(plugTools);

	wndList_.SetItemCount(plugTools.size());
}


void CDlgTools::_PlugComplate(i8desk::TaskNotify taskNotify, DWORD gid)
{
	for(size_t i = 0; i != i8desk::GetDataMgr().GetPlugTools().size(); ++i)
	{
		i8desk::data_helper::PlugToolTraits::ValueType val = i8desk::GetDataMgr().GetPlugTools()[i];
		if( gid == val->PID )
		{
			val->DownloadStatus = 1;

			statusTrait->SetProgress(Progress[gid].first, 100, false);

			btnTrait->EnableButton(i, CCustomColumnTraitPlugButton::CONFIG, true);
			btnTrait->EnableButton(i, CCustomColumnTraitPlugButton::START, true);
			btnTrait->EnableButton(i, CCustomColumnTraitPlugButton::STOP, true);
			btnTrait->EnableButton(i, CCustomColumnTraitPlugButton::DOWNLOAD, true);
			
			wndList_.Invalidate(TRUE);
			break;
		}
	}
	
}


void CDlgTools::OnBtnClick(size_t row, size_t btnPos)
{
	if( row >= i8desk::GetDataMgr().GetPlugTools().size() )
	{
		assert(0);
		return;
	}

	i8desk::data_helper::PlugToolTraits::ValueType &plug = i8desk::GetDataMgr().GetPlugTools()[row];
	switch(btnPos)
	{
	case CCustomColumnTraitPlugButton::CONFIG:
		assert(plug->DownloadStatus == 1);
		if( !i8desk::GetControlMgr().PlugToolOperate(plug->PID, i8desk::Config) )
		{
			CMessageBox dlg(_T("提示"), _T("该插件配置不可用"));
			dlg.DoModal();
		}
		break;
	case CCustomColumnTraitPlugButton::START:
		assert(plug->DownloadStatus == 1);
		// 未启用
		if( i8desk::GetControlMgr().PlugToolOperate(plug->PID, i8desk::Enable) )
		{
			plug->Status = 1; 
		}
		break;
	case CCustomColumnTraitPlugButton::STOP:
		assert(plug->DownloadStatus == 1);
		// 已启用
		if( i8desk::GetControlMgr().PlugToolOperate(plug->PID, i8desk::Disable) )
		{
			plug->Status = 0; 
		}
		break;

	case CCustomColumnTraitPlugButton::DOWNLOAD:
		// 未下载
		i8desk::GetControlMgr().PlugToolOperate(plug->PID, i8desk::Download);
		statusTrait->SetProgress(row, 0, false);

		// 查看状态
		i8desk::GetRealDataMgr().Register(
			std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_PLUGTOOL);
		break;
	default:
		assert(0);
		break;
	}

	wndList_.Invalidate();
}

void CDlgTools::OnInitCallback(size_t row)
{
	const i8desk::data_helper::PlugToolTraits::VectorType &plugs = i8desk::GetDataMgr().GetPlugTools();
	assert(plugs.size() >= row);


	// 调整Button

	if( plugs[row]->DownloadStatus == 0 )
	{
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::CONFIG, false);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::START, false);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::STOP, false);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::DOWNLOAD, true);
	}
	else
	{
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::CONFIG, true);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::START, true);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::STOP, true);
		btnTrait->EnableButton(row, CCustomColumnTraitPlugButton::DOWNLOAD, true);
	}
}

// CDlgTools 消息处理程序

BOOL CDlgTools::OnInitDialog()
{
	BaseWnd::OnInitDialog();
	INIT_EASYSIZE;


	// 初始化工作区图片
	HBITMAP area[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Left_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Bottom_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Content_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Top_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Right_Line.png"))
	};
	for(size_t i = 0; i != _countof(area); ++i)
	{
		area_[i].reset(new CImage);
		area_[i]->Attach(area[i]);
	}

	workOutLine_.reset(new CImage);

	workOutLine_->Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));

	InitList(wndList_, this, i8desk::GetDataMgr().GetPlugTools());


	_Init();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgTools::OnDestroy()
{
	BaseWnd::OnDestroy();

}

void CDlgTools::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, area_);

	CRect rcList;
	wndList_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, workOutLine_);
}

void CDlgTools::OnSize(UINT nType, int cx, int cy)
{
	BaseWnd::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	if( !::IsWindow(wndList_.GetSafeHwnd()) )
		return;

	wndList_.Invalidate();
}


LRESULT CDlgTools::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


void CDlgTools::OnLvnGetdispinfoListTools(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;
	
	const i8desk::data_helper::PlugToolTraits::VectorType &plugs = i8desk::GetDataMgr().GetPlugTools();
	assert(plugs.size() >= itemIndex);
	

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = plugs[itemIndex]->Name;
			break;
		case MASK_SIZE:
			i8desk::FormatSize(plugs[itemIndex]->Size, text);
			break;
		case MASK_RATING:
			//stdex::ToString(plugs[itemIndex]->IdcClick, text);
			text.clear();
			break;
		case MASK_STATUS:
			//stdex::ToString(plugs[itemIndex]->Status, text);
			text.clear();
			break;
		case MASK_OPERATE:
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}
