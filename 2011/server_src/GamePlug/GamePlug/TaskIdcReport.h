#ifndef __I8DESK_TASK_IDC_REPORT_HPP
#define __I8DESK_TASK_IDC_REPORT_HPP


#include "../../../include/MultiThread/Lock.hpp"


namespace i8desk
{

	// forward declare
	class TaskItem;
	typedef std::tr1::shared_ptr<TaskItem>	TaskItemPtr;


	// --------------------------------------------
	// class TaskIdcReport

	// 下载完成过后负责上报到中心

	class TaskIdcReportor
	{
	private:
		TaskItemPtr			taskItem_;	
		ISysOpt				*sysOpt_;
		async::thread::AutoEvent &exit_;

	public:
		TaskIdcReportor(const TaskItemPtr &taskItem, ISysOpt *sysOpt, async::thread::AutoEvent &exit);
		~TaskIdcReportor();

	private:
		TaskIdcReportor(const TaskIdcReportor &);
		TaskIdcReportor &operator=(const TaskIdcReportor &);

	public:
		void Run();
		void Stop();

	private:
		void _RunImpl();
	};

	typedef std::tr1::shared_ptr<TaskIdcReportor>	TaskIdcReportPtr;
}




#endif