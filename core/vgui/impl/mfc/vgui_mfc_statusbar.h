// This is oxl/vgui/impl/mfc/vgui_mfc_statusbar.h
#ifndef vgui_mfc_statusbar_h_
#define vgui_mfc_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author   Marko Bacic, Oxford RRG
// \date     08 Aug 2000
// \brief    Status bar for the main window of the application.
//
//  Contains classes: vgui_mfc_statusbar
//
// \verbatim
//  Modifications:
//     08-AUG-2000 Marko Bacic - Initial version.
// \endverbatim

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>
#include "StdAfx.h"

//: Status bar for the main window of the application.
class vgui_mfc_statusbar : public vgui_statusbar
{
 public:

  //: Constructor, creates an MFC status bar object and displays it.
  vgui_mfc_statusbar();
  //: Destructor.
 ~vgui_mfc_statusbar();

  //: Append given text (of given length) to the status bar
  int write(const char* text, int n);
  //: Append given text to the status bar
  int write(const char* text);
  //: Updates the status bar by calling OnUpdateStatusBar of vgui_mfc_mainfrm
  void update();

  //: MFC status bar object
  CStatusBar *statusbar;
  //: Text displayed on the statusbar
  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  vcl_ostream out;
};

#endif // vgui_mfc_statusbar_h_
