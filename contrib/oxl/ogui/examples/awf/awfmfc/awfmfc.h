// awfmfc.h : main header file for the AWFMFC application
//

#ifndef AFX_AWFMFC_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
#define AFX_AWFMFC_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
        #error include 'StdAfx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcApp:
// See awfmfc.cpp for the implementation of this class
//

class example_CAwfmfcApp : public CWinApp
{
public:
        example_CAwfmfcApp();

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CAwfmfcApp)
        public:
        virtual BOOL InitInstance();
        //}}AFX_VIRTUAL

// Implementation
        //{{AFX_MSG(example_CAwfmfcApp)
        afx_msg void OnAppAbout();
                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_AWFMFC_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
