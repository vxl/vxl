// MainFrm.h : interface of the example_CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AFX_MAINFRM_H__8164CEA5_8BF1_4CCA_B0C9_B25BBF6219AD__INCLUDED_
#define AFX_MAINFRM_H__8164CEA5_8BF1_4CCA_B0C9_B25BBF6219AD__INCLUDED_

#if defined(_MSC_VER)
#pragma once
#endif

class example_CMainFrame : public CFrameWnd
{
 protected: // create from serialization only
        example_CMainFrame();
        DECLARE_DYNCREATE(example_CMainFrame)

// Attributes
 public:

// Operations
 public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CMainFrame)
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        //}}AFX_VIRTUAL

// Implementation
 public:
        virtual ~example_CMainFrame();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

 protected:  // control bar embedded members
        CStatusBar  m_wndStatusBar;
        CToolBar    m_wndToolBar;

// Generated message map functions
 protected:
        //{{AFX_MSG(example_CMainFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_MAINFRM_H__8164CEA5_8BF1_4CCA_B0C9_B25BBF6219AD__INCLUDED_
