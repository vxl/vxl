// awfmfcDoc.h : interface of the example_CAwfmfcDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AFX_AWFMFCDOC_H__AB1D93B0_8FC7_4E71_ABD0_FA4AA666D5CD__INCLUDED_
#define AFX_AWFMFCDOC_H__AB1D93B0_8FC7_4E71_ABD0_FA4AA666D5CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class vgui_tableau;
#include <vgui/vgui_tableau_sptr.h>
struct awf_stuff;

class example_CAwfmfcDoc : public CDocument
{
protected: // create from serialization only
        example_CAwfmfcDoc();
        DECLARE_DYNCREATE(example_CAwfmfcDoc)

// Attributes
public:

// Operations
public:
 vgui_tableau_sptr get_tableau() { return tableau; }
// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CAwfmfcDoc)
        public:
        virtual BOOL OnNewDocument();
        virtual void Serialize(CArchive& ar);
        virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
        //}}AFX_VIRTUAL

// Implementation
public:
        virtual ~example_CAwfmfcDoc();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  // awf: we store our main tableau in the document -- the
  // adaptor(s) view this tableau.
        vgui_tableau_sptr tableau;
        awf_stuff* pimpl;

// Generated message map functions
protected:
        //{{AFX_MSG(example_CAwfmfcDoc)
        afx_msg void Ontool1();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_AWFMFCDOC_H__AB1D93B0_8FC7_4E71_ABD0_FA4AA666D5CD__INCLUDED_
