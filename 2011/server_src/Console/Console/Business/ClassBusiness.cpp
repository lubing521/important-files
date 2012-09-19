#include "stdafx.h"
#include "ClassBusiness.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/UI/IconHelper.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../ManagerInstance.h"
#include "../UI/Skin/SkinMgr.h"

namespace i8desk
{
	namespace business
	{
		namespace Class
		{
			stdex::tString GetCID(const stdex::tString &className)
			{
				const data_helper::ClassTraits::MapType &classes = GetDataMgr().GetClasses();

				for(data_helper::ClassTraits::MapConstIterator iter = classes.begin();
					iter != classes.end(); ++iter)
				{
					if( iter->second->Name == className )
						return iter->first;
				}

				assert(0);
				return CLASS_WL_GUID;
			}


			stdex::tString GetName(const stdex::tString &CID)
			{
				const data_helper::ClassTraits::MapType &classes = GetDataMgr().GetClasses();

				data_helper::ClassTraits::MapConstIterator iter = classes.find(CID);
				if( iter != classes.end() )
					return iter->second->Name;
				else
				{
					//assert(0);
					return _T("");
				}
			}

			bool IsDefault(const stdex::tString &CID)
			{
				static stdex::tString defaultClass[] = 
				{
					CLASS_WL_GUID, 
					CLASS_DJ_GUID,
					CLASS_XX_GUID,
					CLASS_WY_GUID,
					CLASS_DZ_GUID,
					CLASS_QP_GUID,
					CLASS_PL_GUID,
					CLASS_LT_GUID,
					CLASS_CY_GUID,
					CLASS_YY_GUID,
					CLASS_GP_GUID
				};

				for(size_t i = 0; i != _countof(defaultClass); ++i)
				{
					if( defaultClass[i] == CID )
						return true;
				}

				return false;
			}

			void GetSortDefaultClass( data_helper::ClassTraits::VectorType &classes )
			{
				data_helper::ClassTraits::MapType &mapclasses = GetDataMgr().GetClasses();

				data_helper::ClassTraits::MapConstIterator iter_wl = mapclasses.find(CLASS_WL_GUID);
				if( iter_wl == mapclasses.end() )
					return;
				classes.push_back(iter_wl->second);

				data_helper::ClassTraits::MapConstIterator iter_dj = mapclasses.find(CLASS_DJ_GUID);
				if( iter_dj == mapclasses.end() )
					return;
				classes.push_back(iter_dj->second);

				data_helper::ClassTraits::MapConstIterator iter_xx = mapclasses.find(CLASS_XX_GUID);
				if( iter_xx == mapclasses.end() )
					return;
				classes.push_back(iter_xx->second);

				data_helper::ClassTraits::MapConstIterator iter_wy = mapclasses.find(CLASS_WY_GUID);
				if( iter_wy == mapclasses.end() )
					return;
				classes.push_back(iter_wy->second);

				data_helper::ClassTraits::MapConstIterator iter_dz = mapclasses.find(CLASS_DZ_GUID);
				if( iter_dz == mapclasses.end() )
					return;
				classes.push_back(iter_dz->second);

				data_helper::ClassTraits::MapConstIterator iter_qp = mapclasses.find(CLASS_QP_GUID);
				if( iter_qp == mapclasses.end() )
					return;
				classes.push_back(iter_qp->second);

				data_helper::ClassTraits::MapConstIterator iter_pl = mapclasses.find(CLASS_PL_GUID);
				if( iter_pl == mapclasses.end() )
					return;
				classes.push_back(iter_pl->second);

				data_helper::ClassTraits::MapConstIterator iter_lt = mapclasses.find(CLASS_LT_GUID);
				if( iter_lt == mapclasses.end() )
					return;
				classes.push_back(iter_lt->second);

				data_helper::ClassTraits::MapConstIterator iter_cy = mapclasses.find(CLASS_CY_GUID);
				if( iter_cy == mapclasses.end() )
					return;
				classes.push_back(iter_cy->second);

				data_helper::ClassTraits::MapConstIterator iter_yy = mapclasses.find(CLASS_YY_GUID);
				if( iter_yy == mapclasses.end() )
					return;
				classes.push_back(iter_yy->second);

				data_helper::ClassTraits::MapConstIterator iter_gp = mapclasses.find(CLASS_GP_GUID);
				if( iter_gp == mapclasses.end() )
					return;
				classes.push_back(iter_gp->second);

			}

			bool IsSingleGame(const stdex::tString &CID)
			{
				return CID == CLASS_DJ_GUID;
			}

			void GetAllSortedClasses( data_helper::ClassTraits::VectorType &classes )
			{
				data_helper::ClassTraits::MapType &mapclasses = GetDataMgr().GetClasses();

				GetSortDefaultClass(classes);
				
				for ( data_helper::ClassTraits::MapConstIterator  mapiter = mapclasses.begin();	
					mapiter != mapclasses.end(); ++mapiter )
				{
					if( IsDefault(mapiter->first) )
						continue;

					classes.push_back( mapiter->second );
				}
			}


