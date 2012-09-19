#ifndef __IMAGE_HELPER_H
#define __IMAGE_HELPER_H


#include <shlobj.h>
#include "GdiplusHelper.h"


ATLINLINE HBITMAP AtlLoadGdiplusImage(LPCTSTR lpszPath)
{
	std::auto_ptr<Bitmap> pBitmap;
	
	// Load from filename
	pBitmap.reset(new Bitmap(CT2W(lpszPath))); 


	if( pBitmap.get() == NULL ) 
		return NULL;

	HBITMAP hBitmap = NULL;
	pBitmap->GetHBITMAP(NULL, &hBitmap); 

	return hBitmap;
}




#endif 

