/// \file
// \brief  creates a directory browsing dialog, it allows to choose both
//         directories and files
// \author Gamze Tunali, LEMS, Brown University
// \date   16 Nov 2007
//
// The CDirDialog class implements a directory selection dialog by deriving from the file
// selection dialog class (CFileDialog).  This dialog has the advantages of the standard
// file open dialog (resizeable, ability to create/delete directories etc) but is
// customized for the selection of a directory rather than a file.
//
// USER INTERFACE
// --------------
//
// For example, the user can double click on a directory name or type it into the
// edit control to open it in the directory listing display.  To say that this is
// the directory they want the user must click the "Open" button.  When the user
// enters a backslash (\) as the last character in the edit control the display is
// changed to show the contents of the directory if it exists.  The same is done if
// the user presses the Enter key except that if the directory does not exist the
// user is asked if they want to create it.
//
// When the user enters other characters and the contents don't end in a backslash
// then automatic directory name completion is attempted.  If the contents of the
// edit box are the first character(s) of one unique existing directory then the
// rest of the directory name is added to the end of the edit box.  These characters
// are selected so that the user can type something else and it they will be ignored.
//
// When selecting a directory you would normally not want to see files, but you may on
// occasion.  The normal "Files of Type:" drop down list is available but it has an
// extra entry "Show Folders Only" that is selected by default.  When files are
// displayed double clicking of them is ignored (unlike the normal File Open dialog).
// Double-clicking a directory name changes to that directory as normal.
//
// PROGRAMMER INTERFACE
// --------------------
//
// Add DirDialog.cpp and DirDialog.h to your project.  Include DirDialog.h
// in the source file(s) where you want to use the CDirDialog class.
//
// Create a CDirDialog object using the constructor described below.  If necessary
// you may then modify values in the m_ofn member of the CFileDialog base class
// (see the Win32 documentation for OPENFILENAME).  For example, to change the
// text that appears in the title bar of the dialog use m_ofn.lpstrTitle.
//
// Call DoModal() to activate the dialog.  If DoModal() return IDOK you can then
// call GetPath() to obtain the name of the directory that the user selected.
//
//
// CDirDialog::CDirDialog(LPCTSTR lpstrInitialDir = NULL,
//                        LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
// 
//      lpstrInitialDir
//          The initial directory.  If NULL then the current directory is used.
//          See lpstrInitialDir in the Win32 documentation for OPENFILENAME for more info.
//
//      lpszFilter
//          The string pairs that specify the file filters to use.  See lpszFilter
//          in the documentation for the CFileDialog constructor for more info.
//          Note that an extra entry is always added that allows the user to hide the
//          display of all files.  If NULL is used (the default) then only the
//          "no files" entry and an "all files" entry are provided.
//    
//      pParentWnd
//          A pointer to the dialog parent window.
//
// virtual int CDirDialog::DoModal()
//
//      see CFileDialog::DoModal().
//
// CString CDirDialog::GetPath()
//
//      return value
//          The full path name chosen by the user.
//
// Example:
//
//      // Called when the Browse button is clicked in CMyDialog
//      void CMyDialog::OnBrowseDir() 
//      {
//          if (!UpdateData(TRUE))          // Update current value of m_dir from control
//              return;
//
//          CDirDialog dlg(m_dir,
//                        "JPEG Files (*.jpg; *.jpeg)|*.jpg;*.jpeg|All Files (*.*)|*.*||",
//                        this);
//          dlg.m_ofn.lpstrTitle = "Select Folder to Store Images";
//
//          if (dlg.DoModal() == IDOK)
//          {
//              m_dir = dlg.GetPath();      // Store selected directory name back into the control
//              UpdateData(FALSE);
//          }
//      }
//
//
// INTERNAL DESIGN
// ---------------
//
// The following changes are made to the controls in the standard file open dialog:
//
// The "Open" button is hidden and replaced with another button (IDC_OPEN).
// The normal edit control (edt1) where the file name is entered is hidden and replaced 
// by a "subclassed" edit control (IDC_DIR) of class CDirEdit (derived from CEdit).
// By hiding and replacing these controls we can manipulate the behaviour
// of the dialog in ways not provided for in any other way.  For example, by changing
// the contents of the hidden edit control (edt1) and simulating a click of the hidden
// Open button (IDOK) we can force the contents of a directory to be displayed.
//
// An extra entry is added to the file types drop down combo called "Show Folders Only"
// that causes no files to be displayed.  (If no filters are supplied at all by using
// the default value of NULL, then an "All Files" filter type is also added.)
// The filter string is a single dot (full-stop) which will match no files.
//
// The new edit control (IDC_DIR) is subclassed so that the contents are monitored and
// the some keys can be intercepted.  When the contents are changed and they end with
// a backslash the current display is changed to point to the directory entered (if it
// exists).  When return is pressed the directory is also changed, but if it doesn't
// exist then the user is asked if he wants to create it.  The way the directory is
// changed (ie. the files of that directory are shown in the display) is by putting the
// directory name into the original edit control (edt1) and simulating a click of the
// original Open button (IDOK).  Directory name completion is also performed as the
// user types in a directory name.
//
// The IDC_OPEN button is used as a replacement for the IDOK button while still allowing
// the hidden IDOK button to be used to change the displayed directory.
//
// The CDirDialog class is derived from CFileDialog. The following base class members
// are overridden:
// - OnInitDone: so that the dialog controls can be reorganized
// - OnFolderChange: so that when the user clicks on a folder name the edit control can
//   be updated to reflect the name of the currently selected directory
// - OnFileNameOK: always returns TRUE so that the user can't select files by
//   double-clicking them (this is a DIRECTORY selection dialog after all)


