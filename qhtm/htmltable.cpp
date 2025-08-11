/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTable.cpp
Owner:	russf@gipsysoft.com
Purpose:	A table
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "htmlparse.h"
#include "HTMLSectionCreator.h"
#include "defaults.h"
#include "tableLayout.h"
#include "HTMLTableSection.h"

CHTMLTable::CHTMLTable(	int nWidth, int nBorder, CStyle::Align alg, CStyle::Align	valg, int nCellSpacing, int nCellPadding,  CBackground &background, COLORREF crDark, COLORREF crLight)
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_nWidth( nWidth )
	, m_nBorder( nBorder )
	, m_alg( alg )
	, m_valg( valg )
	, m_pCurrentRow( NULL )
	, m_nCellSpacing( nCellSpacing )
	, m_nCellPadding( nCellPadding )
	, m_crBorderLight( crLight )
	, m_crBorderDark( crDark )
	, m_bCellsMeasured( false )
	, m_back( background )
{
	m_borderMask = 0xf;
}


CHTMLTable::~CHTMLTable()
{
	for( UINT nRow = 0; nRow < m_arrRows.GetSize(); nRow++ )
	{
		delete m_arrRows[ nRow ];
	}
	m_arrRows.RemoveAll();
}

void CHTMLTable::AddCell( CHTMLTableCell *pCell )
{
	_ASSERT( m_pCurrentRow );
	if( m_pCurrentRow->m_back.m_crBack.IsSet() )
	{
		m_pCurrentRow->m_back.m_crBack = m_pCurrentRow->m_back.m_crBack;
	}
	m_pCurrentRow->m_arrCells.Add( pCell );
}


void CHTMLTable::NewRow( CStyle::Align valg )
{
	m_pCurrentRow = new CHTMLTableRow( valg );
	m_arrRows.Add( m_pCurrentRow );
}


WinHelper::CSize CHTMLTable::GetRowsCols() const
//
//	Get the dimensions of the table in rows and columns
{
	WinHelper::CSize size( m_arrRows.GetSize(), 0 );
	for( UINT nRow = 0; nRow < m_arrRows.GetSize(); nRow++ )
	{
		if( m_arrRows[ nRow ]->m_arrCells.GetSize() > (UINT)size.cy )
		{
			size.cy = m_arrRows[ nRow ]->m_arrCells.GetSize();
		}
	}
	//	Must have a size
	_ASSERT( size.cx && size.cy );
	return size;
}

#ifdef _DEBUG
void CHTMLTable::Dump() const
{
	WinHelper::CSize size( GetRowsCols() );
	TRACENL( _T("Table----------------\n"));
	TRACENL( _T(" Size %d rows %d cols\n"), size.cx, size.cy );
	TRACENL( _T(" Width( %d )\n"), m_nWidth );
	TRACENL( _T(" Border( %d )\n"), m_nBorder );	
	TRACENL( _T(" Alignment (%s)\n"), GetStringFromAlignment( m_alg ) );
	TRACENL( _T(" CellSpacing (%d)\n"), m_nCellSpacing );
	TRACENL( _T(" CellPadding (%d)\n"), m_nCellPadding );
	TRACENL( _T(" Colors Not Shown.\n") );

	for( UINT nRow = 0; nRow < m_arrRows.GetSize(); nRow++ )
	{
		TRACENL( _T(" Row %d\n"), nRow );
		m_arrRows[ nRow ]->Dump();
	}

}
#endif	//	_DEBUG


CHTMLTable::CHTMLTableRow::CHTMLTableRow( CStyle::Align	valg )
	: m_valg( valg )
	, m_nHeight( 0 )
{
}


CHTMLTable::CHTMLTableRow::~CHTMLTableRow()
{
	for( UINT nCol = 0; nCol < m_arrCells.GetSize(); nCol++ )
	{
		delete m_arrCells[ nCol ];
	}

	m_arrCells.RemoveAll();
}


