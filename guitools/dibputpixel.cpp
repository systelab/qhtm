/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBPutPixel.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


void CDIB::PutPixel( UINT x, UINT y, BYTE r, BYTE g, BYTE b, BYTE a )
{
	const CLineArray & arrDest = GetLineArray();
	if( y < arrDest.GetSize() && y >= 0 )
	{
		DIB_PIXEL *pDest = arrDest[ y ];
		if( x >= 0 && x < (UINT)GetWidth() )
		{
			DIB_PIXEL *pPixel = pDest + x;
			pPixel->rgbRed = r;
      pPixel->rgbGreen = g;
      pPixel->rgbBlue = b;
			pPixel->rgbReserved = a;
		}
	}
}