#include "stdafx.h"

// If you don't want this as part of your project (eg to put in a library) remove
// the above #include "stdafx.h" and uncomment the following 3 lines:
//#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions

#include <Dlgs.h>           // For file dialog control IDs
#include <imagehlp.h>       // For ::MakeSureDirectoryPathExists()

#include "vgui_dir_dialog.h"      // Our own header file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_DIR 181         // New edit control for entering the directory name
#define IDC_OPEN 182        // New "Open" button

// Class CDlgWnd
BEGIN_MESSAGE_MAP(CDlgWnd, CWnd)
        ON_BN_CLICKED(IDC_OPEN, OnOpen)
END_MESSAGE_MAP()

void CDlgWnd::OnOpen()
{
    // Get the text and check whether it is a valid directory
    CString ss;
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_DIR);
    ASSERT(pEdit != NULL);
    pEdit->GetWindowText(ss);
    int len = ss.GetLength();

    if (len == 2 && ss[0] == '\\' && ss[1] == '\\')
    {
        AfxMessageBox(ss + _T("\nThis is not a valid folder."));
        pEdit->SetFocus();
        return;
    }
    else if (len == 0 || len == 1 && ss[0] == '\\')
    {
        // Current directory or root of the current drive (therefore must be valid)
        ;
    }
    else if ((len == 2 && ss[1] == ':') ||
             (len == 3 && ss[1] == ':' && ss[2] == '\\') )
    {
        _TCHAR rootdir[4] = _T("?:\\");
        rootdir[0] = ss[0];

        if (GetDriveType(rootdir) <= DRIVE_NO_ROOT_DIR)
        {
            AfxMessageBox(ss + _T("\nThe drive is invalid."));
            pEdit->SetFocus();
            return;
        }
    }
    else
    {
        // Check that it's a valid directory
        if (ss[len-1] == '\\')
            ss = ss.Left(--len);
        DWORD attr = GetFileAttributes(ss);
        if (attr == 0xFFFFFFFF)
        {
            const char *ss2;

            // Directory not found but maybe it's an invalid drive
            _TCHAR rootdir[4] = _T("?:\\");
            rootdir[0] = ss[0];

            if (len > 1 && ss[1] == ':' && GetDriveType(rootdir) <= DRIVE_NO_ROOT_DIR)
            {
                AfxMessageBox(ss + _T("\nThe drive is invalid."));
                pEdit->SetFocus();
                return;
            }
            else if (len >= 2 && ss[0] == '\\' && ss[1] == '\\' && 
                     ( (ss2 = strchr((const char *)ss+2, '\\')) == NULL || strchr(ss2+1, '\\') == NULL) )
            {
                AfxMessageBox(ss + _T("\nThis is not a valid folder."));
                pEdit->SetFocus();
                return;
            }
            else
            {
                // Appears to be a valid drive (or relative path)
                CString mess(ss);
                mess += _T("\nThis folder does not exist.\n\n"
                      "Do you want to create it?");
                if (AfxMessageBox(mess, MB_YESNO) == IDYES)
                {
                    // MakeSureDirectoryPathExists is not part of Windows but is
                    // in the IMAGHLP.DLL which is always present.  This call
                    // requires linking with IMAGHLP.LIB.
                    if (!::MakeSureDirectoryPathExists(ss + _T("\\")))
                    {
                        switch (GetDriveType(rootdir))
                        {
                        case DRIVE_CDROM:
                            AfxMessageBox(_T("You cannot create this folder\n"
                                          "as the CD ROM medium is read-only."));
                            break;
                        case DRIVE_REMOVABLE:
                            AfxMessageBox(_T("You cannot create this folder.\n"
                                          "The medium may be write-protected."));
                            break;
                        case DRIVE_REMOTE:
                            AfxMessageBox(_T("You do not have permission to create\n"
                                          "this folder on the network."));
                            break;
                        default:
                            AfxMessageBox(_T("You do not have permission\n"
                                          "to create this folder."));
                            break;
                        }
                        pEdit->SetFocus();
                        return;         // Directory could not be created
                    }
                    // directory was created, so continue
                }
                else
                {
                    pEdit->SetFocus();
                    return;             // User did not want to create directory
                }
            }
        }
        else if ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
           // AfxMessageBox(ss + _T("\nThis is a file not a directory."));
            //pEdit->SetFocus();
            //return;
        }
    }

    // We have now selected a directory and will return from the dialog
    CheckDir(ss);

    ::EndDialog(m_hWnd, IDOK);
}