#ifdef _DEBUG
void CHTMLTable::CHTMLTableRow::Dump() const
{
	for( UINT nCol = 0; nCol < m_arrCells.GetSize(); nCol++ )
	{
		TRACENL( _T(" Col %d\n"), nCol );
		m_arrCells[ nCol ]->Dump();
	}
}
#endif	//	_DEBUG


void CHTMLTable::ResetMeasuringKludge()
{
	m_bCellsMeasured = false;
	for( UINT nRow = 0; nRow < m_arrRows.GetSize(); nRow++ )
	{
		for( UINT nCol = 0; nCol < m_arrRows[ nRow ]->m_arrCells.GetSize(); nCol++ )
		{
			m_arrRows[ nRow ]->m_arrCells[ nCol ]->ResetMeasuringKludge();
		}
	}
}


int CHTMLTable::GetColumnWidth( int nColumnsToSpan, int nCol, CHTMLTableLayout &layout, CHTMLSectionCreator *psc )
{
	int nWidth = 0;
	while( nColumnsToSpan )
	{
		nWidth += layout.GetColumnWidth( nCol );
		nColumnsToSpan--;
		nCol++;

		if( nColumnsToSpan > 0 )
		{
			nWidth += 2 * psc->GetDC().ScaleX( m_nCellPadding );
			nWidth += psc->GetDC().ScaleX( (m_nBorder ? 2 : 0) );
			nWidth += psc->GetDC().ScaleX( m_nCellSpacing );
		}
	}

	return nWidth;
}


