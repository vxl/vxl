// awfmfcView.h : interface of the example_CAwfmfcView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AFX_AWFMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_
#define AFX_AWFMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vgui/impl/mfc/vgui_mfc_adaptor.h>


class example_CAwfmfcView : public vgui_mfc_adaptor
{
protected: // create from serialization only
        example_CAwfmfcView();
        DECLARE_DYNCREATE(example_CAwfmfcView)

// Attributes
public:
        example_CAwfmfcDoc* GetDocument();

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CAwfmfcView)
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
        virtual ~example_CAwfmfcView();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
        //{{AFX_MSG(example_CAwfmfcView)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in awfmfcView.cpp
inline example_CAwfmfcDoc* example_CAwfmfcView::GetDocument()
   { return (example_CAwfmfcDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_AWFMFCVIEW_H__E03278BF_9B92_4B98_A4EF_43C38014407A__INCLUDED_
