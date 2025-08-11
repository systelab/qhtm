/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Uninitialize.cpp
Owner:	russf@gipsysoft.com
Purpose:	Uninitialise QHTM
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "qhtm.h"

BOOL WINAPI QHTM_Uninitialize()
{
	extern void StopSubclassing();
	StopSubclassing();

	return TRUE;
}
