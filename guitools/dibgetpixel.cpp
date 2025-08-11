/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBGetPixel.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


bool CDIB::GetPixel( UINT x, UINT y, BYTE &r, BYTE &g, BYTE &b, BYTE &a )
{
	const CLineArray & arrDest = GetLineArray();
	if( y < arrDest.GetSize() && y >= 0 )
	{
		DIB_PIXEL *pDest = arrDest[ y ];
		if( x >= 0 && x < (UINT)GetWidth() )
		{
			const DIB_PIXEL *pPixel = pDest + x;
			r = pPixel->rgbRed;
			g = pPixel->rgbGreen;
			b = pPixel->rgbBlue;
			a = pPixel->rgbReserved;
			return true;
		}
	}
	return false;
}

