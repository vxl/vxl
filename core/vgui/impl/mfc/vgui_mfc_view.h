// This is core/vgui/impl/mfc/vgui_mfc_view.h
#ifndef AFX_VGUIMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_
#define AFX_VGUIMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_
//:
// \file
// \author  Oxford RRG

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vgui/impl/mfc/vgui_mfc_adaptor.h>

class vgui_mfc_view : public vgui_mfc_adaptor
{
 protected: // create from serialization only
  vgui_mfc_view();
  DECLARE_DYNCREATE(vgui_mfc_view)

  // Attributes
 public:
  CDocument* GetDocument();

  // Operations
 public:

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAwfmfcView)
 public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
 protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  //}}AFX_VIRTUAL

  // Implementation
 public:
  virtual ~vgui_mfc_view();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
 protected:
  //{{AFX_MSG(vgui_mfc_view)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in awfmfcView.cpp
inline CDocument* vgui_mfc_view::GetDocument()
{
  return (CDocument*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUIMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_
