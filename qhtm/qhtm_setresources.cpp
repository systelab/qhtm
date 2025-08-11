/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_SetResources.cpp
Owner:	russf@gipsysoft.com
Purpose:
	
				Allow client code to set the resources used in QHTM. 
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "qhtm.h"

UINT g_uHandCursorID = 0;
UINT g_uNoImageBitmapID = 0;
HINSTANCE g_hResourceInstance = NULL;

void WINAPI QHTM_SetResources( HINSTANCE hInst, UINT uHandCursorID, UINT uNoImageBitmapID )
{
	//
	//	Must pass all three!
	_ASSERT( uHandCursorID );
	_ASSERT( uNoImageBitmapID );
	_ASSERT( hInst );

	g_uHandCursorID = uHandCursorID;
	g_uNoImageBitmapID = uNoImageBitmapID;
	g_hResourceInstance = hInst;
}
