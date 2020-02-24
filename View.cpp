/**
    \file View.cpp
    Implementation of the View class.

    \author George J. Grevera, Ph.D., ggrevera@sju.edu

    Copyright (C) 2002, George J. Grevera

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
    USA or from http://www.gnu.org/licenses/gpl.txt.

    This General Public License does not permit incorporating this
    code into proprietary programs.  (So a hypothetical company such
    as GH (Generally Hectic) should NOT incorporate this code into
    their proprietary programs.)
 */
#include  "stdafx.h"
#include  "ImageViewer.h"
#include  <assert.h>
#include  "ImageData.h"
#include  "View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// View

IMPLEMENT_DYNCREATE( View, CView )

BEGIN_MESSAGE_MAP( View, CView )
	//{{AFX_MSG_MAP( View )
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND( ID_FILE_PRINT,         CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_DIRECT,  CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview )
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
/** \brief View ctor.
 *
 *  Indicate that we don't have an image yet and mouse movement coordinates
 *  are not yet valid.
 */
View::View ( ) {
    mMouseMoveValid = false;
    mDisplayData = 0;
	mMouseMoveX = mMouseMoveY = -1;
}
/** \brief View dtor.
 *
 *  Free any image memory used (if any) and indicate that we no longer have an
 *  an image.
 */
View::~View ( ) {
    if (mDisplayData!=0) {  free( mDisplayData );  mDisplayData=0;  }
}

BOOL View::PreCreateWindow ( CREATESTRUCT& cs ) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// View drawing
/** \brief Draw the image and misc. info.
 */
void View::OnDraw ( CDC* pDC ) {
	ImageData*  pDoc = GetDocument();
	ASSERT_VALID( pDoc );

    //did we load an image yet?
    if (!pDoc->dataAvailable()) {
        //no image loaded yet.
        //fill the remainder with black.
        CRect  rcBounds;
	    GetClientRect( &rcBounds );
        pDC->FillRect( rcBounds,
            CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)) );

        //anything left over from last time?
        if (mDisplayData!=0) {
            free( mDisplayData );
            mDisplayData = 0;
        }
        pDC->SetBkColor( 0x00000000 );
        pDC->SetTextColor( 0x0000ffff );
        char  buff[255];
        sprintf( buff, "(%d,%d) \n no image", mMouseMoveX, mMouseMoveY );
        pDC->DrawText( buff, -1, &rcBounds, DT_CENTER );
        return;
    }

    //have we created a displayable version of the image yet?
    if (mDisplayData==0) {
        //create a displayable version of the image.
        mDisplayData = (unsigned char*)malloc( 4 * pDoc->getW()
            * pDoc->getH() * sizeof(unsigned char) );
        assert( mDisplayData!=NULL );
		if (!pDoc->getIsColor()) {  //gray?
            int  src=0, dst=0;
            for (int y=0; y<pDoc->getH(); y++) {
                for (int x=0; x<pDoc->getW(); x++) {
                    int  v = pDoc->getData(src);
                    if (pDoc->getMin()<0 || pDoc->getMax()>255) {
                        const int  diff = pDoc->getMax() - pDoc->getMin();
                        if (diff!=0)    v = 255 * (v-pDoc->getMin()) / diff;
                        else            v = 127;
                    } else if (pDoc->getMin()==0 && pDoc->getMax()==1) {
                        //handle special case of binary image (otherwise, we
                        // won't be able to distinguish between black and white.
                        if (v==1)    v=255;
                    }
                    if (v<0)    v = 0;
                    if (v>255)  v = 255;
                    //0 is dark; 255 is bright
                    mDisplayData[dst]   = v;  //blue
                    mDisplayData[dst+1] = v;  //green
                    mDisplayData[dst+2] = v;  //red
                    mDisplayData[dst+3] = 0;  //not used
                    src++;
                    dst += 4;
                }
            }
        } else {  //color (rgb)
            int  src=0, dst=0;
            for (int y=0; y<pDoc->getH(); y++) {
                for (int x=0; x<pDoc->getW(); x++) {
                    //0 is dark; 255 is bright
                    mDisplayData[dst+2] = pDoc->getData(src++);  //red
                    mDisplayData[dst+1] = pDoc->getData(src++);  //green
                    mDisplayData[dst  ] = pDoc->getData(src++);  //blue
                    mDisplayData[dst+3] = 0;
                    dst += 4;
                }
            }
        }
    }

    CBitmap  bm;
    bm.CreateBitmap( pDoc->getW(), pDoc->getH(), 1, 32, mDisplayData );
    CDC  dcMem;
    dcMem.CreateCompatibleDC( pDC );
    CBitmap*  pbmpOld = dcMem.SelectObject( &bm );
    pDC->BitBlt( 0, 0, pDoc->getW(), pDoc->getH(), &dcMem, 0, 0, SRCCOPY );
    // reselect the original bitmap into the memory DC
    dcMem.SelectObject( pbmpOld );

    //fill the remainder with black
    CRect  rcBounds;
	GetClientRect( &rcBounds );
    const int  oldTop = rcBounds.top;
    rcBounds.top  = pDoc->getH();
    pDC->FillRect( rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)) );
    rcBounds.top  = oldTop;
    rcBounds.left = pDoc->getW();
    pDC->FillRect( rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)) );

    if (mMouseMoveValid) {
        pDC->SetBkColor( 0x007f7f7f );
        pDC->SetTextColor( 0x0000ffff );
        char  buff[255];
        sprintf( buff, "(%d,%d)", mMouseMoveX, mMouseMoveY );
		if (rcBounds.left>600)    rcBounds.left = 100;
        pDC->DrawText( buff, -1, &rcBounds, DT_CENTER );
    }
}
/////////////////////////////////////////////////////////////////////////////
// View printing

BOOL View::OnPreparePrinting ( CPrintInfo* pInfo ) {
	// default preparation
	return DoPreparePrinting(pInfo);
}

void View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
/////////////////////////////////////////////////////////////////////////////
// View diagnostics

#ifdef _DEBUG
void View::AssertValid ( ) const {
	CView::AssertValid();
}

void View::Dump ( CDumpContext& dc ) const {
	CView::Dump(dc);
}

ImageData* View::GetDocument ( ) // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ImageData)));
	return (ImageData*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// View message handlers
/** \brief Track mouse movement.
 */
void View::OnMouseMove ( UINT nFlags, CPoint point ) {
    mMouseMoveValid = true;
    mMouseMoveX     = point.x;
    mMouseMoveY     = point.y;
    RedrawWindow();
	//CView::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
/** \brief This method is called whenever we need to load, reload, or modify
 *  the view of our object.
 */
void View::OnUpdate ( CView* pSender, LPARAM lHint, CObject* pHint ) {
    //did we load an image yet?
	ImageData*  pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    if (!pDoc->dataAvailable())    return;
    //have we created a displayable version of the image yet?
    if (mDisplayData==0)    return;
    //otherwise, clean up the leftover image
    free( mDisplayData );
    mDisplayData = 0;
}
/////////////////////////////////////////////////////////////////////////////
/** \brief Must override this method to reduce flicker.
 */
BOOL View::OnEraseBkgnd ( CDC* pDC ) {
    return FALSE;
	//return CView::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////
