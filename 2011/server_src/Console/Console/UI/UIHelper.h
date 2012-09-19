#ifndef __UI_HELPER_HPP
#define __UI_HELPER_HPP


namespace i8desk
{
	namespace ui
	{
	
		template < typename ImageT, size_t N, size_t M >
		void DrawWorkFrame(CMemDC &memDC, const CRect &rcClient, const ImageT (&leftArea)[N], const ImageT (&rightArea)[M])
		{
			CRect rcLeft = rcClient;
			rcLeft.right = rcLeft.left + leftArea[1]->GetWidth() + 5;
			rcLeft.DeflateRect(3, 7, 0, 10);

			CRect rcLeftImge(0, 0, leftArea[1]->GetWidth(), leftArea[1]->GetHeight());
			leftArea[1]->Draw(memDC.GetDC().GetSafeHdc(), rcLeft, rcLeftImge);

			// 右边工作区
			CRect rcRight = rcClient;
			rcRight.left = rcLeft.right;
			rcRight.DeflateRect(3, 7, 4, 5);
			CRect rcRightContent(0, 0, rightArea[2]->GetWidth(), rightArea[2]->GetHeight());
			rightArea[2]->Draw(memDC.GetDC().GetSafeHdc(), rcRight, rcRightContent);

			// 工作区左边边框
			CRect rcRightLeftLine(rcRight.left, rcRight.top, rcRight.left + rightArea[0]->GetWidth(), rcRight.bottom);
			rcRightLeftLine.DeflateRect(0, 0, 0, 0);
			CRect rcRightLeftImg(0, 0, rightArea[0]->GetWidth(), rightArea[0]->GetHeight());
			rightArea[0]->Draw(memDC.GetDC().GetSafeHdc(), rcRightLeftLine, rcRightLeftImg);

			// 工作区下边边框
			CRect rcRightBottomLine(rcRight.left, rcRight.bottom, rcRight.right, rcRight.bottom + rightArea[1]->GetHeight());
			rcRightBottomLine.DeflateRect(1, 0, -1, 0);
			CRect rcRightBottomImg(0, 0, rightArea[1]->GetWidth(), rightArea[1]->GetHeight());
			rightArea[1]->Draw(memDC.GetDC().GetSafeHdc(), rcRightBottomLine, rcRightBottomImg);

			// 工作区上边边框
			CRect rcRightTopLine(rcRight.left, rcRight.top, rcRight.right, rcRight.top + rightArea[3]->GetHeight());
			rcRightTopLine.DeflateRect(1, 0, 0, 0);
			CRect rcRightTopImg(0, 0, rightArea[3]->GetWidth(), rightArea[3]->GetHeight());
			rightArea[3]->Draw(memDC.GetDC().GetSafeHdc(), rcRightTopLine, rcRightTopImg);

			// 工作区右边边框
			CRect rcRightRightLine(rcRight.right, rcRight.top, rcRight.right + rightArea[4]->GetWidth(), rcRight.bottom);
			rcRightRightLine.DeflateRect(0, 1, 0, 0);
			CRect rcRightRightImg(0, 0, rightArea[4]->GetWidth(), rightArea[4]->GetHeight());
			rightArea[4]->Draw(memDC.GetDC().GetSafeHdc(), rcRightRightLine, rcRightRightImg);
		}