void CHTMLTable::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	// In order to ensure that the borders and backgrounds will be drawn
	// properly, we will need either sections to display each of those properties
	// and ensure that they are displayed before the cell's contents. This way
	// we do not need another layer of indirection (such as 
	// CHTMLTableSection->CHTMLCellSection->CHTMLTextSection)
	//

	const WinHelper::CSize sizeTable( GetRowsCols() );
	if( sizeTable.cx && sizeTable.cy )
	{
		if( m_alg == CStyle::algCentre || m_alg == CStyle::algRight )
		{
			psc->NewParagraph( 1, 1 );
		}

		int nMaxWidth = psc->GetCurrentWidth();

		CHTMLTableLayout layout( const_cast<CHTMLTable*>( this ), psc->GetDC(), psc->GetDefaultRightMargin() - psc->GetDefaultLeftMargin(), psc->GetZoomLevel() );
		int nTableWidth = layout.GetTableWidth();
		
		//
		//	If the table is too wide creep the image/table margins until the table fits or there are no more margins.
		if( nTableWidth  + g_defaults.m_nAlignedTableMargin < nMaxWidth )
		{
			while( psc->GetDefaultRightMargin() != psc->GetRightMargin() && psc->GetDefaultLeftMargin() != psc->GetRightMargin() && nTableWidth + g_defaults.m_nAlignedTableMargin >= psc->GetCurrentWidth() )
			{
				psc->NewParagraph( 1, 1 );
			}

			nMaxWidth = psc->GetCurrentWidth();
		}
		
		const int nKeepYPos = psc->GetCurrentYPos();
		
		//
		//  Note that the default alignment is algTop, which is 
		//  meaningless for tables, but provides the appropriate non-wrapping
		//  behavior
		int nLeftMargin = psc->GetCurrentXPos();
		if( nTableWidth < nMaxWidth )
		{
			switch( m_alg )
			{
			case CStyle::algRight:
				nLeftMargin = psc->GetRightMargin() - nTableWidth;
				break;

			case CStyle::algCentre:
				nLeftMargin += ( nMaxWidth - nTableWidth ) / 2;
				break;
			}
		}


		// If there is a border or background color, we need a CHTMLTableSection to display them.
		// We could create it without checking, but its wasteful.
		CHTMLTableSection* pTableSection = 0;
		if (m_nBorder || m_back.IsSet())
		{
			pTableSection = new CHTMLTableSection( psc->GetHTMLSection(), m_nBorder, m_crBorderDark, m_crBorderLight, m_back );
			pTableSection->m_borderMask = m_borderMask;
			psc->AddSection( pTableSection );
			// This will need to have it's dimensions set, specifically the botttom!
			pTableSection->Set( nLeftMargin, psc->GetCurrentYPos(), nLeftMargin + nTableWidth, psc->GetCurrentYPos() );
			// nShapeId = GetCurrentShapeID() - 1;
		}

		ArrayClass<CHTMLTableSection*> arrCellSection( sizeTable.cy );

		//		iterate over the columns again, this time lay them out by calling this function and remember the
		//			tallest cell.
		//		Move onto the next row by bumping up the creator Y position.
		if( m_nBorder )
		{
			psc->AddYPos( m_nBorder );
		}

		const UINT uTableRows = m_arrRows.GetSize();
		for(UINT nRow = 0; nRow < uTableRows; nRow++ )
		{
			// Skip the border of the table, if there is one.
			psc->SetCurrentXPos( nLeftMargin + psc->GetDC().ScaleX( m_nBorder ) );
			if( m_nCellSpacing )
			{
				psc->AddYPos( psc->GetDC().ScaleY( m_nCellSpacing ) );
			}
			CHTMLTable::CHTMLTableRow *pRow = m_arrRows[ nRow ];
			int nTallestY = psc->GetCurrentYPos();


			ArrayOfInt arrStartShape;
			ArrayOfInt arrLowest;
			ArrayOfInt arrEndShape;

			const UINT uRowColumns = pRow->m_arrCells.GetSize();
			int maxRight = 0;
			for( UINT nCol = 0; nCol < uRowColumns; nCol++ )
			{
				CHTMLTableCell* pcell = pRow->m_arrCells[nCol];
				int nColumnsToSpan = min( pcell->m_nColSpan, layout.GetColumnCount() - (int)nCol );

				// Here we include code to account for borders, spacing, and padding
				const int nOuterLeft = psc->GetCurrentXPos() + psc->GetDC().ScaleX(m_nCellSpacing);
				const int nInnerLeft = nOuterLeft + psc->GetDC().ScaleX(m_nCellPadding + (m_nBorder ? 1 : 0));

				const int nInnerRight = nInnerLeft + GetColumnWidth( nColumnsToSpan, nCol, layout, psc );

				const int nOuterRight = nInnerRight + psc->GetDC().ScaleX(m_nCellPadding + (m_nBorder ? 1 : 0));
				if (maxRight < nOuterRight)
				{
					maxRight = nOuterRight;
				}


				const int nOuterTop = psc->GetCurrentYPos();
				const int nInnerTop = nOuterTop + psc->GetDC().ScaleY(m_nCellPadding + (m_nBorder ? 1 : 0));

				psc->SetCurrentXPos( nOuterRight );

				// Create a TableSection if there is a border or background color
				if (m_nBorder || pcell->m_back.IsSet() )
				{
					arrCellSection[nCol] = new CHTMLTableSection( psc->GetHTMLSection(), m_nBorder ? 1 : 0,  pcell->m_crBorderLight, pcell->m_crBorderDark, pcell->m_back );
					arrCellSection[nCol]->m_borderMask = pcell->m_borderMask;
					psc->AddSection( arrCellSection[nCol] );
					arrCellSection[nCol]->Set( nOuterLeft, nOuterTop, nOuterRight - 1, nOuterTop);
				}
				else
				{
					arrCellSection[nCol] = NULL;
				}

				arrStartShape.Add( psc->GetCurrentShapeID() );

				CHTMLSectionCreator htCreate( psc->GetHTMLSection(), psc->GetDC(), nInnerTop, nInnerLeft, nInnerRight, psc->GetBackgroundColor(), psc->IsMeasuring(), psc->GetZoomLevel(), psc->GetCurrentLink() );
				htCreate.AddDocument( pcell );

				const WinHelper::CSize size( htCreate.GetSize() );
				arrLowest.Add( size.cy );

				if( size.cy > nTallestY )
				{
					nTallestY = size.cy;

					const int nCellHeight = size.cy - nOuterTop;
					const int nRowHeightDC = psc->GetDC().ScaleY( pRow->m_nHeight );
					if( nCellHeight < nRowHeightDC )
					{
						nTallestY = nOuterTop + nRowHeightDC;
					}
				}

				arrEndShape.Add( psc->GetCurrentShapeID() );
			}
			if (pTableSection)
			{
				pTableSection->right = maxRight;
			}

			psc->SetCurrentYPos( nTallestY + m_nCellPadding + (m_nBorder ? 1 : 0) );

			//
			//	Adjust all of the cells to have the same bottom.
			//	Also, moves all of the contained shapes if the vertical alignment dictates
			int maxBottom = 0;
			for (UINT ii = 0; ii < uRowColumns; ++ii)
			{
				if (arrCellSection[ii])
				{
					arrCellSection[ii]->bottom = psc->GetCurrentYPos() - 1;
				}
				if (maxBottom < psc->GetCurrentYPos())
				{
					maxBottom = psc->GetCurrentYPos();
				}

				const CHTMLTableCell* pcell = pRow->m_arrCells[ ii ];
				int nOffset = 0;
				switch( pcell->m_valg )
				{

				case CStyle::algTop:
					//	Leave them as they are
					break;

				case CStyle::algMiddle:
					nOffset = ( nTallestY - arrLowest[ ii ] ) / 2;
					break;

				case CStyle::algBottom:
					nOffset = nTallestY - arrLowest[ ii ];
					break;
				}

				if( nOffset )
				{
					for( int n = arrStartShape[ ii ]; n < arrEndShape[ ii ]; n++ )
					{
						psc->GetHTMLSection()->GetSectionAt( n )->Offset( 0, nOffset );
					}
				}
			}

			if (pTableSection)
			{
				pTableSection->bottom = maxBottom;
			}

			arrStartShape.RemoveAll();
			arrLowest.RemoveAll();
			arrEndShape.RemoveAll();

			// Good place for a page break, between rows.
			if( psc->GetDC().IsPrinting() && psc->GetLeftMargin() == psc->GetDefaultLeftMargin() && psc->GetRightMargin() == psc->GetDefaultRightMargin())
			{
				psc->AddBreak(nRow);
			}
		}

		// Complete the table section!
		psc->AddYPos(2);
		if( m_nCellSpacing )
		{
			psc->AddYPos( m_nCellSpacing );
		}

		//	Insert a paragraph if were not floating
		if (m_alg != CStyle::algLeft && m_alg != CStyle::algRight)
		{
			if( psc->GetDC().IsPrinting() && psc->GetLeftMargin() == psc->GetDefaultLeftMargin() && psc->GetRightMargin() == psc->GetDefaultRightMargin() )
			{
				psc->AddBreak();
			}
		}

		if( nTableWidth < nMaxWidth )
		{
			//
			//	For tables that are right or left aligned we need to alter the margin stack.
			//	For 'normal' tables we do nothing.
			switch( m_alg )
			{
			case CStyle::algRight:
				{
					psc->AddNewRightMargin( nLeftMargin - g_defaults.m_nAlignedTableMargin, psc->GetCurrentYPos() );
					if( psc->GetCurrentXPos() == nLeftMargin )
					{
						psc->SetCurrentXPos( psc->GetRightMargin() );
					}
					psc->SetCurrentYPos( nKeepYPos );
				}
				break;

			case CStyle::algLeft:
				{
					psc->AddNewLeftMargin( nLeftMargin + nTableWidth + g_defaults.m_nAlignedTableMargin, psc->GetCurrentYPos() );
					psc->SetCurrentYPos( nKeepYPos );
				}
				break;
			}
		}
	}
}