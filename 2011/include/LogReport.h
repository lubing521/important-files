#ifndef __LOG_REPORT_HPP
#define __LOG_REPORT_HPP

namespace i8desk
{
	// LogRpt SendMessage 接口
	enum 
	{
		LOG_RPT_CHECK_USER		= 1,	// 检测用户账号
		LOG_RPT_GET_LOGIN_STATE,		// 获取登录状态
		LOG_RPT_SAVE_DATA,				// 保存数据到中心
		LOG_RPT_GET_JOB_URL,			// 获取删除任务URL
		LOG_RPT_GET_CLIENT_MENU_URL		// 获取菜单增值URL
	};
}





#endif