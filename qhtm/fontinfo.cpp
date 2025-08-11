/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FontInfo.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <guitools/guitools.h>
#include "QHTM_Types.h"
#include "smallstringhash.h"
#include "FontInfo.h"

struct SFontInfo
{
	bool m_bExists;
	bool m_bFixedPitch;
};

static MapClass< StringClass, SFontInfo> g_mapFontName;


static int CALLBACK EnumFontFamExProc( ENUMLOGFONTEX * lpelfe, NEWTEXTMETRICEX * /*lpntme*/, int /*FontType*/, LPARAM lParam )
{
	if( lpelfe->elfLogFont.lfPitchAndFamily & FIXED_PITCH )
	{
		*reinterpret_cast<bool*>( lParam ) = true;
	}

	return -1;
}


static SFontInfo *GetFontInfo( const StringClass &strFontName, BYTE cCharSet )
//
//	Determine if the font exists in our map and return that result.
//	If it doesn't exist in our map then ask the system if the font exists and add
//	that known state to our map.
{
	SFontInfo *pfi = g_mapFontName.Lookup( strFontName );
	if( pfi )
	{
		return pfi;
	}

	LOGFONT lf = { 0 };
	_tcscpy( lf.lfFaceName, strFontName );
	lf.lfCharSet = cCharSet;

	GS::CDrawContext dc;
	SFontInfo fi = { false, false };

	//
	//	NOTE: If the font doesn't exist EnumFontFamiliesEx returns 1 - originally I had my callbcak return
	//	true or false. This didn't work if the font didn't exist. Now the callback returns -1 if the font
	//	exists.
	if( EnumFontFamiliesEx( dc.GetSafeHdc(), &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&fi.m_bFixedPitch, 0 ) == -1 )
		fi.m_bExists = true;
	g_mapFontName.SetAt( strFontName, fi );

#ifdef _DEBUG
	if( !fi.m_bExists )
	{
		TRACE( _T("Font doesn't exist '%s'\n"), strFontName );
	}
#endif	//	_DEBUG
	return g_mapFontName.Lookup( strFontName );;
}


bool DoesFontExist( const StringClass &strFontName, BYTE cCharSet )
{
	SFontInfo *pfi = GetFontInfo( strFontName, cCharSet );
	if( pfi )
	{
		return pfi->m_bExists;
	}
	return false;
}


bool IsFontFixed( const StringClass &strFontName, BYTE cCharSet )
{
	SFontInfo *pfi = GetFontInfo( strFontName, cCharSet );
	if( pfi )
	{
		return pfi->m_bFixedPitch;
	}
	return false;
}
