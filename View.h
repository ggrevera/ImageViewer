/**
    \file View.h
    Interface of the View class.

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
#if !defined(AFX_VIEW_H__CA6F5626_B30F_43A5_B2C3_3C717994E4AA__INCLUDED_)
#define AFX_VIEW_H__CA6F5626_B30F_43A5_B2C3_3C717994E4AA__INCLUDED_

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000

/** \brief View class.  Modified for ImageViewer.
 */
class View : public CView {
protected: // create from serialization only
	View();
	DECLARE_DYNCREATE( View )

// Attributes
public:
	ImageData* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(View)
public:
	virtual void OnDraw ( CDC* pDC );  // overridden to draw this view
	virtual BOOL PreCreateWindow ( CREATESTRUCT& cs );
protected:
	virtual BOOL OnPreparePrinting ( CPrintInfo* pInfo );
	virtual void OnBeginPrinting ( CDC* pDC, CPrintInfo* pInfo );
	virtual void OnEndPrinting ( CDC* pDC, CPrintInfo* pInfo );
	virtual void OnUpdate ( CView* pSender, LPARAM lHint, CObject* pHint );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~View ( );
#ifdef _DEBUG
	virtual void AssertValid ( ) const;
	virtual void Dump ( CDumpContext& dc ) const;
#endif

protected:
    bool            mMouseMoveValid;           ///< indicates mouse (x,y) below are valid
    int             mMouseMoveX, mMouseMoveY;  ///< mouse (x,y) for tracking
    unsigned char*  mDisplayData;              ///< displayable image

// Generated message map functions
protected:
	//{{AFX_MSG( View )
	afx_msg void OnMouseMove ( UINT nFlags, CPoint point );
	afx_msg BOOL OnEraseBkgnd ( CDC* pDC );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP( )
};

#ifndef _DEBUG  // debug version in View.cpp
  inline ImageData* View::GetDocument ( ) {
	return (ImageData*)m_pDocument;
  }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_H__CA6F5626_B30F_43A5_B2C3_3C717994E4AA__INCLUDED_)