			const utility::BITMAPPtr &GetIcon(const data_helper::ClassTraits::ValueType &val)
			{
				if( val->Thumbnail != 0 )
					return val->Thumbnail;

				// 试图从主服务上获取，若失败则获取默认
				stdex::tString iconPath = _T("data\\ClassIcon\\");
				iconPath += val->Name;
				iconPath += _T(".ico");

				data_helper::Buffer data;
				if( GetControlMgr().GetFileData(iconPath, data) )
				{
					HBITMAP tmp = AtlLoadGdiplusImage(data.first.get(), data.second);
					
					val->Icon = tmp;	
					val->Thumbnail = GetBitmapThumbnail(tmp, 16, 16);
				}
				else
				{
					if( IsDefault(val->CID) )
						val->Icon = i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/idcclass.png"));
					else
						val->Icon = i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/svrresmgr.png"));
				
					val->Thumbnail = val->Icon;
				}

				return val->Thumbnail;
			}


			void SetIcon(const data_helper::ClassTraits::ValueType &val, const utility::BITMAPPtr &icon)
			{
				val->Icon = icon;
				val->Thumbnail = GetBitmapThumbnail(icon, 16, 16);


				stdex::tString iconPath = _T("data\\ClassIcon\\");
				iconPath += val->Name;
				iconPath += _T(".ico");

				const size_t size = 1024 * 10;
				data_helper::Buffer data(data_helper::BufferAllocator(size), size);
				SaveIconToBuffer(Bitmap2Icon(icon), (BYTE *)data.first.get(), data.second);
				assert(data.second < size);
				
				assert(GetControlMgr().SaveFileData(iconPath, data));
			}

			bool HasSameName(const data_helper::ClassTraits::ValueType &val)
			{
				using i8desk::data_helper::ClassTraits;
				const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

				for(ClassTraits::MapConstIterator iter = classes.begin(); iter != classes.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->CID,val->CID) == 0)
						continue;
					if( utility::Strcmp(iter->second->Name, val->Name) == 0 )
						return true;
				}

				return false;
			}

			stdex::tString GetPath(const data_helper::ClassTraits::ValueType &val)
			{
				using i8desk::data_helper::ClassTraits;
				const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

				stdex::tString defpath;
				defpath = i8desk::GetDataMgr().GetOptVal(OPT_D_INITDIR, defpath).c_str();
				TCHAR path[MAX_PATH] = {0};
				utility::Strcpy(path, defpath.c_str());
				::PathRemoveBackslash(path);
				::PathAddBackslash(path);
				defpath = path;
				defpath += val->Name;
				
				return  defpath;
			}

			bool HasGame(const stdex::tString &CID)
			{
				using i8desk::data_helper::GameTraits;
				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->CID,CID.c_str()) == 0)
						return true;
				}

				return false;
			}

			void SetDeskTopClass( const data_helper::ClassTraits::ValueType &val)
			{
				stdex::tString str = i8desk::GetDataMgr().GetOptVal(OPT_M_CLASSSHUT, _T(""));

				std::vector<stdex::tString> classnames;
				stdex::Split(classnames, str, _T('|'));

				std::vector<stdex::tString>::iterator iter;
				for(iter = classnames.begin(); iter != classnames.end(); ++iter )
				{
					if ( (*iter) == val->Name )
						return;
				}
				
				stdex::tOstringstream os;
				for( iter = classnames.begin(); iter != classnames.end(); ++iter )
				{
					os << (*iter).c_str() << _T("|");
				}
				os << val->Name;

				i8desk::GetDataMgr().SetOptVal(OPT_M_CLASSSHUT,os.str());
			}

			void UnSetDeskTopClass(const data_helper::ClassTraits::ValueType &val)
			{
				stdex::tString str = i8desk::GetDataMgr().GetOptVal(OPT_M_CLASSSHUT, _T(""));

				std::vector<stdex::tString> classnames;
				stdex::Split(classnames, str, _T('|'));

				stdex::tOstringstream os;
				for(std::vector<stdex::tString>::const_iterator iter = classnames.begin(); iter != classnames.end(); ++iter )
				{
					if( *iter != val->Name )
						os << (*iter).c_str() << _T("|");
				}

				i8desk::GetDataMgr().SetOptVal(OPT_M_CLASSSHUT, os.str());
			}
	
			bool IsSetDeskTopClass(const data_helper::ClassTraits::ValueType &val)
			{
				stdex::tString str = i8desk::GetDataMgr().GetOptVal(OPT_M_CLASSSHUT, _T(""));

				std::vector<stdex::tString> classnames;
				stdex::Split(classnames, str, _T('|'));

				for(std::vector<stdex::tString>::const_iterator iter = classnames.begin(); 
					iter != classnames.end(); ++iter )
				{
					if ( (*iter) == val->Name )
						return true;
				}

				return false;
			}

		}
	}
}