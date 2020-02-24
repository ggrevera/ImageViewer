/**
    \file ImageData.cpp
    Implementation of the ImageData class.

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
#include  <assert.h>
#include  "ImageViewer.h"
#include  "ImageData.h"
#include  "pnmHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// ImageData

IMPLEMENT_DYNCREATE(ImageData, CDocument)

BEGIN_MESSAGE_MAP(ImageData, CDocument)
	//{{AFX_MSG_MAP(ImageData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
/** \brief ImageData ctor.
 *
 *  Init image specific members to indicate no image yet.
 */
ImageData::ImageData ( ) {
    mW = mH = mMin = mMax = 0;
	mIsColor = false;
    mOriginalData = 0;  //no image yet
}
//---------------------------------------------------------------------------
/** \brief ImageData dtor.
 *
 *  Free memory allocated for image (if any), and indicate that we no longer
 *  have an image.
 */
ImageData::~ImageData ( ) {
    if (mOriginalData!=0)    free( mOriginalData );
    mW = mH = mMin = mMax = 0;
	mIsColor = false;
    mOriginalData = 0;  //no image yet
}
//---------------------------------------------------------------------------
/** \brief Method to create a new document (blank image).
 */
BOOL ImageData::OnNewDocument ( ) {
	if (!CDocument::OnNewDocument())    return FALSE;

	//reinitialization code
	// (SDI documents will reuse this document)
    if (mOriginalData!=0)    free( mOriginalData );
    mW = mH = mMin = mMax = 0;
	mIsColor = false;
    mOriginalData = 0;  //no image yet

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// ImageData serialization

void ImageData::Serialize ( CArchive& ar ) {
	if (ar.IsStoring()) {
		// TODO: add storing code here
    } else {
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
// ImageData diagnostics

#ifdef _DEBUG
void ImageData::AssertValid ( ) const {
	CDocument::AssertValid();
}
//---------------------------------------------------------------------------
void ImageData::Dump ( CDumpContext& dc ) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// ImageData commands
/** \brief Method to open a document (read in an image).
 *
 *  Currently, only .pgm, .ppm, or .pnm formats are supported so the file name
 *  must end in one of these extensions.
 *  \return True if successfully read; false otherwise.
 */
BOOL ImageData::OnOpenDocument ( LPCTSTR lpszPathName )  {
	if (!CDocument::OnOpenDocument(lpszPathName))    return false;
	
    //convert from dos path to "standard"
    char  buff[256];
    strcpy( buff, lpszPathName );
    char*  cp = buff;
    while (*cp) {
        if (*cp=='\\')    *cp = '/';
        ++cp;
    }

    //is the file name extension something we can read?
    int  where = strlen(buff)-4;
    if (where<0)    return false;
    if ( strcmp(&buff[where], ".pgm")==0 || strcmp(&buff[where], ".pnm")==0
      || strcmp(&buff[where], ".ppm")==0
      || strcmp(&buff[where], ".PGM")==0 || strcmp(&buff[where], ".PNM")==0
      || strcmp(&buff[where], ".PPM")==0 ) {
        //load it!
	    int  imageSamplesPerPixel = 0;
        mOriginalData = pnmHelper::read_pnm_file( buff, &mW, &mH,
                                           &imageSamplesPerPixel,
                                           &mMin, &mMax );
        if (mOriginalData==0)    return false;  //error reading image
		assert( imageSamplesPerPixel==1 || imageSamplesPerPixel==3 );
		if (imageSamplesPerPixel==3)    mIsColor = true;
		else                            mIsColor = false;
        return true;  //indicate that we opened a file
    }

    return false;  //indicate that we can't open this type of file
}
//---------------------------------------------------------------------------
/** \brief Method called in response to save document (image).
	\todo  Not implemented.  Currently, the base class is called which probably
    does nothing.
 */
BOOL ImageData::OnSaveDocument ( LPCTSTR lpszPathName ) {	
	return CDocument::OnSaveDocument(lpszPathName);
}
//---------------------------------------------------------------------------
/** \brief Method called in response to close document (image).
	\todo  Not implemented.  We probably want to check if the image was modified
    and, if so, ask the user if they would like to save it.  Currently, the base
    class is called which probably
    does nothing.
 */
void ImageData::OnCloseDocument ( ) {
	// TODO: Add your specialized code here and/or call the base class
	CDocument::OnCloseDocument();
}
/////////////////////////////////////////////////////////////////////////////
