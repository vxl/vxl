// example_mfcDoc.h : interface of the example_CExample_mfcDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AFX_EXAMPLE_MFCDOC_H__957CF9E8_8EF9_4244_A5B7_DAB9A3EF96A9__INCLUDED_
#define AFX_EXAMPLE_MFCDOC_H__957CF9E8_8EF9_4244_A5B7_DAB9A3EF96A9__INCLUDED_

#if defined(_MSC_VER)
#pragma once
#endif

#include <vgui/vgui_tableau_sptr.h>

class example_CExample_mfcDoc : public CDocument
{
protected: // create from serialization only
        example_CExample_mfcDoc();
        DECLARE_DYNCREATE(example_CExample_mfcDoc)

// Attributes
public:

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CExample_mfcDoc)
        public:
        virtual BOOL OnNewDocument();
        virtual void Serialize(CArchive& ar);
        //}}AFX_VIRTUAL

// Implementation
public:
        vgui_tableau_sptr get_tableau();
        virtual ~example_CExample_mfcDoc();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  // awf: we store our main tableau in the document -- the
  // adaptor(s) view this tableau.  Also added get_tableau() above.
        vgui_tableau_sptr tableau;
        //{{AFX_MSG(example_CExample_mfcDoc)
                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_EXAMPLE_MFCDOC_H__957CF9E8_8EF9_4244_A5B7_DAB9A3EF96A9__INCLUDED_