// This routine updates the directory/file list display using the directory
// name given.  It does this by putting the name in the (hidden) edit control
// and simulating a press of the (hidden) IDOK button.  If the directory is
// invalid in some way the currently displayed list will not be changed and
// some sort of error message may be displayed.
void CDlgWnd::CheckDir(const CString &ss)
{
    // Put the new directory into the old (hidden) edit box
    CEdit *pOld = (CEdit *)GetDlgItem(edt1);
    ASSERT(pOld != NULL);
    pOld->SetWindowText(ss);

    // Save the current text/selection in the edit control
    CString strSaved;                       // Current text in edit box
    int start, end;                         // Current selection in edit box
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_DIR);
    ASSERT(pEdit != NULL);
    pEdit->GetWindowText(strSaved);
    pEdit->GetSel(start, end);

    CWnd *pOK = GetDlgItem(IDOK);
    pOK->SendMessage(WM_LBUTTONDOWN);
    pOK->SendMessage(WM_LBUTTONUP);

    CString strNew;
    pEdit->GetWindowText(strNew);

    // Usually we want to keep what the user has typed (strSaved) rather than what has been
    // put in the edit control due to OnFolderChange.  One exception is if the user has
    // used "..", "..." etc to change to an ancestor directory in which case we don't want to
    // leave this the same as it will cause repeated changes to ancestor directories whenever
    // the user types backslash (\).  Also don;t set the edit string back to what the user
    // typed if it would be empty or unchanged except for case (as the case probably looks
    // better the way it was filled in).
    if (strSaved.IsEmpty() || strSaved[0] == '.' ||
        strNew.CompareNoCase(strSaved) == 0 || strNew.CompareNoCase(strSaved + '\\') == 0)
    {
        pEdit->SetSel(strNew.GetLength(), -1);
    }
    else
    {
        // Restore the edit control the way the user typed it
        pEdit->SetWindowText(strSaved);
        pEdit->SetSel(start, end);
    }
}
// --- class CDlgWnd ---

