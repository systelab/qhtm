/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiBaseObject.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "GuiBaseObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace GS;

CGuiBaseObject::CGuiBaseObject( CGuiBaseObject *pgboParent )
	: m_pgboParent( pgboParent )
	, m_bVisible( true )
	, m_bMouseDown( false )
{
	if( m_pgboParent )
	{
		m_pgboParent->AddChild( this );
	}
}


CGuiBaseObject::~CGuiBaseObject()
{
	if( m_pgboParent )
	{
		m_pgboParent->RemoveChild( this );
	}
}


void CGuiBaseObject::OnDraw( GS::CDrawContext &dc )
{
	if( IsVisible() )
	{
		const WinHelper::CRect &rcClip = dc.GetClipRect();
		const PI_uint32 uSize = m_arrChild.GetSize();

		for( PI_uint32 n = 0; n < uSize; n++ )
		{
			CGuiBaseObject *psect = m_arrChild[ n ];
			if( rcClip.Intersect( *psect ) )
			{
				psect->OnDraw( dc );
			}
		}
	}
}


void CGuiBaseObject::OnLayout( GS::CDrawContext & dc )
{
	UNREF( dc );
}


void CGuiBaseObject::AddChild( CGuiBaseObject *pgboChild )
{
	m_arrChild.Add( pgboChild );
}


void CGuiBaseObject::RemoveChild( CGuiBaseObject *pgboChild )
{
	const PI_uint32 uSize = m_arrChild.GetSize();
	for( PI_uint32 n = 0; n < uSize; n++ )
	{
		if( pgboChild == m_arrChild[ n ] )
		{
			m_arrChild.RemoveAt( n, 1 );
			return;
		}
	}

	//
	//	Couldn't find a child that wanted to be removed
	_ASSERT( false );
}


CGuiBaseObject * CGuiBaseObject::FindObjectFromPoint(const WinHelper::CPoint &pt, bool bIncludeThis ) const
//
//	Given a point return pointer to a section if the pointer is within a section
//	Can return NULL if there is no section at the point.
{
	const PI_uint32 uSize = m_arrChild.GetSize();
	for( PI_uint32 n = 0; n < uSize; n++ )
	{
		CGuiBaseObject *psect = m_arrChild[ n ];
		if( psect->IsPointInSection( pt ) && psect->IsVisible() )
		{
			CGuiBaseObject *pSect2 = psect->FindObjectFromPoint( pt, false );
			return pSect2 ? pSect2 : psect;
		}
	}

	if( bIncludeThis )
	{
		return IsPointInSection( pt ) ? const_cast<CGuiBaseObject *>(this) : NULL;
	}
	else
	{
		return NULL;
	}
}


void CGuiBaseObject::SetCursor()
{
}


PI_uint32 CGuiBaseObject::StartTimer( CGuiBaseObject *pgbo, PI_uint32 uDuration )
{
	if( m_pgboParent )
	{
		return m_pgboParent->StartTimer( pgbo, uDuration );
	}
	return 0;
}


void CGuiBaseObject::StopTimer( PI_uint32 uTimerID )
{
	if( m_pgboParent )
	{
		m_pgboParent->StopTimer( uTimerID );
	}
}


void CGuiBaseObject::OnTimer( PI_uint32 uTimerID )
{
	UNREF( uTimerID );
}


void CGuiBaseObject::OnMouseLeave()
{
}

void CGuiBaseObject::OnMouseEnter()
{
}


HWND CGuiBaseObject::GetSafeHwnd() const
{
	if( m_pgboParent )
	{
		return m_pgboParent->GetSafeHwnd();
	}

	//	Either we have a parent or we do it ourselves
	_ASSERT( FALSE );
	return NULL;
}


void CGuiBaseObject::ForceRedraw( const WinHelper::CRect &rc )
{
	if( m_pgboParent )
	{
		m_pgboParent->ForceRedraw( rc );
	}
	else
	{
		//	Either we have a parent or we handle the drawing ourselves
		_ASSERT( FALSE );
	}
}


void CGuiBaseObject::OnMouseLeftDown( const WinHelper::CPoint &pt )
{
	UNREF( pt );
	m_bMouseDown = true;
}

void CGuiBaseObject::OnMouseLeftUp( const WinHelper::CPoint & )
{
	m_bMouseDown = false;
}


void CGuiBaseObject::OnMouseLeftDoubleclick( const WinHelper::CPoint & )
{
}

void CGuiBaseObject::OnMouseCancelLeftMouse()
{
	m_bMouseDown = false;
}


void CGuiBaseObject::OnMouseWheel( PI_int32 nDelta )
{
	//
	//	Our default behaviour is to call out parent. That way the message
	//	is handled at the highest level but starts at the lowest level.
	if( m_pgboParent )
	{
		m_pgboParent->OnMouseWheel( nDelta );
	}
}