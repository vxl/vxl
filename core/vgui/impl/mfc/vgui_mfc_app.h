// This is ./oxl/vgui/impl/mfc/vgui_mfc_app.h

//:
// \file
// \author Marko Bacic, Oxford RRG
// \date   27 July 2000
// \brief  Defines CWinApp which initialises all the MRC stuff.
//
// \verbatim
//  Modifications:
//    23-AUG-2000 Marko Bacic, Oxford RRG -- Added support for scroll bars
// \endverbatim

#ifndef AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
#define AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
        #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//:  Defines CWinApp which initialises all the MFC stuff.
//
//   This application class (CWinApp) is instantiated once
//   and only once in the application and initialises all the MFC stuff
class vgui_mfc_app : public CWinApp
{
public:
        vgui_mfc_app();

        public:
        virtual BOOL InitInstance();
        virtual BOOL Run();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
