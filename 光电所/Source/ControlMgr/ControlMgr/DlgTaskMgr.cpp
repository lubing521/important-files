// DlgTaskMgr.cpp : 实现文件
//

#include "stdafx.h"
#include "ControlMgr.h"
#include "DlgTaskMgr.h"

#include <string>
#include <vector>
#include <sstream>
#include <regex>

#include <odbcinst.h>
#include <afxdb.h>
#include <comdef.h>

#include "../../Common/DBMgr.hpp"
#include "../../Common/PackageHelper.hpp"
#include "ControlManager.hpp"

#include "./GridListCtrlEx/CGridColumnTraitEdit.h"
#include "./GridListCtrlEx/CGridColumnTraitDateTime.h"
#include "./GridListCtrlEx/CGridColumnTraitCombo.h"

#include "../../../include/UI/ImageHelpers.h"
#include "../../../include/ExtendSTL/UnicodeStl.h"
#include "../../../include/ExtendSTL/StringAlgorithm.h"

struct TaskInfo
{
	int id_;
	std::wstring taskID_;
	COleDateTime startTime_;
	COleDateTime endTime_;
	common::DataType dataType_;
	std::wstring taskPath_;
	std::wstring description_;
	size_t i_freq_;
	size_t j_freq_;
	size_t i_cnt_;
	size_t j_cnt_;
};
typedef std::vector<TaskInfo> Tasks;
Tasks tasks;


static CString GetExcelDriver()
{
	TCHAR szBuf[2001] = {0};
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	TCHAR *pszBuf = szBuf;
	CString sDriver;

	// 获取已安装驱动的名称(涵数在odbcinst.h里)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return L"";

	// 检索已安装的驱动是否有Excel...
	do
	{
		std::wstring driverBuf = pszBuf;
		if (driverBuf.find(L"Excel") != std::wstring::npos )
		{
			//发现 !
			sDriver = driverBuf.c_str();
			break;
		}

		pszBuf = pszBuf + driverBuf.length() + 1;
	}
	while (pszBuf[1] != '\0');

	return sDriver;

}

