// This is core/vgui/impl/wx/vgui_wx_statusbar.h
#ifndef vgui_wx_statusbar_h_
#define vgui_wx_statusbar_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_statusbar.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   04/18/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <string>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

class wxStatusBar;

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_statusbar.
//
// This class allows vgui to post messages to the native GUI's statusbar,
// if available.
//-------------------------------------------------------------------------
class vgui_wx_statusbar : public vgui_statusbar
{
public:
  //: Constructor, creates a wxWidgets status bar and displays it.
  vgui_wx_statusbar();

  //: Destructor.
  virtual ~vgui_wx_statusbar();

  //: Append given text (of given length) to the status bar.
  int write(const char* text, int n);

  //: Append given text to the status bar.
  int write(const char* text);

  vgui_statusbuf* statusbuf(void) const { return statusbuf_; }

  void set_widget(wxStatusBar* widget) { widget_ = widget; }

private:
  wxStatusBar*     widget_;

  vgui_statusbuf*  statusbuf_;
  std::string       linebuffer_;
};

#endif // vgui_wx_statusbar_h_
