/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Initialize.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "qhtm.h"
#include "TipWIndow.h"

extern bool FASTCALL RegisterWindow( HINSTANCE hInst );

BOOL WINAPI QHTM_Initialize( HINSTANCE hInst )
{
//
//	If we are a DLL then this gets set in the DLLMain, however, if we are statically linked
//	then we need to set it to the EXE.
#ifndef QHTM_DLL
	g_hQHTMInstance = hInst;
#endif	//	QHTM_DLL

	#ifdef _DEBUG
		//SETLOGFILE( _T("d:\\QHTM.log") );
	#endif	//	_DEBUG

	if( RegisterWindow( hInst ) )
	{
		return CTipWindow::Register( hInst );
	}

	return FALSE;
}
