#include "StdAfx.h"
#include "I8BGImage.h"
#include "I8SkinPack.h"

using namespace stdex;
using namespace I8SkinCtrl_ns;

CI8BGImage::CI8BGImage(void)
    : m_iTopLeft(0)
    , m_iTopRight(0)
    , m_iTop(0)
    , m_iLeft(0)
    , m_iRight(0)
    , m_iBottomLeft(0)
    , m_iBottomRight(0)
    , m_iBottom(0)
{
}

void CI8BGImage::Load(const tString& sName, BOOL bCheckExist)
{
    Load(sName + _T(".png"), sName, _T("拆分方式"), bCheckExist);
}

void I8SkinCtrl_ns::CI8BGImage::Load(const stdex::tString& sImage, const stdex::tString& sOption, const stdex::tString& sOptionName, BOOL bCheckExist)
{
    CI8SkinPack::Instance().LoadSkin(m_clImage, sImage, bCheckExist);
    if (!bCheckExist && m_clImage.IsNull())
    {
        return;
    }
    m_iTopLeft = CI8SkinPack::Instance().GetOption(sOptionName, _T("左上"), 0, sOption);
    m_iTopRight = CI8SkinPack::Instance().GetOption(sOptionName, _T("右上"), 0, sOption);
    m_iTop = CI8SkinPack::Instance().GetOption(sOptionName, _T("上"), 0, sOption);
    m_iLeft = CI8SkinPack::Instance().GetOption(sOptionName, _T("左"), 0, sOption);
    m_iRight = CI8SkinPack::Instance().GetOption(sOptionName, _T("右"), 0, sOption);
    m_iBottomLeft = CI8SkinPack::Instance().GetOption(sOptionName, _T("左下"), 0, sOption);
    m_iBottomRight = CI8SkinPack::Instance().GetOption(sOptionName, _T("右下"), 0, sOption);
    m_iBottom = CI8SkinPack::Instance().GetOption(sOptionName, _T("下"), 0, sOption);
    assert(m_iTopLeft >= 0 && m_iTopRight >= 0 && m_iTop >= 0 && m_iLeft >= 0 && m_iRight >= 0 && m_iBottomLeft >= 0 && m_iBottomRight >= 0 && m_iBottom >= 0);
    CSize clMidSize = GetMidSize();
    if (clMidSize.cx < 0 || clMidSize.cy < 0)
    {
        assert(0 && "图片拆分方式参数错误");
    }
}
void CI8BGImage::Draw(HDC hDC, const CRect& rtRect) const
{
    CSize clImageSize = m_clImage.GetSize();
    if (rtRect.Size() == clImageSize)
    {
        m_clImage.Draw(hDC, rtRect);
        return;
    }
    //左上角
    m_clImage.AlphaBlend(hDC, rtRect.TopLeft(), CRect(0, 0, m_iTopLeft, m_iTop));
    //右上角
    m_clImage.AlphaBlend(hDC, CPoint(rtRect.right - m_iTopRight, rtRect.top), CRect(clImageSize.cx - m_iTopRight, 0, clImageSize.cx, m_iTop));
    //顶部
    m_clImage.AlphaBlend(hDC, CRect(rtRect.left + m_iTopLeft, rtRect.top, rtRect.right - m_iTopRight, rtRect.top + m_iTop), CRect(m_iTopLeft, 0, clImageSize.cx - m_iTopRight, m_iTop));
    //左下角
    m_clImage.AlphaBlend(hDC, CPoint(rtRect.left, rtRect.bottom - m_iBottom), CRect(0, clImageSize.cy - m_iBottom, m_iBottomLeft, clImageSize.cy));
    //右下角
    m_clImage.AlphaBlend(hDC, CPoint(rtRect.right - m_iBottomRight, rtRect.bottom - m_iBottom), CRect(clImageSize.cx - m_iBottomRight, clImageSize.cy - m_iBottom, clImageSize.cx, clImageSize.cy));
    //底部
    m_clImage.AlphaBlend(hDC, CRect(rtRect.left + m_iBottomLeft, rtRect.bottom - m_iBottom, rtRect.right - m_iBottomRight, rtRect.bottom), CRect(m_iBottomLeft, clImageSize.cy - m_iBottom, clImageSize.cx - m_iBottomRight, clImageSize.cy));
    //左边缘
    m_clImage.AlphaBlend(hDC, CRect(rtRect.left, rtRect.top + m_iTop, rtRect.left + m_iLeft, rtRect.bottom - m_iBottom), CRect(0, m_iTop, m_iLeft, clImageSize.cy - m_iBottom));
    //右边缘
    m_clImage.AlphaBlend(hDC, CRect(rtRect.right - m_iRight, rtRect.top + m_iTop, rtRect.right, rtRect.bottom - m_iBottom), CRect(clImageSize.cx - m_iRight, m_iTop, clImageSize.cx, clImageSize.cy - m_iBottom));
    //内部
    m_clImage.AlphaBlend(hDC, CRect(rtRect.left + m_iLeft, rtRect.top + m_iTop, rtRect.right - m_iRight, rtRect.bottom - m_iBottom), CRect(m_iLeft, m_iTop, clImageSize.cx - m_iRight, clImageSize.cy - m_iBottom));
}

CRect CI8BGImage::GetSpaceArea(const CRect& rtRect) const
{
    return CRect(rtRect.left + m_iLeft, rtRect.top + m_iTop, rtRect.right - m_iRight, rtRect.bottom - m_iBottom);
}

int CI8BGImage::GetLeftSpace() const
{
    return m_iLeft;
}

int CI8BGImage::GetTopSpace() const
{
    return m_iTop;
}

int CI8BGImage::GetRightSpace() const
{
    return m_iRight;
}

int CI8BGImage::GetBottomSpace() const
{
    return m_iBottom;
}

CSize I8SkinCtrl_ns::CI8BGImage::GetMidSize() const
{
    return m_clImage.GetSize() - CSize(m_iLeft + m_iRight, m_iTop + m_iBottom);
}

CSize CI8BGImage::GetSize() const
{
    return m_clImage.GetSize();
}
BOOL CI8BGImage::IsNull() const
{
    return m_clImage.IsNull();
}