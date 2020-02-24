/**
    \file ChildFrame.cpp
    Implementation of the ChildFrame class.

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
#include "stdafx.h"
#include "ImageViewer.h"

#include "ChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ChildFrame

IMPLEMENT_DYNCREATE( ChildFrame, CMDIChildWnd )

BEGIN_MESSAGE_MAP( ChildFrame, CMDIChildWnd )
	//{{AFX_MSG_MAP(ChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChildFrame construction/destruction

ChildFrame::ChildFrame ( ) {
	// TODO: add member initialization code here
}

ChildFrame::~ChildFrame ( ) {
}

BOOL ChildFrame::PreCreateWindow ( CREATESTRUCT& cs ) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if ( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// ChildFrame diagnostics

#ifdef _DEBUG
void ChildFrame::AssertValid ( ) const {
	CMDIChildWnd::AssertValid();
}

void ChildFrame::Dump ( CDumpContext& dc ) const {
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ChildFrame message handlers
