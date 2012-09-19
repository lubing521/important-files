#pragma once

#include "I8Bitmap.h"

namespace I8SkinCtrl_ns
{
    class CI8SkinBitmap : public CI8Bitmap
    {
    public:
        CI8SkinBitmap(void);
        ~CI8SkinBitmap(void);
        CI8SkinBitmap(const CI8SkinBitmap& clBitmap);
        CI8SkinBitmap& operator=(const CI8SkinBitmap& clBitmap);
    };
}
