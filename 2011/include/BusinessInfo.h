#ifndef __BUSINESS_INFO_HPP
#define __BUSINESS_INFO_HPP


namespace i8desk
{
    struct GameExInfoVersion_st
    {
        DWORD gid;
        DWORD dwVersion;
    };
    struct GameExInfoVersionArray_st
    {
        DWORD dwCount;
        GameExInfoVersion_st astItem[1];
    };
	// 操作类型
	enum BusinessCmd_en
    {
        E_BSC_GET_EX_INFO = 1,    //取得扩展信息，param1扩展信息类型，param2相关游戏GID，
                                  //返回值为空表示信息还没有下载，有值就是由CoTaskMemAlloc分配一段内存的地址，
                                  //前四个字节记录扩展信息长度，后面都是扩展信息
        E_BSC_UPDATE_EX_INFO,     //更新扩展信息，param1扩展信息类型，param2相关游戏GID
        E_BSC_GET_GAMEEX_VERSION, //取得所有游戏的扩展信息版本，返回值为是由CoTaskMemAlloc分配一段内存的地址，结构是GameExInfoVersionArray_st
        E_BSC_GET_GAMEEX_GLOBAL_VERSION, //取得游戏的扩展信息总版本，返回值就是版本号
	};

	// 扩展信息类型
	enum 
	{ 
		GameXML = 1,	// 游戏相关
		ClientXML		// 菜单相关
	};

}



#endif