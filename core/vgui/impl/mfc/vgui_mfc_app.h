//-*- c++ -*-------------------------------------------------------------------
//
// .NAME vgui_mfc_app
// .LIBRARY vgui-mfc
// .SECTION Description
//   Defines an application class(CWinApp) which is instantiated once
//   and only once in the application and which initialises all the MFC stuff
// .SECTION Author
//    Marko Bacic, Oxford RRG
// Created: 27 July 2000
// .SECTION Modifications:
//    23-AUG-2000 Marko Bacic, Oxford RRG -- Added support for scroll bars
//-----------------------------------------------------------------------------

#ifndef AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
#define AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
        #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_app:

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
