// This is oxl/ogui/impl/Xm/vgui_Xm_statusbar.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Nov 99
//-----------------------------------------------------------------------------

#include "vgui_Xm_statusbar.h"
#include <X11/Intrinsic.h>
#include <Xm/Text.h>

vgui_Xm_statusbar::vgui_Xm_statusbar()
  : statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
}


vgui_Xm_statusbar::~vgui_Xm_statusbar()
{
}


int vgui_Xm_statusbar::write(const char* text, int n) {

  if (n == 1) {
    if (text[0] == '\n') {

      XtVaSetValues(widget,
                    XmNvalue, linebuffer.c_str(),
                    0);

      linebuffer = "";
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n')) {

      XtVaSetValues(widget,
                    XmNvalue, linebuffer.c_str(),
                    0);

      linebuffer = "";
    }
  }
  return n;
}


int vgui_Xm_statusbar::write(const char* text) {

  XtVaSetValues(widget,
                XmNvalue, text,
                0);
  return 1;
}
