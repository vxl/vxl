// This is core/vgui/impl/wx/vgui_wx_statusbar.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_statusbar.
//
// See vgui_wx_statusbar.h for details.
//=========================================================================

#include "vgui_wx_statusbar.h"

#include <wx/statusbr.h>

//-------------------------------------------------------------------------
// vgui_wx_statusbar implementation - construction & destruction.
//-------------------------------------------------------------------------
//: Constructor, creates a wxWidgets status bar and displays it.
vgui_wx_statusbar::vgui_wx_statusbar(void)
  : widget_(0)
  , statusbuf_(new vgui_statusbuf(this))
{
}

//: Destructor.
vgui_wx_statusbar::~vgui_wx_statusbar(void)
{
  delete statusbuf_;
}

//-------------------------------------------------------------------------
// vgui_wx_statusbar implementation.
//-------------------------------------------------------------------------
//: Append given text (of given length) to the status bar.
int vgui_wx_statusbar::write(const char* text, int n)
{
  if (widget_)
  {
    if (n == 1)
    {
      if (text[0] == '\n')
      {
        // buffer filled, print to wxStatusBar and reset
        widget_->SetStatusText(linebuffer_.c_str());
        linebuffer_ = "";
      }
      else { linebuffer_ += text[0]; }
    }
    else
    {
      linebuffer_.append(text, n);
      if (linebuffer_.find('\n') != vcl_string::npos)
      {
        // buffer filled, print to wxStatusBar and reset
        widget_->SetStatusText(linebuffer_.c_str());
        linebuffer_ = "";
      }
    }
  }
  return n;
}

//: Append given text to the status bar.
int vgui_wx_statusbar::write(const char* text)
{
  if (widget_)
  {
    linebuffer_ = text;
    widget_->SetStatusText(linebuffer_.c_str());
  }

  return 1;
}