static void ExportListToExcel(CListCtrl &pList, CString sExcelFile, CString sSheetName)
{
	CString warningStr;
	if (pList.GetItemCount ()>0) 
	{ 
		CDatabase database;
		CString sDriver;
		CString sSql;

		// 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
		sDriver = GetExcelDriver();
		if (sDriver.IsEmpty())
		{
			// 没有发现Excel驱动
			AfxMessageBox(L"没有安装Excel!/n请先安装Excel软件才能使用导出功能!");
			return;
		}

		// 创建进行存取的字符串
		sSql.Format(L"DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver, sExcelFile, sExcelFile);

		// 创建数据库 (既Excel表格文件)
		if( database.OpenEx(sSql,CDatabase::noOdbcDialog) )
		{
			// 创建表结构
			int i;
			LVCOLUMN columnData;
			CString columnName;
			int columnNum = 0;
			CString strH;
			CString strV;

			sSql = L"";
			strH = L"";
			columnData.mask = LVCF_TEXT;
			columnData.cchTextMax =100;
			columnData.pszText = columnName.GetBuffer (100);
			for(i=1;pList.GetColumn(i,&columnData);i++)
			{
				if (i!=1)
				{
					sSql = sSql + L"," ;
					strH = strH + L"," ;
				}

				sSql = sSql + columnData.pszText +L" TEXT";
				strH = strH + columnData.pszText +L" ";

			}
			columnName.ReleaseBuffer ();
			columnNum = i;

			sSql = L"CREATE TABLE " + sSheetName+ L" (" + sSql +  L")";
			database.ExecuteSQL(sSql);

			// 插入数据项
			int nItemIndex;
			for (nItemIndex=0;nItemIndex<pList.GetItemCount ();nItemIndex++)
			{
				strV = L"";
				for(i=1;i<columnNum;i++)
				{
					if (i!=1)
					{
						strV = strV + L"," ;
					}

					strV = strV + L"'" + pList.GetItemText(nItemIndex,i) + L"'";

				}

				sSql = L"INSERT INTO "+ sSheetName
					+L" ("+ strH + L")"
					+L" VALUES("+ strV + L")";

				database.ExecuteSQL(sSql);
			}

		}      

		// 关闭数据库
		database.Close();
		AfxMessageBox(L"成功导出到指定目录");
	}
	else
	{
		//没有数据
		AfxMessageBox(L"没有数据，不能导出!");

	}

}


static CString GetFolder( HWND hOwner = NULL)
{
	// The BROWSEINFO struct tells the shell 

	// how it should display the dialog.
	CString path;
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags   = BIF_USENEWUI;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = L"选择目录";

	// must call this if using BIF_USENEWUI

	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the 

	// selected folder.

	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if(pIDL != NULL)
	{
		// Create a buffer to store the path, then 

		// get the path.

		TCHAR buffer[_MAX_PATH] = {'\0'};
		if(::SHGetPathFromIDList(pIDL, buffer) != 0)
		{
			// Set the string value.

			path = buffer;
		}

		// free the item id list

		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return path;
}

// CDlgTaskMgr 对话框

IMPLEMENT_DYNAMIC(CDlgTaskMgr, CDialog)

CDlgTaskMgr::CDlgTaskMgr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTaskMgr::IDD, pParent)
	, wndSearchTaskID_(_T(""))
	, historyStatus_(HISTORY_STOPPED)
{

}

CDlgTaskMgr::~CDlgTaskMgr()
{
}

void CDlgTaskMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TASK_MANAGER_TASK_ID, wndSearchTaskID_);
	DDX_Control(pDX, IDC_LIST_TASK_MANAGER, wndListTasks_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TASK_MGR_START, wndStartTime_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TASK_MGR_STOP, wndStopTime_);
}


BEGIN_MESSAGE_MAP(CDlgTaskMgr, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TASK_MANAGER_SEARCH, &CDlgTaskMgr::OnBnClickedButtonTaskManagerSearch)
	ON_MESSAGE(WM_USER + 0x100, &CDlgTaskMgr::OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_TASK_MGR_EDIT, &CDlgTaskMgr::OnBnClickedButtonTaskMgrEdit)
	ON_BN_CLICKED(IDC_BUTTON_TASK_MGR_PLAY, &CDlgTaskMgr::OnBnClickedButtonTaskMgrPlay)
	ON_BN_CLICKED(IDC_BUTTON_TASK_MGR_REFRESH, &CDlgTaskMgr::OnBnClickedButtonTaskMgrRefresh)
	ON_BN_CLICKED(IDC_BUTTON_TASK_MGR_EXPORT, &CDlgTaskMgr::OnBnClickedButtonTaskMgrExport)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


enum
{
	TASK_ID = 1, START_TIME, END_TIME, TYPE, PATH, DESCRIPTION, I_FREQ, J_FREQ, I_CNT, J_CNT
};
// CDlgTaskMgr 消息处理程序

