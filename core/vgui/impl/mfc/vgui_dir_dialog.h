#ifndef vgui_dir_dialog_h_
#define vgui_dir_dialog_h_
//:
// \file
// \brief  creates a directory browsing dialog, it allows to choose both directories and files
// \author Gamze Tunali, LEMS, Brown University
// \date   16 Nov 2007

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CDlgWnd - intercepts messages from child controls
class CDlgWnd : public CWnd
{
 public:
  void CheckDir(const CString &ss);   // Display directory contents

 protected:
  afx_msg void OnOpen();              // Open button clicked

  DECLARE_MESSAGE_MAP()
};

// CDirEdit - edit control class
class CDirEdit : public CEdit
{
 protected:
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();

  DECLARE_MESSAGE_MAP()
};

// vgui_dir_dialog - directory selection dialog
class vgui_dir_dialog : public CFileDialog
{
 public:
  vgui_dir_dialog(LPCTSTR initial = NULL,
                  LPCTSTR filter = NULL, CWnd* pParentWnd = NULL);

  CString GetPath() { return m_strPath; }

  // Overriden members of CFileDialog
  virtual void OnInitDone();
  virtual void OnFolderChange();

  // Disallow selection of files (since we're only selecting directories)
  virtual BOOL OnFileNameOK(); // { return TRUE; }

 private:
  CString m_strPath;                  // Current directory
  CString m_strFilter;                // The current file filters string (used for string storage
                                      // for internal use of File Open dialog)

  CDlgWnd m_DlgWnd;                   // Subclassed dialog window (parent of vgui_dir_dialog window)
  CDirEdit m_Edit;                    // Edit control where you can type in a dir. name
  CButton m_Open;                     // "Open" button (replaces OK button)
};

#endif // vgui_dir_dialog_h_
