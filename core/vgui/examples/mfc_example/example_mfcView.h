// example_mfcView.h : interface of the example_CExample_mfcView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AFX_EXAMPLE_MFCVIEW_H__18E6E4B2_3489_47C5_ABEA_A07DF852DBEC__INCLUDED_
#define AFX_EXAMPLE_MFCVIEW_H__18E6E4B2_3489_47C5_ABEA_A07DF852DBEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vgui/impl/mfc/vgui_mfc_adaptor.h>

class example_CExample_mfcView : public vgui_mfc_adaptor
{
 protected: // create from serialization only
  example_CExample_mfcView();
  DECLARE_DYNCREATE(example_CExample_mfcView)

// Attributes
 public:
#ifdef _DEBUG  // debug version in example_mfcView.cpp
  example_CExample_mfcDoc* GetDocument();
#else
  example_CExample_mfcDoc* GetDocument() { return (example_CExample_mfcDoc*)m_pDocument; }
#endif

// Operations
 public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(example_CExample_mfcView)
 public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
 protected:
  //}}AFX_VIRTUAL

// Implementation
 public:
  virtual ~example_CExample_mfcView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

 protected:

// Generated message map functions
 protected:
  //{{AFX_MSG(example_CExample_mfcView)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_EXAMPLE_MFCVIEW_H__18E6E4B2_3489_47C5_ABEA_A07DF852DBEC__INCLUDED_