BOOL CDlgTaskMgr::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CGridColumnTraitCombo *typeCombo = new CGridColumnTraitCombo;
	wndListTasks_.InsertHiddenLabelColumn();
	wndListTasks_.InsertColumnTrait(TASK_ID, _T("任务号"),		LVCFMT_LEFT, 50, TASK_ID, new CGridColumnTraitEdit);
	wndListTasks_.InsertColumnTrait(START_TIME, _T("起始时间"),	LVCFMT_LEFT, 120, START_TIME, new CGridColumnTraitDateTime);
	wndListTasks_.InsertColumnTrait(END_TIME, _T("结束时间"),		LVCFMT_LEFT, 120, END_TIME, new CGridColumnTraitDateTime);
	wndListTasks_.InsertColumnTrait(TYPE, _T("类型"),			LVCFMT_LEFT, 80, TYPE, typeCombo);
	wndListTasks_.InsertColumnTrait(PATH, _T("文件位置"),			LVCFMT_LEFT, 150, PATH, new CGridColumnTraitEdit);
	wndListTasks_.InsertColumnTrait(DESCRIPTION, _T("描述"),		LVCFMT_LEFT, 100, DESCRIPTION, new CGridColumnTraitEdit);
	wndListTasks_.InsertColumnTrait(I_FREQ, _T("I通道频率"),		LVCFMT_LEFT, 50, I_FREQ, new CGridColumnTraitText);
	wndListTasks_.InsertColumnTrait(J_FREQ, _T("J通道频率"),		LVCFMT_LEFT, 50, J_FREQ, new CGridColumnTraitText);
	wndListTasks_.InsertColumnTrait(I_CNT, _T("I通道文件个数"),	LVCFMT_LEFT, 50, I_CNT, new CGridColumnTraitText);
	wndListTasks_.InsertColumnTrait(J_CNT, _T("J通道文件个数"),	LVCFMT_LEFT, 50, J_CNT, new CGridColumnTraitText);
	
	wndListTasks_.SetCellMargin(1.5);
	
	typeCombo->AddItem(common::REAL_DATA,		_T("实时数据"));
	typeCombo->AddItem(common::HISTORY_DATA,	_T("历史数据"));

	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(7, 0, 0, 0);
	curTime -= timeSpan;

	wndStartTime_.SetTime(&curTime);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgTaskMgr::OnBnClickedButtonTaskManagerSearch()
{
	OnBnClickedButtonTaskMgrRefresh();
}


LRESULT CDlgTaskMgr::OnRefresh(WPARAM, LPARAM)
{
	
	return TRUE;
}

void CDlgTaskMgr::OnBnClickedButtonTaskMgrEdit()
{
	int nSel = wndListTasks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == - 1)
		return;

	int id = wndListTasks_.GetItemData(nSel);
	CString taskID = wndListTasks_.GetItemText(nSel, TASK_ID);
	CString startTime = wndListTasks_.GetItemText(nSel, START_TIME);
	CString endTime = wndListTasks_.GetItemText(nSel, END_TIME);
	CString type = wndListTasks_.GetItemText(nSel, TYPE);
	CString path = wndListTasks_.GetItemText(nSel, PATH);
	CString descpt = wndListTasks_.GetItemText(nSel, DESCRIPTION);

	int typeID = type == _T("实时数据") ? common::REAL_DATA : common::HISTORY_DATA;

	COleVariant vtime(startTime);
	vtime.ChangeType(VT_DATE);
	COleDateTime sqlTime = vtime;

	if( taskID.IsEmpty() ||
		startTime.IsEmpty() || 
		type.IsEmpty() ||
		path.IsEmpty() )
	{
		AfxMessageBox(_T("编辑数据不能为空!"));
		return;
	}

	std::wostringstream os;
	os << L"UPDATE task_t SET "
		<< L"task_id = '"	<< (LPCWSTR)taskID	<< L"',"
		<< L"type = "		<< typeID			<< L","
		<< L"date_added = '"<< (LPCWSTR)(sqlTime.Format(L"%Y-%m-%d %H:%M:%S")) << L"',"
		<< L"path = '"		<< (LPCWSTR)path	<< L"',"
		<< L"description = '" << (LPCWSTR)descpt<< L"' "
		<< L"WHERE id = "	<< id;

	if( !db::DBMgrInstance().ExecuteSQL(os.str().c_str()) )
		::OutputDebugString(L"INSERT INTO ERROR!");
	else
	{
		AfxMessageBox(_T("修改成功"));
		OnBnClickedButtonTaskMgrRefresh();
	}
	
}

