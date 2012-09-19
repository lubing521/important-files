#ifndef __SKIN_INFO_HPP
#define __SKIN_INFO_HPP

#include <array>


namespace i8desk
{
	namespace ui
	{
		// 皮肤信息
		struct SkinInfo  
		{		
			stdex::tString skinPath_;

			// 最大化按钮
			std::tr1::array<stdex::tString, 3> maxBtn_;
			// 最小化按钮
			std::tr1::array<stdex::tString, 3> minBtn_;
			// 关闭按钮
			std::tr1::array<stdex::tString, 3> closeBtn_;
			// 设置按钮
			std::tr1::array<stdex::tString, 3> settingBtn_;

			// Main Frame
			std::tr1::array<stdex::tString, 10> mainFrame_;

			// Top Logo
			stdex::tString topLogo_;

			// Navigate Tabe
			std::tr1::array<stdex::tString, 18> navTab_;

			// Menu Background
			std::tr1::array<stdex::tString, 3> menuBg_;

			// Add Modify Delete
			std::tr1::array<stdex::tString, 3> addBtn_;
			std::tr1::array<stdex::tString, 3> delBtn_;
			std::tr1::array<stdex::tString, 3> modBtn_;

			// List Header
			std::tr1::array<stdex::tString, 2> listHeader_;

			// Left Working Area
			std::tr1::array<stdex::tString, 3> leftWorking_;
			// Right Working Area
			std::tr1::array<stdex::tString, 5> rightWorking_;

			// OutBox Line & Table Line
			std::tr1::array<stdex::tString, 2> boxLine_;

			// Query Input & button
			std::tr1::array<stdex::tString, 2> query_;

			// Server
			stdex::tString serverMain_;
			stdex::tString serverVDiskClient_;
		

			// Sync Task
			stdex::tString syncTaskMain_;
			stdex::tString syncTaskRes_;

			stdex::tString syncTaskTransfering_;
			stdex::tString syncTaskTransferOK_;
			
			std::tr1::array<stdex::tString, 2> syncTaskStateNormal_;
			std::tr1::array<stdex::tString, 2> syncTaskStateCurrent_;
			std::tr1::array<stdex::tString, 2> syncTaskFromTo_;
			std::tr1::array<stdex::tString, 3> syncBg_;

			// Client
			stdex::tString clientMain_;
			std::tr1::array<stdex::tString, 2> clientStatus_;

			// Ctor
			SkinInfo();
			// Dtor
			~SkinInfo();

			const stdex::tString &GetSkinPath() const;			// 获取皮肤路径
			void Load(const stdex::tString &filePath);		// 从指定皮肤配置文件中加载皮肤信息
		};
	}
}

#endif