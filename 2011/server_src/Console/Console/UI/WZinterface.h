#ifndef __WIZARD_INTERFACE_HPP
#define __WIZARD_INTERFACE_HPP


namespace i8desk
{
	namespace wz
	{
		struct WZInterface
		{
			virtual ~WZInterface() = 0
			{}

			virtual void OnComplate() = 0;
			virtual void OnShow(int) = 0;

		};
	}
}



#endif