#ifndef __DRAW_HELPER_HPP
#define __DRAW_HELPER_HPP

#include "../../../include/MultiThread/Thread.hpp"
#include "../../../include/Container/BoundedBlockQueue.hpp"


namespace ui
{
	class draw_helper
	{
		CWnd *parent_;
		typedef async::container::BoundedBlockingQueue<std::list> Queue;
		Queue queueI_;
		Queue queueJ_;
		async::thread::ThreadImplEx threadI_;
		async::thread::ThreadImplEx threadJ_;

	public:
		draw_helper(CWnd *parent)
			: parent_(parent)
		{}

		void start()
		{
			threadI_.RegisterFunc(std::tr1::bind(&draw_helper::_thread_i, this));
			threadI_.Start();

			threadI_.RegisterFunc(std::tr1::bind(&draw_helper::_thread_j, this));
			threadJ_.Start();
		}

		void stop()
		{
			queueI_.Put(utility::BITMAPPtr());
			queueI_.Put(utility::BITMAPPtr());
			threadI_.Stop();
			threadJ_.Stop();
		}

		void put_i(const common::Buffer &buf)
		{
			utility::BITMAPPtr bmp = ui::draw::AtlLoadGdiplusImage(buf.first.get(), buf.second);
			if( bmp == 0 )
				return;

			if( queueI_.Size() > 500 )
				return;

			queueI_.Put(bmp);
		}

		void put_j(const common::Buffer &buf)
		{
			utility::BITMAPPtr bmp = ui::draw::AtlLoadGdiplusImage(buf.first.get(), buf.second);
			if( bmp == 0 )
				return;

			if( queueJ_.Size() > 500 )
				return;

			queueJ_.Put(bmp);
		}

		void DrawImage(const utility::BITMAPPtr &bmp, CMemDC &memDC, size_t offset)
		{
			ATL::CImage img;
			img.Attach(bmp);

			if( !img.IsNull() )
			{
				CRect rcImg(0, 0, img.GetWidth(), img.GetHeight());
				CRect rcDest(CPoint(offset, 231),
					CSize(img.GetWidth(), img.GetHeight()));
				img.BitBlt(memDC.GetDC(), rcDest, CPoint(0, 0));
			}
		}

	private:
		DWORD _thread_i()
		{
			while(!threadI_.IsAborted())
			{
				utility::BITMAPPtr bmp = queueI_.Get();
				if( bmp.get() == 0 )
					break;
				
				CRect rcClient;
				GetClientRect(rcClient);

				CDC *dc = parent_->GetDC();
				CMemDC memDC(*dc, rcClient);

				size_t offsetLeft = (rcClient.Width() - 1280) / 2;

				DrawImage(bmp, memDC, offsetLeft + 62);

				parent_->ReleaseDC();
			}

			return 0;
		}

		DWORD _thread_j()
		{
			while(!threadJ_.IsAborted())
			{
				utility::BITMAPPtr bmp = queueJ_.Get();
				if( bmp.get() == 0 )
					break;

				CRect rcClient;
				GetClientRect(rcClient);

				CDC *dc = parent_->GetDC();
				CMemDC memDC(*dc, rcClient);

				size_t offsetLeft = (rcClient.Width() - 1280) / 2;

				DrawImage(bmp, memDC, offsetLeft + 83 + 600);

				parent_->ReleaseDC();
			}

			return 0;
		}
	};
}




#endif