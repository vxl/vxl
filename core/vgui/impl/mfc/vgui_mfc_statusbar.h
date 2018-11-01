// This is core/vgui/impl/mfc/vgui_mfc_statusbar.h
#ifndef vgui_mfc_statusbar_h_
#define vgui_mfc_statusbar_h_
//:
// \file
// \brief    The MFC implementation of vgui_statusbar.
// \author   Marko Bacic, Oxford RRG
// \date     08 Aug 2000
//
//  Contains class vgui_mfc_statusbar
//
// \verbatim
//  Modifications
//   08-AUG-2000 Marko Bacic - Initial version.
// \endverbatim

#include <string>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>
#include "StdAfx.h"

//: The MFC implementation of vgui_statusbar.
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
  std::string linebuffer;
  vgui_statusbuf* statusbuf;
  std::ostream out;
};

#endif // vgui_mfc_statusbar_h_