// CDirEdit control class
BEGIN_MESSAGE_MAP(CDirEdit, CEdit)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CDirEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CDlgWnd *pp;                           // Parent = the dialog itself
    VERIFY(pp = (CDlgWnd *)GetParent());

    if (nChar == '\t')
    {
        // Because we are getting all keys (see OnGetDlgCode()) so that we can get the Return key,
        // we also get the tab key as a side-effect.  This means that the tabbing between controls
        // in the dialog will stop at the edit control unless we force it to go to the next control.
        CWnd *pWnd = pp->GetDlgItem(IDC_OPEN);
        ASSERT(pWnd != NULL);
        pWnd->SetFocus();                       // Set focus to Open button
    }
    else if (nChar == '\r' || nChar == '\n')
    {
        // If return key is pressed we change to the directory specified OR
        // if the directory name appears valid but does not exist we ask the
        // user if they want to create it.  Note that the string is not
        // validated (although some validation may be done by Windows
        // via the CheckDir() call).  The name is only checked to see if
        // it is possible that a directory needs to be created.
        // Full validation is deferred till the "Open" button is clicked.

        CString ss;
        GetWindowText(ss);
        int len = ss.GetLength();

        // Remove trailing backslash unless root directory or network root
        if (strcmp(ss,"\\") != 0 && strcmp(ss,"\\\\") != 0 && strcmp((const char *)ss+1,":\\") != 0 &&
            len > 0 && ss[len-1] == '\\' )
        {
            ss = ss.Left(--len);
        }

        if (len == 0 || 
            len == 1 && ss[0] == '\\' ||
            len >= 2 && ss[0] == '\\' && ss[1] == '\\' && strchr((const char *)ss+2, '\\') == NULL ||
            len == 2 && ss[1] == ':' ||
            len == 3 && ss[1] == ':' && ss[2] == '\\' )
        {
            // Definitely not a createable directory
            pp->CheckDir(ss);
        }
        else
        {
            // Check if it's an existing directory
            CFileStatus fs;

            DWORD attr = GetFileAttributes(ss);
            if (attr == 0xFFFFFFFF)
            {
                // Directory not found but maybe it's an invalid drive
                _TCHAR rootdir[4] = _T("?:\\");
                rootdir[0] = ss[0];

                if (len == 1 || (len > 1 && ss[1] != ':') ||
                    GetDriveType(rootdir) > DRIVE_NO_ROOT_DIR)
                {
                    // Appears to be a valid drive (or relative path)
                    CString mess(ss);
                    mess += _T("\nThis folder does not exist.\n\n"
                          "Do you want to create it?");
                    if (AfxMessageBox(mess, MB_YESNO) == IDYES)
                    {
                        if (!::MakeSureDirectoryPathExists(ss + _T("\\")))
                        {
                            switch (GetDriveType(rootdir))
                            {
                            case DRIVE_CDROM:
                                AfxMessageBox(_T("You cannot create this folder\n"
                                              "as the CD ROM medium is read-only."));
                                break;
                            case DRIVE_REMOVABLE:
                                AfxMessageBox(_T("You cannot create this folder.\n"
                                              "The medium may be write-protected."));
                                break;
                            case DRIVE_REMOTE:
                                AfxMessageBox(_T("You do not have permission to create\n"
                                              "this folder on the network."));
                                break;
                            default:
                                AfxMessageBox(_T("You do not have permission or\n"
                                              "otherwise cannot create this folder."));
                                break;
                            }
                            return;
                        }
                    }
                    else
                        return;
                }
            }
            pp->CheckDir(ss);
            // Make sure the directory name ends with backslash so user can type sub-drectory name
            GetWindowText(ss);
            if (ss[ss.GetLength()-1] != '\\')
            {
                ss += "\\";
                SetWindowText(ss);
            }
            SetSel(ss.GetLength(), -1);
        }
        SetFocus();                         // Make sure caret stays in this edit control
    }
    else
    {
        CEdit::OnChar(nChar, nRepCnt, nFlags);

        // Get the text and check whether it is a valid directory
        CString ss;                         // Current text in the edit control
        GetWindowText(ss);

        int len = ss.GetLength();
        int start, end;                     // Current selection
        GetSel(start, end);

        if (ss.Compare(_T("\\\\")) == 0)
        {
            // Don't check \\ else we get a message about "\\" being an invalid filename
            ;
        }
        else if (ss.Compare(_T("\\")) == 0)
        {
            // Show root directory
            pp->CheckDir(ss);
        }
        else if (len == 3 && ss[1] == ':' && ss[2] == '\\')
        {
            // Check that it's a valid drive
            if (GetDriveType(ss) > DRIVE_NO_ROOT_DIR)
            {
                pp->CheckDir(ss);
            }
        }
        else if (len > 0 && ss[len-1] == '\\')
        {
            // Check that it's a valid directory
            // xxx does not handle "\\anwar\"
            DWORD attr = GetFileAttributes(ss);
            if (attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                pp->CheckDir(ss);
            }
        }
        else if (start == len && nChar != '\b')
        {
            // Try to do completion of the directory name
            CFileFind ff;                   // Used to find directory names that start with ss
            int count = 0;                  // Number of matching directory names
            CString strMatch;               // The last directory found that matches

            BOOL bContinue = ff.FindFile(ss + "*");

            while (bContinue)
            {
                // At least one match - check them all
                bContinue = ff.FindNextFile();

                if (ff.IsDirectory())
                {
                    // Found a matching directory
                    ++count;
                    strMatch = ff.GetFileName();
                }
            }

            // If there was exactly one matching directory use it
            if (count == 1)
            {
                int ii;
                // The file open dialog changes all uppercase names to lower case with an initial
                // capital (eg WINDOWS displays as Windows).  We do the same so things look nicer.
                for (ii = 0; ii < strMatch.GetLength(); ++ii)
                {
                    // Don't change if it contains spaces or lowercase letters
                    if (isspace(strMatch[ii]) || islower(strMatch[ii]))
                        break;
                }

                ASSERT(ii <= strMatch.GetLength());
                if (!strMatch.IsEmpty() && ii == strMatch.GetLength())
                {
                    CString temp = strMatch.Mid(1);
                    temp.MakeLower();
                    strMatch = strMatch.Left(1) + temp;
                }


                // Get the bit of the directory name that the user has not yet typed
                int lb_len;             // Length of last bit (after \ or :)
                lb_len = ss.ReverseFind('\\');
                if (lb_len == -1) lb_len = ss.ReverseFind('/');
                if (lb_len == -1) lb_len = ss.ReverseFind(':');
                if (lb_len == -1)
                    lb_len = ss.GetLength();
                else
                    lb_len = ss.GetLength() - (lb_len+1);

                // Check if the last char is the same case as the same char in the matched name
                if (!ss.IsEmpty() && lb_len > 0 && strMatch[lb_len-1] != ss[ss.GetLength()-1])
                {
                    // The user used different case to that of the corresponding character in
                    // the matched directory so change the matched name to be the user's case.
                    if (isupper(ss[ss.GetLength()-1]))
                        strMatch.MakeUpper();
                    else
                        strMatch.MakeLower();
                }

#ifdef _DEBUG
                CString temp = strMatch.Left(lb_len);
                ASSERT(temp.CompareNoCase(ss.Right(lb_len)) == 0);
#endif
                end += strMatch.GetLength() - lb_len;
                SetWindowText(ss + strMatch.Mid(lb_len));
                SetSel(start, end);
            }

            // else if (count > 1) pop-up some sort of selection list???
        }
        SetFocus();                         // Make sure caret stays in this edit control
    }
}

void CDirEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

    if (nChar != VK_DELETE)
        return;

    CDlgWnd *pp;                           // Parent = the dialog itself
    VERIFY(pp = (CDlgWnd *)GetParent());

    // Get the current text and check whether it is a valid directory
    CString ss;
    GetWindowText(ss);
    int len = ss.GetLength();

    if (ss.Compare(_T("\\\\")) == 0)
    {
        // Don't check \\ else we get a message about "\\" being an invalid filename
        ;
    }
    else if (ss.Compare(_T("\\")) == 0)
    {
        // Show root directory
        pp->CheckDir(ss);
    }
    else if (len == 3 && ss[1] == ':' && ss[2] == '\\')
    {
        // Check that it's a valid drive
        if (GetDriveType(ss) > DRIVE_NO_ROOT_DIR)
        {
            pp->CheckDir(ss);
        }
    }
    else if (len > 0 && ss[len-1] == '\\')
    {
        // Check that it's a valid directory
        DWORD attr = GetFileAttributes(ss);
        if (attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            pp->CheckDir(ss);
        }
    }
    SetFocus();                         // Make sure caret stays in this edit control
}

UINT CDirEdit::OnGetDlgCode() 
{
    // Get all keys so that we see CR
    return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}
// --- class CDirEdit ---

// class vgui_dir_dialog
vgui_dir_dialog::vgui_dir_dialog(LPCTSTR initial, LPCTSTR filter, CWnd* pParentWnd)
    : CFileDialog(TRUE, NULL, NULL, 
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                  NULL, pParentWnd),
      m_strPath(initial)
{
    // Note: m_strFilter is a member variable so it doesn't disappear because
    // it is used later internally by the file open dialog (via m_ofn.lpstrFilter).
    if (filter != NULL)
      m_strFilter = filter + CString(_T("Show Folders Only|.||"));
    else
        m_strFilter = _T("All Files (*.*)|*.*||Show Folders Only|.|");
    m_strFilter.Replace('|', '\0');
    m_ofn.lpstrFilter = m_strFilter;

    m_ofn.lpstrInitialDir = initial;

    m_ofn.lpstrTitle = _T("Select Folder");

    m_ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;

}

