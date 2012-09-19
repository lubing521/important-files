#ifndef __PLUGIN_OPERATE_INFO_HPP
#define __PLUGIN_OPERATE_INFO_HPP


namespace i8desk
{
	// SendMessage
	enum PlugToolCmd
	{
		Icon = 1,			// 获取插件Icon

		Config = 0x10,		// 配置插件
		Enable,				// 启用插件
		Disable,			// 禁用插件
		Download,			// 下载插件
		Status				// 下载状态
	};
}



#endif