void CDlgTaskMgr::OnBnClickedButtonTaskMgrPlay()
{
	if( historyStatus_ == HISTORY_STARTED )
	{
		GetDlgItem(IDC_BUTTON_TASK_MGR_PLAY)->SetWindowText(_T("开始回放"));

		ctrl::ControlMgrInstance()->StopHistory();
		historyStatus_ = HISTORY_STOPPED;
	}
	else if( historyStatus_ == HISTORY_STOPPED )
	{
		GetDlgItem(IDC_BUTTON_TASK_MGR_PLAY)->SetWindowText(_T("停止回放"));
		
		int nSel = wndListTasks_.GetNextItem(-1, LVIS_SELECTED);
		if( nSel == - 1)
			return;

		CString type = wndListTasks_.GetItemText(nSel, TYPE);
		if( type == L"历史数据" )
		{
			AfxMessageBox(L"历史数据不支持回放");
			return;
		}

		CString path = wndListTasks_.GetItemText(nSel, PATH);
		CString i_freq = wndListTasks_.GetItemText(nSel, I_FREQ);
		CString j_freq = wndListTasks_.GetItemText(nSel, J_FREQ);

		CString taskID = wndListTasks_.GetItemText(nSel, TASK_ID);

		path += taskID;

		size_t iFreq = stdex::ToNumber<size_t>(stdex::tString((LPCTSTR)i_freq));
		size_t jFreq = stdex::ToNumber<size_t>(stdex::tString((LPCTSTR)j_freq));
		ctrl::ControlMgrInstance()->PlayHistory((LPCTSTR)(path + L"/I/"), (LPCTSTR)(path + L"/J/"), iFreq, jFreq);
		historyStatus_ = HISTORY_STARTED;
	}

	
}


