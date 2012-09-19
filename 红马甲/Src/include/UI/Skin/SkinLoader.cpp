#include "stdafx.h"
#include "../../resource.h"
#include "SkinLoader.h"

#include "../../Misc.h"
#include "../../ManagerInstance.h"
#include "../../../../../include/Utility/utility.h"
#include "../../../../../include/ui/ImageHelpers.h"
#include "../../../../../include/ui/IconHelper.h"
#include "../../../../../include/Zip/XUnzip.h"
#include "../../../../../include/Http/HttpHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{
	namespace ui
	{

		namespace detail
		{
			static bool GetGameIcons(data_helper::Buffer &iconBuf)
			{
				if( !GetControlMgr().GetGameIcons(iconBuf) )
				{
					//assert(0); // throw FatalError
					return false;
				}

				return true;
			}
		}

		// ------------------------------------
		// struct LoaderLocal
		// 从本地读取文件

		struct LoaderLocal
		{
			static data_helper::Buffer GetZipBuffer(const stdex::tString &path)
			{
				stdex::tString filePath = utility::GetAppPath() + path; 
				
				std::ifstream in(filePath.c_str(), std::ios::binary | std::ios::in);
				assert( in.good() );

				data_helper::Buffer zipBuf;
				if( in )
				{
					in.seekg(0, std::ios_base::end);
					size_t size = in.tellg();
					in.seekg(0, std::ios_base::beg);

					zipBuf = data_helper::Buffer(data_helper::BufferAllocator(size + 1), size + 1);
					in.read(zipBuf.first.get(), zipBuf.second);
				}
				
				return zipBuf;
			}

			static bool GetGameIcons(data_helper::Buffer &iconBuf)
			{
				return detail::GetGameIcons(iconBuf);
			}
		};

		// ------------------------------------
		// struct LoaderRemote
		// 从网络读取文件

		struct LoaderRemote
		{
			static data_helper::Buffer GetZipBuffer(const stdex::tString &path)
			{
				data_helper::Buffer buf;
				if( !GetControlMgr().GetFileData(path, buf) )
				{
					assert(0);
				}

				return buf;
			}

			static bool GetGameIcons(data_helper::Buffer &iconBuf)
			{
				return detail::GetGameIcons(iconBuf);
			}
		};


		// ------------------------------------
		// class SkinLoaderZip
		// 从网络读取文件Zip

		template < typename LoaderT >
		class SkinLoader
			: public SkinLoaderInterface
		{
			HZIP zip_;

			typedef std::map<long, data_helper::Buffer> Icons;
			Icons icons_;

#ifdef _UNICODE
			typedef ZIPENTRYW ZipEntry;
#else
			typedef ZIPENTRY ZipEntry;
#endif

		public:
			SkinLoader()
				: zip_(0)
			{
				// Skin File
				data_helper::Buffer skinBuf = LoaderT::GetZipBuffer(_T("Skin\\Console\\Console.zip"));

				{
					std::ofstream out(_T("C:\\Console.zip"), std::ios::binary | std::ios::out);
					out.write(skinBuf.first.get(), skinBuf.second);
				}
				//zip_ = ::OpenZip(skinBuf.first.get(), skinBuf.second, ZIP_MEMORY, 0);
				zip_ = ::OpenZip((void *)_T("C:\\Console.zip"), 0, ZIP_FILENAME, 0);


				// Icon File

				// Default Icon
				HICON icon = AfxGetApp()->LoadIcon(IDI_ICON_DEFAULT_GAME);

				const size_t size = 1024 * 10;
				data_helper::Buffer data(data_helper::BufferAllocator(size), size);
				SaveIconToBuffer(icon, (BYTE *)data.first.get(), data.second);
				icons_.insert(std::make_pair(0, data));
				
				// Svr Icon
				data_helper::Buffer iconBuf;
				bool suc = LoaderT::GetGameIcons(iconBuf);
				if( suc )
				{
					CPkgHelper in(iconBuf.first.get());

					size_t ret = 0;
					size_t cnt = 0;

					in >> ret >> cnt;

					for(size_t i = 0; i != cnt; ++i)
					{
						long gid = 0;
						size_t size = 0;
						in >> gid >> size;

						data_helper::Buffer tmp(data_helper::BufferAllocator(size), size);
						in.PopPointer(tmp.first.get(), size);

						icons_.insert(std::make_pair(gid, tmp));
					}
				}
			}
			~SkinLoader()
			{
				assert(zip_ != 0);
				::CloseZip(zip_);
				zip_ = 0;

				::DeleteFile(_T("C:\\Console.zip"));
			}

			virtual utility::BITMAPPtr Load(const stdex::tString &path)
			{
				assert(zip_ != 0);
				ZipEntry ze = {0};
				int idx = 0;

				ZRESULT zr = ::FindZipItem(zip_, path.c_str(), false, &idx, &ze);
				HBITMAP bitmap = 0;
				if( zr == ZR_OK )
				{
					assert(ze.unc_size != 0);
					data_helper::Buffer buf(data_helper::BufferAllocator(ze.unc_size), ze.unc_size);
					::UnzipItem(zip_, ze.index, buf.first.get(), buf.second, ZIP_MEMORY);

					bitmap = AtlLoadGdiplusImage(buf.first.get(), buf.second);
					assert(bitmap != 0);
				}
				else
					assert(0);

				utility::BITMAPPtr bmp = bitmap;
				return bmp;
			}


			// 1. 从压缩包获取
			// 2. 从服务器获取
			// 3. 加载默认
			virtual utility::ICONPtr Load(long gid)
			{
				Icons::iterator iter = icons_.find(gid);

				HICON icon = 0;
				if( iter == icons_.end() )
				{
					unsigned char iconBuf[10 * 1024] = {0};
					size_t size = 0;
					if( GetControlMgr().GetIcon(gid, iconBuf, size) )
					{
						icon = LoadIconFromBuffer(iconBuf, size);
					}
				}
				else
				{
					data_helper::Buffer buf = iter->second;
					icon = LoadIconFromBuffer((const unsigned char *)buf.first.get(), buf.second);
				}

				if( icon == 0 )
				{
					data_helper::Buffer buf = icons_.find(0)->second;
					icon = LoadIconFromBuffer((const unsigned char *)buf.first.get(), buf.second);
				}

				return icon;
			}
		};

		typedef SkinLoader<LoaderLocal>		SkinLoaderLocal;
		typedef SkinLoader<LoaderRemote>	SkinLoaderRemote;



		SkinLoaderPtr GetSkinLoader()
		{
			SkinLoaderPtr loader;

			if( IsRunOnServer() )
			{
				loader.reset(new SkinLoaderLocal);
			}
			else
			{
				loader.reset(new SkinLoaderRemote);
			}

			return loader;
		}
	}
}