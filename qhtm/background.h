/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Background.h
Owner:	russf@gipsysoft.com
Purpose:

		The drawn background of various objects

----------------------------------------------------------------------*/
#ifndef BACKGROUND_H
#define BACKGROUND_H

#ifndef COLOR_H
	#include "Color.h"
#endif	//	COLOR_H

#ifndef GUITOOLS_H
	#include <guitools/guitools.h>
#endif	//	GUITOOLS_H

class CQHTMImageABC;

extern void DrawTiledBitmap( int x, int y, int cx, int cy, HDC hdc, CQHTMImageABC *pImage, UINT uFrame );

class CBackground
{
public:
	CBackground()
			: m_pimgBackground( NULL )
		{
		}

	bool IsSet() const
	{
		return m_pimgBackground || m_crBack.IsSet() || ( m_crLeft.IsSet() && m_crRight.IsSet() ) || ( m_crTop.IsSet() && m_crBottom.IsSet() );
	}

	CColor m_crBack;
	CColor m_crLeft;
	CColor m_crRight;
	CColor m_crTop;
	CColor m_crBottom;

	CQHTMImageABC *m_pimgBackground;

	bool Draw( GS::CDrawContext &dc, const WinHelper::CRect &rc )
	{
		bool bRetVal = false;
		if( m_pimgBackground )
		{
			//	TODO - add support for animated backgrounds
			dc.SetClipRect( rc );
			DrawTiledBitmap( rc.left, rc.top, rc.Width(), rc.Height(), dc.GetSafeHdc(), m_pimgBackground, 0 );
			dc.RemoveClip();
			bRetVal = true;
		}
		else if( m_crLeft.IsSet() && m_crRight.IsSet() )
		{
			GS::GradientFillLeftToRight( dc, rc, m_crLeft, m_crRight );
			bRetVal = true;
		}
		else if( m_crTop.IsSet() && m_crBottom.IsSet() )
		{
			GS::GradientFillTopToBottom( dc, rc, m_crTop, m_crBottom );
			bRetVal = true;
		}
		else if( m_crBack.IsSet() )
		{
			dc.FillRect( rc, m_crBack );
			bRetVal = true;
		}
		return bRetVal;
	}

};

#endif //BACKGROUND_H