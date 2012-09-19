#include "stdafx.h"
#include "I8SkinCtrl.h"
#include <GdiPlus.h>

using namespace I8SkinCtrl_ns;

static ULONG_PTR gs_gdiplusToken = 0;

void I8SkinCtrl_ns::I8_InitSkinCtrl(LPCTSTR sSkinFile)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gs_gdiplusToken, &gdiplusStartupInput, NULL);

    CI8WndShadow::Initialize(AfxGetInstanceHandle());
    CI8SkinPack::Instance().LoadSkinInfo(sSkinFile);
}

void I8SkinCtrl_ns::I8_ReleaseSkinCtrl()
{
    CI8SkinPack::Instance().Release();

    Gdiplus::GdiplusShutdown(gs_gdiplusToken);
}