void vgui_dir_dialog::OnInitDone()
{
    CRect rct;                          // Used to move/resize controls
    CWnd *pp;                           // Parent = the dialog window itself
    VERIFY(pp = GetParent());

    
    ASSERT(pp->GetDlgItem(stc3) != NULL);
    pp->GetDlgItem(stc3)->SetWindowText(_T("Folder:"));
 
    // Create a new CDlgWnd so we can catch dialog control notifications
    VERIFY(m_DlgWnd.SubclassWindow(pp->m_hWnd));

    // Create a new edit control where edt1 now is
    CWnd *w = pp->GetDlgItem(edt1);
    ASSERT(pp->GetDlgItem(edt1) != NULL);
    pp->GetDlgItem(edt1)->GetWindowRect(rct); //Get edt1 rectangle
    pp->ScreenToClient(rct);

    VERIFY(m_Edit.Create(WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                           rct, pp, IDC_DIR));
    if (m_ofn.lpstrInitialDir  != NULL)
        m_Edit.SetWindowText(m_ofn.lpstrInitialDir);
    m_Edit.SetFont(pp->GetDlgItem(edt1)->GetFont());
    m_Edit.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
    m_Edit.SetWindowPos(pp->GetDlgItem(stc3), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//  m_Edit.SetSel(0, strlen(m_ofn.lpstrInitialDir));

    CWnd *pCancel = pp->GetDlgItem(IDCANCEL);
    ASSERT(pCancel != NULL);

    // Create a new button where the OK button now is
    ASSERT(pp->GetDlgItem(IDOK) != NULL);
    pp->GetDlgItem(IDOK)->GetWindowRect(rct); //Get OK button rectangle
    pp->ScreenToClient(rct);

    m_Open.Create(_T("Open"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    rct, pp, IDC_OPEN);
    m_Open.SetFont(pp->GetDlgItem(IDOK)->GetFont());
    m_Open.SetWindowPos(&m_Edit, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    pCancel->SetWindowPos(&m_Open, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Change default push button
    pp->GetDlgItem(IDOK)->ModifyStyle(BS_DEFPUSHBUTTON, 0);
    pp->SendMessage(DM_SETDEFID, IDC_OPEN);

#ifdef DIRDIALOG_TESTING
    // Move controls (rather than hide them) for testing

    // Increase size of dialog
    pp->GetWindowRect(rct);
    pp->SetWindowPos(NULL, 0, 0, rct.Width(), rct.Height() + 70, SWP_NOZORDER | SWP_NOMOVE);

    // Move the replaced controls down
    ASSERT(pp->GetDlgItem(IDOK) != NULL);
    pp->GetDlgItem(IDOK)->GetWindowRect(rct);
    pp->ScreenToClient(rct);
    pp->GetDlgItem(IDOK)->SetWindowPos(NULL, rct.left, rct.top+70,
                   0, 0, SWP_NOZORDER | SWP_NOSIZE);

    ASSERT(pp->GetDlgItem(edt1) != NULL);
    pp->GetDlgItem(edt1)->GetWindowRect(rct);
    pp->ScreenToClient(rct);
    pp->GetDlgItem(edt1)->SetWindowPos(NULL, rct.left, rct.top+70,
                   0, 0, SWP_NOZORDER | SWP_NOSIZE);

#else
    // Hide the controls we don't want the user to use
    HideControl(IDOK);
    HideControl(edt1);
#endif

    CFileDialog::OnInitDone();
}

void vgui_dir_dialog::OnFolderChange()
{
    CWnd *pp;                           // Parent window = the dialog itself
    VERIFY(pp = GetParent());
    ASSERT(::IsWindow(pp->m_hWnd));

    ASSERT(pp->GetDlgItem(IDC_DIR) != NULL);
    m_strPath = GetFolderPath();
    int len = m_strPath.GetLength();
    if (len > 0 && m_strPath[len-1] != '\\')
    {
        m_strPath += "\\";
        ++len;
    }
    pp->GetDlgItem(IDC_DIR)->SetWindowText(m_strPath);
    m_Edit.SetSel(len, len);

    CFileDialog::OnFolderChange();

    m_Edit.SetFocus();
}

BOOL vgui_dir_dialog::OnFileNameOK() 
{ 
  CWnd *pp; // Parent window = the dialog itself 
  VERIFY(pp = GetParent()); 
  ASSERT(::IsWindow(pp->m_hWnd)); 

  ASSERT(pp->GetDlgItem(IDC_DIR) != NULL); 

  m_strPath = GetPathName(); 
  int len = m_strPath.GetLength(); 

  pp->GetDlgItem(IDC_DIR)->SetWindowText(m_strPath); 
  m_Edit.SetSel(len, len); 

  CFileDialog::OnFolderChange(); 

  m_Edit.SetFocus(); 

  return TRUE; 
} 
