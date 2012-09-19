#pragma once

#include <algorithm>

//游戏排序方式，但这两种排序，都是增值游戏一定会排在前面。
#define SORT_WINDOWS	0	//按windows资源管理器默认方式排序
#define SORT_CLICK		1	//按游戏点击率排序
#define SORT_FINDPOS    2   //匹配位置

template<class TYPE, int N = SORT_WINDOWS>
struct Pred
{
    bool operator()(const TYPE* p1, const TYPE* p2)
    {
        int nLen1 = p1->Name.GetLength();
        int nLen2 = p2->Name.GetLength();
        int nLen = nLen1 > nLen2 ? nLen1 : nLen2;

        //增值放最前
        if (p1->dwI8Play || p2->dwI8Play)
        {
            if (p1->dwI8Play && p2->dwI8Play)
            {
                if (p1->dwI8Play == p2->dwI8Play)
                    return p1->dwSvrClick > p2->dwSvrClick;
                return p1->dwI8Play > p2->dwI8Play;
            }
            if (p1->dwI8Play)
                return true;
            if (p2->dwI8Play)
                return false;
        }
        //其它按资源管理器的排列方式来排。(升序)
        return StrNCmpI(p1->Name, p2->Name, nLen) < 0;
    }
};

template<class TYPE>
struct Pred<TYPE, SORT_CLICK>
{
    bool operator()(const TYPE* p1, const TYPE* p2)
    {
        //增值放最前
        if (p1->dwI8Play || p2->dwI8Play)
        {
            if (p1->dwI8Play && p2->dwI8Play)
            {
                if (p1->dwI8Play == p2->dwI8Play)
                    return p1->dwSvrClick > p2->dwSvrClick;
                return p1->dwI8Play > p2->dwI8Play;
            }

            if (p1->dwI8Play)
                return true;
            if (p2->dwI8Play)
                return false;
        }

        //按点击数的升序来排
        return p1->dwSvrClick > p2->dwSvrClick;
    }
};

template<class TYPE>
struct Pred<TYPE, SORT_FINDPOS>
{
    bool operator()(const TYPE* p1, const TYPE* p2)
    {
        //按搜索位置升序来排
        if (p1->nFindPos != p2->nFindPos)
        {
            return p1->nFindPos < p2->nFindPos;
        }

        int nLen1 = p1->Name.GetLength();
        int nLen2 = p2->Name.GetLength();
        int nLen = nLen1 > nLen2 ? nLen1 : nLen2;
        return StrNCmpI(p1->Name, p2->Name, nLen) < 0;
    }
};

template <class TYPE, int N>
void SortGameList(std::vector<const TYPE*>& List)
{
    std::sort(List.begin(), List.end(), Pred<TYPE, N>());

    //方便调试时查看排列是否正确
    // 	for (size_t idx=0; idx<GameList.size(); idx++)
    // 	{
    // 		TCHAR szLog[MAX_PATH] = {0};
    // 		_stprintf_s(szLog, TEXT("%s-%d-%d\r\n"), (LPCTSTR)GameList[idx]->Name, 
    // 			GameList[idx]->dwSvrClick, GameList[idx]->dwI8Play);
    // 		OutputDebugString(szLog);
    // 	}
}
