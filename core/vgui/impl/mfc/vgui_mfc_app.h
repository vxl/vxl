// This is core/vgui/impl/mfc/vgui_mfc_app.h
#ifndef AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
#define AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
//:
// \file
// \brief  Defines CWinApp which initialises all the MRC stuff.
// \author Marko Bacic, Oxford RRG
// \date   27 July 2000
//
//  Contains class  vgui_mfc_app
//
// \verbatim
//  Modifications
//   23-AUG-2000 Marko Bacic, Oxford RRG -- Added support for scroll bars
// \endverbatim

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
        #error include 'StdAfx.h' before including this file for PCH
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

  virtual BOOL InitInstance();
  virtual BOOL Run();
  virtual BOOL OnIdle( LONG lcount );
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUIMFCAPP_H__259D1B15_6A52_45B3_B335_8A7C78097A13__INCLUDED_