		template < typename ImageT, size_t N >
		void DrawWorkFrame(CMemDC &memDC, const CRect &rcClient, const ImageT (&rightArea)[N])
		{
			// 右边工作区
			CRect rcRight = rcClient;
			rcRight.DeflateRect(3, 7, 4, 5);
			CRect rcRightContent(0, 0, rightArea[2]->GetWidth(), rightArea[2]->GetHeight());
			rightArea[2]->Draw(memDC.GetDC().GetSafeHdc(), rcRight, rcRightContent);

			// 工作区左边边框
			CRect rcRightLeftLine(rcRight.left, rcRight.top, rcRight.left + rightArea[0]->GetWidth(), rcRight.bottom);
			rcRightLeftLine.DeflateRect(0, 0, 0, 0);
			CRect rcRightLeftImg(0, 0, rightArea[0]->GetWidth(), rightArea[0]->GetHeight());
			rightArea[0]->Draw(memDC.GetDC().GetSafeHdc(), rcRightLeftLine, rcRightLeftImg);

			// 工作区下边边框
			CRect rcRightBottomLine(rcRight.left, rcRight.bottom, rcRight.right, rcRight.bottom + rightArea[1]->GetHeight());
			rcRightBottomLine.DeflateRect(1, 0, -1, 0);
			CRect rcRightBottomImg(0, 0, rightArea[1]->GetWidth(), rightArea[1]->GetHeight());
			rightArea[1]->Draw(memDC.GetDC().GetSafeHdc(), rcRightBottomLine, rcRightBottomImg);

			// 工作区上边边框
			CRect rcRightTopLine(rcRight.left, rcRight.top, rcRight.right, rcRight.top + rightArea[3]->GetHeight());
			rcRightTopLine.DeflateRect(1, 0, 0, 0);
			CRect rcRightTopImg(0, 0, rightArea[3]->GetWidth(), rightArea[3]->GetHeight());
			rightArea[3]->Draw(memDC.GetDC().GetSafeHdc(), rcRightTopLine, rcRightTopImg);

			// 工作区右边边框
			CRect rcRightRightLine(rcRight.right, rcRight.top, rcRight.right + rightArea[4]->GetWidth(), rcRight.bottom);
			rcRightRightLine.DeflateRect(0, 1, 0, 0);
			CRect rcRightRightImg(0, 0, rightArea[4]->GetWidth(), rightArea[4]->GetHeight());
			rightArea[4]->Draw(memDC.GetDC().GetSafeHdc(), rcRightRightLine, rcRightRightImg);
		}



		template < typename ImageT >
		void DrawFrame(CWnd *wnd, CMemDC &memDC, const CRect &destRect, const ImageT &image)
		{
			CRect rcWork = destRect;
			
			rcWork.DeflateRect(-1, -1, -1, -1);
			wnd->ScreenToClient(rcWork);

			CRect rcOutLine(0, 0, image->GetWidth(), image->GetHeight());
			CRect rcWorkLeft = rcWork;
			rcWorkLeft.right = rcWork.left + 1;
			image->Draw(memDC.GetDC().GetSafeHdc(), rcWorkLeft, rcOutLine);

			CRect rcWorkTop = rcWork;
			rcWorkTop.bottom = rcWork.top + 1;
			image->Draw(memDC.GetDC().GetSafeHdc(), rcWorkTop, rcOutLine);

			CRect rcWorkRight = rcWork;
			rcWorkRight.left = rcWork.right - 1;
			image->Draw(memDC.GetDC().GetSafeHdc(), rcWorkRight, rcOutLine);

			CRect rcWorkBottom = rcWork;
			rcWorkBottom.top = rcWork.bottom - 1;
			image->Draw(memDC.GetDC().GetSafeHdc(), rcWorkBottom, rcOutLine);
		}

		template < typename ImageT, typename DCT >
		void DrawFrame(CWnd *wnd, DCT &dc, const CRect &destRect, const ImageT &image)
		{
			CRect rcWork = destRect;

			rcWork.DeflateRect(-1, -1, -1, -1);
			wnd->ScreenToClient(rcWork);

			CRect rcOutLine(0, 0, image->GetWidth(), image->GetHeight());
			CRect rcWorkLeft = rcWork;
			rcWorkLeft.right = rcWork.left + 1;
			image->Draw(dc, rcWorkLeft, rcOutLine);

			CRect rcWorkTop = rcWork;
			rcWorkTop.bottom = rcWork.top + 1;
			image->Draw(dc, rcWorkTop, rcOutLine);

			CRect rcWorkRight = rcWork;
			rcWorkRight.left = rcWork.right - 1;
			image->Draw(dc, rcWorkRight, rcOutLine);

			CRect rcWorkBottom = rcWork;
			rcWorkBottom.top = rcWork.bottom - 1;
			image->Draw(dc, rcWorkBottom, rcOutLine);
		}
	}
}




#endif