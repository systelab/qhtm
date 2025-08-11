/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SubClassControl.cpp
Owner:	russf@gipsysoft.com
Purpose:

	Subclassing for FORM controls.

	Takes care of a few chortes including:
		* Handling TAB so we can tab around QHTM and then eventually fall out
			of the bottom of the control

		* Handling ESC to prevent window from closing and to then reset the current form

----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLSection.h"


static LPCTSTR g_szOldProc = _T("QHTMgszOldProc");
static LPCTSTR g_szSection = _T("QHTMgszSection");
static LPCTSTR g_szFocus = _T("QHTMgszFocus");

static LRESULT CALLBACK SubClassedProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	WNDPROC	WndProc = reinterpret_cast< WNDPROC>( ::GetProp( hwnd, g_szOldProc ) );
	_ASSERT( WndProc );

	switch( message )
	{
		case WM_GETDLGCODE:
		{
			CHTMLSection *psectHTML = reinterpret_cast<CHTMLSection *>( ::GetProp( hwnd, g_szSection ) );
			_ASSERT( psectHTML );
			{
				LRESULT lr = CallWindowProc( WndProc, hwnd, message, wParam, lParam );

				LPMSG lpMsg = (LPMSG)lParam;
				if( lpMsg && lpMsg->message == WM_KEYDOWN )
				{
					if( lpMsg->wParam == VK_TAB )
					{
						if( WinHelper::IsShiftPressed() )
						{
							if( psectHTML->CanTabBackward() )
							{
								lr |= DLGC_WANTTAB;
							}
						}
						else
						{
							if( psectHTML->CanTabForward() )
							{
								lr |= DLGC_WANTTAB;
							}
						}
					}
				}
				else
				{
					lr |= DLGC_WANTTAB;
				}

				return lr;
			}
		}
		break;

		case WM_KEYDOWN:
			switch( wParam )
			{
			case VK_TAB:
				if( !WinHelper::IsAltPressed() && !WinHelper::IsControlPressed() )
				{
					SendMessage( GetParent( hwnd ), message, wParam, lParam );
					return 0;
				}
				break;

			case VK_RETURN:
				//	TODO - add event to submit current form
				return 0;

			case VK_ESCAPE:
				//	TODO - add event to clear the current form
				return 0;
			}
			break;

		case WM_DESTROY:
		{
			SetWindowLong( hwnd, GWL_WNDPROC, (LONG) WndProc ) ;
			RemoveProp( hwnd, g_szOldProc );
			RemoveProp( hwnd, g_szSection );
			RemoveProp( hwnd, g_szFocus );			
		}
		break;

		case WM_SETFOCUS:
		{
			CFocusControlABC *pFocus = reinterpret_cast< CFocusControlABC * >( ::GetProp( hwnd, g_szFocus ) );
			CHTMLSection *psectHTML = reinterpret_cast< CHTMLSection * >( ::GetProp( hwnd, g_szSection ) );
			_ASSERT( pFocus );
			_ASSERT( psectHTML );
			psectHTML->SetCurrentFocus( pFocus, true );
		}
		break;
			
	}

	return CallWindowProc( WndProc, hwnd, message, wParam, lParam );
}


void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus )
{
	ASSERT_VALID_HWND( hwnd );
	_ASSERT( psectHTML );
	_ASSERT( pFocus );

	FARPROC lpfnWndProc = reinterpret_cast<FARPROC>( SetWindowLong( hwnd, GWL_WNDPROC, (LONG) SubClassedProc ) );
	_ASSERT( lpfnWndProc != NULL );

	VERIFY( ::SetProp(hwnd, g_szOldProc, reinterpret_cast<HANDLE>( lpfnWndProc ) ) );
	VERIFY( ::SetProp(hwnd, g_szSection, reinterpret_cast<HANDLE>( psectHTML ) ) ) ;
	VERIFY( ::SetProp(hwnd, g_szFocus, reinterpret_cast<HANDLE>( pFocus ) ) );
}

