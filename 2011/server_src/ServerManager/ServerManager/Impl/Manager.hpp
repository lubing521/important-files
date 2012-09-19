#ifndef __MANAGER_HPP
#define __MANAGER_HPP

#include <functional>

namespace mgr
{

	class ServiceMgr
	{
		typedef std::tr1::function<stdex::tString (const stdex::tString &)> RunVDiskCallback;

	public:
		ServiceMgr();

	private:
		ServiceMgr(const ServiceMgr &);
		ServiceMgr &operator=(const ServiceMgr &);

	public:
		// 是否为主服务
		bool IsManService() const;

		// 是否为虚拟盘服务
		bool IsVDiskService() const;

		// 是否运行主服务
		bool IsRunMainService() const;

		// 是否运行虚拟盘服务
		bool IsRunVDiskService() const;

		// 运行主服务
		bool RunMainService();

		// 运行虚拟盘服务
		bool RunVDiskService();

		// 停止主服务
		bool StopMainService();

		// 停止虚拟盘服务
		bool StopVDiskService();

		// 运行主服务配置
		void RunMainConfig();

		// 运行虚拟盘配置
		void RunVDiskConfig(const RunVDiskCallback &callback);
	};
}


#endif