void CDlgTaskMgr::OnBnClickedButtonTaskMgrRefresh()
{
	Tasks tmpTasks;
	
	try
	{
		_RecordsetPtr &recordset = db::DBMgrInstance().GetRecordSet(_T("SELECT * FROM task_t"));

		while(!recordset->adoEOF)
		{
			TaskInfo taskInfo;
			taskInfo.id_		= recordset->GetCollect(_T("id"));
			taskInfo.taskID_	= (LPCTSTR)(CString)recordset->GetCollect(_T("task_id"));
			taskInfo.dataType_	= (common::DataType)(int)recordset->GetCollect(_T("type"));
			taskInfo.startTime_	= recordset->GetCollect(_T("date_added"));
			taskInfo.taskPath_  = (LPCTSTR)(CString)recordset->GetCollect(_T("path"));
			_variant_t val_j_freq		= recordset->GetCollect(_T("j_freq"));
			_variant_t val_i_freq		= recordset->GetCollect(_T("i_freq"));
			_variant_t val_date_end		= recordset->GetCollect(_T("date_end"));
			_variant_t val_i_total		= recordset->GetCollect(_T("i_total"));
			_variant_t val_j_total		= recordset->GetCollect(_T("j_total"));
			_variant_t val_desc			=  recordset->GetCollect(_T("description"));
			if( val_desc.vt != VT_NULL )
				taskInfo.description_ = (LPCTSTR)(CString)val_desc;
			if( val_j_freq.vt != VT_NULL )
				taskInfo.j_freq_ = val_j_freq;
			if( val_i_freq.vt != VT_NULL )
				taskInfo.i_freq_ = val_i_freq;
			if( val_i_total.vt != VT_NULL )
				taskInfo.i_cnt_ = val_i_total;
			if( val_j_total.vt != VT_NULL )
				taskInfo.j_cnt_ = val_j_total;
			if( val_date_end.vt != VT_NULL )
				taskInfo.endTime_ = val_date_end;

			tmpTasks.push_back(taskInfo);

			recordset->MoveNext();
		}
	}
	catch(_com_error &e)
	{
		AfxMessageBox(e.Description());
		return;
	}

	wndListTasks_.DeleteAllItems();


	UpdateData();

	CTime start, stop;
	wndStartTime_.GetTime(start);
	wndStopTime_.GetTime(stop);

	CTimeSpan span = stop - start;
	if( span < 0 )
		return;

	tasks.swap(tmpTasks);

	std::tr1::wregex rx((LPCTSTR)wndSearchTaskID_);
	int index = 0;
	for(size_t i = 0; i != tasks.size(); ++i)
	{
		CTime timeTmp(tasks[i].startTime_.GetYear(), tasks[i].startTime_.GetMonth(), tasks[i].startTime_.GetDay(),
			tasks[i].startTime_.GetHour(), tasks[i].startTime_.GetMinute(), tasks[i].startTime_.GetSecond());
		CTimeSpan tmpSpan = timeTmp - start;
		if( tmpSpan <= 0 )
			continue;

		wndListTasks_.SetFocus();
		bool match = wndSearchTaskID_.IsEmpty() ? true : std::tr1::regex_search(tasks[i].taskID_.begin(), tasks[i].taskID_.end(), rx);
		if( span >= tmpSpan && match )
		{
			index = wndListTasks_.InsertItem(index, _T(""));
			wndListTasks_.SetItemText(index, TASK_ID, tasks[i].taskID_.c_str());
			wndListTasks_.SetItemText(index, START_TIME, tasks[i].startTime_.Format(_T("%Y-%m-%d %H:%M:%S")));
			wndListTasks_.SetItemText(index, END_TIME, tasks[i].startTime_.Format(_T("%Y-%m-%d %H:%M:%S")));
			wndListTasks_.SetItemText(index, TYPE, tasks[i].dataType_ == common::REAL_DATA ? _T("实时数据") : _T("历史数据"));
			wndListTasks_.SetItemText(index, PATH, tasks[i].taskPath_.c_str());
			wndListTasks_.SetItemText(index, DESCRIPTION, tasks[i].description_.c_str());
			wndListTasks_.SetItemText(index, I_FREQ, stdex::ToString<stdex::tString>(tasks[i].i_freq_).c_str());
			wndListTasks_.SetItemText(index, J_FREQ, stdex::ToString<stdex::tString>(tasks[i].j_freq_).c_str());
			wndListTasks_.SetItemText(index, I_CNT, stdex::ToString<stdex::tString>(tasks[i].i_cnt_).c_str());
			wndListTasks_.SetItemText(index, J_CNT, stdex::ToString<stdex::tString>(tasks[i].j_cnt_).c_str());
			wndListTasks_.SetItemData(index, tasks[i].id_);
			index++;
		}
		else
		{

		}
	}
}

void CDlgTaskMgr::OnBnClickedButtonTaskMgrExport()
{
	if( wndListTasks_.GetItemCount() == 0 )
	{
		AfxMessageBox(L"列表中没有数据,不能导出");
		return;
	}

	CString path = GetFolder();
	if( path.IsEmpty() )
		return;

	path += _T("\\任务列表");
	ExportListToExcel(wndListTasks_, path, _T("任务记录"));
}

void CDlgTaskMgr::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(dc, this);


	HBRUSH tmp = ui::draw::AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	CBrush brush;
	brush.Attach(tmp);
	CRect rcClient;
	GetClientRect(rcClient);
	memDC.GetDC().FillRect(rcClient, &brush);
}

HBRUSH CDlgTaskMgr::OnCtlColor(CDC   *   pDC,CWnd   *   pWnd,UINT   nCtlColor) 
{ 
	HBRUSH   hbr = CDialog::OnCtlColor(pDC,pWnd,nCtlColor); 
	if   (nCtlColor==CTLCOLOR_STATIC ) 
	{ 
		pDC-> SetBkMode(TRANSPARENT); 
		return   (HBRUSH)::GetStockObject(NULL_BRUSH); 
	} 
	return   hbr; 
} 