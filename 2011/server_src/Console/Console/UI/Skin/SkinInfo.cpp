#include "stdafx.h"
#include "SkinInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{
	namespace ui
	{
		SkinInfo::SkinInfo()
		{

		}

		SkinInfo::~SkinInfo()
		{

		}
		const stdex::tString &SkinInfo::GetSkinPath() const
		{
			return skinPath_;
		}

		void SkinInfo::Load(const stdex::tString &filePath)
		{
			skinPath_ = filePath;
		
			// Max Min Close Setting
			maxBtn_[0]				= skinPath_ + _T("Main\\Max_Default.png");
			maxBtn_[1]				= skinPath_ + _T("Main\\Max_MoveOn.png");
			maxBtn_[2]				= skinPath_ + _T("Main\\Max_Click.png");

			minBtn_[0]				= skinPath_ + _T("Main\\Mini_Default.png");
			minBtn_[1]				= skinPath_ + _T("Main\\Mini_MoveOn.png");
			minBtn_[2]				= skinPath_ + _T("Main\\Mini_Click.png");

			closeBtn_[0]			= skinPath_ + _T("Main\\Close_Default.png");
			closeBtn_[1]			= skinPath_ + _T("Main\\Close_MoveOn.png");
			closeBtn_[2]			= skinPath_ + _T("Main\\Close_Click.png");

			settingBtn_[0]			= skinPath_ + _T("Main\\SetBtn_Default.png");
			settingBtn_[1]			= skinPath_ + _T("Main\\SetBtn_MoveOn.png");
			settingBtn_[2]			= skinPath_ + _T("Main\\SetBtn_Click.png");

			// Main Frame
			mainFrame_[0]			= skinPath_ + _T("Main\\Server_TopBg_Mid.png");
			mainFrame_[1]			= skinPath_ + _T("Main\\Server_TopBg_Left.png");
			mainFrame_[2]			= skinPath_ + _T("Main\\Server_TopBg_Right.png");
			mainFrame_[3]			= skinPath_ + _T("");
			mainFrame_[4]			= skinPath_ + _T("Main\\Server_ContentBg_Left.png");
			mainFrame_[5]			= skinPath_ + _T("Main\\Server_ContentBg_Right.png");
			mainFrame_[6]			= skinPath_ + _T("Main\\Server_BtmMidBg.png");
			mainFrame_[7]			= skinPath_ + _T("Main\\Server_BtmBg_Left.png");
			mainFrame_[8]			= skinPath_ + _T("Main\\Server_BtmBg_Right.png");
			mainFrame_[9]			= skinPath_ + _T("Main\\Server_BtmBg_Line.png");
			
			// Logo
			topLogo_				= skinPath_ + _T("Main\\Server_TopLogo.png");

			// Nav Bar
			navTab_[0]				= skinPath_ + _T("Navigate\\ServerMenu_ControlCenter.png");
			navTab_[1]				= skinPath_ + _T("Navigate\\ServerMenu_ControlCenter_Click.png");
			navTab_[2]				= skinPath_ + _T("Navigate\\ServerMenu_Servers.png");
			navTab_[3]				= skinPath_ + _T("Navigate\\ServerMenu_Servers_Click.png");
			navTab_[4]				= skinPath_ + _T("Navigate\\ServerMenu_Client.png");
			navTab_[5]				= skinPath_ + _T("Navigate\\ServerMenu_Client_Click.png");
			navTab_[6]				= skinPath_ + _T("Navigate\\ServerMenu_Resources.png");
			navTab_[7]				= skinPath_ + _T("Navigate\\ServerMenu_Resources_Click.png");
			navTab_[8]				= skinPath_ + _T("Navigate\\ServerMenu_PushChannel.png");
			navTab_[9]				= skinPath_ + _T("Navigate\\ServerMenu_PushChannel_Click.png");
			navTab_[10]				= skinPath_ + _T("Navigate\\ServerMenu_PowerChannel.png");
			navTab_[11]				= skinPath_ + _T("Navigate\\ServerMenu_PowerChannel_Click.png");
			navTab_[12]				= skinPath_ + _T("Navigate\\ServerMenu_Toolbox.png");
			navTab_[13]				= skinPath_ + _T("Navigate\\ServerMenu_Toolbox_Click.png");
			navTab_[14]				= skinPath_ + _T("Navigate\\ServerMenu_SystemSettings.png");
			navTab_[15]				= skinPath_ + _T("Navigate\\ServerMenu_SystemSettings_Click.png");
			navTab_[16]				= skinPath_ + _T("Navigate\\ServerMenu_i8Housekeeper.png");
			navTab_[17]				= skinPath_ + _T("Navigate\\ServerMenu_i8Housekeeper_Click.png");
			
			// Menu background
			menuBg_[0]				= skinPath_ + _T("Main\\Server_MenuBg_Left.png");
			menuBg_[1]				= skinPath_ + _T("Main\\Server_MenuBg_Mid.png");
			menuBg_[2]				= skinPath_ + _T("Main\\Server_MenuBg_Right.png");

			// Normal--Add Delete Modify
			addBtn_[0]				= skinPath_ + _T("Common\\Left_Btn_Add.png");
			addBtn_[1]				= skinPath_ + _T("Common\\Left_Btn_Add_Hover.png");
			addBtn_[2]				= skinPath_ + _T("Common\\Left_Btn_Add_Press.png");

			delBtn_[0]				= skinPath_ + _T("Common\\Left_Btn_Del.png");
			delBtn_[1]				= skinPath_ + _T("Common\\Left_Btn_Del_Hover.png");
			delBtn_[2]				= skinPath_ + _T("Common\\Left_Btn_Del_Press.png");

			modBtn_[0]				= skinPath_ + _T("Common\\Left_Btn_Modify.png");
			modBtn_[1]				= skinPath_ + _T("Common\\Left_Btn_Modify_Hover.png");
			modBtn_[2]				= skinPath_ + _T("Common\\Left_Btn_Modify_Press.png");
			

			// List Header
			listHeader_[0]			= skinPath_ + _T("Common\\ListHeaderBg.png");
			listHeader_[1]			= skinPath_ + _T("Common\\ListHeaderLine.png");

			// Query
			query_[0]				= skinPath_ + _T("Common\\Query_Input.png");
			query_[1]				= skinPath_ + _T("Common\\Query_Btn.png");


			// Left Working Area
			leftWorking_[0]			= skinPath_ + _T("Common\\Left_Title_bg.png");
			leftWorking_[1]			= skinPath_ + _T("Common\\Left_Content_bg.png");
			leftWorking_[2]			= skinPath_ + _T("Common\\Left_Bottom_bg.png");

			// Right Working Area
			rightWorking_[0]		= skinPath_ + _T("Common\\Right_Left_Line.png");
			rightWorking_[1]		= skinPath_ + _T("Common\\Right_Bottom_Line.png");
			rightWorking_[2]		= skinPath_ + _T("Common\\Right_Content_bg.png");
			rightWorking_[3]		= skinPath_ + _T("Common\\Right_Top_Line.png");
			rightWorking_[4]		= skinPath_ + _T("Common\\Right_Right_Line.png");

			// Outbox Line & Table Line
			boxLine_[0]				= skinPath_ + _T("Common\\OutBoxLine.png");
			boxLine_[1]				= skinPath_ + _T("Common\\TableLine.png");


			// Server
			serverMain_				= skinPath_ + _T("Server\\Ico_Server.png");
			serverVDiskClient_		= skinPath_ + _T("Server\\Icon_Client.png");

			// Sync Task
			syncTaskTransfering_	= skinPath_ + _T("SyncTask\\Ico_CircleBegin.png");
			syncTaskTransferOK_		= skinPath_ + _T("SyncTask\\Ico_CircleOk.png");

			syncTaskMain_			= skinPath_ + _T("SyncTask\\Ico_Sync.png");
			syncTaskRes_			= skinPath_ + _T("SyncTask\\Ico_SyncCircle.png");

			syncTaskStateNormal_[0] = skinPath_ + _T("SyncTask\\BtnSyncStatus_Green_Ing.png");
			syncTaskStateNormal_[1] = skinPath_ + _T("SyncTask\\BtnSyncStatus_Green_Ok.png");

			syncTaskStateCurrent_[0]= skinPath_ + _T("SyncTask\\BtnSyncStatus_Gray_Ing.png");
			syncTaskStateCurrent_[1]= skinPath_ + _T("SyncTask\\BtnSyncStatus_Gray_Ok.png");

			syncTaskFromTo_[0]		= skinPath_ + _T("SyncTask\\SyncFrom.png");
			syncTaskFromTo_[1]		= skinPath_ + _T("SyncTask\\SyncTo.png");

			syncBg_[0]				= skinPath_ + _T("Common\\ListHeaderBg.png");
			syncBg_[1]				= skinPath_ + _T("Common\\ListHeaderLine.png");

			// Client
			clientMain_				= skinPath_ + _T("Client\\Ico_Clients.png");
			clientStatus_[0]		= skinPath_ + _T("Client\\Ico_Offline.png");
			clientStatus_[1]		= skinPath_ + _T("Client\\Ico_Online.png");
		}
	}
}