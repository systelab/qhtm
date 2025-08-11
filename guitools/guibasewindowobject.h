/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiBaseWindowObject.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GUIBASEWINDOWOBJECT_H
#define GUIBASEWINDOWOBJECT_H

#ifndef GUIBASEOBJECT_H
	#include <guitools/GuiBaseObject.h>
#endif	//	GUIBASEOBJECT_H

#ifndef MAP_CONTAINER_H
	#include <map.h>
#endif	//	MAP_CONTAINER_H

namespace GS
{
	#define WINDOW_DATA	0
	enum {knDeadWindow = 1};

	//
	//
	//	Declare your intention to use this as a window
	#define DECLARE_WINDOW_TYPE	\
		static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );	\
		static bool Create( WinHelper::CRect &rc, HWND hwndParent, DWORD dwStyle, HMENU hmenu, HINSTANCE hInstance )	\


	//
	//	Define the object as a window
	#define DEFINE_WINDOW_TYPE( T, pcszClass )	\
		LRESULT CALLBACK T::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )\
		{\
			LPARAM lparam = GetWindowLong( hwnd, WINDOW_DATA );\
			\
			if( lparam == GS::knDeadWindow )\
			{\
				_ASSERT( false );\
				return ::DefWindowProc( hwnd, message, wParam, lParam );\
			}\
			\
			T *pWnd = reinterpret_cast<T*>( lparam );\
			switch( message )\
			{\
			case WM_NCCREATE:\
				if( pWnd == NULL )\
				{\
					LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );\
					pWnd = new T( hwnd, lpcs->style );\
					::SetWindowLong( hwnd, WINDOW_DATA, reinterpret_cast<long>( pWnd ) );\
				}\
			}\
			_ASSERT( pWnd );\
			LRESULT lr = pWnd->OnWindowsMessage( message, wParam, lParam );\
			switch( message )\
			{\
			case WM_NCDESTROY:\
				::SetWindowLong( hwnd, WINDOW_DATA, GS::knDeadWindow );\
				delete pWnd;\
				break;\
			}\
			return lr;\
		}\
		\
		bool T::Create( WinHelper::CRect &rc, HWND hwndParent, DWORD dwStyle, HMENU hmenu, HINSTANCE hInstance )	\
		{	\
			if( hInstance )	\
			{	\
				WNDCLASSEX wcex = {0};	\
				wcex.cbSize = sizeof( WNDCLASSEX );	\
				wcex.style					= CS_DBLCLKS | CS_PARENTDC;	\
				wcex.lpfnWndProc		= (WNDPROC)T::WndProc;	\
				wcex.cbClsExtra			= 0;	\
				wcex.cbWndExtra			= sizeof( T * );	\
				wcex.hInstance			= hInstance;	\
				wcex.hbrBackground	= NULL;	\
				wcex.lpszMenuName		= NULL;	\
				wcex.lpszClassName	= pcszClass;	\
				if( !RegisterClassEx( &wcex ) )	\
				{	\
					TRACE( _T("Failed to register class: %s\n"), pcszClass );\
					return false;	\
				}	\
			}\
			if( ::CreateWindow( pcszClass, NULL, dwStyle, rc.left, rc.top, rc.Width(), rc.Height(), hwndParent, hmenu, hInstance, NULL ) ) \
			{	\
				return true;\
			}\
			return false;	\
		}


	class CGuiBaseWindowObject : public CGuiBaseObject
	{
	public:
		CGuiBaseWindowObject( CGuiBaseObject *pgboParent, HWND hwnd );
		virtual ~CGuiBaseWindowObject();

		virtual bool OnCreateWindow();

		PI_uint32 StartTimer( CGuiBaseObject *pSect, PI_uint32 nInterval );
		void StopTimer( PI_uint32 uTimerID );
		
		void ForceRedraw( const WinHelper::CRect &rc );

		HWND GetSafeHwnd() const { return m_hwnd; }

	protected:
		virtual LRESULT OnWindowsMessage( UINT message, WPARAM wParam, LPARAM lParam );
		virtual void OnTimer( PI_uint32 uTimerID );

		void CancelMouseDown();
		void CancelHighlight();
		HWND m_hwnd;

		//
		//	Timer stuff
		Container::CMap<int, CGuiBaseObject *> m_mapTimerEvents;
		PI_uint32 m_uNextTimerID;


		//
		//	Our mouse enter/leave timer. This MUST MUST MUST come after the
		//	map for timer events otherwise the map is destroyed first and then
		//	out timer attempts to remove itself...which results in an _ASSERT party
		//	that is hard to get out of.
		CTimer m_timerEnterLeave;

		CGuiBaseObject *m_pgboMouseDowned;
		CGuiBaseObject *m_pgboHighlighted;
	};
};

#endif //GUIBASEWINDOWOBJECT_H