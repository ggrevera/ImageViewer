/**
    \file ImageData.h
    Definition of the ImageData class.

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
#if !defined(AFX_IMAGEDATA_H__9925D1E4_AD63_40B4_9E63_B997030F8FE6__INCLUDED_)
#define AFX_IMAGEDATA_H__9925D1E4_AD63_40B4_9E63_B997030F8FE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** \brief ImageData class.  Modified for ImageViewer.
 */
class ImageData : public CDocument {
protected: // create from serialization only
    ImageData ( );
    DECLARE_DYNCREATE( ImageData )

// Attributes
protected:
    bool  mIsColor;        ///< true if color (rgb); false if gray
    bool  mImageModified;  ///< true if image has been modified
    int   mW;              ///< image width
    int   mH;              ///< image height
    int   mMin;            ///< overall min image pixel value
    int   mMax;            ///< overall max image pixel value
    /** \brief Actual image data.
     *  If mIsColor is false, then gray values are stored consecutively.
     *  Otherwise, rgb triples are stored as 3 consecutive values.
     */
    int*  mOriginalData;

// Operations
public:
    inline bool getIsColor ( void ) const { return mIsColor; }
    inline bool getImageModified ( void ) const { return mImageModified; }
    inline int  getW ( void ) const { return mW; }
    inline int  getH ( void ) const { return mH; }
    inline int  getMin ( void ) const { return mMin; }
    inline int  getMax ( void ) const { return mMax; }
    inline int  getData ( const int i ) const { return mOriginalData[i]; }
    //--------------------------------------------------------------------
    /** \brief Given a pixel's row and column location, this function
     *  returns the gray pixel value at that location.
     *  \param   row image row
     *  \param   col image column
     *  \returns the pixel's gray value.
     */
    inline int getGray ( const int row, const int col ) const {
        assert( !mIsColor );
        const int  offset = row*mW + col;
        return mOriginalData[ offset ];
    }
    //--------------------------------------------------------------------
    /** \brief   Given a pixel's row and column location, this function
     *           returns the value of the pixel's red component.
     *  \param   row image row
     *  \param   col image column
     *  \returns the value of the pixel's red component.
     */
    inline int getRed ( const int row, const int col ) const {
        assert( mIsColor );
        const int  offset = 3 * (row*mW + col);
        return mOriginalData[ offset ];
    }
    //--------------------------------------------------------------------
    /** \brief   Given a pixel's row and column location, this function
     *           returns the value of the pixel's green component.
     *  \param   row image row
     *  \param   col image column
     *  \returns the value of the pixel's green component.
     */
    inline int getGreen ( const int row, const int col ) const {
        assert( mIsColor );
        const int  offset = 3 * (row*mW + col);
        return mOriginalData[ offset+1 ];
    }
    //--------------------------------------------------------------------
    /** \brief   Given a pixel's row and column location, this function
     *           returns the value of the pixel's blue component.
     *  \param   row image row
     *  \param   col image column
     *  \returns the value of the pixel's blue component.
     */
    inline int getBlue ( const int row, const int col ) const {
        assert( mIsColor );
        const int  offset = 3 * (row*mW + col);
        return mOriginalData[ offset+2 ];
    }
    //--------------------------------------------------------------------
    bool dataAvailable ( void ) const { return mOriginalData!=0; }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ImageData)
public:
    virtual BOOL OnNewDocument ( );
    virtual void Serialize ( CArchive& ar );
    virtual BOOL OnOpenDocument ( LPCTSTR lpszPathName );
    virtual BOOL OnSaveDocument ( LPCTSTR lpszPathName );
    virtual void OnCloseDocument ( );
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~ImageData();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(ImageData)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEDATA_H__9925D1E4_AD63_40B4_9E63_B997030F8FE6__INCLUDED_)
