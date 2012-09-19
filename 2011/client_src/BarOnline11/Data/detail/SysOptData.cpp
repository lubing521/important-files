#include "stdafx.h"
#include "SysOptData.h"

#include "../Basic.h"
#include "../CustomDataType.h"
#include "../../../../include/Utility/utility.h"

namespace i8desk
{
	namespace data
	{
		namespace detail
		{
			
			SysOptImpl::SysOptImpl()
			{

			}


			void SysOptImpl::Load()
			{
				const stdex::tString path = utility::GetAppPath() + _T("Data/") + _T("sysopt.dat");
				
				utility::CAutoFile hFile = ::CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
				if( !hFile.IsValid() )
					throw exception::SystemExcept(_T("Open SysOpt.dat failed!"));
					
				DWORD dwSize = ::GetFileSize(hFile, 0);
				Buffer buf = MakeBuffer(dwSize);

				DWORD dwReadBytes = 0;
				if( !utility::ReadFileContent(hFile, (BYTE *)buf.first.get(), dwSize) )
					throw exception::SystemExcept(_T("Read SysOpt.dat failed!"));

				try
				{
					CPkgHelper in(buf.first.get());
					DWORD ret = 0;
					in >> ret;

					if( CMD_RET_SUCCESS == ret )
					{
						DWORD dwCount = 0;
						in >> dwCount;

						unsigned char ucInvalid = 0;
						for(size_t i = 0; i != dwCount; ++i)
						{	
							TCHAR key[MAX_NAME_LEN] = {0};
							TCHAR value[MAX_SYSOPT_LEN] = {0};
							
							in >> key;
							
							//Val.MakeLower();
							
							in >> ucInvalid;
							in >> value;
							
							sysOpt_.insert(std::make_pair(key, value));
						}
					}
				}
				catch(std::exception &e) 
				{ 
					throw exception::RuntimeExcept(e.what());
				}
			}
		}
	}
}