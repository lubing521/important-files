#include "StdAfx.h"
#include "I8SkinBitmap.h"

using namespace I8SkinCtrl_ns;

CI8SkinBitmap::CI8SkinBitmap(void)
{
}

CI8SkinBitmap::CI8SkinBitmap(const CI8SkinBitmap& clBitmap)
{
    __super::operator =(clBitmap);
}

CI8SkinBitmap::~CI8SkinBitmap(void)
{
}

CI8SkinBitmap& CI8SkinBitmap::operator=(const CI8SkinBitmap& clBitmap)
{
    __super::operator =(clBitmap);
    return *this;
}