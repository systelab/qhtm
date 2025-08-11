/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListboxSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"

#include "QHTM.h"
#include "HTMLListboxSection.h"
#include "HTMLSectionCreator.h"
#include "HTMLFormInput.h"
#include <WindowText.h>

extern void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus );

CHTMLListboxSection::CHTMLListboxSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLSelect *pFormObject, class CHTMLSectionCreator *psc )
	: CHTMLSectionABC( pSectParent )
	, m_fdef( fdef )
	, m_pFormObject( pFormObject )
{
	SetKeep( true );
	if( !psc->IsMeasuring() )
	{
		DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTIPLESEL;

		HWND hwndParent = pSectParent->GetHwnd();
		_ASSERT( hwndParent );
		m_hwnd = CreateWindowEx( WS_EX_STATICEDGE, _T("LISTBOX"), NULL,  dwStyle, 0, 0, 0, 0, hwndParent, NULL, g_hQHTMInstance, NULL );
		VAPI( m_hwnd );

		for( UINT u = 0; u < pFormObject->m_arrItems.GetSize(); u++ )
		{
			CHTMLOption *pOption = pFormObject->m_arrItems[ u ];
			int nItem = SendMessage( m_hwnd, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)pOption->m_strText );
			if( pOption->m_bSelected )
			{
				SendMessage( m_hwnd, LB_SETSEL, TRUE, nItem );
			}
		}

		if( pFormObject->m_bDisabled )
		{
			EnableWindow( m_hwnd, FALSE );
		}

		SubclassControl( m_hwnd, pSectParent, this );
	}
}


CHTMLListboxSection::~CHTMLListboxSection()
{
	DestroyWindow( m_hwnd );
}


void CHTMLListboxSection::OnDraw( GS::CDrawContext &dc )
{
	if( dc.IsPrinting() )
	{
		dc.FillRect( *this, RGB( 255, 0, 0 ) );
	}
}


void CHTMLListboxSection::FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline )
{
	GS::CDrawContext &dc = psc->GetDC();
	dc.SelectFont( m_fdef );

	SendMessage( m_hwnd, WM_SETFONT, (WPARAM)psc->GetDC().GetCurrentHFONT(), FALSE );

	//
	//	Measure the list
	UINT uWidth = 0;
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		UINT uItemWidth = dc.GetTextExtent( pOption->m_strText, pOption->m_strText.GetLength() );
		if( uItemWidth > uWidth )
		{
			uWidth = uItemWidth;
		}
	}
	
	size.cx = uWidth + 12;
	size.cx += GetSystemMetrics( SM_CXVSCROLL );
	size.cy = dc.GetCurrentFontHeight() * + m_pFormObject->m_uSize + 4;
	nBaseline = size.cy - 4;
}


void CHTMLListboxSection::OnLayout( const WinHelper::CRect &rc )
{
	SetWindowPos( m_hwnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
}


void CHTMLListboxSection::MoveY( int nOffsetY )
{
	CHTMLSectionABC::MoveY( nOffsetY );
	OnLayout( *this );
}


void CHTMLListboxSection::MoveXY( int nOffsetX, int nOffsetY )
{
	CHTMLSectionABC::MoveXY( nOffsetX, nOffsetY );
	OnLayout( *this );
}


void CHTMLListboxSection::SetFocus( bool bHasFocus )
{
	CHTMLSectionABC::SetFocus( bHasFocus );

	if( bHasFocus )
	{
		::SetFocus( m_hwnd );
	}
}


bool CHTMLListboxSection::IsFocused() const
{
	return ::GetFocus() == m_hwnd;
}


void CHTMLListboxSection::ResetContent()
{
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		if( pOption->m_bSelected )
		{
			SendMessage( m_hwnd, LB_SETSEL, TRUE, u );
		}
		else
		{
			SendMessage( m_hwnd, LB_SETSEL, FALSE, u );
		}
	}
}


void CHTMLListboxSection::UpdateFormFromControls()
{
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		if( SendMessage( m_hwnd, LB_GETSEL, u, 0 ) )
		{
			pOption->m_bSelected = true;
		}
		else
		{
			pOption->m_bSelected = false;
		}
	}
}