//----------------------------------------------------------------------
/**
    \file InputDialog.h
    Header and implementation file for InputDialog class which is similar
    to the Java:
        String  value = JOptionPane.showInputDialog( "Enter value: " );

    \author Paul DiLascia, MSDN magazine, June 2005,
    http://msdn.microsoft.com/msdnmag/issues/05/08/CAtWork/.
    With simplifications/modifications by George J. Grevera, Ph.D.,
    ggrevera@sju.edu.

    PixieLib(TM) Copyright 1997-2005 Paul DiLascia

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
//----------------------------------------------------------------------
/** \brief Helper class to build a dialog template in memory.  Only
 *  supports what's needed for InputDialog.
 *  DO NOT USE THIS CLASS DIRECTLY!
 */
class DlgTemplateBuilder {
protected:
    WORD*  m_pBuffer;    // internal buffer holds dialog template
    WORD*  m_pNext;      // next WORD to copy stuff
    WORD*  m_pEndBuf;    // end of buffer
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // align ptr to _nearest_ DWORD
    WORD* AlignDWORD ( WORD* ptr ) {
        ptr++;                        // round up to nearest DWORD
        LPARAM  lp = (LPARAM) ptr;    // convert to long
        lp &= 0xFFFFFFFC;             // make sure on DWORD boundary
        return (WORD*)lp;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Add dialog item (control).
    void AddItemTemplate ( WORD wType, DWORD dwStyle, const CRect& rc,
                           WORD nID, DWORD dwStyleEx )
    {
        ASSERT(m_pNext < m_pEndBuf);

        // initialize DLGITEMTEMPLATE 
        DLGITEMTEMPLATE& it = *((DLGITEMTEMPLATE*)AlignDWORD(m_pNext));
        it.style = dwStyle;
        it.dwExtendedStyle = dwStyleEx;

        CRect rcDlg = rc;
        it.x  = (short)rcDlg.left;
        it.y  = (short)rcDlg.top;
        it.cx = (short)rcDlg.Width();
        it.cy = (short)rcDlg.Height();
        it.id = nID;

        // add class (none)
        m_pNext = (WORD*)(&it+1);
        *m_pNext++ = 0xFFFF;            // next WORD is atom
        *m_pNext++ = wType;             // ..atom identifier
        ASSERT(m_pNext < m_pEndBuf);    // check not out of range

        // increment control/item count
        DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
        hdr->cdit++;
    }

public:
    // Windows predefined atom names
    enum { BUTTON=0x0080, EDIT, STATIC, LISTBOX, SCROLLBAR, COMBOBOX };
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Note: Make sure nBufLen is big enough to hold your entire dialog template!
    DlgTemplateBuilder ( UINT nBufLen=1024 ) {
        m_pBuffer = new WORD[nBufLen];
        m_pNext   = m_pBuffer;
        m_pEndBuf = m_pNext + nBufLen;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ~DlgTemplateBuilder ( ) {
        delete [] m_pBuffer;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    DLGTEMPLATE* GetTemplate ( ) {
        return (DLGTEMPLATE*)m_pBuffer;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // functions to build the template
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Create template (DLGTEMPLATE)
    DLGTEMPLATE* Begin ( DWORD dwStyle, const CRect& rc, LPCTSTR caption,
                         DWORD dwStyleEx=0 )
    {
        ASSERT(m_pBuffer==m_pNext);    // call Begin first and only once!

        DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
        hdr->style = dwStyle;    // copy style..
        hdr->dwExtendedStyle = dwStyleEx;    // ..and extended, too
        hdr->cdit = 0;    // number of items: zero

        // Set dialog rectangle.
        CRect rcDlg = rc;
        hdr->x  = (short)rcDlg.left;
        hdr->y  = (short)rcDlg.top;
        hdr->cx = (short)rcDlg.Width();
        hdr->cy = (short)rcDlg.Height();

        // Append trailing items: menu, class, caption. I only use caption.
        m_pNext = (WORD*)(hdr+1);
        *m_pNext++ = 0;    // menu (none)
        *m_pNext++ = 0;    // dialog class (use standard)
        m_pNext = AddText( m_pNext, caption );    // append dialog caption

        ASSERT(m_pNext < m_pEndBuf);
        return hdr;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Append text to buffer. Convert to Unicode if necessary.
    // Return pointer to next character after terminating NULL.
    WORD* AddText ( WORD* buf, LPCTSTR text ) {
        if (text) {
#ifdef USES_CONVERSION  //vc++ .net
            USES_CONVERSION;
            wcscpy( (WCHAR*)buf, T2W((LPTSTR)text) );
#else  //vc++ version 6
            ConvertToWideChar( text );
            wcscpy( (WCHAR*)buf, ConvertToWideChar(text) );
#endif
            buf += wcslen( (WCHAR*)buf )+1;
        } else {
            *buf++ = 0;
        }
        return buf;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef USES_CONVERSION  //vc++ version 6
    // function which converts char to widechar
    wchar_t*  ConvertToWideChar ( const char* src ) {
        //allocate space for the widechars
        wchar_t*  r = new wchar_t[strlen(src)+1];
        //copy each char to a widechar
        wchar_t*  dest = r;
        while (*dest++ = *src++);
        return r;
    }
#endif
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Add dialog item (control).
    void AddItem ( WORD wType, DWORD dwStyle, const CRect& rc,
                   LPCTSTR text, WORD nID=-1, DWORD dwStyleEx=0 )
    {
        AddItemTemplate( wType, dwStyle, rc, nID, dwStyleEx );
        m_pNext = AddText( m_pNext, text );  // append title
        *m_pNext++ = 0;                      // no creation data
        ASSERT(m_pNext < m_pEndBuf);
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Add dialog item (control).
    void AddItem ( WORD wType, DWORD dwStyle, const CRect& rc,
                   WORD nResID, WORD nID=-1, DWORD dwStyleEx=0 )
    {
        AddItemTemplate( wType, dwStyle, rc, nID, dwStyleEx );
        *m_pNext++ = 0xFFFF;    // next is resource id
        *m_pNext++ = nResID;    // ..here it is
        *m_pNext++ = 0;         // no extra stuff
        ASSERT( m_pNext < m_pEndBuf );
    }

};
//======================================================================
/** \brief Class to implement a simple string input dialog box similar to
 *  <pre>
 *      String  value = JOptionPane.showInputDialog( "Enter value: " );
 *  </pre>
 *  in Java.  Use as follows:
 *  <pre>
 *      InputDialog  dlg( "Enter a string:" );
 *      CString      result = dlg.m_str;    //what the user typed
 *  </pre>
 */
class InputDialog : public CDialog {
private:
    InputDialog ( )  { }                ///< don't use this ctor
    int DoModal ( void ) { return CDialog::DoModal(); }  ///< don't use this method
protected:
    HICON    m_hIcon;    ///< icon if not supplied
public:
    CString  m_str;      ///< the string returned [in,out]
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ~InputDialog ( ) { }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief Call this to create the template with given caption and
     *  prompt.  Create string dialog.  If no icon specified, use 
     *  IDI_QUESTION.  Note that the order in which the controls are 
     *  added is the TAB order.
     */
    InputDialog ( LPCTSTR prompt, LPCTSTR def="",
        LPCTSTR caption="Input requested", CWnd* pParent=NULL, 
        WORD nIDIcon=(WORD)IDI_QUESTION )
    {
        m_str = def;
        const int  CXDIALOG  = 200;    // dialog width
        const int  DLGMARGIN = 7;      // margins all around
        const int  CYSTATIC  = 8;      // height of static text
        const int  CYEDIT    = 12;     // height of edit control
        const int  CYSPACE   = 5;      // vertical space between controls
        const int  CXBUTTON  = 40;     // button width...
        const int  CYBUTTON  = 15;     // ..and height

        DlgTemplateBuilder&  dtb = m_dtb;
        CRect  rc(
            CPoint( 0, 0 ),
            CSize( CXDIALOG, CYSTATIC + CYEDIT + CYBUTTON
                             + 2*DLGMARGIN + 2*CYSPACE) );

        // create dialog header
        DLGTEMPLATE*  pTempl = dtb.Begin( WS_POPUPWINDOW | DS_MODALFRAME
            | WS_DLGFRAME, rc, caption );

        // shrink main rect by margins
        rc.DeflateRect( CSize(DLGMARGIN,DLGMARGIN) );

        // create icon if needed
        if (nIDIcon) {
            if (nIDIcon >= (WORD)IDI_APPLICATION) {
                // if using a system icon, I load it here and set it in OnInitDialog
                // because can't specify system icon in template, only icons from
                // application resource file.
                m_hIcon = ::LoadIcon( NULL, MAKEINTRESOURCE(nIDIcon) );
                nIDIcon = 0;
            } else {
                m_hIcon = NULL;
            }

            // The size is calculated in pixels, but it seems to work OK--???
            CSize  sz(GetSystemMetrics( SM_CXICON),GetSystemMetrics(SM_CYICON) );
            CRect  rcIcon( rc.TopLeft(), sz );
            dtb.AddItem( DlgTemplateBuilder::STATIC, // add icon 
                WS_VISIBLE|WS_CHILD|SS_LEFT|SS_ICON, rc, nIDIcon, IDICON );
            rc.left += sz.cx;  // shrink main rect by width of icon
        }

        // add prompt
        rc.bottom = rc.top + CYSTATIC;    // height = height of static
        dtb.AddItem( DlgTemplateBuilder::STATIC,    // add it
            WS_VISIBLE|WS_CHILD|SS_LEFT, rc, prompt );

        // add edit control
        rc += CPoint( 0, rc.Height() + CYSPACE );    // move below static
        rc.bottom = rc.top + CYEDIT;    // height = height of edit control
        dtb.AddItem( DlgTemplateBuilder::EDIT,    // add it
            WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP, rc, m_str, IDEDIT );

        // add OK button
        rc += CPoint(0, rc.Height() + CYSPACE);    // move below edit control
        rc.bottom = rc.top + CYBUTTON;    // height = button height
        rc.left   = rc.right - CXBUTTON;    // width  = button width
        rc -= CPoint(CXBUTTON + DLGMARGIN,0);    // move left one button width
        dtb.AddItem( DlgTemplateBuilder::BUTTON,    // add it
            WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_DEFPUSHBUTTON, rc, _T("&OK"), IDOK );

        // add Cancel button
        rc += CPoint(CXBUTTON + DLGMARGIN,0);    // move right again
        dtb.AddItem( DlgTemplateBuilder::BUTTON,    // add Cancel button
            WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, _T("&Cancel"), IDCANCEL );

        ASSERT( InitModalIndirect( pTempl, pParent ) );
        DoModal();
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
    DlgTemplateBuilder  m_dtb;    ///< place to build/hold the dialog template
    enum { IDICON=1, IDEDIT };    ///< control IDs
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // MFC virtual overrides
    virtual BOOL OnInitDialog ( ) {
        if (m_hIcon) {
            CStatic* pStatic = (CStatic*)GetDlgItem( IDICON );
            ASSERT( pStatic );
            pStatic->SetIcon( m_hIcon );
        }
        return CDialog::OnInitDialog( );
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    virtual void DoDataExchange ( CDataExchange* pDX ) {
        DDX_Text( pDX, IDEDIT, m_str );
    }
};
//----------------------------------------------------------------------
