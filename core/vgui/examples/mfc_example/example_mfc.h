// example_mfc.h : main header file for the EXAMPLE_MFC application
//

#ifndef AFX_EXAMPLE_MFC_H__D215F64C_C1C9_41E2_B0F8_05ACF6313455__INCLUDED_
#define AFX_EXAMPLE_MFC_H__D215F64C_C1C9_41E2_B0F8_05ACF6313455__INCLUDED_

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AFXWIN_H__
        #error include 'StdAfx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcApp:
// See example_mfc.cpp for the implementation of this class
//

class example_CExample_mfcApp : public CWinApp
{
public:
        example_CExample_mfcApp();

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(example_CExample_mfcApp)
        public:
        virtual BOOL InitInstance();
        //}}AFX_VIRTUAL

// Implementation
        //{{AFX_MSG(example_CExample_mfcApp)
        afx_msg void OnAppAbout();
                // NOTE - the ClassWizard will add and remove member functions here.
                //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_EXAMPLE_MFC_H__D215F64C_C1C9_41E2_B0F8_05ACF6313455__INCLUDED_
